# Development Setup

This document describes how to set up the development environment for CinderPeak.

## Pre-commit Hooks

CinderPeak uses pre-commit hooks to ensure code quality and consistency. The hooks run:

- **clang-format**: Ensures consistent code formatting
- **clang-tidy**: Performs static analysis and linting

### Installation

#### Linux/macOS:
```bash
# Install clang tools
sudo apt install clang-format clang-tidy  # Ubuntu/Debian
brew install clang-format llvm            # macOS

# Install hooks
./scripts/install-hooks.sh
```

#### Windows:
```cmd
# Install LLVM (includes clang-format and clang-tidy)
# Download from: https://llvm.org/builds/

# Install hooks
scripts\install-hooks.bat
```

### Usage

Once installed, the hooks will automatically run on each commit. If any issues are found, the commit will be blocked until they are fixed.

To run hooks manually:
```bash
# Linux/macOS
.git/hooks/pre-commit

# Windows
.git\hooks\pre-commit.bat
```

To bypass hooks (not recommended):
```bash
git commit --no-verify
```

## Continuous Integration

The project uses GitHub Actions for continuous integration with the following jobs:

- **build-linux**: Builds on Ubuntu using GCC
- **build-macos**: Builds on macOS using Clang
- **build-windows**: Builds on Windows using MSVC with Ninja generator

All builds use:
- CMake 3.20+
- C++17 standard
- Release configuration
- Tests and examples disabled for CI builds

## Code Style

The project follows the existing `.clang-format` and `.clang-tidy` configurations in the repository root.
