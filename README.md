
# 📚 epub-repack

Command line tool to repack epub files with zopfli reducing their size and to apply some fixes.

## Usage

```
epub-repack (v0.0.1):
  Fix and repack epub files

Usage:
  epub-repack [OPTION...] FILE...

  -f, --fix NAME,...       Apply fixes:
                             all      - apply all fixes
                             none     - do not apply fixes
                             series   - fix series information for PocketBook
  -r, --repack yes|no|N    Repack file; with integer value N it's alias for: -r yes -i N (default: yes)
  -i, --iterations N       Number of iteration (default: 16)
  -o, --output PATH        Output patern. Path with placeholder for output.
                           If pattern ends with '/' or output is directory then appends {FILENAME}
                             {DIR}      - Path of directory with input file
                             {FILENAME} - Input file name. Same as {NAME}{EXT}
                             {NAME}     - Input file name without extension
                             {EXT}      - Input File extension
                            (default: {NAME}.epub)
  -c, --color yes|no|auto  Use color (default: auto)
  -s, --silent             Supress log messages. Overrides verbose flag
  -v, --verbose            Increase verbosity of messages. Can be used multiple times.
  -h, --help               Print help and exit
  -V, --version            Print version and exit. With -v list also used libraries
```

## Fixes

### Series

Pocketbook can use information about series saved in ebooks metadata by
[Calibre](https://calibre-ebook.com/ "E-book management suite") to automatically group ebooks.

But it requires this information to be last thing in metadata and with attributes with correct order.

This fix ensures that, if provided, series metadata would be seen by Pocketbook.

## Build

### Requirements

Epub-repack needs C++ compiler supporting at least C++17 standard for example due to use of `std::filesystem`.

All dependencies can be handled by [conan](https://conan.io/ "The open-source C and C++ package manager").
See [conanfile.txt](conanfile.txt) for information about versions of libraries used in development..

Used libraries:

 * [fmt](https://fmt.dev) - Formatting library providing a fast and safe alternative to C stdio and C++ iostreams.
 * [cxxopts](https://github.com/jarro2783/cxxopts) - Lightweight C++ command line option parser.
 * [pugixml](https://pugixml.org) - Light-weight, simple and fast XML parser for C++ with XPath support.
 * [libdeflate](https://github.com/ebiggers/libdeflate) - Heavily optimized library for DEFLATE/zlib/gzip compression and decompression.
 * [zopfli](https://github.com/google/zopfli) - Compression library to perform very good, but slow, deflate or zlib compression.

### Build instructions

Example of building on Linux system using conan, cmake, ninja and gcc.

But it's quite generic that it might help to build in other environments.

```sh
build_dir=build
conan install . -of "$build_dir/conan" --build=missing
cmake -B "$build_dir" -G Ninja -DCMAKE_BUILD_TYPE=Release --toolchain "$build_dir/conan/conan_toolchain.cmake"
cmake --build "$build_dir" --config Release
```

## License

Epub-repack is distributed under [MIT license](LICENSE).

