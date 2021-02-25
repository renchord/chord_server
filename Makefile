# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_SOURCE_DIR = /home/chord/workspace/chord_server/chord_server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/chord/workspace/chord_server/chord_server

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/chord/workspace/chord_server/chord_server/CMakeFiles /home/chord/workspace/chord_server/chord_server/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/chord/workspace/chord_server/chord_server/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named chord

# Build rule for target.
chord: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 chord
.PHONY : chord

# fast build rule for target.
chord/fast:
	$(MAKE) -f CMakeFiles/chord.dir/build.make CMakeFiles/chord.dir/build
.PHONY : chord/fast

#=============================================================================
# Target rules for targets named test_config

# Build rule for target.
test_config: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test_config
.PHONY : test_config

# fast build rule for target.
test_config/fast:
	$(MAKE) -f CMakeFiles/test_config.dir/build.make CMakeFiles/test_config.dir/build
.PHONY : test_config/fast

#=============================================================================
# Target rules for targets named test01

# Build rule for target.
test01: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 test01
.PHONY : test01

# fast build rule for target.
test01/fast:
	$(MAKE) -f CMakeFiles/test01.dir/build.make CMakeFiles/test01.dir/build
.PHONY : test01/fast

chord/config.o: chord/config.cpp.o

.PHONY : chord/config.o

# target to build an object file
chord/config.cpp.o:
	$(MAKE) -f CMakeFiles/chord.dir/build.make CMakeFiles/chord.dir/chord/config.cpp.o
.PHONY : chord/config.cpp.o

chord/config.i: chord/config.cpp.i

.PHONY : chord/config.i

# target to preprocess a source file
chord/config.cpp.i:
	$(MAKE) -f CMakeFiles/chord.dir/build.make CMakeFiles/chord.dir/chord/config.cpp.i
.PHONY : chord/config.cpp.i

chord/config.s: chord/config.cpp.s

.PHONY : chord/config.s

# target to generate assembly for a file
chord/config.cpp.s:
	$(MAKE) -f CMakeFiles/chord.dir/build.make CMakeFiles/chord.dir/chord/config.cpp.s
.PHONY : chord/config.cpp.s

chord/logger.o: chord/logger.cpp.o

.PHONY : chord/logger.o

# target to build an object file
chord/logger.cpp.o:
	$(MAKE) -f CMakeFiles/chord.dir/build.make CMakeFiles/chord.dir/chord/logger.cpp.o
.PHONY : chord/logger.cpp.o

chord/logger.i: chord/logger.cpp.i

.PHONY : chord/logger.i

# target to preprocess a source file
chord/logger.cpp.i:
	$(MAKE) -f CMakeFiles/chord.dir/build.make CMakeFiles/chord.dir/chord/logger.cpp.i
.PHONY : chord/logger.cpp.i

chord/logger.s: chord/logger.cpp.s

.PHONY : chord/logger.s

# target to generate assembly for a file
chord/logger.cpp.s:
	$(MAKE) -f CMakeFiles/chord.dir/build.make CMakeFiles/chord.dir/chord/logger.cpp.s
.PHONY : chord/logger.cpp.s

chord/util.o: chord/util.cpp.o

.PHONY : chord/util.o

# target to build an object file
chord/util.cpp.o:
	$(MAKE) -f CMakeFiles/chord.dir/build.make CMakeFiles/chord.dir/chord/util.cpp.o
.PHONY : chord/util.cpp.o

chord/util.i: chord/util.cpp.i

.PHONY : chord/util.i

# target to preprocess a source file
chord/util.cpp.i:
	$(MAKE) -f CMakeFiles/chord.dir/build.make CMakeFiles/chord.dir/chord/util.cpp.i
.PHONY : chord/util.cpp.i

chord/util.s: chord/util.cpp.s

.PHONY : chord/util.s

# target to generate assembly for a file
chord/util.cpp.s:
	$(MAKE) -f CMakeFiles/chord.dir/build.make CMakeFiles/chord.dir/chord/util.cpp.s
.PHONY : chord/util.cpp.s

tests/test.o: tests/test.cc.o

.PHONY : tests/test.o

# target to build an object file
tests/test.cc.o:
	$(MAKE) -f CMakeFiles/test01.dir/build.make CMakeFiles/test01.dir/tests/test.cc.o
.PHONY : tests/test.cc.o

tests/test.i: tests/test.cc.i

.PHONY : tests/test.i

# target to preprocess a source file
tests/test.cc.i:
	$(MAKE) -f CMakeFiles/test01.dir/build.make CMakeFiles/test01.dir/tests/test.cc.i
.PHONY : tests/test.cc.i

tests/test.s: tests/test.cc.s

.PHONY : tests/test.s

# target to generate assembly for a file
tests/test.cc.s:
	$(MAKE) -f CMakeFiles/test01.dir/build.make CMakeFiles/test01.dir/tests/test.cc.s
.PHONY : tests/test.cc.s

tests/testconfig.o: tests/testconfig.cc.o

.PHONY : tests/testconfig.o

# target to build an object file
tests/testconfig.cc.o:
	$(MAKE) -f CMakeFiles/test_config.dir/build.make CMakeFiles/test_config.dir/tests/testconfig.cc.o
.PHONY : tests/testconfig.cc.o

tests/testconfig.i: tests/testconfig.cc.i

.PHONY : tests/testconfig.i

# target to preprocess a source file
tests/testconfig.cc.i:
	$(MAKE) -f CMakeFiles/test_config.dir/build.make CMakeFiles/test_config.dir/tests/testconfig.cc.i
.PHONY : tests/testconfig.cc.i

tests/testconfig.s: tests/testconfig.cc.s

.PHONY : tests/testconfig.s

# target to generate assembly for a file
tests/testconfig.cc.s:
	$(MAKE) -f CMakeFiles/test_config.dir/build.make CMakeFiles/test_config.dir/tests/testconfig.cc.s
.PHONY : tests/testconfig.cc.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... chord"
	@echo "... test_config"
	@echo "... test01"
	@echo "... chord/config.o"
	@echo "... chord/config.i"
	@echo "... chord/config.s"
	@echo "... chord/logger.o"
	@echo "... chord/logger.i"
	@echo "... chord/logger.s"
	@echo "... chord/util.o"
	@echo "... chord/util.i"
	@echo "... chord/util.s"
	@echo "... tests/test.o"
	@echo "... tests/test.i"
	@echo "... tests/test.s"
	@echo "... tests/testconfig.o"
	@echo "... tests/testconfig.i"
	@echo "... tests/testconfig.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

