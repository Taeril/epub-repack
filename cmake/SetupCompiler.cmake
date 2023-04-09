cmake_minimum_required(VERSION 3.23)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	#message(STATUS "Setting build type to 'Release' as none was specified")
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
		"Debug" "Release" "MinSizeRel" "RelWithDebInfo"
	)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Flags for CMAKE_BUILD_TYPE:
# Debug: -g
# Release: -O3 -DNDEBUG
# MinSizeRel: -Os -DNDEBUG
# RelWithDebInfo: -O2 -g -DNDEBUG

# Setup C++ compiler
if(CMAKE_CXX_COMPILER_ID MATCHES "(Clang|GNU)")
	#set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -pedantic ${CMAKE_CXX_FLAGS}")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wconversion ${CMAKE_CXX_FLAGS}")
	if(${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel")
		set(CMAKE_CXX_LINK_FLAGS "-s ${CMAKE_CXX_LINK_FLAGS}")
	endif()
endif()

# Setup Ninja
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_GENERATOR STREQUAL "Ninja")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_GENERATOR STREQUAL "Ninja")
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
		# do nothing - we want >= 4.9
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color")
	endif()
endif()

