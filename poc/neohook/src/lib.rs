// NeoNeo Hook DLL
// This DLL is injected into game processes to hook text rendering functions
// Based on the original C++ implementation in src/main.cpp

use std::ffi::c_void;
use tracing::info;
use windows::core::PCSTR;
use windows::Win32::Foundation::{BOOL, HINSTANCE};
use windows::Win32::System::Console::{AllocConsole, SetConsoleOutputCP};
use windows::Win32::System::SystemServices::{
    DLL_PROCESS_ATTACH, DLL_PROCESS_DETACH,
};

mod common;
mod dialogue;
mod engine;
mod hook;
mod hook_param;
mod server;

use engine::Engine;
use server::NeoServer;

/// DLL entry point - called when the DLL is loaded/unloaded
/// This is the Rust equivalent of DllMain in C++
#[no_mangle]
#[allow(non_snake_case)]
pub extern "system" fn DllMain(
    _hinst_dll: HINSTANCE,
    fdw_reason: u32,
    _lpv_reserved: *mut c_void,
) -> BOOL {
    match fdw_reason {
        DLL_PROCESS_ATTACH => {
            // Initialize console for debugging output
            unsafe {
                let _ = AllocConsole();
                let _ = SetConsoleOutputCP(65001); // UTF-8
            }

            // Initialize logging
            tracing_subscriber::fmt()
                .with_max_level(tracing::Level::INFO)
                .init();

            info!("NeoNeo Hook DLL loaded");

            // Initialize the hook system
            std::thread::spawn(|| {
                if let Err(e) = initialize_hooks() {
                    tracing::error!("Failed to initialize hooks: {}", e);
                }
            });
        }
        DLL_PROCESS_DETACH => {
            info!("NeoNeo Hook DLL unloaded");
            // Cleanup is handled automatically by Rust's Drop trait
        }
        _ => {}
    }

    BOOL(1) // TRUE
}

/// Initialize the hook system
fn initialize_hooks() -> anyhow::Result<()> {
    info!("Initializing hook system...");

    // Create the output server (ZMQ publisher)
    let server = NeoServer::new()?;
    
    // Initialize the dialogue sink with 500ms flush timeout
    dialogue::Sink::init(Box::new(server), 500);

    // Deduce the game engine type and attach hooks
    if let Some(engine) = Engine::deduce_engine_type() {
        info!("Detected engine: {}", engine.name());
        engine.attach_hooks()?;
    } else {
        tracing::warn!("Could not detect game engine type");
    }

    info!("Hook system initialized successfully");
    Ok(())
}