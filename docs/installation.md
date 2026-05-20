# Building CinderPeak

CinderPeak is a fast and efficient, open-source C++ graph library built to handle a wide range of graph types.

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

## Fast Workflows with `build.py`

CinderPeak includes a `build.py` script to automate builds and testing for faster workflows. You can use this script as a convenient alternative to raw CMake commands.

```bash
# Build the whole project automatically with 8 parallel jobs
python build.py all --with-tests --with-examples -j8

# Or step-by-step:
# Configure the project with tests and examples
python build.py configure --with-tests --with-examples

# Build the configured project
python build.py build -j8

# Run all tests cleanly
python build.py test
```

While `build.py` simplifies the process, the raw CMake commands are also detailed below for manual control.

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

# 2. Configure the project
cmake -S . -B build -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON

# Note: Many developers use Ninja with modern CMake projects.
# If you have Ninja installed, CMake often uses it by default, 
# or you can explicitly specify it:
# cmake -S . -B build -G Ninja -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON

# 3. Build the project
cmake --build build
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
cmake -S . -B build -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=ON
cmake --build build
```

**Tests only (CI use):**
```bash
cmake -S . -B build -DBUILD_TESTS=ON -DBUILD_EXAMPLES=OFF
cmake --build build
```

---

## Windows

### Option A — Visual Studio

```cmd
cmake -S . -B build -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
cmake --build build
```

### Option B — NMake (MSVC command prompt)

```cmd
cmake -S . -B build -G "NMake Makefiles" -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
cmake --build build
```

### Option C — MinGW

```cmd
cmake -S . -B build -G "MinGW Makefiles" -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
cmake --build build
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
rmdir /s build
cmake -S . -B build -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
cmake --build build
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
# or run individual test binaries directly 
# (<test_binary_name> is a placeholder, e.g., addEdge_test):
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
