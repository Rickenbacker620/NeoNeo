// Game engine adapters
// Based on src/engines/engine_base.h

use crate::hook::HookManager;
use anyhow::Result;
use tracing::info;

pub mod kirikiri;
pub mod pc;

/// Trait for game engine implementations
pub trait EngineImpl: Send + Sync {
    /// Get the engine name
    fn name(&self) -> &str;
    
    /// Attach hooks for this engine
    fn attach_hooks(&self) -> Result<()>;
}

/// Engine wrapper
pub struct Engine {
    implementation: Box<dyn EngineImpl>,
}

impl Engine {
    /// Create a new engine
    pub fn new(implementation: Box<dyn EngineImpl>) -> Self {
        Self { implementation }
    }

    /// Get the engine name
    pub fn name(&self) -> &str {
        self.implementation.name()
    }

    /// Attach hooks for this engine
    pub fn attach_hooks(&self) -> Result<()> {
        self.implementation.attach_hooks()
    }

    /// Deduce the engine type based on loaded modules
    pub fn deduce_engine_type() -> Option<Self> {
        info!("Attempting to deduce engine type...");

        // Try each engine matcher in order
        let matchers: Vec<fn() -> Option<Box<dyn EngineImpl>>> = vec![
            kirikiri::KirikiriEngine::try_match,
            pc::PCEngine::try_match,
        ];

        for matcher in matchers {
            if let Some(engine_impl) = matcher() {
                let engine = Engine::new(engine_impl);
                info!("Detected engine: {}", engine.name());
                return Some(engine);
            }
        }

        info!("Could not detect engine type");
        None
    }
}