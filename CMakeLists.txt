cmake_minimum_required( VERSION 3.0 )

# Create Project
project( to_vector )
add_executable( to_vector main.cpp )

# Set OpenCVConfig.cmake Search Directory
set( OpenCV_DIR "/usr/local/include/opencv4" )	

# Find Package
find_package( OpenCV REQUIRED )

if( OpenCV_FOUND )
  # Additional Include Directories
  include_directories( ${OpenCV_INCLUDE_DIRS} )

  # Additional Library Directories
  link_directories( ${OpenCV_LIB_DIR} )

  # Additional Dependencies
  target_link_libraries( to_vector ${OpenCV_LIBS} )
endif()
