# CMakeList.txt : CMake project for gTerm, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(gTerm)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect operating system for vcpkg toolchain file
if (WIN32)
    set(CMAKE_TOOLCHAIN_FILE "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
elseif (UNIX)
    set(CMAKE_TOOLCHAIN_FILE "/home/mac/dev/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()


# Add source to this project's executable.
add_executable (gTerm "gTerm.cpp" "gTerm.h" "terminal.cpp" "terminal.h"  "terminal_output.cpp" "terminal_output.h" "DebugMenu.cpp" "DebugMenu.h" "mainMenu.cpp" "mainMenu.h")

# TODO: Add tests and install targets if needed.
find_package(glad CONFIG REQUIRED)
target_link_libraries(gTerm PRIVATE glad::glad)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(gTerm PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(gTerm PRIVATE imgui::imgui)

#testing here - Icon support loading -Fucking annoying, cant get this to access the stb_image.h without CRASHING. FUCK!
# Add the include directory for stb (header-only library)
#message(STATUS "VCPKG_INSTALLED_DIR: ${VCPKG_INSTALLED_DIR}")
#include_directories(${VCPKG_INSTALLED_DIR}/include)
#include_directories("C:/dev/vcpkg/installed/x64-windows/include")
#target_include_directories(gTerm PRIVATE "C:/dev/vcpkg/installed/x64-windows/include")
#find_package(stb CONFIG REQUIRED)
#target_link_libraries(gTerm PRIVATE stb::stb)
#end testing

# Specify the path to the font file
set(FONT_FILE_PATH "${CMAKE_SOURCE_DIR}/res/arial.ttf")
# Pass the path to your code as a preprocessor definition
add_definitions(-DFONT_FILE_PATH=\"${FONT_FILE_PATH}\")

#specify icon location
set(ICON_FILE_PATH "${CMAKE_SOURCE_DIR}/res/icon.png")
# Pass the path to your code as a preprocessor definition
add_definitions(-DICON_FILE_PATH=\"${ICON_FILE_PATH}\")
