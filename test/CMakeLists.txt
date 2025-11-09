find_package(GTest)

add_executable(tests
    module.cpp
    space.cpp)
target_link_libraries(tests PRIVATE merc GTest::GTest GTest::Main)

add_dependencies(tests buzz)
target_compile_definitions(tests PRIVATE TEST_CMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}")

add_test(NAME tests COMMAND tests)
