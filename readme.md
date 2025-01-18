Building on Linux
=================

1. Install Required Tools:
   sudo apt install build-essential gcc g++ clang cmake

2. Install vcpkg:
   Clone vcpkg from GitHub:
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.sh -disableMetrics

3. Integrate vcpkg:
   ./vcpkg integrate install

4. Install Dependencies:
   ./vcpkg install glad imgui glfw3

5. Configure with CMake:
   Run this command in the project root:
   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg.cmake

6. Build the Project:
   make -C build

7. Run the Executable:
   ./gterm
