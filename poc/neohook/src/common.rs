// Common types and utilities
// Based on src/common.h

/// Memory address type - represents a pointer address in the target process
pub type Address = usize;

/// Hook context - contains information about the hooked function call
#[derive(Debug, Clone)]
pub struct HookContext {
    /// Base address (typically the stack pointer or register value)
    pub base: Address,
}

impl HookContext {
    pub fn new(base: Address) -> Self {
        Self { base }
    }
}