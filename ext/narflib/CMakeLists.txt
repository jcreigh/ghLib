cmake_minimum_required(VERSION 2.8)
project(narflib_builder C CXX)
include(ExternalProject)

ExternalProject_Add(narflib
	GIT_REPOSITORY https://github.com/narfblock/narflib.git
	CMAKE_ARGS -DSTATIC=ON
	           -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
	           -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
	           -DCMAKE_CXX_FLAGS=${MSVC_COMPILER_DEFS}
	           -DUSEPNG=OFF
	           -DUSEZLIB=OFF
	           -DTEST=OFF
	PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
	# Disable install step
	INSTALL_COMMAND ""
	)

# Specify include dir
ExternalProject_Get_Property(narflib source_dir)
set(NARFLIB_INCLUDE_DIR ${source_dir}/include PARENT_SCOPE)

# Specify MainTest's link libraries
ExternalProject_Get_Property(narflib binary_dir)
set(NARFLIB_LIBS_DIR ${binary_dir} PARENT_SCOPE)



