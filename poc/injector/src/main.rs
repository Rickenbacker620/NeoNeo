// DLL Injector for NeoNeo
// This program injects the neohook.dll into a target game process
// Based on the original C++ implementation in injector/main.cpp

use anyhow::{Context, Result};
use std::ffi::OsStr;
use std::os::windows::ffi::OsStrExt;
use std::path::PathBuf;
use std::thread;
use std::time::Duration;
use tracing::{error, info};
use windows::core::PCWSTR;
use windows::Win32::Foundation::{CloseHandle, BOOL, HANDLE, HMODULE};
use windows::Win32::System::Diagnostics::ToolHelp::{
    CreateToolhelp32Snapshot, Process32FirstW, Process32NextW, PROCESSENTRY32W, TH32CS_SNAPPROCESS,
};
use windows::Win32::System::LibraryLoader::GetModuleFileNameW;
use windows::Win32::System::Memory::{
    VirtualAllocEx, VirtualFreeEx, MEM_COMMIT, MEM_RELEASE, MEM_RESERVE, PAGE_READWRITE,
};
use windows::Win32::System::ProcessStatus::GetModuleFileNameExW;
use windows::Win32::System::Threading::{
    CreateRemoteThread, OpenProcess, WaitForSingleObject, INFINITE, PROCESS_ALL_ACCESS,
    PROCESS_QUERY_INFORMATION, PROCESS_VM_READ,
};

/// Get the filename of a module in the current process
fn get_module_filename(module: Option<HMODULE>) -> Result<PathBuf> {
    let mut buffer = vec![0u16; 260]; // MAX_PATH
    let len = unsafe { GetModuleFileNameW(module.unwrap_or_default(), &mut buffer) };

    if len == 0 {
        anyhow::bail!("Failed to get module filename");
    }

    let path = String::from_utf16_lossy(&buffer[..len as usize]);
    Ok(PathBuf::from(path))
}

/// Get the filename of a module in a specific process
fn get_module_filename_ex(process_id: u32, module: Option<HMODULE>) -> Result<PathBuf> {
    unsafe {
        let process = OpenProcess(
            PROCESS_VM_READ | PROCESS_QUERY_INFORMATION,
            BOOL(0),
            process_id,
        )
        .context("Failed to open process")?;

        let mut buffer = vec![0u16; 260]; // MAX_PATH
        let len = GetModuleFileNameExW(process, module.unwrap_or_default(), &mut buffer);

        CloseHandle(process)?;

        if len == 0 {
            anyhow::bail!("Failed to get module filename for process {}", process_id);
        }

        let path = String::from_utf16_lossy(&buffer[..len as usize]);
        Ok(PathBuf::from(path))
    }
}

/// Get process ID by process name
fn get_process_id_by_name(process_name: &str) -> Result<u32> {
    unsafe {
        let snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
            .context("Failed to create process snapshot")?;

        let mut process_entry = PROCESSENTRY32W {
            dwSize: std::mem::size_of::<PROCESSENTRY32W>() as u32,
            ..Default::default()
        };

        if !Process32FirstW(snapshot, &mut process_entry).as_bool() {
            CloseHandle(snapshot)?;
            anyhow::bail!("Failed to get first process");
        }

        loop {
            // Convert the process name from wide string
            let current_name = String::from_utf16_lossy(
                &process_entry.szExeFile[..process_entry
                    .szExeFile
                    .iter()
                    .position(|&c| c == 0)
                    .unwrap_or(process_entry.szExeFile.len())],
            );

            if current_name == process_name {
                let process_id = process_entry.th32ProcessID;
                CloseHandle(snapshot)?;
                return Ok(process_id);
            }

            if !Process32NextW(snapshot, &mut process_entry).as_bool() {
                break;
            }
        }

        CloseHandle(snapshot)?;
        anyhow::bail!("Process '{}' not found", process_name);
    }
}

/// Inject DLL into target process
fn inject_process(process_id: u32) -> Result<()> {
    info!("Injecting into process {}", process_id);

    // Don't inject into ourselves
    if process_id == unsafe { windows::Win32::System::Threading::GetCurrentProcessId() } {
        anyhow::bail!("Cannot inject into self");
    }

    unsafe {
        // Open the target process
        let process = OpenProcess(PROCESS_ALL_ACCESS, BOOL(0), process_id)
            .context("Failed to open target process")?;

        // Get the path to neohook.dll (in the same directory as the injector)
        let dll_path = get_module_filename(None)?
            .parent()
            .context("Failed to get parent directory")?
            .join("neohook.dll");

        info!("DLL path: {}", dll_path.display());

        // Convert path to wide string
        let dll_path_wide: Vec<u16> = OsStr::new(&dll_path)
            .encode_wide()
            .chain(std::iter::once(0))
            .collect();

        let dll_path_size = dll_path_wide.len() * std::mem::size_of::<u16>();

        // Allocate memory in the target process
        let remote_data = VirtualAllocEx(
            process,
            None,
            dll_path_size,
            MEM_RESERVE | MEM_COMMIT,
            PAGE_READWRITE,
        );

        if remote_data.is_null() {
            CloseHandle(process)?;
            anyhow::bail!("Failed to allocate memory in target process");
        }

        // Write the DLL path to the target process
        let mut bytes_written = 0;
        let write_result = windows::Win32::System::Diagnostics::Debug::WriteProcessMemory(
            process,
            remote_data,
            dll_path_wide.as_ptr() as *const _,
            dll_path_size,
            Some(&mut bytes_written),
        );

        if !write_result.as_bool() {
            VirtualFreeEx(process, remote_data, 0, MEM_RELEASE)?;
            CloseHandle(process)?;
            anyhow::bail!("Failed to write to process memory");
        }

        // Get LoadLibraryW address
        let kernel32 = windows::Win32::System::LibraryLoader::GetModuleHandleW(
            windows::core::w!("kernel32.dll"),
        )
        .context("Failed to get kernel32.dll handle")?;

        let load_library_addr = windows::Win32::System::LibraryLoader::GetProcAddress(
            kernel32,
            windows::core::s!("LoadLibraryW"),
        )
        .context("Failed to get LoadLibraryW address")?;

        // Create remote thread to load the DLL
        let thread = CreateRemoteThread(
            process,
            None,
            0,
            Some(std::mem::transmute(load_library_addr)),
            Some(remote_data),
            0,
            None,
        )
        .context("Failed to create remote thread")?;

        // Wait for the thread to finish
        WaitForSingleObject(thread, INFINITE);

        // Check if DLL was loaded successfully
        let mut exit_code = 0u32;
        if windows::Win32::System::Threading::GetExitCodeThread(thread, &mut exit_code).as_bool() {
            if exit_code == 0 {
                error!("Failed to load DLL. LoadLibraryW returned NULL");
            } else {
                info!("DLL loaded successfully");
            }
        } else {
            error!("Failed to get exit code of remote thread");
        }

        // Clean up
        VirtualFreeEx(process, remote_data, 0, MEM_RELEASE)?;
        CloseHandle(thread)?;
        CloseHandle(process)?;

        Ok(())
    }
}

fn main() -> Result<()> {
    // Initialize logging
    tracing_subscriber::fmt()
        .with_max_level(tracing::Level::INFO)
        .init();

    info!("NeoNeo Injector starting...");

    // Wait a bit before injecting (gives time to start the target process)
    thread::sleep(Duration::from_secs(3));

    // Find and inject into the target process
    // TODO: Make this configurable via command line arguments
    let target_process = "殻ノ少女 HD.exe";
    
    match get_process_id_by_name(target_process) {
        Ok(process_id) => {
            info!("Found process '{}' with PID: {}", target_process, process_id);
            inject_process(process_id)?;
            info!("Injection complete");
        }
        Err(e) => {
            error!("Failed to find process '{}': {}", target_process, e);
            return Err(e);
        }
    }

    Ok(())
}