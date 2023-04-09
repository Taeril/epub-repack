
# epub-repack

## Build

```sh
build_dir=build
conan install . -of "$build_dir/conan" --build=missing
cmake -B "$build_dir" -G Ninja -DCMAKE_BUILD_TYPE=Release --toolchain "$build_dir/conan/conan_toolchain.cmake"
cmake --build "$build_dir"
```

