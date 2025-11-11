find_package(GTest)

add_executable(tests
    element-test.cpp
    module-test.cpp
    space-test.cpp
)
target_link_libraries(tests PRIVATE illumia GTest::GTest GTest::Main)

add_dependencies(tests buzz triangle)
target_compile_definitions(tests
    PRIVATE TEST_CMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}")

add_test(NAME tests COMMAND tests --gtest_color=yes)
