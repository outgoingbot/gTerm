cmake_minimum_required(VERSION 3.15)
project(gTerm)

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

# ─── Resource Paths (relative) ───────────────────────────────────────────────────
set(ICON_FILE_PATH "res/icon.png")
add_definitions(-DICON_FILE_PATH=\"${ICON_FILE_PATH}\")

set(FONT_FILE_PATH "res/Glass_TTY_VT220.ttf")
add_definitions(-DFONT_FILE_PATH=\"${FONT_FILE_PATH}\")

set(LOGO_FILE_PATH "res/logo.png")
add_definitions(-DLOGO_FILE_PATH=\"${LOGO_FILE_PATH}\")

# ─── Source Files + Windows .rc for embedded icon ────────────────────────────────
if(WIN32)
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
        ConfigManager.cpp ConfigManager.h
        external/json.hpp
        ${SERIAL_SRC}
        ${SERIAL_INC}
        "${CMAKE_SOURCE_DIR}/gTerm.rc"
    )
else()
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
        ConfigManager_linux.cpp ConfigManager.h
        external/json.hpp
        ${SERIAL_SRC}
        ${SERIAL_INC}
    )
endif()

# ─── Vcpkg Packages ──────────────────────────────────────────────────────────────
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)

target_link_libraries(gTerm PRIVATE
    glfw
    glad::glad
    imgui::imgui
    implot::implot
)

# ─── Copy res folder next to the exe on every build ─────────────────────────────
add_custom_command(
    TARGET gTerm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/res"
            "$<TARGET_FILE_DIR:gTerm>/res"
    COMMENT "Copying resources (icon.png, font, logo) to output directory"
)

# ─── Linux .desktop (optional) ───────────────────────────────────────────────────
if(UNIX)
    install(FILES "${CMAKE_SOURCE_DIR}/res/gTerm.desktop" DESTINATION share/applications)
endif()