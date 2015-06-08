cmake_minimum_required(VERSION 2.8.8)
project(libwebsockets_builder C CXX)
include(ExternalProject)

ExternalProject_Add(libwebsockets
	GIT_REPOSITORY git://git.libwebsockets.org/libwebsockets
	GIT_TAG v1.4-chrome43-firefox-36
	CMAKE_ARGS -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=DebugLibs
	           -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=ReleaseLibs
	           -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
	           -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
	           -DLWS_WITH_SSL=OFF
	           -DLWS_WITHOUT_EXTENSIONS=ON
	           -DCMAKE_CXX_FLAGS=${MSVC_COMPILER_DEFS}
	PREFIX "${CMAKE_CURRENT_BINARY_DIR}"
	# Disable install step
	INSTALL_COMMAND ""
	)

# Specify include dir
ExternalProject_Get_Property(libwebsockets source_dir)
set(LIBWEBSOCKETS_INCLUDE_DIRS ${source_dir}/lib PARENT_SCOPE)

# Specify MainTest's link libraries
ExternalProject_Get_Property(libwebsockets binary_dir)
set(LIBWEBSOCKETS_LIBS_DIR ${binary_dir}/lib PARENT_SCOPE)



