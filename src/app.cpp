#include "app.hpp"

int App::run(int argc, char** argv) {
	int ret = args(argc, argv);
	if(ret != 0) return ret;


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

