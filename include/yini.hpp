#ifndef YINI_HPP
#define YINI_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <variant>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <cctype>

namespace yini {

// Forward declarations
class Section;
class Value;

// Type alias for the value variant
using ValueType = std::variant<std::string, int, double, bool, std::vector<Value>>;

// Exception classes
class ParseError : public std::runtime_error {
public:
    explicit ParseError(const std::string& message) : std::runtime_error("YINI Parse Error: " + message) {}
};

class FileError : public std::runtime_error {
public:
    explicit FileError(const std::string& message) : std::runtime_error("YINI File Error: " + message) {}
};

// Value class to hold different types of values
class Value {
private:
    ValueType data_;

public:
    Value() : data_(std::string{}) {}
    Value(const std::string& value) : data_(value) {}
    Value(const char* value) : data_(std::string(value)) {}
    Value(int value) : data_(value) {}
    Value(double value) : data_(value) {}
    Value(bool value) : data_(value) {}
    Value(const std::vector<Value>& value) : data_(value) {}

    // Type checking methods
    bool is_string() const { return std::holds_alternative<std::string>(data_); }
    bool is_int() const { return std::holds_alternative<int>(data_); }
    bool is_double() const { return std::holds_alternative<double>(data_); }
    bool is_bool() const { return std::holds_alternative<bool>(data_); }
    bool is_array() const { return std::holds_alternative<std::vector<Value>>(data_); }

    // Getters with type conversion
    std::string as_string() const {
        if (is_string()) return std::get<std::string>(data_);
        if (is_int()) return std::to_string(std::get<int>(data_));
        if (is_double()) return std::to_string(std::get<double>(data_));
        if (is_bool()) return std::get<bool>(data_) ? "true" : "false";
        throw std::runtime_error("Cannot convert array to string");
    }

    int as_int() const {
        if (is_int()) return std::get<int>(data_);
        if (is_double()) return static_cast<int>(std::get<double>(data_));
        if (is_string()) {
            try {
                return std::stoi(std::get<std::string>(data_));
            } catch (...) {
                throw std::runtime_error("Cannot convert string to int");
            }
        }
        throw std::runtime_error("Cannot convert to int");
    }

    double as_double() const {
        if (is_double()) return std::get<double>(data_);
        if (is_int()) return static_cast<double>(std::get<int>(data_));
        if (is_string()) {
            try {
                return std::stod(std::get<std::string>(data_));
            } catch (...) {
                throw std::runtime_error("Cannot convert string to double");
            }
        }
        throw std::runtime_error("Cannot convert to double");
    }

    bool as_bool() const {
        if (is_bool()) return std::get<bool>(data_);
        if (is_string()) {
            std::string str = std::get<std::string>(data_);
            std::transform(str.begin(), str.end(), str.begin(), ::tolower);
            return str == "true" || str == "yes" || str == "on" || str == "1";
        }
        if (is_int()) return std::get<int>(data_) != 0;
        throw std::runtime_error("Cannot convert to bool");
    }

    std::vector<Value> as_array() const {
        if (is_array()) return std::get<std::vector<Value>>(data_);
        throw std::runtime_error("Value is not an array");
    }

    // Operators for easy assignment
    Value& operator=(const std::string& value) { data_ = value; return *this; }
    Value& operator=(const char* value) { data_ = std::string(value); return *this; }
    Value& operator=(int value) { data_ = value; return *this; }
    Value& operator=(double value) { data_ = value; return *this; }
    Value& operator=(bool value) { data_ = value; return *this; }
    Value& operator=(const std::vector<Value>& value) { data_ = value; return *this; }
};

// Section class to represent nested sections
class Section {
private:
    std::unordered_map<std::string, Value> values_;
    std::unordered_map<std::string, std::unique_ptr<Section>> subsections_;

public:
    // Value access
    Value& operator[](const std::string& key) {
        return values_[key];
    }

    const Value& at(const std::string& key) const {
        auto it = values_.find(key);
        if (it == values_.end()) {
            throw std::out_of_range("Key not found: " + key);
        }
        return it->second;
    }

    bool has_value(const std::string& key) const {
        return values_.find(key) != values_.end();
    }

    // Section access
    Section& section(const std::string& name) {
        if (subsections_.find(name) == subsections_.end()) {
            subsections_[name] = std::make_unique<Section>();
        }
        return *subsections_[name];
    }

    const Section& get_section(const std::string& name) const {
        auto it = subsections_.find(name);
        if (it == subsections_.end()) {
            throw std::out_of_range("Section not found: " + name);
        }
        return *it->second;
    }

    bool has_section(const std::string& name) const {
        return subsections_.find(name) != subsections_.end();
    }

    // Iterators for values
    auto values_begin() const { return values_.begin(); }
    auto values_end() const { return values_.end(); }

    // Iterators for sections
    auto sections_begin() const { return subsections_.begin(); }
    auto sections_end() const { return subsections_.end(); }

    // Clear all data
    void clear() {
        values_.clear();
        subsections_.clear();
    }
};

// Main YINI parser/writer class
class Parser {
private:
    Section root_;

    // Helper functions
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }

    std::string remove_multiline_comments(const std::string& content) {
        std::string result = content;
        size_t start = 0;
        
        while ((start = result.find("/*", start)) != std::string::npos) {
            size_t end = result.find("*/", start + 2);
            if (end == std::string::npos) {
                // Unclosed multiline comment - remove everything from start
                result = result.substr(0, start);
                break;
            }
            // Remove the comment including the delimiters
            result.erase(start, end - start + 2);
        }
        
        return result;
    }

    std::string remove_line_comments(const std::string& line) {
        size_t comment_pos = line.find("//");
        if (comment_pos != std::string::npos) {
            return line.substr(0, comment_pos);
        }
        return line;
    }

    int count_carets(const std::string& line) {
        int count = 0;
        for (char c : line) {
            if (c == '^') count++;
            else break;
        }
        return count;
    }

    Value parse_value(const std::string& value_str) {
        std::string trimmed = trim(value_str);
        
        // Handle quoted strings
        if ((trimmed.front() == '\'' && trimmed.back() == '\'') ||
            (trimmed.front() == '"' && trimmed.back() == '"')) {
            return Value(trimmed.substr(1, trimmed.length() - 2));
        }

        // Handle arrays (lists)
        if (trimmed.front() == '[' && trimmed.back() == ']') {
            std::vector<Value> array;
            std::string content = trimmed.substr(1, trimmed.length() - 2);
            std::stringstream ss(content);
            std::string item;
            
            while (std::getline(ss, item, ',')) {
                item = trim(item);
                if (!item.empty()) {
                    array.push_back(parse_value(item));
                }
            }
            return Value(array);
        }

        // Handle boolean values
        std::string lower = trimmed;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "true" || lower == "yes" || lower == "on") {
            return Value(true);
        }
        if (lower == "false" || lower == "no" || lower == "off") {
            return Value(false);
        }

        // Handle numeric values
        try {
            if (trimmed.find('.') != std::string::npos) {
                return Value(std::stod(trimmed));
            } else {
                return Value(std::stoi(trimmed));
            }
        } catch (...) {
            // If not a number, treat as string
            return Value(trimmed);
        }
    }

    Section* navigate_to_section(const std::vector<std::string>& section_path) {
        Section* current = &root_;
        for (const auto& section_name : section_path) {
            current = &current->section(section_name);
        }
        return current;
    }

    void write_section(std::ostream& out, const Section& section, 
                      const std::vector<std::string>& path, int indent_level = 0) {
        // Write section header if not root
        if (!path.empty()) {
            out << std::string(indent_level * 4, ' ') 
                << std::string(indent_level + 1, '^') << ' ' 
                << path.back() << '\n';
        }

        // Write values
        for (auto it = section.values_begin(); it != section.values_end(); ++it) {
            out << std::string((indent_level + (path.empty() ? 0 : 1)) * 4, ' ');
            out << it->first << " = ";
            write_value(out, it->second);
            out << '\n';
        }

        // Write subsections
        for (auto it = section.sections_begin(); it != section.sections_end(); ++it) {
            if (!path.empty() || it != section.sections_begin()) {
                out << '\n';
            }
            
            std::vector<std::string> new_path = path;
            new_path.push_back(it->first);
            write_section(out, *it->second, new_path, indent_level + (path.empty() ? 0 : 1));
        }
    }

    void write_value(std::ostream& out, const Value& value) {
        if (value.is_string()) {
            out << '\'' << value.as_string() << '\'';
        } else if (value.is_bool()) {
            out << (value.as_bool() ? "true" : "false");
        } else if (value.is_array()) {
            out << '[';
            auto array = value.as_array();
            for (size_t i = 0; i < array.size(); ++i) {
                if (i > 0) out << ", ";
                write_value(out, array[i]);
            }
            out << ']';
        } else {
            out << value.as_string();
        }
    }

public:
    Parser() = default;

    // Parse from file
    void parse_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw FileError("Cannot open file: " + filename);
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        parse_string(content);
    }

    // Parse from string
    void parse_string(const std::string& content) {
        root_.clear();
        
        // First, remove all multiline comments
        std::string cleaned_content = remove_multiline_comments(content);
        
        std::vector<std::string> section_stack;
        std::istringstream stream(cleaned_content);
        std::string line;
        int line_number = 0;

        while (std::getline(stream, line)) {
            line_number++;
            line = remove_line_comments(line);
            line = trim(line);

            if (line.empty()) continue;

            try {
                int caret_count = count_carets(line);
                
                if (caret_count > 0) {
                    // This is a section header
                    std::string section_name = trim(line.substr(caret_count));
                    
                    // Adjust section stack to match nesting level
                    section_stack.resize(caret_count - 1);
                    section_stack.push_back(section_name);
                } else {
                    // This is a key-value pair
                    size_t equals_pos = line.find('=');
                    if (equals_pos == std::string::npos) {
                        throw ParseError("Invalid line format at line " + std::to_string(line_number) + ": " + line);
                    }

                    std::string key = trim(line.substr(0, equals_pos));
                    std::string value_str = trim(line.substr(equals_pos + 1));

                    if (key.empty()) {
                        throw ParseError("Empty key at line " + std::to_string(line_number) + ": " + line);
                    }

                    Section* target_section = navigate_to_section(section_stack);
                    (*target_section)[key] = parse_value(value_str);
                }
            } catch (const std::exception& e) {
                throw ParseError("Error at line " + std::to_string(line_number) + ": " + e.what());
            }
        }
    }

    // Write to file
    void write_file(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw FileError("Cannot write to file: " + filename);
        }
        write_stream(file);
    }

    // Write to string
    std::string write_string() {
        std::ostringstream oss;
        write_stream(oss);
        return oss.str();
    }

    // Write to stream
    void write_stream(std::ostream& out) {
        write_section(out, root_, {});
    }

    // Access root section
    Section& root() { return root_; }
    const Section& root() const { return root_; }

    // Convenience accessors
    Value& operator[](const std::string& key) { return root_[key]; }
    Section& section(const std::string& name) { return root_.section(name); }
};

} // namespace yini

#endif // YINI_HPP
