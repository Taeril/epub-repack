#include "app.hpp"

#include <exception>

int main(int argc, char** argv) {
	App app;
	int ret = 0;

	try {
		ret = app.run(argc, argv);
	} catch(std::exception const& e) {
		// clang-format off

		fmt::print("{}\n  {}\n",
			app.xstyled("Error:", App::fg_red),
			app.xstyled(e.what(), App::fg_red)
		);
		// clang-format on

		ret += 1;
	}

	return ret;
}

