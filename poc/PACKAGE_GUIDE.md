# 打包指南

## 方法 1: 使用 Git 打包（推荐）

如果你使用 Git，这是最简单的方法：

```bash
# 在项目根目录（NeoNeo）
git add poc/
git commit -m "Add Rust POC implementation"

# 创建压缩包（只包含 poc 文件夹）
git archive --format=zip --output=neoneo-rust-poc.zip HEAD:poc
```

## 方法 2: 使用 PowerShell 压缩

```powershell
# 在 NeoNeo 目录下
Compress-Archive -Path poc -DestinationPath neoneo-rust-poc.zip
```

## 方法 3: 使用 7-Zip（如果已安装）

```bash
# 在 NeoNeo 目录下
7z a -tzip neoneo-rust-poc.zip poc
```

## 方法 4: 使用 tar（Git Bash 或 WSL）

```bash
# 在 NeoNeo 目录下
tar -czf neoneo-rust-poc.tar.gz poc/
```

## 打包前清理（可选）

如果已经构建过，建议先清理构建产物以减小包大小：

```bash
cd poc
cargo clean
cd ..
```

然后再打包。

## 打包内容

打包后的文件应该包含：

```
neoneo-rust-poc.zip
└── poc/
    ├── Cargo.toml
    ├── README.md
    ├── BUILD_GUIDE.md
    ├── PACKAGE_GUIDE.md
    ├── .gitignore
    ├── injector/
    │   ├── Cargo.toml
    │   └── src/
    │       └── main.rs
    └── neohook/
        ├── Cargo.toml
        └── src/
            ├── lib.rs
            ├── common.rs
            ├── hook_param.rs
            ├── hook.rs
            ├── dialogue.rs
            ├── server.rs
            └── engine/
                ├── mod.rs
                ├── kirikiri.rs
                └── pc.rs
```

## 解压和使用

接收者解压后：

```bash
# 解压
unzip neoneo-rust-poc.zip
# 或
tar -xzf neoneo-rust-poc.tar.gz

# 进入目录
cd poc

# 构建
cargo build --release
```

## 分享二进制文件

如果要分享已编译的二进制文件：

```bash
# 先构建
cd poc
cargo build --release

# 打包二进制文件
cd target/release
# Windows
Compress-Archive -Path injector.exe,neohook.dll -DestinationPath ../../neoneo-rust-binaries.zip

# 或使用 7-Zip
7z a ../../neoneo-rust-binaries.zip injector.exe neohook.dll
```

## 完整打包（源码 + 二进制）

```bash
# 在 NeoNeo 目录
# 1. 构建
cd poc
cargo build --release

# 2. 创建打包目录
cd ..
mkdir neoneo-rust-release
cp -r poc neoneo-rust-release/source
mkdir neoneo-rust-release/binaries
cp poc/target/release/injector.exe neoneo-rust-release/binaries/
cp poc/target/release/neohook.dll neoneo-rust-release/binaries/

# 3. 打包
Compress-Archive -Path neoneo-rust-release -DestinationPath neoneo-rust-complete.zip
```

## 注意事项

1. **不要打包 `target/` 目录** - 它很大且可以重新构建
2. **包含 `Cargo.lock`** - 如果想要可重现的构建
3. **包含文档** - README.md 和 BUILD_GUIDE.md 很重要
4. **检查 `.gitignore`** - 确保不包含不必要的文件