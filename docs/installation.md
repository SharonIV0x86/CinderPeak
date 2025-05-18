# Building CinderPeak

CinderPeak uses SFML (Simple and Fast Multimedia Library) for its interactive visualization engine and integrates Google Test (GTest) for robust unit testing.

The project uses CMake as the primary build system, but alternative build tools like Ninja can also be used seamlessly.


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

---



## Build with Tests Only

```js
cmake .. -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF
cmake --build .
```

This builds just the example programs—great for trying out features without running tests.

---

## Build with Core Library Only

```js
cmake .. -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=ON
cmake --build .
```

This configuration is ideal for using CinderPeak as a library dependency in other projects.

---

## Output Structure

After building, the compiled binaries can be found in the following directories:
- **Examples**: build/examples/
- **Tests**: build/tests/

Make sure all dependencies like SFML and GTest are correctly installed or discoverable by CMake.