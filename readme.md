# gTerm - Real-Time Serial Terminal & Plotting

**gTerm** is a modern, high-performance serial terminal and real-time graphing tool designed for embedded developers, and hardware engineers who need fast, responsive visualization of serial data.

`gTerm` combines a classic terminal interface with powerful real-time plotting capabilities.
<br>
Think **Serial Terminal** + **Serial Oscilloscope** for streaming data. Great for monitoring sensor data / telemetry. `gTerm` gives you immediate visual feedback without the lag or poor performance of other tools. Inspired from the limitations of other serial terminals being very antiquated and other plotting software having excessive buffer, lag, and lack of plot customization.
Written in C++ to achieve high performance.

### Key Features
- **Cross Platform** builds / runs on Windows and Linux
- **Modern, responsive UI** built with Dear ImGui using GPU hardware Acceleration
- **High-speed serial communication** with minimal data Buffering (gets live data as fast as possible to screen)
- **Classic terminal view** with font settings
- **Live data parsing** with flexible channel support
- **Real-time plotting** using ImPlot for smooth waveform visualization and dynamic layout




### Built With

- **Dear ImGui** + **ImPlot** for the immediate-mode GUI and graphing
- **GLFW** + **OpenGL 3/4** for the rendering backend
- Custom serial communication layer (Windows + Linux support)

`gTerm` aims to be lightweight `(doing my best here)`, highly responsive, and focused on delivering the best possible real-time experience when working with serial devices.

---


<img src="https://github.com/outgoingbot/gTerm/blob/master/Screenshot.png" height="512" width="815" >

Building on Windows
=================

1. Install Required Tools:

    - Visual Studio 202x with **Desktop development with C++** (recommended)
    - CMake 3.15 or higher (if not installed with visuals studio)
    - Git

2. clone vcpk into a well chosen director (like: `C:\dev`)
    
    ```cmd
    cd C:\dev
    git clone https://github.com/microsoft/vcpkg.git
    cd C:\dev\vcpkg
    .\bootstrap-vcpkg.bat -disableMetrics
    ```
3. Clone gTerm

    ```cmd
    cd C:\dev
    git clone https://github.com/outgoingbot/gTerm.git
    cd C:\dev\gTerm
    ```

4. download/install dependencies using vcpkg.json
    ```cmd
    cd C:\dev\gTerm
    vcpkg install
    ```
    <br>
5. Build the project

#### Option A: Using Visual Studio (recommended)

1. Open the `gTerm` folder in Visual Studio 2022 (`File` -> `Open` -> `CMake...`)
2. Visual Studio will automatically detect `CMakeLists.txt` and `vcpkg.json`
3. It will configure CMake using the vcpkg toolchain
4. Select **x64** as the active configuration
5. Build the project (`Build` -> `Build All`)

The executable will be generated in `build/Release/` (or `build/Debug/`).

#### Option B: Using Command Line
    
```cmd
    cmake -B build -S . ^
    -DCMAKE_TOOLCHAIN_FILE=..\vcpkg\scripts\buildsystems\vcpkg.cmake ^
    -DCMAKE_BUILD_TYPE=Release

    cmake --build build --config Release
```

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
