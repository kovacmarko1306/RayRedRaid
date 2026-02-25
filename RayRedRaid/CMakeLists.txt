# RayRedRaid CMakeLists - Cross-platform (Windows & Linux)

cmake_minimum_required(VERSION 3.21)

project(RayRedRaid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES RayRedRaid.cpp)

# Platform-specific resources (Windows only)
if(MSVC OR WIN32)
    list(APPEND SOURCES redball.rc)
endif()

add_executable(RayRedRaid ${SOURCES})

# Optional: raygui (header-only, in ../external/raygui/src/)
set(RAYGUI_DIR "${CMAKE_SOURCE_DIR}/../../external/raygui/src")
if(EXISTS "${RAYGUI_DIR}")
    target_include_directories(RayRedRaid PRIVATE "${RAYGUI_DIR}")
endif()

# Platform-specific linking
if(WIN32)
    target_link_libraries(RayRedRaid PRIVATE
        raylib
        winmm
        user32
        gdi32
        shell32
        opengl32
        ws2_32
        advapi32
        libcmt$<$<CONFIG:Debug>:d>
    )

    target_compile_options(RayRedRaid PRIVATE
        /D_CRT_SECURE_NO_WARNINGS
    )

    target_link_options(RayRedRaid PRIVATE
        /SUBSYSTEM:WINDOWS
        /ENTRY:mainCRTStartup
        /NODEFAULTLIB:MSVCRT
        /NODEFAULTLIB:LIBCMTD
    )
else()
    target_link_libraries(RayRedRaid PRIVATE
        raylib
        GL
        m
        pthread
    )

    target_compile_options(RayRedRaid PRIVATE
        -Wall
        -Wextra
    )
endif()