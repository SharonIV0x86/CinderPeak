# Building CinderPeak

CinderPeak uses SFML (Simple Fast Multimedia Library) as a dependency for its visualization engine. Additionally Google's Testing Framework (GTest) is also used for extensive testing.


CinderPeak uses CMake as a build system, however any other build system like Ninja can also be used.

## Building with Tests
First make sure you have created a build directory in the root level of the project.

```sh
mkdir build
cd build
```

Once you have entered in the build folder you can choose whether to build the tests and examples or not.

---

### Building with Examples and Tests
```js
cmake .. -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
cmake --build .
```
Executing above commands will build the CinderPeak with both tests and examples.

---

### Building with Tests only
```js
cmake .. -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF
cmake --build .
```
Executing above commands will build the CinderPeak with only examples

---

### Building with Examples only
```js
cmake .. -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=ON
cmake --build .
```

Executing above commands will build examples only.

---

Once you have built the CinderPeak with your desired configuration the tests and examples are stored in the folder ``build/examples`` and ``build/tests``