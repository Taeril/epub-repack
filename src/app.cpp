#include "app.hpp"
#include "filesystem.hpp"

#include "zip.hpp"
#include "xml.hpp"
#include "utils.hpp"

static void replace_all(std::string& str, std::string const& from, std::string const& to) {
	std::string::size_type pos = 0;
	while((pos = str.find(from, pos)) != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length() - from.length() + 1;
	}
}

int App::run(int argc, char** argv) {
	int ret = args(argc, argv);
	if(ret != 0) {
		return ret + 1;
	}

	for(auto const& file : files_) {
		fs::path p(file);
		std::string filename = p.filename().string();
		std::string name = p.stem().string();
		std::string ext = p.extension().string();
		std::string dir = p.parent_path().string();
		if(dir.empty()) {
			dir = ".";
		}
		if(!ext.empty() && ext[0] == '.') {
			ext = ext.substr(1);
		}

		std::string output(output_pattern_);
		replace_all(output, "{DIR}", dir);
		replace_all(output, "{FILENAME}", filename);
		replace_all(output, "{NAME}", name);
		replace_all(output, "{EXT}", ext);

		fs::path out(output);
		if(!fs::exists(out.parent_path())) {
			fs::create_directories(out.parent_path());
		}
		if(fs::is_directory(out)) {
			output = (out / filename).string();
		}

		// clang-format off
		xprint(1, "{} => {}\n",
			xstyled(file, fg_bright_green),
			xstyled(output, fg_yellow)
		);
		// clang-format on

		Zip zip{file};

		fix_series(zip);

		save_zip(zip, output);
	}

	return 0;
}

constexpr std::underlying_type<App::Fix>::type fix2num(App::Fix fix) noexcept {
	return static_cast<std::underlying_type<App::Fix>::type>(fix);
}

void App::print_info() {
	// clang-format off
	xprint(3,
		"App {{\n"
		"  output_pattern: ... {}\n"
		"  log_level: ........ {}\n"
		"  color: ............ {}\n"
		"  repack: ........... {}\n"
		"  iterations: ....... {}\n"
		"  fix_series: ....... {}\n"
		"}}\n",
		xstyled(output_pattern_, fg_bright_white),
		xstyled(log_level_, fg_bright_white),
		xstyled(color_, fg_bright_white),
		xstyled(repack_, fg_bright_white),
		xstyled(iterations_, fg_bright_white),
		xstyled(bool(fixes_ & fix2num(Fix::Series)), fg_bright_white)
	);

	xprint(3, "Files:\n");
	for(auto const& file : files_) {
		xprint(3, "{}\n",
			xstyled(file, fg_bright_white)
		);
	}
	xprint(3, "END\n");
	// clang-format on
}

void App::fix_series(Zip& zip) {
	File* file = zip.find_file("META-INF/container.xml");
	if(file) {
		xprint(2, "Found: {}\n", file->lfh.file_name);
		XML xml(file->content);
		std::string rootfile = xml.get_rootfile();

		xprint(2, "rootfile: {}\n", rootfile);

		File* f = zip.find_file(rootfile);
		if(f) {
			XML x(f->content);
			std::string v = x.fix_metadata();

			f->content = v;
			f->lfh.uncompressed_size = static_cast<uint32_t>(v.size());
			f->lfh.crc32 = crc32(v);
		}
	}
}

void App::save_zip(Zip& zip, std::string const& filename) {
	std::ofstream ofs(filename.c_str(), std::ios::binary);
	std::vector<uint32_t> offsets;
	uint32_t pos = 0;

	auto write_str = [&ofs](std::string_view const& str) {
		// Hide warning about conversion changing signedness
		// write() accepts parameter of type streamsize (signed)
		// but size() return size_t (unsigned)
		ofs.write(str.data(), static_cast<std::streamsize>(str.size()));
	};

	for(size_t i = 0; i < zip.files.size(); ++i) {
		LFH& lfh = zip.files[i].lfh;

		xprint(2, "LFH({}/{}): {}\n", i + 1, zip.files.size(), lfh.file_name);

		std::string v;
		if(lfh.compression_method == 8) {
			v = compress(zip.files[i].content);
			uint32_t v_size = static_cast<uint32_t>(v.size());
			auto d_size = lfh.compressed_size - v_size;
			// clang-format off
			xprint(2, " - zopfli saved {} bytes\n",
				xstyled(d_size,
					d_size > 0 ? fg_green :
					d_size == 0 ? fg_bright_white :
					fg_red
				)
			);
			// clang-format on
			lfh.compressed_size = v_size;
		} else if(lfh.compression_method == 0) {
			xprint(2, " - store\n");
		}

		write4(ofs, lfh.signature);
		write2(ofs, lfh.version);
		write2(ofs, lfh.bit_flag);
		write2(ofs, lfh.compression_method);
		write2(ofs, lfh.modification_time);
		write2(ofs, lfh.modification_date);
		write4(ofs, lfh.crc32);
		write4(ofs, lfh.compressed_size);
		write4(ofs, lfh.uncompressed_size);
		write2(ofs, lfh.file_name_length);
		write2(ofs, lfh.extra_field_length);

		write_str(lfh.file_name);
		write_str(lfh.extra_field);

		offsets.push_back(pos);
		pos += 30 + lfh.file_name_length + lfh.extra_field_length;

		if(lfh.compression_method == 8) {
			write_str(v);
			pos += static_cast<uint32_t>(v.size());
		} else {
			write_str(lfh.data);
			pos += static_cast<uint32_t>(lfh.data.size());
		}
	}

	uint32_t cdfh_pos = pos;

	for(size_t i = 0; i < zip.files.size(); ++i) {
		LFH& lfh = zip.files[i].lfh;
		CDFH& cdfh = zip.files[i].cdfh;

		xprint(2, "CDFH({}/{}): {}\n", i + 1, zip.files.size(), lfh.file_name);

		write4(ofs, cdfh.signature);
		write2(ofs, cdfh.version_made_by);
		write2(ofs, cdfh.version_needed);

		write2(ofs, lfh.bit_flag);
		write2(ofs, lfh.compression_method);
		write2(ofs, lfh.modification_time);
		write2(ofs, lfh.modification_date);
		write4(ofs, lfh.crc32);
		write4(ofs, lfh.compressed_size);
		write4(ofs, lfh.uncompressed_size);
		write2(ofs, lfh.file_name_length);

		write2(ofs, cdfh.extra_field_length);
		write2(ofs, cdfh.file_comment_length);
		write2(ofs, cdfh.disk_number);
		write2(ofs, cdfh.internal_file_attributes);
		write4(ofs, cdfh.external_file_attributes);
		write4(ofs, offsets[i]);

		write_str(lfh.file_name);
		write_str(cdfh.extra_field);
		write_str(cdfh.file_comment);

		pos += 46 + lfh.file_name_length + cdfh.extra_field_length + cdfh.file_comment_length;
	}

	EOCD& eocd = zip.eocd;

	xprint(2, "EOCD\n");

	write4(ofs, eocd.signature);
	write2(ofs, eocd.number_of_this_disk);
	write2(ofs, eocd.central_directory_disk_no);
	write2(ofs, eocd.entries_in_this_disk);
	write2(ofs, eocd.total_entries);
	write4(ofs, pos - cdfh_pos);
	write4(ofs, cdfh_pos);
	write2(ofs, eocd.comment_length);

	write_str(eocd.comment);
}

