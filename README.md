# Zippy

A pet project oriented on learning of common software algorithms: hashing, compressing, encryption.
Written on C++ using the standard library only.

## Development

### Prerequisites

- CMake installed (includeing CMake CLI tools)

### Local development

```sh
# This will produce the Makefile, CMakeFiles folder and a couple of
# CMake-specific files (e.g. CMakeCache.txt). We don't need to commit them.
cmake .

# This command will build the executable. This is platform specific one so
# you have to keep it mind when you'll try to run the build on Windows
make
```

## Testing

TODO: Add description
use node 14 (via nvm)

TODO: Setup CI (GitHub Actions)

## Release

Execute the following command in order to build the release version of the program:

```sh
cmake -DCMAKE_BUILD_TYPE=Release . && make
```