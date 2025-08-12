# Create test executables
add_executable(test_basic test_basic.cpp)
add_executable(test_parser test_parser.cpp)
add_executable(test_writer test_writer.cpp)

# Link against the header-only library
target_link_libraries(test_basic PRIVATE yini-pp)
target_link_libraries(test_parser PRIVATE yini-pp)
target_link_libraries(test_writer PRIVATE yini-pp)

# Register tests with CTest
add_test(NAME basic_tests COMMAND test_basic)
add_test(NAME parser_tests COMMAND test_parser)
add_test(NAME writer_tests COMMAND test_writer)

# Set test properties
set_tests_properties(basic_tests PROPERTIES
    TIMEOUT 30
    PASS_REGULAR_EXPRESSION "All tests passed"
)

set_tests_properties(parser_tests PROPERTIES
    TIMEOUT 30
    PASS_REGULAR_EXPRESSION "All tests passed"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

set_tests_properties(writer_tests PROPERTIES
    TIMEOUT 30
    PASS_REGULAR_EXPRESSION "All tests passed"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
