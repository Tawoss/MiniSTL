file(GLOB TEST_SOURCE *.cpp)

enable_testing()

add_executable(unit_test ${TEST_SOURCE})

target_link_libraries(unit_test GTest::gtest_main)

include(GoogleTest)
gtest_add_tests(TARGET unit_test)
