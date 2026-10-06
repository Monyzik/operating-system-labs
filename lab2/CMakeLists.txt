add_library(lab2_io STATIC src/io.c)
target_include_directories(lab2_io PUBLIC src/include)
target_compile_definitions(lab2_io PUBLIC _POSIX_C_SOURCE=200809L)

add_executable(parent src/parent.c)
target_link_libraries(parent PRIVATE lab2_io)

add_executable(child1 src/child1.c)
target_link_libraries(child1 PRIVATE lab2_io)

add_executable(child2 src/child2.c)
target_link_libraries(child2 PRIVATE lab2_io)

target_compile_definitions(parent PRIVATE
        CHILD1_PATH="$<TARGET_FILE:child1>"
        CHILD2_PATH="$<TARGET_FILE:child2>"
)
add_dependencies(parent child1 child2)
