// ZeroMQ server for publishing extracted text
// Based on src/server.h and src/server.cpp

use anyhow::{Context, Result};
use tracing::{error, info};

/// Output trait for dialogue text
pub trait NeoOutput: Send + Sync {
    /// Output a dialogue with ID and text
    fn output_dialogue(&self, id: &str, text: &str);
}

/// ZeroMQ server that publishes extracted dialogue text
pub struct NeoServer {
    context: zmq::Context,
    publisher: zmq::Socket,
}

impl NeoServer {
    const SERVER_PORT: u16 = 12345;

    /// Create a new NeoServer
    pub fn new() -> Result<Self> {
        info!("Initializing NeoServer on port {}", Self::SERVER_PORT);

        let context = zmq::Context::new();
        let publisher = context
            .socket(zmq::PUB)
            .context("Failed to create ZMQ PUB socket")?;

        let bind_addr = format!("tcp://*:{}", Self::SERVER_PORT);
        publisher
            .bind(&bind_addr)
            .context("Failed to bind ZMQ socket")?;

        info!("NeoServer listening on {}", bind_addr);

        Ok(Self {
            context,
            publisher,
        })
    }

    /// Format a message for publishing
    fn format_message(&self, id: &str, text: &str) -> String {
        format!("{}|{}", id, text)
    }
}

impl NeoOutput for NeoServer {
    fn output_dialogue(&self, id: &str, text: &str) {
        let message = self.format_message(id, text);
        
        match self.publisher.send(&message, 0) {
            Ok(_) => {
                info!("Published: {} ({})", id, text.chars().take(50).collect::<String>());
            }
            Err(e) => {
                error!("Failed to publish message: {}", e);
            }
        }
    }
}

impl Drop for NeoServer {
    fn drop(&mut self) {
        info!("Shutting down NeoServer");
    }
}

/// Temporary output for testing (prints to console)
pub struct TempOutput;

impl NeoOutput for TempOutput {
    fn output_dialogue(&self, id: &str, text: &str) {
        println!("{}: {}", id, text);
    }
}