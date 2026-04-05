<img src="https://github.com/outgoingbot/gTerm/blob/master/Screenshot.png" height="512" width="815" >


Building on Linux
=================

1. Install Required Tools:
    
    Debian
    ```bash
    sudo apt update
    sudo apt install build-essential cmake ninja-build pkg-config \
        libx11-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev \
        libglu1-mesa-dev libxrandr-dev
    ```
    
    Arch
    ```bash
    sudo pacman -Syu --needed base-devel cmake ninja pkgconf \
        libx11 libxrandr libxinerama libxcursor libxi \
        mesa libgl glu curl zip unzip tar git
    ```

2. Install vcpkg:

    Clone `vcpkg` from GitHub into a workspace (e.g. `~/dev` or `/opt`):
    
    ```bash
    mkdir ~/dev
    cd ~/dev
    git clone https://github.com/microsoft/vcpkg.git ~/dev/vcpkg
    cd ~/dev/vcpkg
    ./bootstrap-vcpkg.sh -disableMetrics
    ```


5. Clone gTerm

    ```bash
    cd ~/dev
    git clone https://github.com/outgoingbot/gTerm.git
    cd ~/dev/gTerm
    ```


5. Configure with CMake:

    ```bash
    cmake -B build -S . \
        -DCMAKE_TOOLCHAIN_FILE=~/dev/vcpkg/scripts/buildsystems/vcpkg.cmake \
        -DCMAKE_BUILD_TYPE=Release
    ```


6. Build the project
    
    ```bash    
    cmake --build build --config Release -j$(nproc)
    ```


7. Run the Executable:
    ```bash
    ./build/gTerm/gTerm
    ```
