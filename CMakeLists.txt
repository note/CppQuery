cmake_minimum_required (VERSION 2.6)
project (CppQueryTest)

set (VERSION 0.1)

include_directories ("${PROJECT_SOURCE_DIR}/src", "${PROJECT_SOURCE_DIR}/tests")
add_subdirectory (src)
add_subdirectory (tests)
