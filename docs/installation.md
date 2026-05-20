# Building CinderPeak

CinderPeak uses **CMake** as its primary build system and **Google Test (GTest)** for unit testing. The library is header-only — no separate compilation step is needed to use it.

---

## System Requirements

| Component | Requirement |
|:----------|:------------|
| C++ Standard | C++17 or later |
| Compiler | GCC 12+, Clang 14+, MSVC 2019+ (with `/std:c++17`) |
| Build System | CMake 3.14+ |
| Test Framework | Google Test (GTest) — only required for tests |

---

## Project Layout After Build

```
build/
├── bin/
│   ├── examples/     ← compiled example programs
│   └── tests/        ← compiled test binaries
```

---

## Linux / macOS

### Prerequisites

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install cmake g++ libgtest-dev
```

**macOS (Homebrew):**
```bash
brew install cmake googletest
```

### Build Steps

```bash
# 1. Clone the repository
git clone https://github.com/SharonIV0x86/CinderPeak.git
cd CinderPeak

# 2. Create the build directory
mkdir build && cd build

# 3. Configure (choose your options)
cmake .. -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON

# 4. Build
cmake --build .
```

### Build Options

| CMake Flag | Effect |
|:-----------|:-------|
| `-DBUILD_TESTS=ON` | Build unit tests |
| `-DBUILD_TESTS=OFF` | Skip unit tests |
| `-DBUILD_EXAMPLES=ON` | Build example programs |
| `-DBUILD_EXAMPLES=OFF` | Skip examples |

**Examples only (no tests needed):**
```bash
cmake .. -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=ON
cmake --build .
```

**Tests only (CI use):**
```bash
cmake .. -DBUILD_TESTS=ON -DBUILD_EXAMPLES=OFF
cmake --build .
```

---

## Windows

### Option A — Visual Studio

```cmd
mkdir build
cd build
cmake .. -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
cmake --build .
```

### Option B — NMake (MSVC command prompt)

```cmd
mkdir build && cd build
cmake .. -G "NMake Makefiles" -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
nmake
```

### Option C — MinGW

```cmd
mkdir build && cd build
cmake .. -G "MinGW Makefiles" -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
mingw32-make
```

### Common Windows Error

If you see:
```
'nmake' '-?' failed with: no such file or directory
```

This means NMake isn't on your PATH. Fix:
1. Open "Developer Command Prompt for Visual Studio"
2. Or remove the build dir and try a different generator (`MinGW Makefiles` or let CMake auto-detect)

```cmd
cd ..
rmdir /s build
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
cmake --build .
```

---

## Using CinderPeak as a Header-Only Library

Since CinderPeak is header-only, you can also use it without CMake:

```cpp
// Just point your compiler at the src/ directory
// g++ -std=c++17 -I/path/to/CinderPeak/src my_program.cpp -o my_program
```

```cpp
#include "CinderPeak.hpp"
using namespace CinderPeak;

int main() {
    CinderGraph<int, int> g;
    g.addVertex(1);
    g.addVertex(2);
    g.addEdge(1, 2, 10);
    return 0;
}
```

---

## Running Tests

After building with `-DBUILD_TESTS=ON`:

```bash
cd build
ctest --output-on-failure
# or run individual test binaries directly:
./bin/tests/<test_binary_name>
```

---

## Running Examples

After building with `-DBUILD_EXAMPLES=ON`:

```bash
./build/bin/examples/PrimitiveGraph
./build/bin/examples/addEdge_usage
./build/bin/examples/toDot_usage
```

For DOT file visualization (requires [Graphviz](https://graphviz.org/)):
```bash
dot -Tpng g1_directed.dot -o g1_directed.png
```
