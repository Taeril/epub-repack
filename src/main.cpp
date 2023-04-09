#include "version.hpp"

#include <fmt/core.h>

// version
std::string version() { return VERSION; }
unsigned version_major() { return VERSION_MAJOR; }
unsigned version_minor() { return VERSION_MINOR; }
unsigned version_patch() { return VERSION_PATCH; }
unsigned version_number() {
	return VERSION_MAJOR * 10000 + VERSION_MINOR * 100 + VERSION_PATCH;
}

inline void print_version(const char* name, unsigned int version) {
	fmt::print("{} v{}.{}.{}\n", name,
		((version / 10000) % 100),
		((version / 100) % 100),
		(version % 100)
	);
}

int main(int argc, char** argv) {
	fmt::print("epub-repack v" VERSION "\n");
	print_version("fmt", FMT_VERSION);

	return 0;
}

