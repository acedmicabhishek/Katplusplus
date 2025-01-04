#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <regex>
#include <iostream>
#include <algorithm>

// Token structure
struct Token {
    std::string type;  // Type of the token (e.g., "identifier", "keyword", "literal", "operator")
    std::string value; // Actual value of the token
    int line;          // Line number in the source code
    int column;        // Column number in the line
};

class TokenStore {
private:
    std::vector<Token> tokens;
    int lineNumber;
    int columnNumber;

    // Helper function to add a token
    void addToken(const std::string& type, const std::string& value) {
        tokens.push_back({type, value, lineNumber, columnNumber});
    }

    const std::vector<std::string> keywords = {
        "start", "close", "intbox", "floatbox", "stringbox", "charbox",
        "boolbox", "out", "in", "if", "else", "true", "false", "endl"
    };
    const std::vector<std::string> operators = {
        "+", "-", "*", "/", "%", "==", "!=", "<", ">", "<=", ">=", "<<", ">>", "="
    };

    // Regular expressions for token patterns
    const std::regex identifierRegex = std::regex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    const std::regex integerRegex = std::regex(R"(\d+)");
    const std::regex floatRegex = std::regex(R"(\d+\.\d+)");
    const std::regex stringLiteralRegex = std::regex(R"("(\\.|[^"\\])*")");
    const std::regex charLiteralRegex = std::regex(R"('(\\.|[^'\\])')");
    
    // Skipping comments
    void skipComments(const std::string& source, size_t& pos) {
        if (source.substr(pos, 2) == "//") {
            while (pos < source.size() && source[pos] != '\n') pos++;
        } else if (source.substr(pos, 2) == "/*") {
            pos += 2; // Skip "/*"
            while (pos < source.size() && source.substr(pos, 2) != "*/") {
                if (source[pos] == '\n') {
                    lineNumber++;
                    columnNumber = 1;
                } else {
                    columnNumber++;
                }
                pos++;
            }
            if (pos < source.size()) pos += 2; // Skip "*/"
        }
    }

public:
    TokenStore() : lineNumber(1), columnNumber(1) {}

    // Tokenize function
    void tokenize(const std::string& source) {
        size_t pos = 0;
        size_t length = source.size();

        while (pos < length) {
            char currentChar = source[pos];

            // Handle whitespace and track line/column numbers
            if (isspace(currentChar)) {
                if (currentChar == '\n') {
                    lineNumber++;
                    columnNumber = 1;
                } else {
                    columnNumber++;
                }
                pos++;
                continue;
            }

            // Skip comments
            if (pos + 1 < length && (source.substr(pos, 2) == "//" || source.substr(pos, 2) == "/*")) {
                skipComments(source, pos);
                continue;
            }

            // Match keywords or identifiers
            if (std::isalpha(currentChar) || currentChar == '_') {
                size_t start = pos;
                while (pos < length && (std::isalnum(source[pos]) || source[pos] == '_')) pos++;
                std::string word = source.substr(start, pos - start);
                columnNumber += (pos - start);
                if (std::find(keywords.begin(), keywords.end(), word) != keywords.end()) {
                    addToken("keyword", word);
                } else {
                    addToken("identifier", word);
                }
                continue;
            }

            // Match numbers (integer or float)
            if (std::isdigit(currentChar)) {
                size_t start = pos;
                while (pos < length && std::isdigit(source[pos])) pos++;
                if (pos < length && source[pos] == '.') {
                    pos++;
                    while (pos < length && std::isdigit(source[pos])) pos++;
                    columnNumber += (pos - start);
                    addToken("float_literal", source.substr(start, pos - start));
                } else {
                    columnNumber += (pos - start);
                    addToken("integer_literal", source.substr(start, pos - start));
                }
                continue;
            }

            // Match string literals
            if (currentChar == '"') {
                size_t start = pos;
                pos++;
                while (pos < length && source[pos] != '"') {
                    if (source[pos] == '\\') pos++; // Skip escaped characters
                    pos++;
                }
                if (pos < length && source[pos] == '"') pos++;
                else throw std::runtime_error("Unterminated string literal at line " +
                                              std::to_string(lineNumber));
                columnNumber += (pos - start);
                addToken("string_literal", source.substr(start, pos - start));
                continue;
            }

            // Match char literals
            if (currentChar == '\'') {
                size_t start = pos;
                pos++;
                if (pos < length && (source[pos] == '\\' || source[pos + 1] == '\'')) pos += 2;
                else pos++;
                if (pos < length && source[pos] == '\'') pos++;
                else throw std::runtime_error("Unterminated char literal at line " +
                                              std::to_string(lineNumber));
                columnNumber += (pos - start);
                addToken("char_literal", source.substr(start, pos - start));
                continue;
            }

            // Match operators
            bool matchedOperator = false;
            for (const std::string& op : operators) {
                if (source.substr(pos, op.size()) == op) {
                    addToken("operator", op);
                    columnNumber += op.size();
                    pos += op.size();
                    matchedOperator = true;
                    break;
                }
            }
            if (matchedOperator) continue;

            // Match braces
            if (currentChar == '{' || currentChar == '}') {
                addToken("brace", std::string(1, currentChar));
                columnNumber++;
                pos++;
                continue;
            }

            // Match semicolon
            if (currentChar == ';') {
                addToken("semicolon", ";");
                columnNumber++;
                pos++;
                continue;
            }

            // Unrecognized character
            throw std::runtime_error("Unknown token '" + std::string(1, currentChar) +
                                     "' at line " + std::to_string(lineNumber) +
                                     ", column " + std::to_string(columnNumber));
        }
    }

    // Get tokens
    const std::vector<Token>& getTokens() const {
        return tokens;
    }

    // Debugging utility to print all tokens
    void printTokens() const {
        for (const auto& token : tokens) {
            std::cout << "Token(" << token.type << ", " << token.value
                      << ", line " << token.line << ", column " << token.column << ")\n";
        }
    }
};

