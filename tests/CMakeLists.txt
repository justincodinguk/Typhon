include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_MakeAvailable(googletest)


set(DATA_SRC "${CMAKE_CURRENT_SOURCE_DIR}/ty_samples/")
set(DATA_DST "${CMAKE_CURRENT_BINARY_DIR}/ty_samples/")


add_custom_target(copy_test_data
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${DATA_SRC} ${DATA_DST}
        COMMENT "Syncing test data from source to build directory..."
)
add_executable(unit_tests test_main.cpp lexer_tests.cpp parsing/parsing_tests.cpp parsing/utils.cpp parsing/utils.h sema/sema_tests.cpp emitter/py_emitter_tests.cpp)
add_dependencies(unit_tests copy_test_data)

target_link_libraries(unit_tests PRIVATE GTest::gtest_main typhon_core)

include(GoogleTest)
gtest_discover_tests(unit_tests)