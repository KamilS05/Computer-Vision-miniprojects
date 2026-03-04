
cmake_minimum_required(VERSION 3.10)
project(fsiv_calibAR VERSION 0.1.0)

include(CTest)
enable_testing()

FIND_PACKAGE(OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )


add_executable(fsiv_calibAR  fsiv_main_p3.cpp fsiv_funcs.cpp )

target_link_libraries(fsiv_calibAR ${OpenCV_LIBS} )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)