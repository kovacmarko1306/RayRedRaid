#!/bin/bash
# RayRedRaid Linux Build Setup
# Installs required dependencies for building on Ubuntu/Debian

set -e

echo "🔧 RayRedRaid Linux Build Setup"
echo ""

# Check if running on Linux
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "❌ This script is for Linux only!"
    exit 1
fi

# Update package list
echo "📦 Updating package list..."
sudo apt-get update

# Install build tools
echo "📦 Installing build tools..."
sudo apt-get install -y \
    cmake \
    build-essential \
    pkg-config \
    git

# Install raylib dependencies
echo "📦 Installing raylib dependencies..."
sudo apt-get install -y \
    libgl1-mesa-dev \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libasound2-dev \
    libpulse-dev

# Option 1: Install raylib from system packages (if available)
if apt-cache show libraylib-dev &>/dev/null; then
    echo "📦 Installing raylib from system packages..."
    sudo apt-get install -y libraylib-dev libglfw3-dev
else
    echo "⚠️  libraylib-dev not found in system packages"
    echo "📝 You have two options:"
    echo "   1. Install from source (recommended):"
    echo "      git clone https://github.com/raysan5/raylib.git"
    echo "      cd raylib/src && make && sudo make install"
    echo ""
    echo "   2. Install via vcpkg (if you have it configured):"
    echo "      vcpkg install raylib"
    echo ""
fi

echo ""
echo "✅ Setup complete!"
echo ""
echo "Next step: Run ./BUILD.sh to build the project"
echo ""
