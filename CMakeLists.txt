cmake_minimum_required (VERSION 2.6)
project (CppQueryTest)

set (VERSION 0.1)

set(CppQueryTest_SRCS test.cpp)

include_directories ("${PROJECT_SOURCE_DIR}/src") 
add_subdirectory (src)
set(EXTRA_LIBS ${EXTRA_LIBS} CppQuery)

add_executable (CppQueryTest ${CppQueryTest_SRCS})
target_link_libraries(CppQueryTest ${EXTRA_LIBS})
