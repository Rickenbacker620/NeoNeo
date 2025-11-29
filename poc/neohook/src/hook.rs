// Hook implementation using retour
// Based on src/hook.h and src/hook.cpp

use crate::common::{Address, HookContext};
use crate::dialogue;
use crate::hook_param::HookParam;
use anyhow::{Context, Result};
use retour::GenericDetour;
use std::sync::Arc;
use tracing::{debug, error, info};

/// A hook that intercepts function calls to extract text
pub struct Hook {
    /// Hook parameters
    param: Arc<HookParam>,
    
    /// The detour (function hook)
    detour: Option<GenericDetour<unsafe extern "system" fn(usize) -> usize>>,
}

impl Hook {
    /// Create a new hook
    pub fn new(param: HookParam) -> Self {
        Self {
            param: Arc::new(param),
            detour: None,
        }
    }

    /// Get the hook name
    pub fn name(&self) -> &str {
        &self.param.name
    }

    /// Attach the hook to the target function
    pub fn attach(&mut self) -> Result<()> {
        if self.param.address == 0 {
            anyhow::bail!("Cannot attach hook with address 0");
        }

        info!("Attaching hook '{}' at address 0x{:x}", self.param.name, self.param.address);

        // Create the detour
        let param = self.param.clone();
        
        unsafe {
            // Cast the address to a function pointer
            let target_fn: unsafe extern "system" fn(usize) -> usize =
                std::mem::transmute(self.param.address);

            // Create the detour with our hook function
            let detour = GenericDetour::new(target_fn, move |arg: usize| -> usize {
                // Call our hook handler
                hook_handler(arg, &param);
                
                // Call the original function
                // Note: In a real implementation, we would call the trampoline here
                // For now, we just return the argument
                arg
            })
            .context("Failed to create detour")?;

            // Enable the hook
            detour.enable().context("Failed to enable detour")?;

            self.detour = Some(detour);
        }

        info!("Hook '{}' attached successfully", self.param.name);
        Ok(())
    }

    /// Detach the hook
    pub fn detach(&mut self) {
        if let Some(detour) = self.detour.take() {
            unsafe {
                let _ = detour.disable();
            }
            info!("Hook '{}' detached", self.param.name);
        }
    }

    /// Send extracted text to the dialogue system
    fn send(&self, context: &HookContext) {
        // Get text address
        let text_addr = self.param.get_text_address(context.base);
        if text_addr == 0 {
            return;
        }

        // Get text length
        let text_length = self.param.get_text_length(context.base, text_addr);
        if text_length == 0 || text_length > 10000 {
            return;
        }

        // Read text bytes
        let text_bytes = unsafe {
            std::slice::from_raw_parts(text_addr as *const u8, text_length)
        };

        // Get context for deduplication
        let context_id = if let Some(ctx_addr) = self.param.get_context_address(context.base) {
            format!("{}:0x{:x}", self.param.name, ctx_addr)
        } else {
            self.param.name.clone()
        };

        // Push each byte to the dialogue system
        for &byte in text_bytes {
            dialogue::Sink::push(
                context_id.clone(),
                self.param.encoding.as_str().to_string(),
                byte,
            );
        }

        debug!(
            "Extracted {} bytes from hook '{}' at 0x{:x}",
            text_length, self.param.name, text_addr
        );
    }
}

impl Drop for Hook {
    fn drop(&mut self) {
        self.detach();
    }
}

/// Hook handler function - called when a hooked function is invoked
fn hook_handler(base: usize, param: &HookParam) {
    let context = HookContext::new(base);
    
    // Create a temporary hook to call send
    // In a real implementation, we would have a better way to access the Hook instance
    let hook = Hook {
        param: Arc::new(param.clone()),
        detour: None,
    };
    
    hook.send(&context);
}

/// Hook manager - manages multiple hooks
pub struct HookManager {
    hooks: Vec<Hook>,
}

impl HookManager {
    /// Create a new hook manager
    pub fn new() -> Self {
        Self { hooks: Vec::new() }
    }

    /// Add a hook
    pub fn add_hook(&mut self, param: HookParam) {
        self.hooks.push(Hook::new(param));
    }

    /// Attach all hooks
    pub fn attach_all(&mut self) -> Result<()> {
        info!("Attaching {} hooks", self.hooks.len());
        
        for hook in &mut self.hooks {
            if let Err(e) = hook.attach() {
                error!("Failed to attach hook '{}': {}", hook.name(), e);
            }
        }
        
        Ok(())
    }

    /// Detach all hooks
    pub fn detach_all(&mut self) {
        info!("Detaching all hooks");
        
        for hook in &mut self.hooks {
            hook.detach();
        }
    }
}

impl Drop for HookManager {
    fn drop(&mut self) {
        self.detach_all();
    }
}