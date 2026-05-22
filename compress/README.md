# cxfileio

[![CI](https://github.com/cxwx/cxcompress/actions/workflows/ci.yml/badge.svg)](https://github.com/cxwx/cxcompress/actions/workflows/ci.yml)

A lightweight C++ library for reading plain and compressed files transparently.

Supported formats: `.gz`, `.bz2`, `.xz`, `.zst`, `.zlib`, and plain files.

## Build

```bash
mkdir build && cd build
cmake ..
make
```

Requires: C++17, Boost (iostreams).

```bash
# macOS
brew install boost

# Ubuntu/Debian
sudo apt install libboost-iostreams-dev
```

## Usage

### From file (auto-detect compression by extension)

```cpp
#include "cxFileIO.hh"
#include <iostream>

int main() {
  cxfunc::cxFileIO file("data.csv.gz");
  std::string line;
  while (std::getline(file.stream(), line)) {
    std::cout << line << '\n';
  }
}
```

### From file (manual format)

```cpp
cxfunc::cxFileIO file("data.bin", cxfunc::cxFileIOFormat::kGzip);
```

### From any istream

```cpp
std::istringstream ss("compressed content");
cxfunc::cxFileIO io(ss, cxfunc::cxFileIOFormat::kPlain);
```

### From stdin

```cpp
cxfunc::cxFileIO file;  // no argument → reads from std::cin
std::string line;
while (std::getline(file.stream(), line)) {
  std::cout << line << '\n';
}
```

## Example program

```bash
cd build
./example ../test.txt.gz
```
