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
    dataParser.cpp
    serial/serialManager.cpp serial/serialManager.h
    serial/virtualComm.h
    ${SERIAL_SRC}
    ${SERIAL_INC}
)

# ─── Vcpkg Packages ──────────────────────────────────────────────────────────────
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

target_link_libraries(gTerm PRIVATE
    glfw
    glad::glad
    imgui::imgui
)

# ─── ImPlot (submodule, built manually) ──────────────────────────────────────────
add_library(implot STATIC
    ${CMAKE_SOURCE_DIR}/external/implot/implot.cpp
    ${CMAKE_SOURCE_DIR}/external/implot/implot_items.cpp
    # Optional demo support:
    # ${CMAKE_SOURCE_DIR}/external/implot/implot_demo.cpp
)

# Use ImGui target's include path so it works across platforms and modes
get_target_property(IMGUI_INCLUDE_DIR imgui::imgui INTERFACE_INCLUDE_DIRECTORIES)

# ImPlot needs its own headers plus ImGui’s headers
target_include_directories(implot PUBLIC
    ${CMAKE_SOURCE_DIR}/external/implot
    ${IMGUI_INCLUDE_DIR}
)

message(STATUS "ImPlot will look for ImGui headers in: ${IMGUI_INCLUDE_DIR}")

target_link_libraries(gTerm PRIVATE implot)

# ─── Resource Paths as Defines ───────────────────────────────────────────────────
set(FONT_FILE_PATH "${CMAKE_SOURCE_DIR}/res/arial.ttf")
add_definitions(-DFONT_FILE_PATH=\"${FONT_FILE_PATH}\")

set(ICON_FILE_PATH "${CMAKE_SOURCE_DIR}/res/icon.png")
add_definitions(-DICON_FILE_PATH=\"${ICON_FILE_PATH}\")

set(LOGO_FILE_PATH "${CMAKE_SOURCE_DIR}/res/logo.png")
add_definitions(-DLOGO_FILE_PATH=\"${LOGO_FILE_PATH}\")

# ─── (Optional) Debug notes and cleanup ──────────────────────────────────────────
# If stb_image or other headers crash, clean up how you include them
# You no longer need global include_directories() thanks to target_*
