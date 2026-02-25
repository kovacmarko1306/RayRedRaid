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

# raygui (header-only, in ../../external/raygui/src/ relative to top-level)
get_filename_component(RAYGUI_DIR "${CMAKE_SOURCE_DIR}/../external/raygui/src" ABSOLUTE)
if(EXISTS "${RAYGUI_DIR}/raygui.h")
    target_include_directories(RayRedRaid PRIVATE "${RAYGUI_DIR}")
else()
    message(FATAL_ERROR "raygui.h not found at: ${RAYGUI_DIR}")
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
        /Zc:strictStrings-
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

# Install helpers for Linux desktops (do not affect Windows builds)
if(UNIX)
    # Install executable to standard bin dir
    install(TARGETS RayRedRaid RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    # Install icon (we ship redball.ico in source tree)
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/redball.ico
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/apps)

    # Configure .desktop file and install
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/rayredraid.desktop.in
                   ${CMAKE_CURRENT_BINARY_DIR}/rayredraid.desktop @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rayredraid.desktop
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
endif()