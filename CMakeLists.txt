cmake_minimum_required(VERSION 3.23)
project(epub-repack VERSION "0.0.1" LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(SetupCompiler)


find_package(fmt CONFIG REQUIRED)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in" "${CMAKE_CURRENT_BINARY_DIR}/version.hpp")

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_BINARY_DIR}"
)

target_sources(${PROJECT_NAME} PRIVATE
	"${CMAKE_CURRENT_BINARY_DIR}/version.hpp"
	"src/main.cpp"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	fmt::fmt
)

