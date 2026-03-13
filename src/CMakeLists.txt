file(GLOB_RECURSE TYPHON_SOURCES
        "lexer/*.cpp"
        "lexer/*.h"
        "parser/*.cpp"
        "parser/*.h"
        "analyzer/*.cpp"
        "analyzer/*.h"
        "common/*.cpp"
        "common/*.h"
        "errors/*.cpp"
        "errors/*.h"
        "types/*.cpp"
        "types/*.h"
)

add_library(typhon_core STATIC ${TYPHON_SOURCES})
target_include_directories(typhon_core PUBLIC .)
target_link_directories(typhon_core PUBLIC .)
add_executable(typhon main.cpp)
target_link_libraries(typhon PRIVATE typhon_core)

if(MSVC)
    target_compile_options(typhon_core PRIVATE /W4)
else()
    target_compile_options(typhon_core PRIVATE -Wall -Wextra -Wpedantic)
endif()