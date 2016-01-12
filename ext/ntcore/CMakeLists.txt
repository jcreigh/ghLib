cmake_minimum_required(VERSION 2.8.8)
project(ntcore_builder C CXX)
include(ExternalProject)

ExternalProject_Add(ntcore
	GIT_REPOSITORY https://usfirst.collab.net/gerrit/ntcore
	CMAKE_ARGS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
	           -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
	           -DCMAKE_CXX_FLAGS=${MSVC_COMPILER_DEFS}
	           -DWITHOUT_JAVA=ON
	           -DWITHOUT_TESTS=ON
	PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
	# Disable install step
	INSTALL_COMMAND ""
	)

# Specify include dir
ExternalProject_Get_Property(ntcore source_dir)
set(NWT_INCLUDE_DIR ${source_dir}/include PARENT_SCOPE)

# Specify MainTest's link libraries
ExternalProject_Get_Property(ntcore binary_dir)
set(NWT_LIBS_DIR ${binary_dir} PARENT_SCOPE)



