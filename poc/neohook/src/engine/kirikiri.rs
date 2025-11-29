// KiriKiri engine adapter
// Based on src/engines/kirikiri/engine.cpp

use super::EngineImpl;
use crate::hook::HookManager;
use crate::hook_param::{Encoding, HookParam};
use anyhow::Result;
use tracing::info;
use windows::Win32::System::LibraryLoader::GetModuleHandleW;

pub struct KirikiriEngine {
    name: String,
}

impl KirikiriEngine {
    /// Try to match this engine
    pub fn try_match() -> Option<Box<dyn EngineImpl>> {
        // Check if KiriKiri-related modules are loaded
        unsafe {
            // KiriKiri games typically have these DLLs
            let kirikiri_dlls = [
                windows::core::w!("krkr.dll"),
                windows::core::w!("krkrz.dll"),
                windows::core::w!("krkrrel.dll"),
            ];

            for dll_name in &kirikiri_dlls {
                if GetModuleHandleW(*dll_name).is_ok() {
                    info!("Detected KiriKiri engine (found {})", dll_name.display());
                    return Some(Box::new(KirikiriEngine {
                        name: "KiriKiri".to_string(),
                    }));
                }
            }
        }

        None
    }
}

impl EngineImpl for KirikiriEngine {
    fn name(&self) -> &str {
        &self.name
    }

    fn attach_hooks(&self) -> Result<()> {
        info!("Attaching KiriKiri hooks");

        let mut manager = HookManager::new();

        // KiriKiri typically uses GetGlyphOutlineW for text rendering
        // This is a simplified version - the original C++ code has complex pattern matching
        // For a POC, we'll hook common Windows text functions
        
        // Hook GetGlyphOutlineW
        if let Ok(gdi32) = unsafe { GetModuleHandleW(windows::core::w!("gdi32.dll")) } {
            if let Some(addr) = unsafe {
                windows::Win32::System::LibraryLoader::GetProcAddress(
                    gdi32,
                    windows::core::s!("GetGlyphOutlineW"),
                )
            } {
                let hook_param = HookParam::new(
                    "KiriKiri:GetGlyphOutlineW",
                    addr as usize,
                    Encoding::UTF16,
                )
                .with_text_offset(0x8); // Second parameter (uChar)

                manager.add_hook(hook_param);
            }
        }

        manager.attach_all()?;

        info!("KiriKiri hooks attached successfully");
        Ok(())
    }
}