cmake_minimum_required(VERSION 3.10)
project(to_vector)

# Find openCV
find_package(OpenCV REQUIRED)

# Create project
add_executable(to_vector main.cpp 
			   src/to_vector.cpp)
target_include_directories(to_vector PRIVATE include)

target_link_libraries(to_vector PRIVATE ${OpenCV_LIBS})

# Create testing
add_executable(to_vector_test test.cpp 
			   src/to_vector.cpp)
target_include_directories(to_vector_test PRIVATE include)

target_link_libraries(to_vector_test PRIVATE ${OpenCV_LIBS})

