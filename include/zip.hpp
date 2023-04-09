#ifndef HEADER_ZIP_HPP
#define HEADER_ZIP_HPP

#include <string>
#include <vector>
#include <cstdint>

struct LFH {
	/*  0 */ uint32_t signature;          // 0x04034b50
	/*  4 */ uint16_t version;
	/*  6 */ uint16_t bit_flag;
	/*  8 */ uint16_t compression_method;
	/* 10 */ uint16_t modification_time;
	/* 12 */ uint16_t modification_date;
	/* 14 */ uint32_t crc32;
	/* 18 */ uint32_t compressed_size;
	/* 22 */ uint32_t uncompressed_size;
	/* 26 */ uint16_t file_name_length;
	/* 28 */ uint16_t extra_field_length;
	/* 30 */
	// filename
	// extra field
	std::string file_name;
	std::string_view extra_field;
	std::string_view data;

	LFH() = default;
	LFH(std::string const& str, size_t offset);
	void print();
};

struct CDFH {
	/*  0 */ uint32_t signature;          // 0x02014b50
	/*  4 */ uint16_t version_made_by;
	/*  6 */ uint16_t version_needed;

	/*  8 */ uint16_t bit_flag;
	/* 10 */ uint16_t compression_method;
	/* 12 */ uint16_t modification_time;
	/* 14 */ uint16_t modification_date;
	/* 16 */ uint32_t crc32;
	/* 20 */ uint32_t compressed_size;
	/* 24 */ uint32_t uncompressed_size;
	/* 28 */ uint16_t file_name_length;
	/* 30 */ uint16_t extra_field_length;

	/* 32 */ uint16_t file_comment_length;
	/* 34 */ uint16_t disk_number;
	/* 36 */ uint16_t internal_file_attributes;
	/* 38 */ uint32_t external_file_attributes;
	/* 42 */ uint32_t local_file_header_offset;

	/* 46 */
	// filename
	// extra field
	// file comment
	std::string file_name;
	std::string_view extra_field;
	std::string_view file_comment;
	
	CDFH() = default;
	CDFH(std::string const& str, size_t offset);
	void print();
};

struct EOCD {
	/*  0 */ uint32_t signature;                  // 0x06054b50
	/*  4 */ uint16_t number_of_this_disk;        // 0
	/*  6 */ uint16_t central_directory_disk_no;  // 0
	/*  8 */ uint16_t entries_in_this_disk;       // n
	/* 10 */ uint16_t total_entries;              // n
	/* 12 */ uint32_t central_directory_size;     // ???
	/* 16 */ uint32_t central_directory_offset;   // ???
	/* 20 */ uint16_t comment_length;             // 0
	/* 22 */
	// comment
	std::string_view comment;

	EOCD() = default;
	EOCD(std::string const& str, size_t offset);
	void print();
};


struct File {
	CDFH cdfh;
	LFH lfh;
	std::string content;
};

struct Zip {
	std::string content;
	EOCD eocd;
	std::vector<File> files;

	Zip(std::string const& path);

	File* find_file(std::string const& fname);
};

#endif /* HEADER_ZIP_HPP */

