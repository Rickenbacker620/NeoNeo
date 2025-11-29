# Build Guide for NeoNeo Rust POC

## Prerequisites

### 1. Install Rust

Download and install Rust from [rustup.rs](https://rustup.rs/):

```bash
# Windows (PowerShell)
Invoke-WebRequest -Uri https://win.rustup.rs/x86_64 -OutFile rustup-init.exe
.\rustup-init.exe

# Or use the installer from https://rustup.rs/
```

After installation, verify:
```bash
rustc --version
cargo --version
```

### 2. Install Visual Studio Build Tools

Rust on Windows requires the Microsoft C++ build tools:

1. Download [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/)
2. Install with "Desktop development with C++" workload
3. Or install Visual Studio Community with C++ support

### 3. Install ZeroMQ (Optional)

The `zmq` crate will automatically download and build ZeroMQ, but you can install it manually:

```bash
# Using vcpkg
vcpkg install zeromq:x64-windows

# Or download from https://zeromq.org/download/
```

## Building the Project

### Quick Build

```bash
cd poc
cargo build --release
```

This will:
1. Download all dependencies
2. Compile the injector and neohook
3. Place binaries in `target/release/`

### Build Individual Components

```bash
# Build only the injector
cargo build --release -p injector

# Build only the hook DLL
cargo build --release -p neohook
```

### Development Build (Faster, with Debug Info)

```bash
cargo build
```

Binaries will be in `target/debug/`

## Build Output

After building, you'll find:

```
poc/target/release/
├── injector.exe        # The injector executable
├── neohook.dll         # The hook DLL
└── neohook.pdb         # Debug symbols (if debug build)
```

## Troubleshooting

### Issue: "linker 'link.exe' not found"

**Solution**: Install Visual Studio Build Tools with C++ support

### Issue: ZeroMQ build fails

**Solution**: 
1. Make sure you have CMake installed
2. Or install ZeroMQ manually via vcpkg
3. Set environment variable: `set LIBZMQ_PREFIX=C:\path\to\zeromq`

### Issue: "failed to run custom build command for `zmq-sys`"

**Solution**:
```bash
# Install CMake
winget install Kitware.CMake

# Or download from https://cmake.org/download/
```

### Issue: Slow compile times

**Solution**: Use a faster linker
```bash
# Install lld (LLVM linker)
cargo install -f cargo-binutils
rustup component add llvm-tools-preview

# Add to .cargo/config.toml:
[target.x86_64-pc-windows-msvc]
rustflags = ["-C", "link-arg=-fuse-ld=lld"]
```

## Running Tests

```bash
# Run all tests
cargo test

# Run tests with output
cargo test -- --nocapture

# Run specific test
cargo test test_name
```

## Cleaning Build Artifacts

```bash
# Remove all build artifacts
cargo clean

# Remove only release builds
cargo clean --release
```

## Cross-Compilation (Advanced)

To build for 32-bit Windows:

```bash
# Add 32-bit target
rustup target add i686-pc-windows-msvc

# Build for 32-bit
cargo build --release --target i686-pc-windows-msvc
```

## Build Optimization

### For Smaller Binary Size

Add to `Cargo.toml`:
```toml
[profile.release]
opt-level = "z"     # Optimize for size
lto = true          # Enable Link Time Optimization
codegen-units = 1   # Better optimization
strip = true        # Strip symbols
```

### For Faster Compilation

Add to `Cargo.toml`:
```toml
[profile.dev]
opt-level = 1       # Some optimization in dev builds
```

## IDE Setup

### Visual Studio Code

1. Install Rust extension: `rust-analyzer`
2. Install CodeLLDB for debugging
3. Open the `poc` folder in VS Code

### Visual Studio

1. Install "Rust for Visual Studio" extension
2. Open `poc/Cargo.toml` as a project

## Next Steps

After building successfully:

1. Copy `neohook.dll` to the same directory as `injector.exe`
2. Modify the target process name in `injector/src/main.rs`
3. Run `injector.exe` while the target game is running
4. Check the console output for logs

## Performance Tips

1. **Always use `--release` for production**: Debug builds are 10-100x slower
2. **Use `cargo check`** for faster syntax checking during development
3. **Use `cargo clippy`** for linting and suggestions
4. **Use `cargo fmt`** to format code automatically

## Getting Help

- Rust documentation: https://doc.rust-lang.org/
- Cargo book: https://doc.rust-lang.org/cargo/
- Windows crate docs: https://docs.rs/windows/
- Retour crate docs: https://docs.rs/retour/
- ZMQ crate docs: https://docs.rs/zmq/