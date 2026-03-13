include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_MakeAvailable(googletest)
add_executable(unit_tests test_main.cpp)

target_link_libraries(unit_tests PRIVATE GTest::gtest_main typhon_core)

include(GoogleTest)
gtest_discover_tests(unit_tests)