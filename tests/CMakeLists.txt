set(EXTRA_LIBS ${EXTRA_LIBS} CppQuery boost_locale gtest)

set(CppQueryTest_SRCS basic_test.cpp)

add_executable (CppQueryTest ${CppQueryTest_SRCS})
add_executable (tests tests.cpp)
add_executable (node_tests node_tests.cpp)

target_link_libraries(CppQueryTest ${EXTRA_LIBS})
target_link_libraries(tests ${EXTRA_LIBS})
target_link_libraries(node_tests ${EXTRA_LIBS})

