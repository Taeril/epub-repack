cmake_minimum_required(VERSION 3.23)
project(epub-repack VERSION "0.0.1" LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(SetupCompiler)


find_package(fmt CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
find_package(pugixml CONFIG REQUIRED)
find_package(libdeflate CONFIG REQUIRED)
find_package(Zopfli CONFIG REQUIRED)

find_package(Filesystem REQUIRED)

message(STATUS "fmt: ${fmt_VERSION}")
message(STATUS "cxxopts: ${cxxopts_VERSION}")
message(STATUS "pugixml: ${pugixml_VERSION}")
message(STATUS "libdeflate: ${libdeflate_VERSION}")
message(STATUS "Zopfli: ${Zopfli_VERSION}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in" "${CMAKE_CURRENT_BINARY_DIR}/version.hpp")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/filesystem.hpp.in" "${CMAKE_CURRENT_BINARY_DIR}/filesystem.hpp")

add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_BINARY_DIR}"
)

target_sources(${PROJECT_NAME} PRIVATE
	"${CMAKE_CURRENT_BINARY_DIR}/version.hpp"
	"src/main.cpp"
	"src/app.cpp"
	"src/app-args.cpp"
	"src/utils.cpp"
	"src/xml.cpp"
	"src/zip.cpp"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	fmt::fmt
	cxxopts::cxxopts
	pugixml::pugixml
	libdeflate::libdeflate_static
	zopfli::zopfli
	std::filesystem
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

