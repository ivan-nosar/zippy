Google: clang-format, gtest

# Development

## Prerequisites

- CMake installed (includeing CMake CLI tools)
- CMake Tools (`ms-vscode.cmake-tools`) extension for VS Code installed.

## Local development

1. `Cmd (Ctrl) + Shift + P` -> `CMake: Configure`;
1. `Cmd (Ctrl) + Shift + P` -> `CMake: Select a Kit`;
1. `Cmd (Ctrl) + Shift + P` -> `CMake: Select a Variant` -> `Debug` (for debugging) or `Release` (for releasing);

2.
```sh
# This will produce the Makefile, CMakeFiles folder and a couple of
# CMake-specific files (e.g. CMakeCache.txt). We don't need to commit them.
cmake .

# This will build the executable
make
```