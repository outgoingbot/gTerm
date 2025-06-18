Building on Linux
=================

1. Install Required Tools:
   ```bash
   sudo apt install build-essential gcc g++ clang cmake libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev pkg-config
   ```
2. Install vcpkg:

   Clone vcpkg from GitHub into a workspace (like `/home/user/dev`):
   ```bash
   git clone https://github.com/microsoft/vcpkg.git
   ```
   ```bash
   cd vcpkg
   ```
   ```bash
   ./bootstrap-vcpkg.sh -disableMetrics
   ```

3. Integrate vcpkg:
   ```bash
   ./vcpkg integrate install
   ```
4. Install Dependencies:
   ```bash
   ./vcpkg install glad imgui glfw3
   ```

5. Clone gTerm and submodules
   
   ```bash
   cd /home/user/dev
   ```

   ```bash
   git --recurse-submodules clone https://github.com/outgoingbot/gTerm.git
   ```


5. Configure with CMake:

   ```bash
   cd /home/user/dev/gTerm
   ```
   
   Run this command in the project root: `cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg.cmake`
   ```bash
   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=/home/mac/dev/vcpkg/scripts/buildsystems/vcpkg.cmake
   ```
6. Build the Project:
   ```bash
   make -C build
   ```
7. Run the Executable:
   ```bash
   cd gTerm/build/gTerm
   ```
   ```bash
   ./gterm
   ```
