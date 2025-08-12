#include <iostream>
#include <cassert>
#include "yini.hpp"

int main() {
    std::cout << "Running basic tests..." << std::endl;
    
    try {
        // Test 1: Header inclusion and basic functionality
        std::cout << "Testing header inclusion..." << std::endl;
        yini::Parser parser;
        
        // Test basic value assignment and retrieval
        parser["test_string"] = "hello";
        parser["test_int"] = 42;
        parser["test_bool"] = true;
        parser["test_double"] = 3.14;
        
        assert(parser["test_string"].as_string() == "hello");
        assert(parser["test_int"].as_int() == 42);
        assert(parser["test_bool"].as_bool() == true);
        assert(parser["test_double"].as_double() == 3.14);
        
        // Test 2: Type checking
        std::cout << "Testing type checking..." << std::endl;
        assert(parser["test_string"].is_string());
        assert(parser["test_int"].is_int());
        assert(parser["test_bool"].is_bool());
        assert(parser["test_double"].is_double());
        
        // Test 3: Section creation and access
        std::cout << "Testing section access..." << std::endl;
        parser.section("config")["setting"] = "value";
        assert(parser.section("config")["setting"].as_string() == "value");
        assert(parser.section("config").has_value("setting"));
        assert(!parser.section("config").has_value("nonexistent"));
        
        // Test 4: Exception handling
        std::cout << "Testing exception handling..." << std::endl;
        bool caught_exception = false;
        try {
            parser.parse_string("invalid line without equals");
        } catch (const yini::ParseError& e) {
            caught_exception = true;
            std::cout << "Correctly caught parse error: " << e.what() << std::endl;
        }
        assert(caught_exception);
        
        // Test 5: Value conversion
        std::cout << "Testing value conversion..." << std::endl;
        yini::Value string_number("123");
        assert(string_number.as_int() == 123);
        assert(string_number.as_double() == 123.0);
        
        yini::Value string_bool("true");
        assert(string_bool.as_bool() == true);
        
        std::cout << "All tests passed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
