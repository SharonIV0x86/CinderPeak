=#!/bin/bash

# Install pre-commit hooks for CinderPeak
# This script sets up clang-format as pre-commit hook

set -e

echo "Installing pre-commit hooks for CinderPeak..."

# Check if we're in a git repository
if [ ! -d ".git" ]; then
    echo "Error: Not in a git repository"
    exit 1
fi

# Check if clang-format is available
if ! command -v clang-format &> /dev/null; then
    echo "Warning: clang-format not found. Please install clang-format."
    echo "On Ubuntu/Debian: sudo apt install clang-format"
    echo "On macOS: brew install clang-format"
    echo "On Windows: Install LLVM from https://llvm.org/builds/"
fi

# Create hooks directory if it doesn't exist
mkdir -p .git/hooks

# Create pre-commit hook
cat > .git/hooks/pre-commit << 'EOF'
#!/bin/bash

# Pre-commit hook for CinderPeak
# Runs clang-format on staged C++ files

set -e

echo "Running pre-commit checks..."

# Get list of staged C++ files
STAGED_FILES=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(cpp|hpp|h|cc|cxx)$' || true)

if [ -z "$STAGED_FILES" ]; then
    echo "No C++ files to check."
    exit 0
fi

echo "Checking files: $STAGED_FILES"

# Check clang-format
if command -v clang-format &> /dev/null; then
    echo "Running clang-format..."
    for file in $STAGED_FILES; do
        if [ -f "$file" ]; then
            # Check if file needs formatting
            if ! clang-format --dry-run --Werror "$file" &> /dev/null; then
                echo "❌ $file needs formatting"
                echo "Run: clang-format -i $file"
                exit 1
            else
                echo "✅ $file is properly formatted"
            fi
        fi
    done
else
    echo "⚠️  clang-format not found, skipping format check"
fi

echo "✅ All pre-commit checks passed!"
EOF

# Make the hook executable
chmod +x .git/hooks/pre-commit

echo "✅ Pre-commit hooks installed successfully!"
echo ""
echo "The following checks will run on each commit:"
echo "  - clang-format: Ensures code formatting consistency"
echo ""
echo "To bypass hooks (not recommended): git commit --no-verify"
echo "To run hooks manually: .git/hooks/pre-commit"
