# RayRedRaid Build System

This project supports building on **Windows** and **Linux** with automated build scripts.

## 🪟 Windows

### Quick Start
```powershell
.\BUILD.ps1              # Release (static, 1.48 MB)
.\BUILD.ps1 -Debug       # Debug  
.\BUILD.ps1 -Clean       # Clean
```

Or with Batch:
```cmd
BUILD.bat                # Release
BUILD.bat Debug          # Debug
BUILD.bat Clean          # Clean
```

See [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md) for detailed Windows setup.

---

## 🐧 Linux / Ubuntu

### Quick Start
```bash
chmod +x BUILD.sh SETUP_LINUX.sh    # Make executable (one-time)
./SETUP_LINUX.sh                     # Install dependencies
./BUILD.sh                           # Release build
```

Or manual:
```bash
chmod +x BUILD.sh
./BUILD.sh                           # Release
./BUILD.sh debug                     # Debug
./BUILD.sh clean                     # Clean
```

See [LINUX_BUILD_GUIDE.md](LINUX_BUILD_GUIDE.md) for detailed Linux setup.

---

## 📋 Available Scripts

| Script | Platform | Purpose |
|--------|----------|---------|
| `BUILD.bat` | Windows (CMD) | Batch build script |
| `BUILD.ps1` | Windows (PowerShell) | PowerShell build script |
| `BUILD.sh` | Linux/Mac | Bash build script |
| `SETUP_LINUX.sh` | Linux | Install Linux dependencies |

---

## 🎯 Build Options

All scripts support these operations:

```bash
# Release build (default)
./BUILD.sh                # Linux/Mac
.\BUILD.ps1               # Windows PowerShell

# Debug build
./BUILD.sh debug          # Linux/Mac
.\BUILD.ps1 -Debug        # Windows PowerShell
BUILD.bat Debug           # Windows CMD

# Clean build directory
./BUILD.sh clean          # Linux/Mac
.\BUILD.ps1 -Clean        # Windows PowerShell
BUILD.bat Clean           # Windows CMD
```

---

## 📦 Output

### Windows
- **Release**: `build\RayRedRaid\Release\RayRedRaid.exe` (1.48 MB static)
- **Debug**: `build\RayRedRaid\Debug\RayRedRaid.exe`

### Linux
- **Release**: `build/RayRedRaid/RayRedRaid` (dynamically linked)
- **Debug**: `build/RayRedRaid/RayRedRaid`

---

## 🔧 Requirements

### Windows
- Visual Studio 2026 Community/Professional
- CMake 3.10+
- vcpkg with raylib, glfw3

### Linux
- CMake 3.10+
- GCC/Clang with C++20 support
- raylib (system or vcpkg)
- OpenGL and development headers

---

## 🚀 First Time Setup

### Windows
1. Ensure Visual Studio 2026 and vcpkg are installed
2. Run `.\BUILD.ps1` or `BUILD.bat`
3. Find executable in `build\RayRedRaid\Release\`

### Linux
1. Run `./SETUP_LINUX.sh` to install dependencies
2. Run `./BUILD.sh` to build
3. Run executable: `./build/RayRedRaid/RayRedRaid`

---

## 📖 More Information

- **Windows Users**: See [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md)
- **Linux Users**: See [LINUX_BUILD_GUIDE.md](LINUX_BUILD_GUIDE.md)
- **CMake Configuration**: See `CMakePresets.json` and `CMakeLists.txt`

---

## 🆘 Troubleshooting

**Windows**: CMake not found
- Add CMake to PATH or install from https://cmake.org

**Linux**: Build fails
- Run `./SETUP_LINUX.sh` first to install all dependencies
- Check [LINUX_BUILD_GUIDE.md](LINUX_BUILD_GUIDE.md) troubleshooting section

**Both**: Clean rebuild
- Windows: `BUILD.bat Clean` or `.\BUILD.ps1 -Clean`
- Linux: `./BUILD.sh clean`

---

## 🎮 Running the Game

```powershell
# Windows PowerShell
.\build\RayRedRaid\Release\RayRedRaid.exe

# Windows CMD
build\RayRedRaid\Release\RayRedRaid.exe
```

```bash
# Linux/Mac
./build/RayRedRaid/RayRedRaid
```

---

## 💡 Tips

- **Faster builds on Linux**: Scripts use `make -j$(nproc)` for parallel compilation
- **Visual Studio**: Use CMake integration (File → Open Folder)
- **Release builds recommended**: Windows uses static linking, Linux uses system libraries
- **Debug symbols**: Keep Debug builds for development and testing

Enjoy! 🚀
