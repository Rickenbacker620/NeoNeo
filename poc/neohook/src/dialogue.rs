// Dialogue management - buffers and flushes text to output
// Based on src/dialogue.h and src/dialogue.cpp

use crate::server::NeoOutput;
use std::collections::HashMap;
use std::sync::{Arc, Mutex};
use std::time::{Duration, Instant};
use tracing::{debug, info};

/// A dialogue buffer that accumulates text before flushing
pub struct Dialogue {
    /// Unique identifier for this dialogue
    id: String,
    
    /// Text encoding
    encoding: String,
    
    /// Buffer for accumulating text
    buffer: Vec<u8>,
    
    /// Output destination
    output: Arc<dyn NeoOutput>,
    
    /// Timeout before auto-flush
    flush_timeout: Duration,
    
    /// Last time text was received
    last_received_time: Instant,
}

impl Dialogue {
    /// Create a new dialogue
    pub fn new(
        id: String,
        encoding: String,
        output: Arc<dyn NeoOutput>,
        flush_timeout: Duration,
    ) -> Self {
        Self {
            id,
            encoding,
            buffer: Vec::new(),
            output,
            flush_timeout,
            last_received_time: Instant::now(),
        }
    }

    /// Check if the dialogue needs to be flushed
    pub fn need_flush(&self) -> bool {
        !self.buffer.is_empty() && self.last_received_time.elapsed() >= self.flush_timeout
    }

    /// Flush the buffered text to output
    pub fn flush(&mut self) {
        if self.buffer.is_empty() {
            return;
        }

        let text = self.get_utf8_text();
        if !text.is_empty() {
            self.output.output_dialogue(&self.id, &text);
        }

        self.buffer.clear();
    }

    /// Push a byte to the buffer
    pub fn push_byte(&mut self, byte: u8) {
        self.buffer.push(byte);
        self.last_received_time = Instant::now();
    }

    /// Get the text as UTF-8 string
    fn get_utf8_text(&self) -> String {
        match self.encoding.as_str() {
            "shift_jis" => {
                // Convert Shift-JIS to UTF-8
                // Note: This is a simplified version. For production, use encoding_rs crate
                String::from_utf8_lossy(&self.buffer).to_string()
            }
            "utf-8" => String::from_utf8_lossy(&self.buffer).to_string(),
            "utf-16" => {
                // Convert UTF-16 bytes to string
                let u16_vec: Vec<u16> = self
                    .buffer
                    .chunks_exact(2)
                    .map(|chunk| u16::from_le_bytes([chunk[0], chunk[1]]))
                    .collect();
                String::from_utf16_lossy(&u16_vec)
            }
            _ => String::from_utf8_lossy(&self.buffer).to_string(),
        }
    }

    /// Get hex representation of the buffer (for debugging)
    #[allow(dead_code)]
    fn get_hex_text(&self) -> String {
        self.buffer
            .iter()
            .map(|b| format!("{:02x}", b))
            .collect::<Vec<_>>()
            .join(" ")
    }
}

/// Sink manages multiple dialogues and handles flushing
pub struct Sink {
    output: Arc<dyn NeoOutput>,
    dialogues: Arc<Mutex<HashMap<String, Dialogue>>>,
    flush_timeout: Duration,
}

impl Sink {
    /// Create a new sink
    pub fn new(output: Box<dyn NeoOutput>, flush_timeout_ms: u64) -> Self {
        let output = Arc::from(output);
        let flush_timeout = Duration::from_millis(flush_timeout_ms);

        let sink = Self {
            output: output.clone(),
            dialogues: Arc::new(Mutex::new(HashMap::new())),
            flush_timeout,
        };

        // Start the flush thread
        sink.start_flush_thread();

        sink
    }

    /// Initialize the global sink instance
    pub fn init(output: Box<dyn NeoOutput>, flush_timeout_ms: u64) {
        let sink = Self::new(output, flush_timeout_ms);
        GLOBAL_SINK.lock().unwrap().replace(sink);
        info!("Dialogue sink initialized with {}ms timeout", flush_timeout_ms);
    }

    /// Push text to a dialogue
    pub fn push(id: String, encoding: String, byte: u8) {
        if let Some(sink) = GLOBAL_SINK.lock().unwrap().as_ref() {
            sink.push_text_to_dialogue(id, encoding, byte);
        }
    }

    /// Push text to a specific dialogue
    fn push_text_to_dialogue(&self, id: String, encoding: String, byte: u8) {
        let mut dialogues = self.dialogues.lock().unwrap();

        let dialogue = dialogues.entry(id.clone()).or_insert_with(|| {
            debug!("Creating new dialogue: {}", id);
            Dialogue::new(id, encoding, self.output.clone(), self.flush_timeout)
        });

        dialogue.push_byte(byte);
    }

    /// Start the background flush thread
    fn start_flush_thread(&self) {
        let dialogues = self.dialogues.clone();

        std::thread::spawn(move || {
            loop {
                std::thread::sleep(Duration::from_millis(100));

                let mut dialogues = dialogues.lock().unwrap();
                for dialogue in dialogues.values_mut() {
                    if dialogue.need_flush() {
                        dialogue.flush();
                    }
                }
            }
        });
    }
}

/// Global sink instance
static GLOBAL_SINK: Mutex<Option<Sink>> = Mutex::new(None);