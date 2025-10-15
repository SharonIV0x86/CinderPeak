#!/usr/bin/env python3

from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter, REMAINDER
from pathlib import Path
import platform
from subprocess import Popen, PIPE
import sys
from typing import List, Any, Optional, Tuple
from shutil import which
import os

FLAG_MAP = {
    "with_tests": ("-DBUILD_TESTS=ON",),
    "with_examples": ("-DBUILD_EXAMPLES=ON",),
    "sanitize": ("-DSANITIZE=ON",),
    "coverage": ("-DBUILD_COVERAGE=ON",),
}

def run(*args: str, msg: Optional[str] = None, verbose: bool = False, **kwargs: Any) -> Popen:
    sys.stdout.flush()
    if verbose:
        print(f"$ {' '.join(args)}")

    p = Popen(args, **kwargs)
    code = p.wait()
    if code != 0:
        err = f"\nfailed to run: {' '.join(args)}\nexit with code: {code}\n"
        if msg:
            err += f"error message: {msg}\n"
        raise RuntimeError(err)
    return p


def run_pipe(*args: str, msg: Optional[str] = None, verbose: bool = False, **kwargs: Any):
    p = run(*args, msg=msg, verbose=verbose, stdout=PIPE, universal_newlines=True, **kwargs)
    return p.stdout


def find_command(command: str, msg: Optional[str] = None) -> str:
    cmd_path = which(command)
    if cmd_path is None:
        raise RuntimeError(msg or f"Command '{command}' not found in PATH")
    return cmd_path

def configure(build_dir: str = "build", build_type: str = "RelWithDebInfo",
              with_tests: bool = False, with_examples: bool = False,
              sanitize: bool = False, coverage: bool = False,
              generator: Optional[str] = None, toolchain: Optional[str] = None,
              cmake_path: str = "cmake", ninja: bool = False,
              D: Optional[List[str]] = None, **kwargs: Any) -> None:

    basedir = Path(__file__).parent.absolute()
    cmake = find_command(cmake_path, msg="CMake is required")
    
    os.makedirs(build_dir, exist_ok=True)
    
    cmake_options = [f"-DCMAKE_BUILD_TYPE={build_type}"]
    
    if with_tests:
        cmake_options.extend(FLAG_MAP["with_tests"])
    if with_examples:
        cmake_options.extend(FLAG_MAP["with_examples"])
    if sanitize:
        cmake_options.extend(FLAG_MAP["sanitize"])
    if coverage:
        cmake_options.extend(FLAG_MAP["coverage"])
    
    if ninja:
        cmake_options.append("-GNinja")
    elif generator:
        cmake_options.append(f"-G{generator}")
    
    if toolchain:
        cmake_options.append(f"-DCMAKE_TOOLCHAIN_FILE={toolchain}")
    
    if D:
        cmake_options.extend([f"-D{opt}" for opt in D])
    
    run(cmake, str(basedir), *cmake_options, verbose=True, cwd=build_dir)
    print(f"\nConfiguration complete. Build directory: {build_dir}")


def build(build_dir: str = "build", jobs: Optional[int] = None,
          target: Optional[str] = None, cmake_path: str = "cmake",
          skip_build: bool = False, **kwargs: Any) -> None:

    if skip_build:
        print("Skipping build as requested.")
        return
    
    if not os.path.exists(build_dir):
        raise RuntimeError(f"Build directory '{build_dir}' not found. Run configure first.")
    
    cmake = find_command(cmake_path, msg="CMake is required")
    
    options = ["--build", build_dir]
    if jobs is not None:
        options.extend(["-j", str(jobs)])
    if target:
        options.extend(["--target", target])
    
    run(cmake, *options, verbose=True)
    print("\nBuild complete.")


def test(build_dir: str = "build", cpp: bool = False, all: bool = False,
         rest: Optional[List[str]] = None, **kwargs: Any) -> None:
    if not os.path.exists(build_dir):
        raise RuntimeError(f"Build directory '{build_dir}' not found.")
    
    ctest = find_command("ctest", msg="CTest is required")
    
    options = ["--test-dir", build_dir, "--output-on-failure"]
    if rest:
        options.extend(rest)
    
    run(ctest, *options, verbose=True)
    print("\nTests complete.")


def clean(build_dir: str = "build", **kwargs: Any) -> None:
    import shutil
    if os.path.exists(build_dir):
        print(f"Removing build directory: {build_dir}")
        shutil.rmtree(build_dir)
        print("Clean complete.")
    else:
        print(f"Build directory '{build_dir}' does not exist.")


def format_code(clang_format_path: str = "clang-format", fix: bool = False, **kwargs: Any) -> None:
    from glob import glob
    basedir = Path(__file__).parent.absolute()
    
    command = find_command(clang_format_path, msg="clang-format is required")
    
    sources = [
        *glob(str(basedir / "src/**/*.hpp"), recursive=True),
        *glob(str(basedir / "src/**/*.cpp"), recursive=True),
        *glob(str(basedir / "tests/**/*.cpp"), recursive=True),
        *glob(str(basedir / "examples/**/*.cpp"), recursive=True),
    ]
    
    if not sources:
        print("No source files found.")
        return
    
    options = ['-i'] if fix else ['--dry-run', '--Werror']
    run(command, *options, *sources, verbose=True)
    print("Format complete." if fix else "Format check complete.")


def check(subcommand: str = None, **kwargs: Any) -> None:
    if subcommand == "format":
        format_code(fix=False, **kwargs)
    elif subcommand == "tidy":
        check_tidy(**kwargs)
    else:
        print("Usage: build.py check {format|tidy}")


def check_tidy(build_dir: str = "build", jobs: Optional[int] = None,
               clang_tidy_path: str = "clang-tidy",
               run_clang_tidy_path: str = "run-clang-tidy",
               fix: bool = False, **kwargs: Any) -> None:
    tidy_command = find_command(clang_tidy_path, msg="clang-tidy is required")
    
    compile_commands = Path(build_dir) / 'compile_commands.json'
    if not compile_commands.exists():
        raise RuntimeError(f"compile_commands.json not found in {build_dir}")
    
    run_command = which(run_clang_tidy_path)
    basedir = Path(__file__).parent.absolute()
    
    if run_command:
        options = ['-p', build_dir, '-clang-tidy-binary', tidy_command]
        if jobs is not None:
            options.append(f'-j{jobs}')
        if fix:
            options.append('-fix')
        options.append('-header-filter=src/|tests/|examples/')
        
        run(run_command, *options, 'src/', verbose=True, cwd=str(basedir))
    else:
        print(f"Warning: {run_clang_tidy_path} not found")
        from glob import glob
        sources = glob(str(basedir / "src/**/*.cpp"), recursive=True)
        options = [f'-p={build_dir}']
        if fix:
            options.append('-fix')
        run(tidy_command, *options, *sources, verbose=True)
    
    print("Tidy check complete.")


def prepare(**kwargs: Any) -> None:
    basedir = Path(__file__).parent.absolute()
    hooks_dir = basedir / "dev" / "hooks"
    git_hooks_dir = basedir / ".git" / "hooks"
    
    if not git_hooks_dir.exists():
        print("Warning: .git/hooks not found.")
        return
    
    if not hooks_dir.exists():
        print(f"No hooks found in {hooks_dir}")
        return
    
    import filecmp
    git_hooks_dir.mkdir(exist_ok=True)
    
    for hook in hooks_dir.iterdir():
        if hook.is_file():
            dst = git_hooks_dir / hook.name
            if dst.exists():
                if filecmp.cmp(hook, dst, shallow=False):
                    print(f"{hook.name} already installed.")
                    continue
            else:
                if platform.system() == "Windows":
                    import shutil
                    shutil.copy2(hook, dst)
                else:
                    dst.symlink_to(hook)
                print(f"{hook.name} installed at {dst}.")
    
    print("Development environment prepared.")


def package(subcommand: str = None, release_version: str = None, **kwargs: Any) -> None:
    if subcommand == "source":
        if not release_version:
            raise RuntimeError("--release-version is required")
        
        git = find_command('git', msg='git is required')
        folder = f'cinderpeak-{release_version}-src'
        tarball = f'{folder}.tar.gz'
        
        print(f"Creating source tarball: {tarball}")
        run(git, 'archive', '--format=tar.gz', f'--output={tarball}',
            f'--prefix={folder}/', 'HEAD', verbose=True)
        print(f"Source package created: {tarball}")
    else:
        print("Usage: build.py package source --release-version VERSION")

if __name__ == '__main__':
    parser = ArgumentParser(
        description="CinderPeak Build Orchestrator",
        formatter_class=ArgumentDefaultsHelpFormatter
    )
    subparsers = parser.add_subparsers(dest='command')
    
    p = subparsers.add_parser('configure', help="Configure build system")
    p.add_argument('build_dir', nargs='?', default='build')
    p.add_argument('--build-type', default='RelWithDebInfo')
    p.add_argument('--with-tests', action='store_true')
    p.add_argument('--with-examples', action='store_true')
    p.add_argument('--sanitize', action='store_true')
    p.add_argument('--coverage', action='store_true')
    p.add_argument('--generator')
    p.add_argument('--toolchain')
    p.add_argument('--cmake-path', default='cmake')
    p.add_argument('--ninja', action='store_true')
    p.add_argument('-D', action='append', metavar='key=value')
    p.set_defaults(func=configure)
    
    p = subparsers.add_parser('build', help="Build project")
    p.add_argument('build_dir', nargs='?', default='build')
    p.add_argument('-j', '--jobs', type=int)
    p.add_argument('--target')
    p.add_argument('--cmake-path', default='cmake')
    p.add_argument('--skip-build', action='store_true')
    p.set_defaults(func=build)
    
    p = subparsers.add_parser('test', help="Run tests")
    p.add_argument('build_dir', nargs='?', default='build')
    p.add_argument('--cpp', action='store_true')
    p.add_argument('--all', action='store_true')
    p.add_argument('rest', nargs=REMAINDER)
    p.set_defaults(func=test)
    
    p = subparsers.add_parser('clean', help="Clean build")
    p.add_argument('build_dir', nargs='?', default='build')
    p.set_defaults(func=clean)
    
    p = subparsers.add_parser('format', help="Format code")
    p.add_argument('--clang-format-path', default='clang-format')
    p.add_argument('--fix', action='store_true')
    p.set_defaults(func=format_code)
    
    p = subparsers.add_parser('check', help="Check code")
    p_sub = p.add_subparsers(dest='subcommand')
    
    p_fmt = p_sub.add_parser('format')
    p_fmt.add_argument('--clang-format-path', default='clang-format')
    p_fmt.set_defaults(func=lambda **a: format_code(**a, fix=False))
    
    p_tidy = p_sub.add_parser('tidy')
    p_tidy.add_argument('build_dir', nargs='?', default='build')
    p_tidy.add_argument('-j', '--jobs', type=int)
    p_tidy.add_argument('--clang-tidy-path', default='clang-tidy')
    p_tidy.add_argument('--run-clang-tidy-path', default='run-clang-tidy')
    p_tidy.add_argument('--fix', action='store_true')
    p_tidy.set_defaults(func=check_tidy)
    
    p.set_defaults(func=check)
    
    p = subparsers.add_parser('prepare', help="Setup dev environment")
    p.set_defaults(func=prepare)
    
    p = subparsers.add_parser('package', help="Package project")
    p_sub = p.add_subparsers(dest='subcommand')
    p_src = p_sub.add_parser('source')
    p_src.add_argument('--release-version', required=True)
    p_src.set_defaults(func=package)
    p.set_defaults(func=package)
    
    args = parser.parse_args()
    
    if not hasattr(args, 'func'):
        parser.print_help()
        sys.exit(1)
    
    arg_dict = dict(vars(args))
    func = arg_dict.pop('func')
    arg_dict.pop('command', None)
    
    try:
        func(**arg_dict)
    except Exception as e:
        print(f"\nError: {e}", file=sys.stderr)
        sys.exit(1)