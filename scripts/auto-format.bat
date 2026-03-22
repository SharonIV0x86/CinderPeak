@echo off
set INCLUDED_DIRS=src examples tests

echo Formatting only in folders: %INCLUDED_DIRS%

for %%d in (%INCLUDED_DIRS%) do (
    if exist %%d (
        for /r "%%d" %%f in (*.cpp *.hpp *.h *.cc *.cxx) do (
            echo -> Formatting: %%f
            clang-format -i "%%f"
        )
    )
)

echo.
echo Formatting complete!