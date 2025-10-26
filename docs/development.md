# Developer Guide: Building and Testing Locally

Welcome! This guide explains how to set up your local environment, build, test, and format the CinderPeak project.

We use a Python script, `build.py`, to orchestrate all common tasks like configuration, compilation, and testing.

## 1. Prerequisites

Before you begin, please ensure you have the following tools installed on your system.

- **Git:** For cloning the repository.
- **Python 3.x:** Required to run the `build.py` script.
- **A modern C++ Compiler:** (C++17/C++20 compatible).
- **CMake:** The cross-platform build system.
- **Ninja:** (Recommended) A fast, small build tool that works with CMake.
- **Clang Tools:** `clang-format` and `clang-tidy` are required for formatting and linting.

### Prerequisite Installation

#### 🖥️ Windows

1.  **Compiler/Build Tools:** Install **Visual Studio 2019 (or newer)**. In the installer, select the "**Desktop development with C++**" workload. This includes the MSVC compiler, CMake, and Git.
2.  **Python:** Install Python 3 from [python.org](https://www.python.org/) or the Microsoft Store.
3.  **Ninja:** Download the `ninja.exe` binary from [its GitHub releases page](https://github.com/ninja-build/ninja/releases) and place it in a directory included in your system's `PATH`.
4.  **Clang Tools:** Install the **LLVM tools**. Download the installer from the [LLVM GitHub releases page](https://github.com/llvm/llvm-project/releases).

#### 🍎 macOS

1.  **Compiler/Git:** Run `xcode-select --install` in your terminal to get the Apple Clang compiler and Git.
2.  **Other Tools (via Homebrew):**

    ```bash
    # Install Homebrew if you don't have it: /bin/bash -c "$(curl -fsSL [https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh](https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh))"

    # Install Python, CMake, Ninja, and LLVM (for clang-format/tidy)
    brew install python cmake ninja llvm
    ```

#### 🐧 Linux (Ubuntu/Debian)

```bash
# Install all required tools
sudo apt update
sudo apt install build-essential python3 python3-pip cmake ninja-build clang-format clang-tidy
```

## 2\. The `build.py` Orchestrator

The `build.py` script at the root of the repository is the primary tool for all development tasks. It standardizes configuration, building, testing, and formatting.

- On macOS/Linux, you will run it as: `./build.py`
- On Windows (PowerShell/CMD), you will run it as: `.\build.py` or `python build.py`

You can see all available commands by running:

```bash
# macOS / Linux
./build.py --help

# Windows
.\build.py --help
```

## 3\. Configuration

The first step is to configure the project using CMake. The `configure` command generates the build files inside the `build/` directory.

```bash
# macOS / Linux
./build.py configure

# Windows
.\build.py configure
```

### Configuration Options

You can pass flags to the `configure` command to customize the build:

- `--with-tests`: (Default: on) Include the Google Test suite in the build.
- `--with-examples`: (Default: on) Include the usage examples in the build.
- `--release`: Configure a release build with optimizations.

**Example:** Configure a release build _without_ examples:

```bash
# macOS / Linux
./build.py configure --release --no-with-examples

# Windows
.\build.py configure --release --no-with-examples
```

## 4\. Building

Once configured, you can compile the project using the `build` command.

```bash
# macOS / Linux
./build.py build

# Windows
.\build.py build
```

### Build Options

- `--target <name>`: Build a specific target (e.g., the main library or a specific test).
- `--jobs <number>`: Specify the number of parallel build jobs. (Defaults to the number of CPU cores).

**Example:** Build only the tests using 8 parallel jobs:

```bash
# macOS / Linux
./build.py build --target CinderPeakTests --jobs 8

# Windows
.\build.py build --target CinderPeakTests --jobs 8
```

## 5\. Testing

After building, you can run the entire GTest suite using the `test` command. This will execute the tests via `ctest`.

```bash
# macOS / Linux
./build.py test

# Windows
.\build.py test
```

**Note:** This command requires the project to have been configured with tests enabled (`--with-tests`, which is the default).

## 6\. Cleaning & Formatting

### Cleaning the Build

To remove the `build/` directory and all build artifacts, use the `clean` command.

```bash
# macOS / Linux
./build.py clean

# Windows
.\build.py clean
```

### Code Formatting (clang-format)

We use `clang-format` to maintain a consistent code style.

```bash
# macOS / Linux
./build.py format

# Windows
.\build.py format
```

### Linting (Static Analysis)

We use `clang-tidy` to check for common programming errors and style issues.

```bash
# macOS / Linux
./build.py check-tidy

# Windows
.\build.py check-tidy
```

### Pre-commit Hooks (Recommended)

To automatically run `format` and `check-tidy` before each commit, you can install the Git pre-commit hooks. This is highly recommended.

```bash
# macOS / Linux
./scripts/install-hooks.sh

# Windows (CMD or PowerShell)
scripts\install-hooks.bat
```

If you commit and the hooks fail, fix the reported issues and `git add` the files again.

## 7\. End-to-End Workflow

For a fresh start, you can configure, build, and test the project all at once using the `all` command.

This is perfect for verifying your setup or checking your changes before pushing.

```bash
# macOS / Linux
./build.py all

# Windows
.\build.py all
```

**Example:** Run a full _release_ build and test:

```bash
# macOS / Linux
./build.py all --release

# Windows
.\build.py all --release
```

## 8\. Continuous Integration (CI)

The project uses GitHub Actions for continuous integration. The CI jobs build the project on Linux (GCC), macOS (Clang), and Windows (MSVC).

Please run `./build.py format` (or `.\build.py format`), `./build.py check-tidy`, and `./build.py all` locally before pushing your changes. This will help ensure your contribution passes CI on the first try.

```

```
