cmake_minimum_required(VERSION 2.8)
project(wpilib_builder C CXX)
include(ExternalProject)

ExternalProject_Add(wpilib
	GIT_REPOSITORY https://usfirst.collab.net/gerrit/allwpilib
	CMAKE_ARGS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
	           -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
	           -DCMAKE_CXX_FLAGS=${MSVC_COMPILER_DEFS}
	PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
	# Disable build step (only need headers)
	CMAKE_COMMAND ""
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	# Disable install step
	INSTALL_COMMAND ""
	)

# Specify include dir
ExternalProject_Get_Property(wpilib source_dir)
set(WPILIB_SRC_DIR ${source_dir} PARENT_SCOPE)

# Specify MainTest's link libraries
ExternalProject_Get_Property(wpilib binary_dir)
set(WPILIB_LIBS_DIR ${binary_dir} PARENT_SCOPE)



