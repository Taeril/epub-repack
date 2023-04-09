#include "zip.hpp"
#include "utils.hpp"

#include <exception>

#include <fmt/core.h>
#include <fmt/ostream.h>

#include <libdeflate.h>

LFH::LFH(std::string const& str, size_t offset) :
	signature(read4(str, offset + 0)),
	version(read2(str, offset + 4)),

	bit_flag(read2(str, offset + 6)),
	compression_method(read2(str, offset + 8)),
	modification_time(read2(str, offset + 10)),
	modification_date(read2(str, offset + 12)),
	crc32(read4(str, offset + 14)),
	compressed_size(read4(str, offset + 18)),
	uncompressed_size(read4(str, offset + 22)),
	file_name_length(read2(str, offset + 26)),
	extra_field_length(read2(str, offset + 28)),

	file_name(&str[offset + 30], file_name_length),
	extra_field(&str[offset + 30 + file_name_length], extra_field_length),
	data(&str[offset + 30 + file_name_length + extra_field_length], compressed_size) {
}

void LFH::print() {
	fmt::print("LFH:\n");
	fmt::print(" - signature:          {:08X}\n", signature);
	fmt::print(" - version:            {:04X}\n", version);

	fmt::print(" - bit_flag:           {:04X}\n", bit_flag);
	fmt::print(" - compression_method: {:04X}\n", compression_method);
	fmt::print(" - modification_time:  {:04X}\n", modification_time);
	fmt::print(" - modification_date:  {:04X}\n", modification_date);
	fmt::print(" - crc32:              {:08X}\n", crc32);
	fmt::print(" - compressed_size:    {:08X}\n", compressed_size);
	fmt::print(" - uncompressed_size:  {:08X}\n", uncompressed_size);
	fmt::print(" - file_name_length:   {:04X}\n", file_name_length);
	fmt::print(" - extra_field_length: {:04X}\n", extra_field_length);
					

	fmt::print(" - file name({}): {}\n", file_name.size(), file_name);
	fmt::print(" - extra field({}): {}\n", extra_field.size(), extra_field);
	fmt::print(" - data({}): {}\n", data.size(), "...");
}

CDFH::CDFH(std::string const& str, size_t offset) :
	signature(read4(str, offset + 0)),
	version_made_by(read2(str, offset + 4)),
	version_needed(read2(str, offset + 6)),

	bit_flag(read2(str, offset + 8)),
	compression_method(read2(str, offset + 10)),
	modification_time(read2(str, offset + 12)),
	modification_date(read2(str, offset + 14)),
	crc32(read4(str, offset + 16)),
	compressed_size(read4(str, offset + 20)),
	uncompressed_size(read4(str, offset + 24)),
	file_name_length(read2(str, offset + 28)),
	extra_field_length(read2(str, offset + 30)),
					 
	file_comment_length(read2(str, offset + 32)),
	disk_number(read2(str, offset + 34)),
	internal_file_attributes(read2(str, offset + 36)),
	external_file_attributes(read4(str, offset + 38)),
	local_file_header_offset(read4(str, offset + 42)),

	file_name(&str[offset + 46], file_name_length),
	extra_field(&str[offset + 46 + file_name_length], extra_field_length),
	file_comment(&str[offset + 46 + file_name_length + extra_field_length], file_comment_length) {
}

void CDFH::print() {
	fmt::print("CDFH:\n");
	fmt::print(" - signature:                {:08X}\n", signature);
	fmt::print(" - version_made_by:          {:04X}\n", version_made_by);
	fmt::print(" - version_needed:           {:04X}\n", version_needed);

	fmt::print(" - bit_flag:                 {:04X}\n", bit_flag);
	fmt::print(" - compression_method:       {:04X}\n", compression_method);
	fmt::print(" - modification_time:        {:04X}\n", modification_time);
	fmt::print(" - modification_date:        {:04X}\n", modification_date);
	fmt::print(" - crc32:                    {:08X}\n", crc32);
	fmt::print(" - compressed_size:          {:08X}\n", compressed_size);
	fmt::print(" - uncompressed_size:        {:08X}\n", uncompressed_size);
	fmt::print(" - file_name_length:         {:04X}\n", file_name_length);
	fmt::print(" - extra_field_length:       {:04X}\n", extra_field_length);
					
	fmt::print(" - file_comment_length:      {:04X}\n", file_comment_length);
	fmt::print(" - disk_number:              {:04X}\n", disk_number);
	fmt::print(" - internal_file_attributes: {:04X}\n", internal_file_attributes);
	fmt::print(" - external_file_attributes: {:08X}\n", external_file_attributes);
	fmt::print(" - local_file_header_offset: {:08X}\n", local_file_header_offset);

	fmt::print(" - file name({}): {}\n", file_name.size(), file_name);
	fmt::print(" - extra field({}): {}\n", extra_field.size(), extra_field);
	fmt::print(" - file comment({}): {}\n", file_comment.size(), file_comment);
}

EOCD::EOCD(std::string const& str, size_t offset) :
	signature(read4(str, offset + 0)),
	number_of_this_disk(read2(str, offset + 4)),
	central_directory_disk_no(read2(str, offset + 6)),
	entries_in_this_disk(read2(str, offset + 8)),
	total_entries(read2(str, offset + 10)),
	central_directory_size(read4(str, offset + 12)),
	central_directory_offset(read4(str, offset + 16)),
	comment_length(read2(str, offset + 20)),

	comment(&str[offset + 22], comment_length) {
}

void EOCD::print() {
	fmt::print("EOCD:\n");
	fmt::print(" - signature:                 {:08X}\n", signature);
	fmt::print(" - number_of_this_disk:       {}\n", number_of_this_disk);
	fmt::print(" - central_directory_disk_no: {}\n", central_directory_disk_no);
	fmt::print(" - entries_in_this_disk:      {}\n", entries_in_this_disk);
	fmt::print(" - total_entries:             {}\n", total_entries);
	fmt::print(" - central_directory_size:    {:x}\n", central_directory_size);
	fmt::print(" - central_directory_offset:  {:08X}\n", central_directory_offset);
	fmt::print(" - comment_length:            {}\n", comment_length);

	fmt::print(" - comment({}): {}\n", comment.size(), comment);
}

Zip::Zip(std::string const& path) {
	content = read_file(path);

	size_t eocd_pos = content.rfind("PK\05\06");
	eocd = EOCD(content, eocd_pos);

	size_t cdfh_pos = eocd.central_directory_offset;

	for(uint16_t i = 0; i < eocd.total_entries; ++i) {
		CDFH cdfh(content, cdfh_pos);
		LFH lfh(content, cdfh.local_file_header_offset);

		size_t file_content_pos = cdfh.local_file_header_offset + 30 + lfh.file_name_length + lfh.extra_field_length;

		File file;
		file.cdfh = cdfh;
		file.lfh = lfh;

		if(lfh.compression_method == 0) {
			file.content = content.substr(file_content_pos, lfh.uncompressed_size);
		} else if(lfh.compression_method == 8) {
			file.content = std::string(lfh.uncompressed_size, '\0');

			auto d = libdeflate_alloc_decompressor();
			size_t ret;
			libdeflate_deflate_decompress(d,
				content.data() + file_content_pos, lfh.compressed_size,
				file.content.data(), file.content.size(),
				&ret
			);
			libdeflate_free_decompressor(d);
		} else {
			throw std::runtime_error("unsupported compression metod");;
		}

		files.push_back(file);

		cdfh_pos += 46 + cdfh.file_name_length + cdfh.extra_field_length + cdfh.file_comment_length;
	}
}

File* Zip::find_file(std::string const& fname) {
	for(auto& file : files) {
		if(file.lfh.file_name == fname) {
			return &file;
		}
	}

	return nullptr;
}

