#ifndef HEADER_UTILS_HPP
#define HEADER_UTILS_HPP

#include <fstream>
#include <string>
#include <cstdint>

std::string read_file(std::string const& path);
void write_file(std::string const& path, std::string const& str);


uint32_t read4(std::string const& str, size_t offset);
void write4(std::ofstream& ofs, uint32_t value);
uint16_t read2(std::string const& str, size_t offset);
void write2(std::ofstream& ofs, uint16_t value);

std::string compress(std::string_view str);

uint32_t crc32(const std::string_view str);

#endif /* HEADER_UTILS_HPP */

