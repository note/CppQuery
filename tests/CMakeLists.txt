set(EXTRA_LIBS ${EXTRA_LIBS} CppQuery boost_locale)

set(CppQueryTest_SRCS basic_test.cpp)

add_executable (CppQueryTest ${CppQueryTest_SRCS})
target_link_libraries(CppQueryTest ${EXTRA_LIBS})
