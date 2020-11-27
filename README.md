Google: clang-format, gtest

# Development

## Prerequisites

- CMake installed (includeing CMake CLI tools)

## Local development

```sh
# This will produce the Makefile, CMakeFiles folder and a couple of
# CMake-specific files (e.g. CMakeCache.txt). We don't need to commit them.
# This command will also build the executable so we don't need to execute
# the platform-specific build command explicitly
cmake --build .
```