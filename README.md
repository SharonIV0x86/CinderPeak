# CinderPeak: Work In Progress.
CinderPeak is a high-performance, modern C++ graph library designed to support directed, undirected, and mixed graphs with customizable vertex and edge types. The library will feature an integrated visualization engine using SFML (Simple and Fast Multimedia Library) to provide graph rendering. CinderPeak aims to be a comprehensive tool for graph theory applications, offering both algorithmic functionality and visual insights.


## Development Phases
### Phase 1 : Initialization (1 week)
The phase one includes laying out the foundations of the project, phase 1 includes the following tasks.
<br>
1. **Project Structure**: We need to plan a scalable project structure which is easy to work with and is scalable in future, and can be easily intergated with a build system like **CMAKE**
2. **Build System**: We will be using **Cmake** as a build system for cross platform compilation and for faster development.
3. **Tech Stack**: We will be using modern C++ standards specifically the whole project will be developed in **Modern C++17** standard or above, possibly **C++20** for templates and concepts.
4. **Performance Concerns**: To write faster and optimized code we need to use modern C++ features such as smart pointers and use STL for storage and algorithms. While designing we also need to use OOP concepts for structuring of code, however this designing part will be discussed and implemted in upcoming phases. With perfomance in mind we also have to keep the intuitive API design in the back of our head.
5. **Generics**: The whole project will be fully templated and will utilize the C++ templates in a large amount, almost every class will be templated to provide **flexibility and customizations for end user**.
6. **Testing, Documentation and examples**: This is one of the major parts and one of the easiest, writing documentation will be pretty straight forward, to host the documentation we will be using **docusaurus**. 
<br>**Testing** is very important therefore we will be using and writing tests since the starting of the development for each feature that we feel is correct. Testing will make the development faster by ensuring that features A,B are not accidentally broken when working on the feature C. We will use **GTest** for writing tests.
7. **Thread Safety**: Cinder Peak is meant to be portable and easy to be integrated into larger projects, we need to ensure that the library is thread safe, means we need to ensure our code runs correctly when multiple threads access shared resources simultaneously. In short our library should work well with some other asynchronous code.

# Possible Project Structure
```
/CinderPeak                 # Project root
│── /src                    # Source files
│   ├── GraphBase.hpp       # Abstract base class
│   ├── GraphList.hpp       # Adjacency List
│   ├── GraphMatrix.hpp     # Adjacency Matrix
│   ├── GraphFactory.hpp    # Factory for graph types
│   ├── Utils.hpp           # Utility functions
│── /include                # Public API headers
│   ├── CinderPeak.hpp      # Main API (single entry point)
│── /tests                  # Unit tests
│   ├── test_GraphList.cpp  
│   ├── test_GraphMatrix.cpp  
│── /benchmarks             # Benchmarking code
│── /docs                   # Documentation (Docusaurus)
│── /examples               # Sample usage
│── CMakeLists.txt          # Build system config
│── README.md               # Documentation
│── LICENSE                 # License file
```