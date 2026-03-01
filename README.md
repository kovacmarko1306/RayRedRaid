# RayRedRaid

A cross-platform 2D arcade shooter built with **[raylib](https://www.raylib.com/)** and **[raygui](https://github.com/raysan5/raygui)** in modern C++20.

Shoot red targets, survive 30 seconds, rack up your score. Simple concept, embedded everything — no external asset files needed at runtime.

---

## Platform Support

| Platform | Status | Binary |
|----------|--------|--------|
| **Windows** (x64) | Fully supported | Static `.exe` (~1.5 MB, no DLL dependencies) |
| **Linux** (x64) | Fully supported | Dynamic binary + desktop launcher |
| **macOS** | Should work | Use `BUILD.sh` with Homebrew raylib |

---

## Gameplay

- **Objective**: Shoot as many red targets as possible within 30 seconds
- **Controls**:

| Key / Input | Action |
|-------------|--------|
| `WASD` / Arrow keys | Move player |
| Left Mouse / `Space` | Shoot |
| `N` | Nuke — destroy all targets (3 uses per session) |
| `Enter` | Restart after game over |

---

## Project Structure

```
RayRedRaid/
├── CMakeLists.txt              # Top-level CMake
├── CMakePresets.json           # VS/VSCode presets (x64-debug, x64-release)
├── BUILD.sh                    # Linux/macOS build script
├── BUILD.ps1                   # Windows PowerShell build script
├── BUILD.bat                   # Windows batch build script
├── SETUP_LINUX.sh              # Linux dependency installer
├── external/                   # External libraries (NOT in repo — see below)
│   ├── raylib/                 # raylib source
│   └── raygui/                 # raygui header-only library
└── RayRedRaid/
    ├── RayRedRaid.cpp          # Main game source
    ├── redball_icon.h          # Embedded window icon (32x32 RGBA)
    ├── redball.rc              # Windows icon resource
    ├── redball.ico             # Windows icon file
    ├── rayredraid.desktop.in   # Linux desktop entry template
    ├── images/
    │   ├── background.png      # Source background image
    │   └── background_embed.h  # Embedded image (auto-generated)
    └── sounds/
        ├── shoot_embed.h           # Embedded audio
        ├── explosion_embed.h       # Embedded audio
        ├── horn_embed.h            # Embedded audio
        └── thompson_hipodrom_horn_plus_embed.h  # Embedded audio
```

> **Note:** All assets (images, sounds, icon) are embedded directly into the binary as C header files. The game has **zero runtime asset dependencies** — a single executable is all you need.

---

## Building

### Prerequisites

#### External Libraries (required for all platforms)

Clone the following repositories into an `external/` directory **next to** the `RayRedRaid/` project folder:

```
parent-directory/
├── external/
│   ├── raylib/      ← https://github.com/raysan5/raylib
│   └── raygui/      ← https://github.com/raysan5/raygui
└── RayRedRaid/      ← this repository
```

```bash
# From the parent directory of RayRedRaid:
mkdir external && cd external
git clone https://github.com/raysan5/raylib.git
git clone https://github.com/raysan5/raygui.git
```

---

### Windows

**Requirements:**
- [Visual Studio 2022+](https://visualstudio.microsoft.com/) with "Desktop development with C++"
- [CMake 3.21+](https://cmake.org/download/) (usually included with Visual Studio)
- [vcpkg](https://vcpkg.io/) (optional, only if not using the `external/` folder approach)

**Build with PowerShell (recommended):**
```powershell
.\BUILD.ps1          # Release build (static, self-contained)
.\BUILD.ps1 -Debug   # Debug build
.\BUILD.ps1 -Clean   # Clean build directory
```

**Build with batch script:**
```bat
BUILD.bat
BUILD.bat Debug
BUILD.bat Clean
```

**Build manually with CMake:**
```powershell
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

**Output:** `build\RayRedRaid\Release\RayRedRaid.exe`

The release build links everything statically — no runtime DLLs required. Copy the `.exe` anywhere and run it.

**Visual Studio / VS Code:**
Open the root folder — CMake presets (`CMakePresets.json`) are already configured:
- `x64-debug` — debug build with dynamic linking
- `x64-release` — release build with full static linking

---

### Linux

**Install dependencies:**
```bash
chmod +x SETUP_LINUX.sh
./SETUP_LINUX.sh
```

This installs: `cmake`, `build-essential`, `pkg-config`, `libgl1-mesa-dev`, `libx11-dev`, `libxrandr-dev`, `libxinerama-dev`, `libxcursor-dev`, `libxi-dev`, `libasound2-dev`, `libpulse-dev`.

**Build:**
```bash
chmod +x BUILD.sh
./BUILD.sh           # Release build
./BUILD.sh debug     # Debug build
./BUILD.sh clean     # Clean build directory
```

**Output:** `build/RayRedRaid/RayRedRaid`

**Install system-wide (optional, adds desktop launcher):**
```bash
sudo cmake --install build --prefix /usr/local
```

This installs:
- Binary → `/usr/local/bin/RayRedRaid`
- Icon → `/usr/local/share/icons/hicolor/64x64/apps/redball.png`
- Desktop entry → `/usr/local/share/applications/rayredraid.desktop`

**Tested on:** Ubuntu 20.04, 22.04, 24.04 · Debian 11, 12

---

### macOS

Install dependencies with [Homebrew](https://brew.sh/):
```bash
brew install cmake raylib
```

Then use the Linux build script:
```bash
chmod +x BUILD.sh
./BUILD.sh
```

---

## Changing Sounds and Images

All assets are embedded in the binary as C header files. To change an asset, you replace the source file and regenerate the corresponding header.

### Generating Embed Headers

Use the `xxd` utility (available on Linux/macOS by default; on Windows install via [Git for Windows](https://git-scm.com/), WSL, or [xxd standalone port](https://sourceforge.net/projects/xxd-for-windows/)):

```bash
xxd -i input_file.wav > output_embed.h
```

Then manually edit the generated header to match the naming convention used in the existing headers (variable name, array length variable name).

**Example — replacing the shoot sound:**

```bash
# 1. Put your new sound file in the sounds/ directory
cp my_new_shoot.wav RayRedRaid/sounds/my_new_shoot.wav

# 2. Generate the embed header
xxd -i RayRedRaid/sounds/my_new_shoot.wav > RayRedRaid/sounds/shoot_embed.h

# 3. Open shoot_embed.h and rename the generated variable to match
#    what RayRedRaid.cpp expects (check the existing header for the name)
```

**Example — replacing the background image:**

```bash
# 1. Replace the source image
cp my_background.png RayRedRaid/images/background.png

# 2. Regenerate the embed header
xxd -i RayRedRaid/images/background.png > RayRedRaid/images/background_embed.h
```

After regenerating, rebuild the project. The new asset will be baked into the binary.

---

## Creating New Sounds

You can use free and open-source tools to create or modify sounds:

| Tool | Use Case | Platform |
|------|----------|----------|
| [Audacity](https://www.audacityteam.org/) | Record, edit, export WAV/OGG | Win/Lin/Mac |
| [sfxr / jsfxr](https://sfxr.me/) | Generate retro 8-bit sound effects online | Browser |
| [ChipTone](https://sfbgames.itch.io/chiptone) | Procedural sound effect generator | Browser |
| [LMMS](https://lmms.io/) | Full DAW for music composition | Win/Lin/Mac |
| [Bfxr](https://www.bfxr.net/) | Expanded sfxr for game sound effects | Browser |

**Recommended workflow:**
1. Generate or record your sound in Audacity
2. Export as **WAV** (PCM 16-bit, 44100 Hz, mono or stereo)
3. Optionally convert to **OGG** for smaller file size: `ffmpeg -i input.wav output.ogg`
4. Generate the embed header with `xxd -i`
5. Replace the include in `RayRedRaid.cpp` and rebuild

> raylib supports: WAV, OGG, MP3, FLAC, QOA

---

## Creating New Images

| Tool | Use Case | Platform |
|------|----------|----------|
| [GIMP](https://www.gimp.org/) | Full image editor, export PNG/JPG | Win/Lin/Mac |
| [Inkscape](https://inkscape.org/) | Vector graphics, export to PNG | Win/Lin/Mac |
| [Krita](https://krita.org/) | Pixel art and digital painting | Win/Lin/Mac |
| [Aseprite](https://www.aseprite.org/) | Pixel art and sprite animation | Win/Lin/Mac |
| [Piskel](https://www.piskelapp.com/) | Free online pixel art editor | Browser |
| [Libresprite](https://libresprite.github.io/) | Free Aseprite fork | Win/Lin/Mac |

**Recommended workflow:**
1. Create or edit your image in the tool of your choice
2. Export as **PNG** (preferred) or JPG
3. Keep in mind the game renders at **1800×900** pixels — size your background accordingly
4. Generate the embed header with `xxd -i`
5. Replace the include in `RayRedRaid.cpp` and rebuild

> raylib supports: PNG, BMP, TGA, JPG, GIF, QOI, PSD, HDR, KTX, ASTC, PVR, DDS

---

## Replacing the Window Icon

The window icon is embedded as a 32×32 RGBA pixel array in `redball_icon.h`.

**To replace it:**

1. Create a **32×32** PNG image (RGBA, transparent background recommended)
2. Convert to a raw RGBA C array. A simple Python helper:

```python
from PIL import Image

img = Image.open("my_icon.png").convert("RGBA").resize((32, 32))
pixels = list(img.getdata())
flat = [ch for px in pixels for ch in px]

with open("redball_icon.h", "w") as f:
    f.write("#pragma once\n")
    f.write(f"static unsigned char ICON_DATA[] = {{\n")
    f.write(", ".join(str(b) for b in flat))
    f.write("\n};\n")
    f.write("static int ICON_WIDTH = 32;\n")
    f.write("static int ICON_HEIGHT = 32;\n")
```

3. Check `RayRedRaid.cpp` for the exact variable names used and adjust accordingly

**On Windows** — also replace `redball.ico` for the taskbar/Explorer icon. Use [IcoFX](https://icofx.ro/) or convert with ImageMagick:
```bash
magick my_icon.png -resize 256x256 redball.ico
```

---

## Architecture Notes

- **No runtime assets** — everything embedded in the binary via `xxd -i` generated headers
- **Static linking on Windows** — release build links `libcmt` and all system libraries statically; the `.exe` runs on any Windows 10/11 machine without installers
- **Dynamic linking on Linux** — links against system raylib; install dependencies before distributing
- **C++20** throughout — uses `std::vector`, `std::string`, range algorithms
- **raygui** used for the start screen UI (username input, buttons)
- **CMake** is the single build system for all platforms; platform-specific scripts are thin wrappers

---

## Dependencies

| Library | Version | License | Purpose |
|---------|---------|---------|---------|
| [raylib](https://github.com/raysan5/raylib) | 5.x | zlib | Graphics, audio, input, windowing |
| [raygui](https://github.com/raysan5/raygui) | 4.x | zlib | Immediate-mode GUI (header-only) |
| [raymath](https://github.com/raysan5/raylib) | bundled | zlib | Vector/matrix math |

---

## License

This project is provided as-is. The bundled raylib and raygui libraries are licensed under the [zlib License](https://github.com/raysan5/raylib/blob/master/LICENSE).
