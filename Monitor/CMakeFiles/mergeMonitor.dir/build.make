# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/SF_MergeServer/Monitor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/SF_MergeServer/Monitor

# Include any dependencies generated for this target.
include CMakeFiles/mergeMonitor.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/mergeMonitor.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/mergeMonitor.dir/flags.make

CMakeFiles/mergeMonitor.dir/mergeMonitor.cpp.o: CMakeFiles/mergeMonitor.dir/flags.make
CMakeFiles/mergeMonitor.dir/mergeMonitor.cpp.o: mergeMonitor.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/SF_MergeServer/Monitor/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/mergeMonitor.dir/mergeMonitor.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/mergeMonitor.dir/mergeMonitor.cpp.o -c /home/SF_MergeServer/Monitor/mergeMonitor.cpp

CMakeFiles/mergeMonitor.dir/mergeMonitor.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mergeMonitor.dir/mergeMonitor.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/SF_MergeServer/Monitor/mergeMonitor.cpp > CMakeFiles/mergeMonitor.dir/mergeMonitor.cpp.i

CMakeFiles/mergeMonitor.dir/mergeMonitor.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mergeMonitor.dir/mergeMonitor.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/SF_MergeServer/Monitor/mergeMonitor.cpp -o CMakeFiles/mergeMonitor.dir/mergeMonitor.cpp.s

# Object files for target mergeMonitor
mergeMonitor_OBJECTS = \
"CMakeFiles/mergeMonitor.dir/mergeMonitor.cpp.o"

# External object files for target mergeMonitor
mergeMonitor_EXTERNAL_OBJECTS =

/home/SF_MergeServer/mergeMonitor: CMakeFiles/mergeMonitor.dir/mergeMonitor.cpp.o
/home/SF_MergeServer/mergeMonitor: CMakeFiles/mergeMonitor.dir/build.make
/home/SF_MergeServer/mergeMonitor: /home/SF_MergeServer/libs/libbase.a
/home/SF_MergeServer/mergeMonitor: CMakeFiles/mergeMonitor.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/SF_MergeServer/Monitor/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable /home/SF_MergeServer/mergeMonitor"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mergeMonitor.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/mergeMonitor.dir/build: /home/SF_MergeServer/mergeMonitor

.PHONY : CMakeFiles/mergeMonitor.dir/build

CMakeFiles/mergeMonitor.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/mergeMonitor.dir/cmake_clean.cmake
.PHONY : CMakeFiles/mergeMonitor.dir/clean

CMakeFiles/mergeMonitor.dir/depend:
	cd /home/SF_MergeServer/Monitor && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/SF_MergeServer/Monitor /home/SF_MergeServer/Monitor /home/SF_MergeServer/Monitor /home/SF_MergeServer/Monitor /home/SF_MergeServer/Monitor/CMakeFiles/mergeMonitor.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/mergeMonitor.dir/depend
