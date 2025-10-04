#!/usr/bin/env python3

import argparse
import os
import platform
import shutil
import subprocess
import sys

CMAKE_FLAG_MAP = {
    "build_type": "CMAKE_BUILD_TYPE",
    "with_tests": "ENABLE_TESTS",
    "with_examples": "ENABLE_EXAMPLES",
    "sanitize": "ENABLE_SANITIZERS",
}

COMMANDS = {}

def register_command(name):
    """Decorator to register a subcommand."""
    def decorator(func):
        COMMANDS[name] = func
        return func
    return decorator

def run(cmd, cwd=None, env=None):
    print(f"Running: {' '.join(cmd)}")
    subprocess.check_call(cmd, cwd=cwd, env=env)

def run_cmake_configure(build_dir, cmake_defs=None, generator=None, toolchain=None, cmake_path="cmake"):
    """Run CMake configure step"""
    os.makedirs(build_dir, exist_ok=True)
    cmd = [cmake_path, "-S", ".", "-B", build_dir]

    if generator:
        cmd += ["-G", generator]
    if toolchain:
        cmd += ["-DCMAKE_TOOLCHAIN_FILE=" + toolchain]

    if cmake_defs:
        for k, v in cmake_defs.items():
            cmd += [f"-D{k}={v}"]

    run(cmd)

def run_cmake_build(build_dir, jobs=None, target=None, cmake_path="cmake"):
    """Run CMake build step"""
    cmd = [cmake_path, "--build", build_dir]
    if target:
        cmd += ["--target", target]
    if jobs:
        cmd += ["--parallel", str(jobs)]
    run(cmd)


def detect_default_generator():
    system = platform.system()
    if system == "Windows":
        if shutil.which("ninja"):
            return "Ninja"
        return "Visual Studio 17 2022"  
    return "Ninja" if shutil.which("ninja") else None

@register_command("configure")
def cmd_configure(args):
    cmake_defs = {}
    for flag, cmake_opt in CMAKE_FLAG_MAP.items():
        value = getattr(args, flag, None)
        if value is not None:
            cmake_defs[cmake_opt] = value
    if args.D:
        for kv in args.D:
            key, val = kv.split("=", 1)
            cmake_defs[key] = val

    run_cmake_configure(
        build_dir=args.build_dir,
        cmake_defs=cmake_defs,
        generator=args.generator or detect_default_generator(),
        toolchain=args.toolchain,
        cmake_path=args.cmake_path,
    )

@register_command("build")
def cmd_build(args):
    run_cmake_build(
        build_dir=args.build_dir,
        jobs=args.jobs,
        target=args.target,
        cmake_path=args.cmake_path,
    )

@register_command("test")
def cmd_test(args):
    # Placeholder
    print("Running tests...")

@register_command("clean")
def cmd_clean(args):
    if os.path.isdir(args.build_dir):
        shutil.rmtree(args.build_dir)
        print(f"Removed build directory {args.build_dir}")

@register_command("format")
def cmd_format(args):
    print("Formatting code...")

@register_command("check")
def cmd_check(args):
    print(f"Running check: {args.check_type}")

@register_command("prepare")
def cmd_prepare(args):
    print("Preparing build environment...")

@register_command("package")
def cmd_package(args):
    print("Packaging artifacts...")

def main():
    parser = argparse.ArgumentParser(description="Project build system")
    parser.add_argument("--cmake-path", default="cmake", help="Path to cmake executable")
    parser.add_argument("--generator", help="CMake generator")
    parser.add_argument("--toolchain", help="CMake toolchain file")
    parser.add_argument("--ninja", action="store_true", help="Use Ninja generator if possible")
    parser.add_argument("-D", action="append", help="Pass -D key=val to CMake")
    parser.add_argument("--jobs", type=int, help="Number of parallel build jobs")
    parser.add_argument("--skip-build", action="store_true", help="Skip build step")

    subparsers = parser.add_subparsers(dest="command", required=True)

    p = subparsers.add_parser("configure")
    p.add_argument("--build-type", choices=["Debug", "Release"], help="Build type")
    p.add_argument("--with-tests", action="store_true")
    p.add_argument("--with-examples", action="store_true")
    p.add_argument("--sanitize", action="store_true")
    p.add_argument("--build-dir", default="build")

    p = subparsers.add_parser("build")
    p.add_argument("--target", help="Build target")
    p.add_argument("--jobs", type=int)
    p.add_argument("--build-dir", default="build")

    p = subparsers.add_parser("test")
    p.add_argument("--cpp", action="store_true")
    p.add_argument("--all", action="store_true")
    p.add_argument("--build-dir", default="build")

    p = subparsers.add_parser("check")
    p.add_argument("check_type", choices=["tidy", "format"])

    p = subparsers.add_parser("format")
    p.add_argument("--fix", action="store_true")

    p = subparsers.add_parser("clean")
    p.add_argument("--build-dir", default="build")

    subparsers.add_parser("prepare")

    subparsers.add_parser("package")

    args = parser.parse_args()

    if args.command in COMMANDS:
        COMMANDS[args.command](args)
    else:
        parser.print_help()
        sys.exit(1)

if __name__ == "__main__":
    main()
