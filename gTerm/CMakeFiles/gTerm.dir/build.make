# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/mac/dev/gTerm

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mac/dev/gTerm

# Include any dependencies generated for this target.
include gTerm/CMakeFiles/gTerm.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include gTerm/CMakeFiles/gTerm.dir/compiler_depend.make

# Include the progress variables for this target.
include gTerm/CMakeFiles/gTerm.dir/progress.make

# Include the compile flags for this target's objects.
include gTerm/CMakeFiles/gTerm.dir/flags.make

gTerm/CMakeFiles/gTerm.dir/gTerm.cpp.o: gTerm/CMakeFiles/gTerm.dir/flags.make
gTerm/CMakeFiles/gTerm.dir/gTerm.cpp.o: gTerm/gTerm.cpp
gTerm/CMakeFiles/gTerm.dir/gTerm.cpp.o: gTerm/CMakeFiles/gTerm.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mac/dev/gTerm/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object gTerm/CMakeFiles/gTerm.dir/gTerm.cpp.o"
	cd /home/mac/dev/gTerm/gTerm && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT gTerm/CMakeFiles/gTerm.dir/gTerm.cpp.o -MF CMakeFiles/gTerm.dir/gTerm.cpp.o.d -o CMakeFiles/gTerm.dir/gTerm.cpp.o -c /home/mac/dev/gTerm/gTerm/gTerm.cpp

gTerm/CMakeFiles/gTerm.dir/gTerm.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gTerm.dir/gTerm.cpp.i"
	cd /home/mac/dev/gTerm/gTerm && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mac/dev/gTerm/gTerm/gTerm.cpp > CMakeFiles/gTerm.dir/gTerm.cpp.i

gTerm/CMakeFiles/gTerm.dir/gTerm.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gTerm.dir/gTerm.cpp.s"
	cd /home/mac/dev/gTerm/gTerm && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mac/dev/gTerm/gTerm/gTerm.cpp -o CMakeFiles/gTerm.dir/gTerm.cpp.s

# Object files for target gTerm
gTerm_OBJECTS = \
"CMakeFiles/gTerm.dir/gTerm.cpp.o"

# External object files for target gTerm
gTerm_EXTERNAL_OBJECTS =

gTerm/gTerm: gTerm/CMakeFiles/gTerm.dir/gTerm.cpp.o
gTerm/gTerm: gTerm/CMakeFiles/gTerm.dir/build.make
gTerm/gTerm: vcpkg_installed/x64-linux/debug/lib/libglad.a
gTerm/gTerm: vcpkg_installed/x64-linux/debug/lib/libglfw3.a
gTerm/gTerm: vcpkg_installed/x64-linux/debug/lib/libimguid.a
gTerm/gTerm: vcpkg_installed/x64-linux/debug/lib/libglfw3.a
gTerm/gTerm: /usr/lib/x86_64-linux-gnu/librt.a
gTerm/gTerm: /usr/lib/x86_64-linux-gnu/libm.so
gTerm/gTerm: gTerm/CMakeFiles/gTerm.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mac/dev/gTerm/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable gTerm"
	cd /home/mac/dev/gTerm/gTerm && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gTerm.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
gTerm/CMakeFiles/gTerm.dir/build: gTerm/gTerm
.PHONY : gTerm/CMakeFiles/gTerm.dir/build

gTerm/CMakeFiles/gTerm.dir/clean:
	cd /home/mac/dev/gTerm/gTerm && $(CMAKE_COMMAND) -P CMakeFiles/gTerm.dir/cmake_clean.cmake
.PHONY : gTerm/CMakeFiles/gTerm.dir/clean

gTerm/CMakeFiles/gTerm.dir/depend:
	cd /home/mac/dev/gTerm && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mac/dev/gTerm /home/mac/dev/gTerm/gTerm /home/mac/dev/gTerm /home/mac/dev/gTerm/gTerm /home/mac/dev/gTerm/gTerm/CMakeFiles/gTerm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : gTerm/CMakeFiles/gTerm.dir/depend
