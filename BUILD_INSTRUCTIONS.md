# RayRedRaid - Build Instructions

This document provides build instructions for both Windows and Linux/Ubuntu.

## 🪟 Windows

### Prerequisites
- Visual Studio 2026 (Community/Professional)
- CMake 3.10+
- vcpkg with raylib and glfw3 packages

### Build with Script (Recommended)
```powershell
# Release build (static linking)
.\BUILD.ps1

# Debug build
.\BUILD.ps1 -Debug

# Clean build directory
.\BUILD.ps1 -Clean
```

### Or with Batch Script
```cmd
:: Release build (static linking)
BUILD.bat

:: Debug build
BUILD.bat Debug

:: Clean build directory
BUILD.bat Clean
```

### Manual Build
```powershell
cd build
cmake .. -G "Visual Studio 18 2026" -A x64 `
    -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake `
    -DVCPKG_TARGET_TRIPLET=x64-windows-static
cmake --build . --config Release
cd ..
```

### Build in Visual Studio
1. File → Open → Folder → Select project directory
2. Wait for CMake to detect presets
3. Select **x64 Release (vcpkg Static)** from the preset dropdown
4. Build → Build All (Ctrl+Shift+B)

---

## 🐧 Linux / Ubuntu

### Prerequisites Installation
```bash
# Install all dependencies at once
./BUILD.sh install-deps

# Or manually:
sudo apt-get update
sudo apt-get install -y \
    cmake \
    build-essential \
    libgl1-mesa-dev \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libasound2-dev
```

### Build with Script (Recommended)
```bash
# Release build (recommended)
./BUILD.sh

# Debug build
./BUILD.sh debug

# Clean build directory
./BUILD.sh clean

# Install dependencies first time
./BUILD.sh install-deps
```

### Manual Build
```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j$(nproc)
cd ..
```

### Run the Game
```bash
./build/RayRedRaid/RayRedRaid
```

---

## 📋 CMake Configuration

The project uses CMakePresets.json for configuration management.

### Available Presets (Windows)
- **x64-debug** - Debug configuration with dynamic linking
- **x64-release** - Release configuration with static linking (fully self-contained)

### Available Toolchains
- vcpkg (Windows and Linux if configured)
- System libraries (Linux with apt)

---

## 🔧 Troubleshooting

### Windows
**Error: "Invalid parent name in preset"**
- Delete `build` folder and regenerate CMakePresets.json
- Ensure Visual Studio 2026 is installed

**CMake not found**
- Add CMake to PATH or install from https://cmake.org

### Linux/Ubuntu
**CMake configuration fails**
```bash
# Reinstall CMake
sudo apt-get install --reinstall cmake

# Or build from source:
sudo apt-get install curl
curl https://cmake.org/files/latest/cmake-linux.tar.gz | tar xz
sudo mv cmake-*/bin/* /usr/local/bin/
```

**Missing libraries**
```bash
# Run dependency installer
./BUILD.sh install-deps

# Or install specific packages:
sudo apt-get install libraylib-dev libglfw3-dev
```

**Build fails with permission denied**
```bash
# Make script executable
chmod +x BUILD.sh
./BUILD.sh
```

---

## 📦 Output

### Windows
- **Release**: `build\RayRedRaid\Release\RayRedRaid.exe` (~1.48 MB, fully static)
- **Debug**: `build\RayRedRaid\Debug\RayRedRaid.exe`

### Linux
- **Release**: `build/RayRedRaid/Release/RayRedRaid` or `build/RayRedRaid/RayRedRaid`
- **Debug**: `build/RayRedRaid/Debug/RayRedRaid` or `build/RayRedRaid/RayRedRaid`

---

## 🚀 Quick Start

### Windows
```powershell
.\BUILD.ps1
.\build\RayRedRaid\Release\RayRedRaid.exe
```

### Linux
```bash
./BUILD.sh
./build/RayRedRaid/RayRedRaid
```

Enjoy! 🎮
