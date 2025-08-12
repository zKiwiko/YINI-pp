#include <iostream>
#include <cassert>
#include <string>
#include "yini.hpp"

int main() {
    std::cout << "Running parser tests..." << std::endl;
    
    try {
        // Test 1: Basic parsing
        std::cout << "Testing basic parsing..." << std::endl;
        yini::Parser parser;
        
        std::string basic_config = R"(
host = 'localhost'
port = 8080
enabled = true
timeout = 30.5
)";
        
        parser.parse_string(basic_config);
        
        assert(parser["host"].as_string() == "localhost");
        assert(parser["port"].as_int() == 8080);
        assert(parser["enabled"].as_bool() == true);
        assert(parser["timeout"].as_double() == 30.5);
        
        // Test 2: Nested sections
        std::cout << "Testing nested sections..." << std::endl;
        
        std::string nested_config = R"(
^ server
    ^^ connection
    host = 'localhost'
    port = 8080
    
    ^^ auth
    enabled = true
        ^^^ credentials
        username = 'admin'
        password = 'secret'
)";
        
        parser.parse_string(nested_config);
        
        assert(parser.section("server").section("connection")["host"].as_string() == "localhost");
        assert(parser.section("server").section("connection")["port"].as_int() == 8080);
        assert(parser.section("server").section("auth")["enabled"].as_bool() == true);
        assert(parser.section("server").section("auth").section("credentials")["username"].as_string() == "admin");
        
        // Test 3: Arrays/Lists
        std::cout << "Testing arrays..." << std::endl;
        
        std::string array_config = R"(
numbers = [1, 2, 3, 4, 5]
names = ['alice', 'bob', 'charlie']
mixed = [1, 'test', true, 3.14]
)";
        
        parser.parse_string(array_config);
        
        auto numbers = parser["numbers"].as_array();
        assert(numbers.size() == 5);
        assert(numbers[0].as_int() == 1);
        assert(numbers[4].as_int() == 5);
        
        auto names = parser["names"].as_array();
        assert(names.size() == 3);
        assert(names[0].as_string() == "alice");
        assert(names[2].as_string() == "charlie");
        
        // Test 4: Comments
        std::cout << "Testing comments..." << std::endl;
        
        std::string comment_config = R"(
host = 'localhost'  // This is a line comment
port = 8080
/* This is a 
   multiline comment 
   that spans multiple lines */
enabled = true
/* Another multiline comment */ timeout = 30.5
)";
        
        parser.parse_string(comment_config);
        
        assert(parser["host"].as_string() == "localhost");
        assert(parser["port"].as_int() == 8080);
        assert(parser["enabled"].as_bool() == true);
        assert(parser["timeout"].as_double() == 30.5);
        
        // Test 5: Boolean variations
        std::cout << "Testing boolean variations..." << std::endl;
        
        std::string bool_config = R"(
flag1 = true
flag2 = false
flag3 = yes
flag4 = no
flag5 = on
flag6 = off
)";
        
        parser.parse_string(bool_config);
        
        assert(parser["flag1"].as_bool() == true);
        assert(parser["flag2"].as_bool() == false);
        assert(parser["flag3"].as_bool() == true);
        assert(parser["flag4"].as_bool() == false);
        assert(parser["flag5"].as_bool() == true);
        assert(parser["flag6"].as_bool() == false);
        
        // Test 6: Multiline comments
        std::cout << "Testing multiline comments..." << std::endl;
        
        std::string multiline_config = R"(
/* This is a header comment
   explaining the configuration format */
name = 'test'
/* 
   Nested configuration section
   with detailed explanations
*/
^ section
    /* Inline comment */ value = 42
    /* Comment before key */ another = 'test'
/* Final comment */
)";
        
        parser.parse_string(multiline_config);
        
        assert(parser["name"].as_string() == "test");
        assert(parser.section("section")["value"].as_int() == 42);
        assert(parser.section("section")["another"].as_string() == "test");
        
        // Test 7: File parsing
        std::cout << "Testing file parsing..." << std::endl;
        parser.parse_file("example.yini");
        
        assert(parser.section("server").section("connection")["host"].as_string() == "localhost");
        assert(parser.section("server").section("connection")["port"].as_int() == 8080);
        assert(parser.section("server").section("auth")["enabled"].as_bool() == true);
        
        std::cout << "All tests passed" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
