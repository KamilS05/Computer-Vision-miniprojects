
cmake_minimum_required(VERSION 3.10)
project(fsiv_smartfilter VERSION 0.1.0)

include(CTest)
enable_testing()

FIND_PACKAGE(OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )


add_executable(fsiv_smartfilter  fsiv_main_p4.cpp fsiv_funcs.cpp )

target_link_libraries(fsiv_smartfilter ${OpenCV_LIBS} )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)