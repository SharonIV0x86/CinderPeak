set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic")

add_library(CinderPeak STATIC
    src/GraphBase.hpp
    src/Utils.hpp    

)
target_include_directories(CinderPeak PUBLIC include)
