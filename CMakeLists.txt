cmake_minimum_required( VERSION 3.0 )
set( CMAKE_TOOLCHAIN_FILE "/media/alexe/win/vcpkg/scripts/buildsystems/vcpkg.cmake" )

# Create Project
project( solution )
add_executable( solution solution.cpp )

# Set OpenCVConfig.cmake Search Directory
set( OpenCV_DIR )
if( NOT CMAKE_CL_64 )
  set( OpenCV_DIR "/media/alexe/win/vcpkg/installed/x86-windows/share/opencv4" )
else()
  set( OpenCV_DIR "/media/alexe/win/vcpkg/installed/x64-windows/share/opencv4" )
endif()

# Find Package
find_package( OpenCV REQUIRED )

if( OpenCV_FOUND )
  # Additional Include Directories
  include_directories( ${OpenCV_INCLUDE_DIRS} )

  # Additional Library Directories
  link_directories( ${OpenCV_LIB_DIR} )

  # Additional Dependencies
  target_link_libraries( solution ${OpenCV_LIBS} )
endif()
