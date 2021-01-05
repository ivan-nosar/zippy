Google: clang-format, gtest

# Development

## Prerequisites

- CMake installed (includeing CMake CLI tools)

## Local development

```sh
# This will produce the Makefile, CMakeFiles folder and a couple of
# CMake-specific files (e.g. CMakeCache.txt). We don't need to commit them.
cmake .

# This command will build the executable. This is platform specific one so
# you have to keep it mind when you'll try to run the build on Windows
make
```