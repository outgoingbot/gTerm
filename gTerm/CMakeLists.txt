# CMakeList.txt : CMake project for gTerm, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(gTerm)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add vcpkg toolchain file (update the path to your vcpkg.cmake)
set(CMAKE_TOOLCHAIN_FILE "C:\\dev\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")


# Add source to this project's executable.
add_executable (gTerm "gTerm.cpp" "gTerm.h")

# TODO: Add tests and install targets if needed.
find_package(glad CONFIG REQUIRED)
target_link_libraries(gTerm PRIVATE glad::glad)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(gTerm PRIVATE glfw)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(gTerm PRIVATE imgui::imgui)