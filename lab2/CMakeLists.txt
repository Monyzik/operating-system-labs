add_library(lab2_io STATIC src/io.c)
target_include_directories(lab2_io PUBLIC src/include)
target_compile_definitions(lab2_io PUBLIC _POSIX_C_SOURCE=200809L)

foreach(program parent child1 child2)
    add_executable(${program} src/${program}.c)
    target_link_libraries(${program} PRIVATE lab2_io)
endforeach()

target_compile_definitions(parent PRIVATE
    CHILD1_PATH="$<TARGET_FILE:child1>"
    CHILD2_PATH="$<TARGET_FILE:child2>"
)
add_dependencies(parent child1 child2)
