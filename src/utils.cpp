#include "utils.hpp"

#include <fstream>
#include <iterator>

#include <fmt/core.h>

#include <zopfli.h>

std::string read_file(std::string const& path) {
	std::ifstream file(path, std::ios::binary);
	return std::string(std::istreambuf_iterator{file}, {});
}

void write_file(std::string const& path, std::string const& str) {
	std::ofstream file(path, std::ios::binary);
	file.write(str.data(), str.size());
}

uint32_t read4(std::string const& str, size_t offset) {
	return 
		(uint32_t(uint8_t(str[offset+0])) << 0)  |
		(uint32_t(uint8_t(str[offset+1])) << 8)  |
		(uint32_t(uint8_t(str[offset+2])) << 16) |
		(uint32_t(uint8_t(str[offset+3])) << 24);
}

void write4(std::ofstream& ofs, uint32_t value) {
	const uint8_t v[] = {
		static_cast<uint8_t>(value >> 0),
		static_cast<uint8_t>(value >> 8),
		static_cast<uint8_t>(value >> 16),
		static_cast<uint8_t>(value >> 24),
	};
	//fmt::print(ofs, "{}{}{}{}", char(v[0]), char(v[1]), char(v[2]), char(v[3]));
	ofs.write(reinterpret_cast<const char*>(v), 4);
}

uint16_t read2(std::string const& str, size_t offset) {
	return 
		(uint16_t(uint8_t(str[offset+0])) << 0) |
		(uint16_t(uint8_t(str[offset+1])) << 8);
}

void write2(std::ofstream& ofs, uint16_t value) {
	const uint8_t v[] = {
		static_cast<uint8_t>(value >> 0),
		static_cast<uint8_t>(value >> 8),
	};
	//fmt::print(ofs, "{}{}", char(v[0]), char(v[1]));
	ofs.write(reinterpret_cast<const char*>(v), 2);
}


std::string compress(std::string_view str) {
	ZopfliOptions zo;
	ZopfliInitOptions(&zo);
	zo.numiterations = 16;

	const unsigned char* in = reinterpret_cast<const unsigned char*>(str.data());
	size_t out_size = 0;
	unsigned char* out = nullptr;
	ZopfliCompress(&zo, ZOPFLI_FORMAT_DEFLATE,
		in, str.size(),
		&out, &out_size
	);

	std::string ret(reinterpret_cast<char*>(out), out_size);
	free(out);
	return ret;
}

uint32_t crc32(const std::string_view str) {
	uint32_t crc = ~uint32_t{0};

	const uint8_t* p = reinterpret_cast<const uint8_t*>(str.data());
	size_t len = str.size();
	while(len-- != 0) {
		crc ^= *p++;

		for(int i=0; i<8; ++i) {
			crc = (crc >> 1) ^ (-int32_t(crc & 1) & 0xEDB88320);
		}
	}

	return ~crc;
}

