cmake_minimum_required(VERSION 3.10)
project(composeIllum VERSION 0.1.0)

include(CTest)
enable_testing()

FIND_PACKAGE(OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(composeIllum main.cpp functions.h functions.cpp)

target_link_libraries( composeIllum ${OpenCV_LIBS} )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)