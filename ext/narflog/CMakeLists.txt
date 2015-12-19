cmake_minimum_required(VERSION 2.8.8)
project(narflog_builder C CXX)
include(ExternalProject)

ExternalProject_Add(narflog
	GIT_REPOSITORY https://github.com/jcreigh/narflog.git
	CMAKE_ARGS -DSTATIC=ON
	           -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
	           -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
	           -DCMAKE_CXX_FLAGS=${MSVC_COMPILER_DEFS}
	           -DTEST=OFF
	PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
	# Disable install step
	INSTALL_COMMAND ""
	)

# Specify include dir
ExternalProject_Get_Property(narflog source_dir)
set(NARFLOG_INCLUDE_DIR ${source_dir}/include PARENT_SCOPE)

# Specify MainTest's link libraries
ExternalProject_Get_Property(narflog binary_dir)
set(NARFLOG_LIBS_DIR ${binary_dir} PARENT_SCOPE)



