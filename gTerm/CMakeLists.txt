cmake_minimum_required(VERSION 3.15)
project(gTerm)

# Use modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─── OS Detection ────────────────────────────────────────────────────────────────
if (WIN32)
    message(STATUS "Detected Windows OS")
    add_compile_definitions(IS_WINDOWS)
    set(SERIAL_SRC serial/RS232Comm.cpp)
    set(SERIAL_INC serial/RS232Comm.h)
elseif (UNIX)
    message(STATUS "Detected Unix/Linux OS")
    add_compile_definitions(IS_LINUX)
    set(SERIAL_SRC serial/LinuxSerialComm.cpp)
    set(SERIAL_INC serial/LinuxSerialComm.h)
endif()

# ─── Source Files ────────────────────────────────────────────────────────────────
add_executable(gTerm
    gTerm.cpp gTerm.h
    terminal.cpp terminal.h
    terminal_output.cpp terminal_output.h
    DebugMenu.cpp DebugMenu.h
    mainMenu.cpp mainMenu.h
    dataParser.cpp dataParser.h
    serial/serialManager.cpp serial/serialManager.h
    serial/virtualComm.h
    textselect.cpp textselect.hpp
    dataPlotter.cpp dataPlotter.h
    ${SERIAL_SRC}
    ${SERIAL_INC}
)

# ─── Vcpkg Packages ──────────────────────────────────────────────────────────────
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

target_link_libraries(gTerm PRIVATE
    glfw
    glad::glad
)

# ─── ImGui (manual integration) ──────────────────────────────────────────────────
add_library(imgui STATIC
    ${CMAKE_SOURCE_DIR}/external/imgui-1.92.5/imgui.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui-1.92.5/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui-1.92.5/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui-1.92.5/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui-1.92.5/imgui_demo.cpp

    ${CMAKE_SOURCE_DIR}/external/imgui-1.92.5/backends/imgui_impl_glfw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui-1.92.5/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${CMAKE_SOURCE_DIR}/external/imgui-1.92.5
    ${CMAKE_SOURCE_DIR}/external/imgui-1.92.5/backends
)

target_link_libraries(imgui PUBLIC glfw glad::glad)

# ─── ImPlot (manual build) ───────────────────────────────────────────────────────
add_library(implot STATIC
    ${CMAKE_SOURCE_DIR}/external/implot/implot.cpp
    ${CMAKE_SOURCE_DIR}/external/implot/implot_items.cpp
    # ${CMAKE_SOURCE_DIR}/external/implot/implot_demo.cpp
)

target_include_directories(implot PUBLIC
    ${CMAKE_SOURCE_DIR}/external/implot
    ${CMAKE_SOURCE_DIR}/external/imgui-1.92.5
)

target_link_libraries(implot PUBLIC imgui)

# Link ImGui + ImPlot into the executable
target_link_libraries(gTerm PRIVATE imgui implot)

# ─── Resource Paths as Defines ───────────────────────────────────────────────────
set(FONT_FILE_PATH "${CMAKE_SOURCE_DIR}/res/Glass_TTY_VT220.ttf")
add_definitions(-DFONT_FILE_PATH=\"${FONT_FILE_PATH}\")

set(ICON_FILE_PATH "${CMAKE_SOURCE_DIR}/res/icon.png")
add_definitions(-DICON_FILE_PATH=\"${ICON_FILE_PATH}\")

set(LOGO_FILE_PATH "${CMAKE_SOURCE_DIR}/res/logo.png")
add_definitions(-DLOGO_FILE_PATH=\"${LOGO_FILE_PATH}\")
