cmake_minimum_required (VERSION 2.6)
project (Loader)

#set(CMAKE_CXX_COMPILER gcc)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_COMPILER_FLAG} -lstdc++ -L/usr/lib64 -lm")

link_libraries(m stdc++)

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

#find_library(GLEW NAMES glew1.6)
#find_package(GLEW REQUIRED)
find_path(GLEW_INCLUDE_DIR GL/glew.h)
find_library(GLEW_LIBRARY NAMES GLEW glew32 glew glew32s PATH_SUFFIXES lib64)

set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})
set(GLEW_LIBRARIES ${GLEW_LIBRARY})

#include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
#find_package_handle_standard_args(GLEW
#       	REQUIRED_VARS GLEW_INCLUDE_DIR GLEW_LIBRARY)

mark_as_advanced(GLEW_INCLUDE_DIR GLEW_LIBRARY)
include_directories(${GLEW_INCLUDE_DIR})


find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})


add_executable(test glut_obj.cpp)
target_link_libraries(test ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLUT_LIBRARY})

message(${GLEW_LIBRARIES})
