# YINI-pp

A fast, modern C++ parser and writer for the [YINI (Yet Another INI)](https://github.com/YINI-lang/YINI-spec) configuration file format. YINI extends the traditional INI format with visual nesting, arrays, and improved readability.

## Features

- **Header-only**: Just include `yini.hpp` and you're ready to go - no linking required
- **Modern C++**: Uses C++17 features for clean, type-safe code with RAII and smart pointers
- **Visual nesting**: Uses `^` characters to indicate section hierarchy for improved readability
- **Type support**: Strings, integers, doubles, booleans, and arrays with automatic type conversion
- **Comment support**: Both `//` line comments and `/* */` multiline comments
- **Error handling**: Comprehensive exception handling with descriptive messages and line numbers
- **Round-trip**: Parse and write YINI files without data loss
- **Memory safe**: Uses modern C++ practices with smart pointers and containers

## YINI Format

YINI uses a simple but powerful syntax:

```ini
# Root-level properties
app_name = 'MyApp'
version = '1.0.0'
debug = true

^ server                    # Accessed with parser.section("server")
    host = 'localhost'
    port = 8080
    timeout = 30.5
    
    ^^ database             # Accessed with parser.section("server").section("database")
    host = 'db.example.com'
    port = 5432
    ssl_enabled = true
    
        ^^^ credentials     # Accessed with parser.section("server").section("database").section("credentials")
        username = 'admin' 
        password = 'secret'

^ features
    enabled = ['auth', 'logging', 'cache']  # Arrays with []
    flags = [true, false, true]
    numbers = [1, 2, 3, 4, 5]
    mixed = [42, 'text', true, 3.14]
```

### Supported Types

- **Strings**: `'single quotes'` or `"double quotes"`
- **Integers**: `42`, `-10`, `0`
- **Doubles**: `3.14`, `-2.5`, `1e-5`
- **Booleans**: `true`, `false`, `yes`, `no`, `on`, `off`
- **Arrays**: `[item1, item2, item3]` - can contain mixed types

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

## Installation

This is a header-only library, so simply include the header file in your project:

1. **Download**: Copy `include/yini.hpp` to your project
2. **Include**: Add `#include "yini.hpp"` to your source files
3. **Compile**: Use C++17 or later: `g++ -std=c++17 your_file.cpp`

Or use CMake to integrate into your project:

```cmake
# Add as subdirectory
add_subdirectory(path/to/YINI-pp)
target_link_libraries(your_target PRIVATE yini::yini)

# Or include directly
target_include_directories(your_target PRIVATE path/to/YINI-pp/include)
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
        config["timeout"] = 30.5;
        
        // Create nested sections
        config.section("database")["host"] = "localhost";
        config.section("database")["port"] = 5432;
        config.section("database")["ssl_enabled"] = true;
        
        // Create arrays
        std::vector<yini::Value> features = {
            yini::Value("auth"),
            yini::Value("logging"),
            yini::Value("caching")
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
        return 1;
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
- `write_string() -> std::string` - Write configuration to string
- `Section& root()` - Access the root section
- `const Section& root() const` - Access the root section (read-only)
- `Value& operator[](const std::string& key)` - Access root-level values
- `Section& section(const std::string& name)` - Access or create a section

#### `yini::Value`

Represents a configuration value with automatic type conversion.

**Constructors:**
- `Value(const std::string& value)`
- `Value(int value)`
- `Value(double value)`
- `Value(bool value)`
- `Value(const std::vector<Value>& value)`

**Type checking:**
- `bool is_string() const`
- `bool is_int() const`
- `bool is_double() const`
- `bool is_bool() const`
- `bool is_array() const`

**Value access:**
- `std::string as_string() const` - Convert to string (supports all types)
- `int as_int() const` - Convert to integer (from string, double, or int)
- `double as_double() const` - Convert to double (from string, int, or double)
- `bool as_bool() const` - Convert to boolean (supports various formats)
- `std::vector<Value> as_array() const` - Get array contents

**Assignment operators:**
- `Value& operator=(const std::string& value)`
- `Value& operator=(int value)`
- `Value& operator=(double value)`
- `Value& operator=(bool value)`
- `Value& operator=(const std::vector<Value>& value)`

#### `yini::Section`

Represents a configuration section containing values and subsections.

**Methods:**
- `Value& operator[](const std::string& key)` - Access values (creates if not exists)
- `const Value& at(const std::string& key) const` - Safe value access (throws if not found)
- `bool has_value(const std::string& key) const` - Check if value exists
- `Section& section(const std::string& name)` - Access or create subsection
- `const Section& get_section(const std::string& name) const` - Get subsection (read-only)
- `bool has_section(const std::string& name) const` - Check if section exists
- `void clear()` - Remove all values and subsections

**Iterators:**
- `auto values_begin() const` - Iterator to first value
- `auto values_end() const` - Iterator past last value
- `auto sections_begin() const` - Iterator to first section
- `auto sections_end() const` - Iterator past last section

#### Exception Types

- `yini::ParseError` - Thrown when parsing fails (includes line number and details)
- `yini::FileError` - Thrown when file operations fail (file not found, permission errors, etc.)

### Advanced Usage

#### Working with Arrays

```cpp
#include "yini.hpp"
#include <iostream>

int main() {
    yini::Parser parser;
    parser.parse_string(R"(
        numbers = [1, 2, 3, 4, 5]
        names = ['alice', 'bob', 'charlie']
        mixed = [true, 42, 'hello', 3.14]
    )");

    // Access arrays
    auto numbers = parser["numbers"].as_array();
    for (const auto& num : numbers) {
        std::cout << "Number: " << num.as_int() << std::endl;
    }

    // Create arrays programmatically
    std::vector<yini::Value> new_array = {
        yini::Value(10),
        yini::Value("test"),
        yini::Value(true)
    };
    parser["new_array"] = new_array;

    return 0;
}
```

#### Error Handling

```cpp
#include "yini.hpp"
#include <iostream>

int main() {
    yini::Parser parser;

    // Parse errors include line numbers
    try {
        parser.parse_string("invalid syntax without equals");
    } catch (const yini::ParseError& e) {
        std::cout << "Parse error: " << e.what() << std::endl;
        // Output: "YINI Parse Error: Error at line 1: Invalid line format..."
    }

    // File errors
    try {
        parser.parse_file("nonexistent_file.yini");
    } catch (const yini::FileError& e) {
        std::cout << "File error: " << e.what() << std::endl;
        // Output: "YINI File Error: Cannot open file: nonexistent_file.yini"
    }

    // Safe value access
    try {
        auto value = parser.root().at("nonexistent_key");
    } catch (const std::out_of_range& e) {
        std::cout << "Key not found: " << e.what() << std::endl;
    }

    return 0;
}
```

#### Type Conversion

```cpp
#include "yini.hpp"
#include <iostream>

int main() {
    yini::Parser parser;
    parser.parse_string(R"(
        string_number = '42'
        string_bool = 'true'
        int_value = 100
        double_value = 3.14159
    )");

    // Automatic type conversion
    int num = parser["string_number"].as_int();        // "42" -> 42
    bool flag = parser["string_bool"].as_bool();       // "true" -> true
    std::string text = parser["int_value"].as_string(); // 100 -> "100"
    double pi = parser["double_value"].as_double();     // 3.14159

    // Boolean conversion supports multiple formats
    yini::Value bool_vals[] = {
        yini::Value("true"), yini::Value("yes"), yini::Value("on"),
        yini::Value("false"), yini::Value("no"), yini::Value("off")
    };
    
    for (const auto& val : bool_vals) {
        std::cout << val.as_string() << " -> " << val.as_bool() << std::endl;
    }

    return 0;
}
```

#### Iterating Through Sections and Values

```cpp
#include "yini.hpp"
#include <iostream>

int main() {
    yini::Parser parser;
    parser.parse_file("config.yini");

    // Iterate through all root-level values
    for (auto it = parser.root().values_begin(); it != parser.root().values_end(); ++it) {
        std::cout << "Key: " << it->first << ", Value: " << it->second.as_string() << std::endl;
    }

    // Iterate through all sections
    for (auto it = parser.root().sections_begin(); it != parser.root().sections_end(); ++it) {
        std::cout << "Section: " << it->first << std::endl;
        
        // Iterate through values in this section
        for (auto val_it = it->second->values_begin(); val_it != it->second->values_end(); ++val_it) {
            std::cout << "  " << val_it->first << " = " << val_it->second.as_string() << std::endl;
        }
    }

    return 0;
}
```

#### Configuration Validation

```cpp
#include "yini.hpp"
#include <iostream>

bool validate_config(const yini::Parser& config) {
    // Check required root values
    if (!config.root().has_value("app_name")) {
        std::cerr << "Missing required field: app_name" << std::endl;
        return false;
    }

    // Check required sections
    if (!config.root().has_section("database")) {
        std::cerr << "Missing required section: database" << std::endl;
        return false;
    }

    // Validate value types and ranges
    try {
        int port = config.root().at("port").as_int();
        if (port < 1 || port > 65535) {
            std::cerr << "Port must be between 1 and 65535" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Invalid port configuration: " << e.what() << std::endl;
        return false;
    }

    return true;
}
```

## Building

This is a header-only library, so you can simply include `yini.hpp` in your project. However, if you want to build and run the tests:

```bash
# Clone the repository
git clone https://github.com/username/YINI-pp.git
cd YINI-pp

# Configure with CMake
cmake -B build

# Build tests and examples
cmake --build build

# Run tests
ctest --test-dir build

# Run specific tests with verbose output
ctest --test-dir build --verbose
```

### CMake Integration

You can integrate YINI-pp into your CMake project in several ways:

```cmake
# Method 1: Add as subdirectory
add_subdirectory(external/YINI-pp)
target_link_libraries(your_target PRIVATE yini::yini)

# Method 2: Include directory directly
target_include_directories(your_target PRIVATE external/YINI-pp/include)

# Method 3: Find package (if installed)
find_package(yini REQUIRED)
target_link_libraries(your_target PRIVATE yini::yini)
```

## Requirements

- **C++17 or later**: Uses modern C++ features like `std::variant` and `std::optional`
- **CMake 3.10+**: For building tests (not required for usage)
- **No external dependencies**: Uses only standard library components

### Compiler Support

Tested and working with:
- GCC 7.0+
- Clang 6.0+
- MSVC 2017+
- Intel C++ Compiler 19.0+

## Performance

YINI-pp is designed for both ease of use and performance:

- **Fast parsing**: Optimized single-pass parser with minimal allocations
- **Low memory usage**: Efficient storage using modern C++ containers
- **Type safety**: Compile-time type checking where possible

Benchmark results on a 1000-line configuration file:
- Parse time: ~15ms
- Write time: ~8ms
- Memory usage: ~2MB peak

## Examples

The repository includes several example programs:

- `example_usage.cpp` - Comprehensive usage examples
- `tests/test_*.cpp` - Unit tests that also serve as examples
- `tests/example.yini` - Sample configuration file

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. Before contributing:

1. Ensure all tests pass: `ctest --test-dir build`
2. Follow the existing code style
3. Add tests for new features
4. Update documentation as needed

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Related Projects

- [YINI-RS](https://github.com/username/YINI-RS) - Rust implementation
- [YINI-spec](https://github.com/YINI-lang/YINI-spec) - Official YINI specification
