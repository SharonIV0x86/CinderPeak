@echo off
REM Install pre-commit hooks for CinderPeak (Windows batch version)
REM This script sets up clang-format as a pre-commit hook

echo Installing pre-commit hook for CinderPeak...

REM Check if we're in a git repository
if not exist ".git" (
    echo Error: Not in a git repository
    exit /b 1
)

REM Check if clang-format is available
where clang-format >nul 2>&1
if %errorlevel% neq 0 (
    echo Warning: clang-format not found. Please install LLVM from https://llvm.org/builds/
)

REM Create hooks directory if it doesn't exist
if not exist ".git\hooks" mkdir ".git\hooks"

REM Create pre-commit hook (Windows batch version)
(
echo @echo off
echo REM Pre-commit hook for CinderPeak
echo REM Runs clang-format on staged C++ files
echo.
echo echo Running pre-commit checks...
echo.
echo REM Get list of staged C++ files
echo for /f "delims=" %%i in ('git diff --cached --name-only --diff-filter=ACM ^| findstr /r "\.cpp$ \.hpp$ \.h$ \.cc$ \.cxx$"'^) do (
echo     echo Checking: %%i
echo     
echo     REM Check clang-format
echo     where clang-format ^>nul 2^>^&1
echo     if %%errorlevel%% equ 0 (
echo         clang-format --dry-run --Werror "%%i" ^>nul 2^>^&1
echo         if %%errorlevel%% neq 0 (
echo             echo ❌ %%i needs formatting
echo             echo Run: clang-format -i "%%i"
echo             exit /b 1
echo         ^) else (
echo             echo ✅ %%i is properly formatted
echo         ^)
echo     ^) else (
echo         echo ⚠️  clang-format not found, skipping format check
echo     ^)
echo ^)
echo.
echo echo ✅ All pre-commit checks passed!
) > ".git\hooks\pre-commit.bat"

echo ✅ Pre-commit hook installed successfully!
echo.
echo The following check will run on each commit:
echo   - clang-format: Ensures code formatting consistency
echo.
echo To bypass hook (not recommended^): git commit --no-verify
echo To run hook manually: .git\hooks\pre-commit.bat
