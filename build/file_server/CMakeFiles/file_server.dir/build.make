# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ken/DiRA2020/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ken/DiRA2020/build

# Include any dependencies generated for this target.
include file_server/CMakeFiles/file_server.dir/depend.make

# Include the progress variables for this target.
include file_server/CMakeFiles/file_server.dir/progress.make

# Include the compile flags for this target's objects.
include file_server/CMakeFiles/file_server.dir/flags.make

file_server/CMakeFiles/file_server.dir/src/file_server.cpp.o: file_server/CMakeFiles/file_server.dir/flags.make
file_server/CMakeFiles/file_server.dir/src/file_server.cpp.o: /home/ken/DiRA2020/src/file_server/src/file_server.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ken/DiRA2020/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object file_server/CMakeFiles/file_server.dir/src/file_server.cpp.o"
	cd /home/ken/DiRA2020/build/file_server && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/file_server.dir/src/file_server.cpp.o -c /home/ken/DiRA2020/src/file_server/src/file_server.cpp

file_server/CMakeFiles/file_server.dir/src/file_server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/file_server.dir/src/file_server.cpp.i"
	cd /home/ken/DiRA2020/build/file_server && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ken/DiRA2020/src/file_server/src/file_server.cpp > CMakeFiles/file_server.dir/src/file_server.cpp.i

file_server/CMakeFiles/file_server.dir/src/file_server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/file_server.dir/src/file_server.cpp.s"
	cd /home/ken/DiRA2020/build/file_server && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ken/DiRA2020/src/file_server/src/file_server.cpp -o CMakeFiles/file_server.dir/src/file_server.cpp.s

file_server/CMakeFiles/file_server.dir/src/file_server.cpp.o.requires:

.PHONY : file_server/CMakeFiles/file_server.dir/src/file_server.cpp.o.requires

file_server/CMakeFiles/file_server.dir/src/file_server.cpp.o.provides: file_server/CMakeFiles/file_server.dir/src/file_server.cpp.o.requires
	$(MAKE) -f file_server/CMakeFiles/file_server.dir/build.make file_server/CMakeFiles/file_server.dir/src/file_server.cpp.o.provides.build
.PHONY : file_server/CMakeFiles/file_server.dir/src/file_server.cpp.o.provides

file_server/CMakeFiles/file_server.dir/src/file_server.cpp.o.provides.build: file_server/CMakeFiles/file_server.dir/src/file_server.cpp.o


# Object files for target file_server
file_server_OBJECTS = \
"CMakeFiles/file_server.dir/src/file_server.cpp.o"

# External object files for target file_server
file_server_EXTERNAL_OBJECTS =

/home/ken/DiRA2020/devel/lib/file_server/file_server: file_server/CMakeFiles/file_server.dir/src/file_server.cpp.o
/home/ken/DiRA2020/devel/lib/file_server/file_server: file_server/CMakeFiles/file_server.dir/build.make
/home/ken/DiRA2020/devel/lib/file_server/file_server: /opt/ros/melodic/lib/libroscpp.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /usr/lib/x86_64-linux-gnu/libboost_signals.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /opt/ros/melodic/lib/librosconsole.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /opt/ros/melodic/lib/librosconsole_log4cxx.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /opt/ros/melodic/lib/librosconsole_backend_interface.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /usr/lib/x86_64-linux-gnu/liblog4cxx.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /usr/lib/x86_64-linux-gnu/libboost_regex.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /opt/ros/melodic/lib/libxmlrpcpp.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /opt/ros/melodic/lib/libroscpp_serialization.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /opt/ros/melodic/lib/librostime.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /opt/ros/melodic/lib/libcpp_common.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /usr/lib/x86_64-linux-gnu/libboost_thread.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /usr/lib/x86_64-linux-gnu/libboost_chrono.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /usr/lib/x86_64-linux-gnu/libboost_date_time.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /usr/lib/x86_64-linux-gnu/libboost_atomic.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /usr/lib/x86_64-linux-gnu/libpthread.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /usr/lib/x86_64-linux-gnu/libconsole_bridge.so.0.4
/home/ken/DiRA2020/devel/lib/file_server/file_server: /opt/ros/melodic/lib/libroslib.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /opt/ros/melodic/lib/librospack.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /usr/lib/x86_64-linux-gnu/libpython2.7.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /usr/lib/x86_64-linux-gnu/libboost_program_options.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /usr/lib/x86_64-linux-gnu/libboost_system.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: /usr/lib/x86_64-linux-gnu/libtinyxml2.so
/home/ken/DiRA2020/devel/lib/file_server/file_server: file_server/CMakeFiles/file_server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ken/DiRA2020/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable /home/ken/DiRA2020/devel/lib/file_server/file_server"
	cd /home/ken/DiRA2020/build/file_server && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/file_server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
file_server/CMakeFiles/file_server.dir/build: /home/ken/DiRA2020/devel/lib/file_server/file_server

.PHONY : file_server/CMakeFiles/file_server.dir/build

file_server/CMakeFiles/file_server.dir/requires: file_server/CMakeFiles/file_server.dir/src/file_server.cpp.o.requires

.PHONY : file_server/CMakeFiles/file_server.dir/requires

file_server/CMakeFiles/file_server.dir/clean:
	cd /home/ken/DiRA2020/build/file_server && $(CMAKE_COMMAND) -P CMakeFiles/file_server.dir/cmake_clean.cmake
.PHONY : file_server/CMakeFiles/file_server.dir/clean

file_server/CMakeFiles/file_server.dir/depend:
	cd /home/ken/DiRA2020/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ken/DiRA2020/src /home/ken/DiRA2020/src/file_server /home/ken/DiRA2020/build /home/ken/DiRA2020/build/file_server /home/ken/DiRA2020/build/file_server/CMakeFiles/file_server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : file_server/CMakeFiles/file_server.dir/depend

