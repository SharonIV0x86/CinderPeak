@echo off
REM Auto-format only selected folders recursively
REM Update the list in INCLUDED_DIRS if needed

set INCLUDED_DIRS=src examples tests

echo 🔍 Formatting only in folders: %INCLUDED_DIRS%
echo.

for %%d in (%INCLUDED_DIRS%) do (
    if exist %%d (
        for /r %%d %%f in (*.cpp *.hpp *.h *.cc *.cxx) do (
            echo -> Formatting: %%f
            clang-format -i "%%f"
        )
    )
)

echo.
echo ✅ Formatting complete!
