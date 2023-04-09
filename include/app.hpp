#ifndef HEADER_APP_HPP
#define HEADER_APP_HPP

#include <vector>
#include <string>

#include <fmt/core.h>
#include <fmt/color.h>

#include "zip.hpp"

class App {
public:
	int run(int argc, char** argv);

	void print_info();

	enum class Fix : unsigned {
		None = 0,
		All = ~0u,

		Series = 1 << 0,
	};

private:
	std::vector<std::string> files_;
	std::string output_pattern_ = "{NAME}.epub";
	int log_level_ = 1;
	bool color_ = true;
	bool repack_ = true;
	int iterations_ = 16;
	unsigned fixes_ = ~0u;

	int args(int argc, char** argv);

	void fix_series(Zip& zip);

	void save_zip(Zip& zip, std::string const& filename);

	template<typename S, typename... Args>
	inline void xprint(int level, const S& format_str, const Args&... args) {
		if(log_level_ > 0 && level <= log_level_) {
			fmt::print(format_str, args...);
		}
	}

	template<typename T>
	inline auto xstyled(const T& value, fmt::text_style ts) -> decltype(fmt::styled(value, ts)) {
		return fmt::styled(value, color_ ? ts : fmt::text_style{});
	}

	inline fmt::text_style xcolor(fmt::text_style ts) {
		return color_ ? ts : fmt::text_style{};
	}

	static FMT_CONSTEXPR fmt::text_style fg_blue = fmt::fg(fmt::terminal_color::blue);
	static FMT_CONSTEXPR fmt::text_style fg_bright_black = fmt::fg(fmt::terminal_color::bright_black);
	static FMT_CONSTEXPR fmt::text_style fg_bright_blue = fmt::fg(fmt::terminal_color::bright_blue);
	static FMT_CONSTEXPR fmt::text_style fg_bright_cyan = fmt::fg(fmt::terminal_color::bright_cyan);
	static FMT_CONSTEXPR fmt::text_style fg_bright_green = fmt::fg(fmt::terminal_color::bright_green);
	static FMT_CONSTEXPR fmt::text_style fg_bright_magenta = fmt::fg(fmt::terminal_color::bright_magenta);
	static FMT_CONSTEXPR fmt::text_style fg_bright_red = fmt::fg(fmt::terminal_color::bright_red);
	static FMT_CONSTEXPR fmt::text_style fg_bright_white = fmt::fg(fmt::terminal_color::bright_white);
	static FMT_CONSTEXPR fmt::text_style fg_bright_yellow = fmt::fg(fmt::terminal_color::bright_yellow);
	static FMT_CONSTEXPR fmt::text_style fg_cyan = fmt::fg(fmt::terminal_color::cyan);
	static FMT_CONSTEXPR fmt::text_style fg_green = fmt::fg(fmt::terminal_color::green);
	static FMT_CONSTEXPR fmt::text_style fg_magenta = fmt::fg(fmt::terminal_color::magenta);
	static FMT_CONSTEXPR fmt::text_style fg_red = fmt::fg(fmt::terminal_color::red);
	static FMT_CONSTEXPR fmt::text_style fg_white = fmt::fg(fmt::terminal_color::white);
	static FMT_CONSTEXPR fmt::text_style fg_yellow = fmt::fg(fmt::terminal_color::yellow);
};

#endif /* HEADER_APP_HPP */

