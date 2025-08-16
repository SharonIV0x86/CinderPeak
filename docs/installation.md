# Building CinderPeak

CinderPeak uses SFML (Simple and Fast Multimedia Library) for its interactive visualization engine and integrates Google Test (GTest) for robust unit testing.

The project uses CMake as the primary build system, but alternative build tools like Ninja can also be used seamlessly.

## System and Compiler requirements
- C++17-compatible compiler, GCC 12+, MSVC (with C++17 enabled), or Clang with C++17 support.
- Build system, CMake (latest stable version recommended) or any compatible C++ toolchain.

## Setting Up the Build Directory

Before building, create a separate ``build`` directory in the root of the project:
```sh
mkdir build
cd build
```

This keeps all build artifacts cleanly separated from the source code.

---

# Build Configurations

You can customize your build to include examples and/or tests by passing the appropriate flags to CMake.

### Build with Tests and Examples

```js
cmake .. -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
cmake --build .
```

This will build everything: core library, tests, and example applications.


### Build with Tests Only

```js
cmake .. -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF
cmake --build .
```

This builds just the example programs—great for trying out features without running tests.

---

### Build with Examples Only

```js
cmake .. -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=ON
cmake --build .
```

This configuration is ideal for using CinderPeak as a library dependency in other projects.

---

## Build Configurations for Windows


#### First set up the build directory
   ```js
   mkdir build
   ```
#### Enter the build directory.
   ```js
   cd build
   ```

### Build options :
- **Using NMake** :

   ```js
   cmake .. -G "NMake Makefiles" -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
   ```
   To build the files , run:
   ```js
   nmake
   ```

- **Using MinGW** : 

   ```js
   cmake .. -G "MinGW Makefiles" -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
   ```

   To build the files, run:
   ```js
   mingw32-make
   ```

- **Using Visual Studio** :

   ```js
   cmake .. -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
   cmake --build .
   ```
   To build the files, run:
   ```js
   make -j
   ```
   
### Possible errors:

   If you encounter :
   ```js
   'nmake'  '-?'
   failed with :
   no such file or directory
   ```
   Remove the current build directory
   ```js
   cd ..
   rmdir /s build
   ```
   Recreate the build directory and enter in it.
   ```js
   mkdir build
   cd build
   ```
   Now you can build using another build option. 







## Output Structure

After building, the compiled binaries can be found in the following directories:
- **Examples**: build/bin/examples/
- **Tests**: build/bin/tests/

Make sure all dependencies like SFML and GTest are correctly installed or discoverable by CMake.
