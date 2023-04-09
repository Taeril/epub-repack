#include "app.hpp"
#include "version.hpp"

#include <fmt/core.h>
#include <fmt/color.h>
#include <cxxopts.hpp>


#ifdef _WIN32
#include <io.h>

#ifndef isatty
#define isatty _isatty
#endif

#ifndef fileno
#define fileno _fileno
#endif

#else
#include <unistd.h>
#endif


// version
std::string version() { return VERSION; }
unsigned version_major() { return VERSION_MAJOR; }
unsigned version_minor() { return VERSION_MINOR; }
unsigned version_patch() { return VERSION_PATCH; }
unsigned version_number() {
	return VERSION_MAJOR * 10000 + VERSION_MINOR * 100 + VERSION_PATCH;
}

inline std::string format_version(unsigned int version) {
	return fmt::format("{}.{}.{}",
		((version / 10000) % 100),
		((version / 100) % 100),
		(version % 100)
	);
}


static bool use_color() {
	return isatty(fileno(stdout));
}

static std::string str_tolower(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c){ return std::tolower(c); }
	);

	return str;
}

static bool opt_is_true(std::string value) {
	const auto v = str_tolower(value);

	if(v == "true") return true;
	if(v == "yes") return true;
	if(v == "always") return true;
	if(v == "1") return true;
	if(v == "t") return true;
	return false;
}

// static bool opt_is_false(std::string value) {
// 	const auto v = str_tolower(value);
//
// 	if(v == "false") return true;
// 	if(v == "no") return true;
// 	if(v == "none") return true;
// 	if(v == "0") return true;
// 	if(v == "f") return true;
// 	return false;
// }

static bool opt_is_num(std::string const& value) {
	return value.find_first_not_of("0123456789") == std::string::npos;
}

static constexpr std::underlying_type<App::Fix>::type fix2num(App::Fix fix) noexcept {
    return static_cast<std::underlying_type<App::Fix>::type>(fix);
}


int App::args(int argc, char** argv) {
	cxxopts::Options options("epub-repack", "epub-repack (v" VERSION "):\n  Fix and repack epub files\n");

	std::string repack_spec = "yes";
	std::string color_spec = "auto";
	std::vector<std::string> fix_spec;
	bool help = false;
	bool version = false;

	try {
		options.positional_help("FILE...");
		options.set_width(120);

		options.add_options()
			("f,fix",
				"Apply fixes:\n"
				"  all      - apply all fixes\n"
				"  none     - do not apply fixes\n"
				"  series   - fix series information for PocketBook",
				cxxopts::value<std::vector<std::string>>(fix_spec), "NAME,...")
			("r,repack", "Repack file; with integer value N it's alias for: -r yes -i N",
				cxxopts::value<std::string>(repack_spec)->default_value("yes"), "yes|no|N")
			("i,iterations", "Number of iteration",
				cxxopts::value<int>(iterations_)->default_value("16"), "N")
			("o,output",
				"Output patern. Path with placeholder for output.\n"
				"  {DIR}      - Path of directory with input file\n"
				"  {FILENAME} - Input file name. Same as {NAME}{EXT}\n"
				"  {NAME}     - Input file name without extension\n"
				"  {EXT}      - Input File extension\n",
				cxxopts::value<std::string>(output_pattern_)->default_value("{NAME}.epub"),
				"PATH")
			("c,color", "Use color",
				cxxopts::value<std::string>(color_spec)->default_value("auto"), "yes|no|auto")
			("s,silent", "Supress log messages. Overrides verbose flag")
			("v,verbose", "Increase verbosity of messages. Can be used multiple times.")
			("h,help", "Print help and exit",
				cxxopts::value<bool>(help)->default_value("false"))
			("V,version", "Print version and exit. With -v list also used libraries",
				cxxopts::value<bool>(version)->default_value("false"))
			("files", "files...", cxxopts::value<std::vector<std::string>>(files_))
		;

		options.parse_positional({"files"});

		auto result = options.parse(argc, argv);

		//help = result.count("help") > 0;
		help = result["help"].as<bool>();
		//version = result.count("version") > 0;
		version = result["version"].as<bool>();

		log_level_ = std::min(int(result.count("verbose") + 1), 3);
		if(result.count("silent") > 0) {
			log_level_ = 0;
		}

		if(color_spec == "auto") {
			color_ = use_color();
		} else if(result.count("color")) {
			color_ = opt_is_true(color_spec);
		}

		// set repack and maybe also iterations
		if(result.count("repack")) {
			repack_spec = result["repack"].as<std::string>();

			if(opt_is_num(repack_spec)) {
				int n = std::stoi(repack_spec);
				repack_ = true;
				if(result.count("terations") == 0) {
					iterations_ = n;
				}
			} else {
				repack_ = opt_is_true(repack_spec);
			}
		}

		if(!fix_spec.empty()) {
			fixes_ = fix2num(Fix::None);

			for(auto const& fix : fix_spec) {
				if(fix == "all") {
					fixes_ = fix2num(Fix::All);
					break;
				} else if(fix == "none") {
					fixes_ = fix2num(Fix::None);
					break;
				} else if(fix == "series") {
					fixes_ |= fix2num(Fix::Series);
				} else {
					throw std::runtime_error(fmt::format("Unknown fix name: {}", fix));
				}
			}
		}
	} catch(std::exception const& e) {
		//fmt::print("Error:\n  {}\n\n{}\n", e.what(), options.help());
		fmt::print("{}\n  {}\n\n{}\n",
			xstyled("Error:", fg_red),
			xstyled(e.what(), fg_red),
			options.help()
		);
		return 1;
	}

	print_info();

	if(help) {
		fmt::print("{}\n", options.help());
		return 0;
	}

	if(version) {
		xprint(1, "epub-repack v" VERSION "\n");
		xprint(2, "fmt v{}\n", format_version(FMT_VERSION));
		xprint(2, "cxxopts v{}.{}.{}\n",
			cxxopts::version.major, cxxopts::version.minor, cxxopts::version.patch
		);
		return 0;
	}

	if(files_.empty()) {
		//fmt::print("Error:\n  Missing files to work on...\n\n{}\n", options.help());
		fmt::print("\n\n{}\n",
			xstyled("Error:\n  Missing files to work on...", fg_red), 
			options.help()
		);
		return 1;
	}

	return 0;
}

