#!/usr/bin/env python3
import os
import subprocess

# Only these folders (and their subfolders) will be formatted
INCLUDED_DIRS = ["src", "examples", "tests"]

# File extensions to check
EXTENSIONS = (".cpp", ".hpp", ".h", ".cc", ".cxx")

def get_all_files():
    """Collect all source files under INCLUDED_DIRS recursively."""
    for root_dir in INCLUDED_DIRS:
        if not os.path.exists(root_dir):
            continue
        for dirpath, _, filenames in os.walk(root_dir):
            for f in filenames:
                if f.endswith(EXTENSIONS):
                    yield os.path.join(dirpath, f)

def main():
    print(f"🔍 Formatting only in folders: {', '.join(INCLUDED_DIRS)}")
    files = list(get_all_files())
    if not files:
        print("No files found to format.")
        return

    print(f"Found {len(files)} files. Running clang-format...\n")

    for file in files:
        print(f"-> Formatting: {file}")
        subprocess.run(["clang-format", "-i", file], check=False)

    print("\n✅ Formatting complete!")

if __name__ == "__main__":
    main()
