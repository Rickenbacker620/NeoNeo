// PC (Generic) engine adapter - hooks common Windows text functions
// Based on src/engines/PC/engine.cpp

use super::EngineImpl;
use crate::hook::HookManager;
use crate::hook_param::{Encoding, HookParam};
use anyhow::Result;
use tracing::info;
use windows::Win32::System::LibraryLoader::GetModuleHandleW;

pub struct PCEngine {
    name: String,
}

impl PCEngine {
    /// Try to match this engine (always matches as fallback)
    pub fn try_match() -> Option<Box<dyn EngineImpl>> {
        // PC engine is the fallback - it always matches
        info!("Using generic PC engine (fallback)");
        Some(Box::new(PCEngine {
            name: "PC".to_string(),
        }))
    }

    /// Get function address from a DLL
    fn get_function_address(dll_name: &str, function_name: &str) -> Option<usize> {
        unsafe {
            // Convert to wide string
            let dll_wide: Vec<u16> = dll_name
                .encode_utf16()
                .chain(std::iter::once(0))
                .collect();

            if let Ok(module) = GetModuleHandleW(windows::core::PCWSTR(dll_wide.as_ptr())) {
                // Convert function name to PCSTR
                let func_cstr = std::ffi::CString::new(function_name).ok()?;
                if let Some(addr) = windows::Win32::System::LibraryLoader::GetProcAddress(
                    module,
                    windows::core::PCSTR(func_cstr.as_ptr() as *const u8),
                ) {
                    return Some(addr as usize);
                }
            }
        }
        None
    }
}

impl EngineImpl for PCEngine {
    fn name(&self) -> &str {
        &self.name
    }

    fn attach_hooks(&self) -> Result<()> {
        info!("Attaching PC (generic) hooks");

        let mut manager = HookManager::new();

        // Hook GetGlyphOutlineW (most common for Japanese text)
        if let Some(addr) = Self::get_function_address("gdi32.dll", "GetGlyphOutlineW") {
            let hook_param = HookParam::new(
                "PC:GetGlyphOutlineW",
                addr,
                Encoding::UTF16,
            )
            .with_text_offset(0x8); // Second parameter (uChar)

            manager.add_hook(hook_param);
            info!("Added hook for GetGlyphOutlineW at 0x{:x}", addr);
        }

        // Hook TextOutW
        if let Some(addr) = Self::get_function_address("gdi32.dll", "TextOutW") {
            let hook_param = HookParam::new(
                "PC:TextOutW",
                addr,
                Encoding::UTF16,
            )
            .with_text_offset(0x10) // Fourth parameter (lpString)
            .with_length_offset(0x14); // Fifth parameter (c)

            manager.add_hook(hook_param);
            info!("Added hook for TextOutW at 0x{:x}", addr);
        }

        // Hook ExtTextOutW
        if let Some(addr) = Self::get_function_address("gdi32.dll", "ExtTextOutW") {
            let hook_param = HookParam::new(
                "PC:ExtTextOutW",
                addr,
                Encoding::UTF16,
            )
            .with_text_offset(0x18) // Sixth parameter (lpString)
            .with_length_offset(0x1c); // Seventh parameter (c)

            manager.add_hook(hook_param);
            info!("Added hook for ExtTextOutW at 0x{:x}", addr);
        }

        // Hook DrawTextW
        if let Some(addr) = Self::get_function_address("user32.dll", "DrawTextW") {
            let hook_param = HookParam::new(
                "PC:DrawTextW",
                addr,
                Encoding::UTF16,
            )
            .with_text_offset(0x8) // Second parameter (lpchText)
            .with_length_offset(0xc); // Third parameter (cchText)

            manager.add_hook(hook_param);
            info!("Added hook for DrawTextW at 0x{:x}", addr);
        }

        // Hook GetTextExtentPoint32W
        if let Some(addr) = Self::get_function_address("gdi32.dll", "GetTextExtentPoint32W") {
            let hook_param = HookParam::new(
                "PC:GetTextExtentPoint32W",
                addr,
                Encoding::UTF16,
            )
            .with_text_offset(0x8) // Second parameter (lpString)
            .with_length_offset(0xc); // Third parameter (c)

            manager.add_hook(hook_param);
            info!("Added hook for GetTextExtentPoint32W at 0x{:x}", addr);
        }

        manager.attach_all()?;

        info!("PC hooks attached successfully");
        Ok(())
    }
}