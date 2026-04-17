#!/usr/bin/env python3
import subprocess

# Only include these directories
INCLUDED_DIRS = ["src", "examples", "tests"]

# Exclude specific files
EXCLUDE_FILES = {"src/ArialFontDataEmbed.hpp"}

# File extensions
EXTENSIONS = (".cpp", ".hpp", ".h", ".cc", ".cxx")


def get_files():
    """Get tracked files from git matching extensions and directories."""
    try:
        result = subprocess.run(
            ["git", "ls-files"],
            capture_output=True,
            text=True,
            check=True,
        )
        files = result.stdout.splitlines()
    except subprocess.CalledProcessError:
        print("❌ Failed to run git ls-files")
        return []

    filtered = []
    for f in files:
        if not f.endswith(EXTENSIONS):
            continue

        if not any(f.startswith(d + "/") for d in INCLUDED_DIRS):
            continue

        if f in EXCLUDE_FILES:
            continue

        filtered.append(f)

    return filtered


def main():
    print(f"🔍 Formatting only in: {', '.join(INCLUDED_DIRS)}")

    files = get_files()
    if not files:
        print("No files found.")
        return

    print(f"Found {len(files)} files.\n")

    for f in files:
        print(f"-> Formatting: {f}")
        subprocess.run(["clang-format", "-i", f], check=False)

    print("\n✅ Formatting complete!")


if __name__ == "__main__":
    main()