cmake_minimum_required( VERSION 3.0 )

# Create Project
project( to_vector )
add_executable( to_vector main.cpp )

# Find Package
find_package( OpenCV REQUIRED )

if( OpenCV_FOUND )
  include_directories( ${OpenCV_INCLUDE_DIRS} )

  link_directories( ${OpenCV_LIB_DIR} )

  target_link_libraries( to_vector ${OpenCV_LIBS} )
endif()
