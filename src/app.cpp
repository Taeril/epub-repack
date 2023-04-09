#include "app.hpp"
#include "filesystem.hpp"

static void replace_all(std::string& str, std::string const& from, std::string const& to) {
    std::string::size_type pos = 0;
    while((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length() - from.length() + 1;
    }
}

int App::run(int argc, char** argv) {
	int ret = args(argc, argv);
	if(ret != 0) return ret + 1;

	for(auto const& file : files_) {
		xprint(1, "File: {}\n", file);

		fs::path p(file);
		std::string filename = p.filename().string();
		std::string name = p.stem().string();
		std::string ext = p.extension().string();
		std::string dir = p.parent_path().string();
		if(dir.empty()) dir = ".";

		std::string output(output_pattern_);
		replace_all(output, "{DIR}", dir);
		replace_all(output, "{FILENAME}", filename);
		replace_all(output, "{NAME}", name);
		replace_all(output, "{EXT}", ext);


		xprint(1, " - DIR: {}\n", dir);
		xprint(1, " - FILENAME: {}\n", filename);
		xprint(1, " - NAME: {}\n", name);
		xprint(1, " - EXT: {}\n", ext);
		xprint(1, " => {}\n", output);
	}

	return 0;
}

constexpr std::underlying_type<App::Fix>::type fix2num(App::Fix fix) noexcept {
    return static_cast<std::underlying_type<App::Fix>::type>(fix);
}

void App::print_info() {
	xprint(1,
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

	xprint(2, "Files:\n");
	for(auto const& file : files_) {
		xprint(2, "  {}\n", file);
	}
}

