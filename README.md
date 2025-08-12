# YINI-pp

A modern, header-only C++ parser and writer for the [YINI (Yet Another INI)](https://github.com/YINI-lang/YINI-spec) configuration file format. YINI extends the traditional INI format with visual nesting, arrays, and improved readability.

## Features

- **Header-only**: Just include `yini.hpp` and you're ready to go
- **Modern C++**: Uses C++17 features for clean, type-safe code
- **Type support**: Strings, integers, doubles, booleans, and arrays
- **Comment support**: Both `//` line comments and `/* */` multiline comments
- **Error handling**: Comprehensive exception handling with descriptive messages
- **Round-trip**: Parse and write YINI files without data loss

## YINI Format

YINI uses a simple but powerful syntax:

```ini
# Root-level properties
app_name = 'MyApp'
version = '1.0.0'
debug = true

^ server                    # Accessed with Obj.section(server)
    host = 'localhost'
    port = 8080
    
    ^^ database             # Accessed with Obj.section(server).section(database)
    host = 'db.example.com'
    port = 5432
    
        ^^^ credentials     # Accessed with Obj.section(server).section(database).section(credentials)
        username = 'admin' 
        password = 'secret'

^ features
    enabled = ['auth', 'logging', 'cache']  # Arrays with []
    flags = [true, false, true]
    numbers = [1, 2, 3, 4, 5]
```

### Supported Types

- **Strings**: `'single quotes'` or `"double quotes"`
- **Integers**: `42`, `-10`
- **Doubles**: `3.14`, `-2.5`
- **Booleans**: `true`, `false`, `yes`, `no`, `on`, `off`
- **Arrays**: `[item1, item2, item3]`

### Comments

- Line comments: `// This is a comment`
- End-of-line comments: `key = value  // Comment here`
- Multiline comments: `/* This is a multiline comment */`
- Block comments spanning multiple lines:
  ```yini
  /*
   * This is a longer comment
   * that spans multiple lines
   */
  ```

## Usage

### Basic Example

```cpp
#include "yini.hpp"
#include <iostream>

int main() {
    try {
        // Create a new configuration
        yini::Parser config;
        
        // Set values
        config["app_name"] = "MyApplication";
        config["port"] = 8080;
        config["debug"] = true;
        
        // Create nested sections
        config.section("database")["host"] = "localhost";
        config.section("database")["port"] = 5432;
        
        // Create arrays
        std::vector<yini::Value> features = {
            yini::Value("auth"),
            yini::Value("logging")
        };
        config["features"] = features;
        
        // Write to file
        config.write_file("config.yini");
        
        // Read from file
        yini::Parser reader;
        reader.parse_file("config.yini");
        
        // Access values
        std::cout << "App: " << reader["app_name"].as_string() << std::endl;
        std::cout << "Port: " << reader["port"].as_int() << std::endl;
        std::cout << "Debug: " << reader["debug"].as_bool() << std::endl;
        
        // Access nested values
        std::cout << "DB Host: " << reader.section("database")["host"].as_string() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    return 0;
}
```

### API Reference

#### `yini::Parser`

Main class for parsing and writing YINI files.

**Methods:**
- `parse_file(const std::string& filename)` - Parse a YINI file
- `parse_string(const std::string& content)` - Parse YINI content from string
- `write_file(const std::string& filename)` - Write configuration to file
- `write_string()` - Write configuration to string
- `Section& root()` - Access the root section
- `Value& operator[](const std::string& key)` - Access root-level values
- `Section& section(const std::string& name)` - Access or create a section

#### `yini::Value`

Represents a configuration value with automatic type conversion.

**Type checking:**
- `bool is_string() const`
- `bool is_int() const`
- `bool is_double() const`
- `bool is_bool() const`
- `bool is_array() const`

**Value access:**
- `std::string as_string() const`
- `int as_int() const`
- `double as_double() const`
- `bool as_bool() const`
- `std::vector<Value> as_array() const`

#### `yini::Section`

Represents a configuration section containing values and subsections.

**Methods:**
- `Value& operator[](const std::string& key)` - Access values
- `const Value& at(const std::string& key) const` - Safe value access
- `bool has_value(const std::string& key) const` - Check if value exists
- `Section& section(const std::string& name)` - Access or create subsection
- `bool has_section(const std::string& name) const` - Check if section exists

#### Exception Types

- `yini::ParseError` - Thrown when parsing fails
- `yini::FileError` - Thrown when file operations fail

## Building

This is a header-only library, so you can simply include `yini.hpp` in your project. However, if you want to build and run the tests:

```bash
# Configure
cmake -B build

# Build
cmake --build build

# Run tests
ctest --test-dir build
```

## Requirements

- C++17 or later
- CMake 3.10+ (for building tests)

