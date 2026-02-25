#!/bin/bash
# RayRedRaid Build Script for Ubuntu/Linux
# Usage:
#   ./BUILD.sh              - Release build
#   ./BUILD.sh debug        - Debug build
#   ./BUILD.sh clean        - Clean build directory
#   ./BUILD.sh install-deps - Install system dependencies

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

CONFIG="Release"
BUILD_DIR="build"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
EXTERNAL_DIR="$SCRIPT_DIR/../external"

if [ "$1" == "debug" ]; then
    CONFIG="Debug"
elif [ "$1" == "clean" ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    echo "Done"
    exit 0
elif [ "$1" == "install-deps" ]; then
    echo "Installing system dependencies (raylib builds from source in ../external/raylib)..."
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
    echo -e "${GREEN}Done. Run ./BUILD.sh to build.${NC}"
    exit 0
elif [ -n "$1" ]; then
    echo "Unknown argument: $1"
    echo "Usage: $0 [debug|clean|install-deps]"
    exit 1
fi

echo ""
echo "=================================================="
echo "  RayRedRaid Build Script (Linux)"
echo "=================================================="
echo "Configuration: $CONFIG"
echo ""

# Check prerequisites
check_command() {
    if ! command -v "$1" &> /dev/null; then
        echo -e "${RED}ERROR: $1 not found!${NC}"
        echo "Run: ./BUILD.sh install-deps"
        exit 1
    fi
}
check_command "cmake"
check_command "gcc"

# Check external/raylib exists
if [ ! -f "$EXTERNAL_DIR/raylib/CMakeLists.txt" ]; then
    echo -e "${RED}ERROR: ../external/raylib not found!${NC}"
    echo "Expected: $EXTERNAL_DIR/raylib/CMakeLists.txt"
    echo "Run: git clone https://github.com/raysan5/raylib.git $EXTERNAL_DIR/raylib"
    exit 1
fi

mkdir -p "$BUILD_DIR"

echo "[1/2] CMake configuration..."
cd "$BUILD_DIR"

cmake .. \
    -DCMAKE_BUILD_TYPE="$CONFIG" \
    -G "Unix Makefiles"

echo ""
echo "[2/2] Build ($CONFIG)..."
cmake --build . --config "$CONFIG" -- -j$(nproc)

# Check for executable (Linux: no config subfolder)
EXE_PATH="./RayRedRaid/RayRedRaid"
if [ ! -f "$EXE_PATH" ]; then
    echo -e "${RED}ERROR: Executable not found at $EXE_PATH${NC}"
    cd ..
    exit 1
fi

SIZE=$(du -h "$EXE_PATH" | cut -f1)

echo ""
echo "=================================================="
echo "  BUILD SUCCESSFUL!"
echo "=================================================="
echo "Output: $SCRIPT_DIR/$BUILD_DIR/$EXE_PATH"
echo "Size:   $SIZE"
echo ""
echo "Run: cd $BUILD_DIR && $EXE_PATH"
echo ""

cd ..
