# RayRedRaid - Linux Build Guide

## Quick Start

```bash
# 1. Clone or download the project
cd ~/projects/RayRedRaid

# 2. Make build scripts executable (only needed once)
chmod +x BUILD.sh
chmod +x SETUP_LINUX.sh

# 3. Install dependencies (one-time setup)
./SETUP_LINUX.sh

# 4. Build the project
./BUILD.sh
```

## Step-by-Step Instructions

### 1. Install Dependencies

**Option A: Automatic (Recommended)**
```bash
./SETUP_LINUX.sh
```

**Option B: Manual Installation**
```bash
# Update package list
sudo apt-get update

# Install build tools
sudo apt-get install -y cmake build-essential pkg-config

# Install graphics libraries
sudo apt-get install -y \
    libgl1-mesa-dev \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libasound2-dev

# Install raylib (choose one)

# From system packages (if available):
sudo apt-get install -y libraylib-dev libglfw3-dev

# OR from source (recommended):
git clone https://github.com/raysan5/raylib.git
cd raylib/src
make
sudo make install
cd ../..
```

### 2. Build the Project

```bash
# Make scripts executable (first time only)
chmod +x BUILD.sh
chmod +x SETUP_LINUX.sh

# Release build (recommended)
./BUILD.sh

# Debug build
./BUILD.sh debug

# Clean and rebuild
./BUILD.sh clean
./BUILD.sh
```

### 3. Run the Game

```bash
# Run from build directory
./build/RayRedRaid/RayRedRaid

# Or run with full path
./build/RayRedRaid/Release/RayRedRaid
```

## Troubleshooting

### CMake not found
```bash
sudo apt-get install cmake
cmake --version
```

### Build tools missing
```bash
sudo apt-get install build-essential
gcc --version
```

### raylib not found
```bash
# Check if raylib is installed
pkg-config --modversion raylib

# If not found, install from source:
git clone https://github.com/raysan5/raylib.git
cd raylib/src && make && sudo make install
cd ../..
```

### Permission denied on BUILD.sh
```bash
chmod +x BUILD.sh
./BUILD.sh
```

### Cannot find GL library
```bash
sudo apt-get install libgl1-mesa-dev
```

### Build fails with missing headers
```bash
# Install all graphics dev packages
sudo apt-get install -y \
    libgl1-mesa-dev \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libasound2-dev
```

## Advanced

### Using vcpkg on Linux

If you have vcpkg configured:
```bash
# Set vcpkg toolchain path and rebuild
export VCPKG_ROOT=$HOME/vcpkg
./BUILD.sh clean
./BUILD.sh
```

### Build with Ninja instead of Make
```bash
sudo apt-get install ninja-build
cd build
cmake .. -G Ninja
ninja
cd ..
```

### Enable optimizations
```bash
# Edit BUILD.sh and change:
# CONFIG="Release" to use -O3 optimizations
./BUILD.sh
```

## System Requirements

- **OS**: Ubuntu 20.04 LTS or later (Debian 11+)
- **CPU**: Any 64-bit processor
- **RAM**: 2GB minimum, 4GB recommended
- **Storage**: 500MB for build files

## Tested On

- Ubuntu 20.04 LTS
- Ubuntu 22.04 LTS  
- Ubuntu 24.04 LTS
- Debian 11
- Debian 12

## Notes

- raylib is linked **dynamically** on Linux (unlike Windows static build)
- The executable may be larger on first run due to runtime library loading
- For production deployment, consider building raylib as static library

Enjoy! 🎮
