@echo off
setlocal enabledelayedexpansion

REM Included directories
set INCLUDED_DIRS=src examples tests

REM Excluded files (space-separated)
set EXCLUDE_FILES=src\ArialFontDataEmbed.hpp

echo 🔍 Formatting only in: %INCLUDED_DIRS%
echo.

for %%d in (%INCLUDED_DIRS%) do (
    if exist %%d (
        for /r %%d %%f in (*.cpp *.hpp *.h *.cc *.cxx) do (

            set "skip=false"
            for %%e in (%EXCLUDE_FILES%) do (
                if "%%f"=="%%e" set "skip=true"
            )

            if "!skip!"=="false" (
                echo -> Formatting: %%f
                clang-format -i "%%f"
            )
        )
    )
)

echo.
echo ✅ Formatting complete!