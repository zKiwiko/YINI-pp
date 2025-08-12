#include <iostream>
#include <cassert>
#include <string>
#include <sstream>
#include "yini.hpp"

int main() {
    std::cout << "Running writer tests..." << std::endl;
    
    try {
        // Test 1: Basic writing
        std::cout << "Testing basic writing..." << std::endl;
        yini::Parser parser;
        
        parser["host"] = "localhost";
        parser["port"] = 8080;
        parser["enabled"] = true;
        parser["timeout"] = 30.5;
        
        std::string output = parser.write_string();
        std::cout << "Basic output:\n" << output << std::endl;
        
        // Parse it back to verify
        yini::Parser parser2;
        parser2.parse_string(output);
        assert(parser2["host"].as_string() == "localhost");
        assert(parser2["port"].as_int() == 8080);
        assert(parser2["enabled"].as_bool() == true);
        assert(parser2["timeout"].as_double() == 30.5);
        
        // Test 2: Nested sections writing
        std::cout << "Testing nested sections writing..." << std::endl;
        yini::Parser nested_parser;
        
        nested_parser.section("server").section("connection")["host"] = "localhost";
        nested_parser.section("server").section("connection")["port"] = 8080;
        nested_parser.section("server").section("auth")["enabled"] = true;
        nested_parser.section("server").section("auth").section("credentials")["username"] = "admin";
        nested_parser.section("server").section("auth").section("credentials")["password"] = "secret";
        
        std::string nested_output = nested_parser.write_string();
        std::cout << "Nested output:\n" << nested_output << std::endl;
        
        // Parse it back to verify
        yini::Parser nested_parser2;
        nested_parser2.parse_string(nested_output);
        assert(nested_parser2.section("server").section("connection")["host"].as_string() == "localhost");
        assert(nested_parser2.section("server").section("connection")["port"].as_int() == 8080);
        assert(nested_parser2.section("server").section("auth")["enabled"].as_bool() == true);
        assert(nested_parser2.section("server").section("auth").section("credentials")["username"].as_string() == "admin");
        
        // Test 3: Arrays writing
        std::cout << "Testing arrays writing..." << std::endl;
        yini::Parser array_parser;
        
        std::vector<yini::Value> numbers = {yini::Value(1), yini::Value(2), yini::Value(3)};
        std::vector<yini::Value> names = {yini::Value("alice"), yini::Value("bob"), yini::Value("charlie")};
        std::vector<yini::Value> mixed = {yini::Value(42), yini::Value("test"), yini::Value(true)};
        
        array_parser["numbers"] = numbers;
        array_parser["names"] = names;
        array_parser["mixed"] = mixed;
        
        std::string array_output = array_parser.write_string();
        std::cout << "Array output:\n" << array_output << std::endl;
        
        // Parse it back to verify
        yini::Parser array_parser2;
        array_parser2.parse_string(array_output);
        auto parsed_numbers = array_parser2["numbers"].as_array();
        assert(parsed_numbers.size() == 3);
        assert(parsed_numbers[0].as_int() == 1);
        assert(parsed_numbers[2].as_int() == 3);
        
        // Test 4: File writing
        std::cout << "Testing file writing..." << std::endl;
        yini::Parser file_parser;
        file_parser["test_key"] = "test_value";
        file_parser.section("test_section")["nested_key"] = "nested_value";
        
        file_parser.write_file("test_output.yini");
        
        // Read it back
        yini::Parser file_parser2;
        file_parser2.parse_file("test_output.yini");
        assert(file_parser2["test_key"].as_string() == "test_value");
        assert(file_parser2.section("test_section")["nested_key"].as_string() == "nested_value");
        
        // Test 5: Round-trip test with example.yini
        std::cout << "Testing round-trip with example.yini..." << std::endl;
        yini::Parser original;
        original.parse_file("example.yini");
        
        std::string serialized = original.write_string();
        std::cout << "Serialized example.yini:\n" << serialized << std::endl;
        
        yini::Parser round_trip;
        round_trip.parse_string(serialized);
        
        // Verify key values are preserved
        assert(round_trip.section("server").section("connection")["host"].as_string() == "localhost");
        assert(round_trip.section("server").section("connection")["port"].as_int() == 8080);
        assert(round_trip.section("server").section("auth")["enabled"].as_bool() == true);
        
        std::cout << "All tests passed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
