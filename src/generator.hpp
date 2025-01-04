// generator.hpp
#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

class CodeGenerator {
private:
    std::ofstream asmFile;
    int tempVarCounter = 0;

    // Utility to create unique temporary variable names
    std::string generateTempVar() {
        return "temp" + std::to_string(tempVarCounter++);
    }

    // Generate code for variable declaration
    void generateVariableDeclaration(const std::string& varName, const std::string& type) {
        if (type == "intbox") {
            asmFile << varName << " dd 0\n";  // Declare a 32-bit integer
        } else if (type == "floatbox") {
            asmFile << varName << " dq 0.0\n";  // Declare a 64-bit float
        }
    }

    // Generate code for assignment (e.g., x = 5)
    void generateAssignment(const std::string& varName, const std::string& value) {
        asmFile << "mov eax, " << value << "\n";  // Load value into eax
        asmFile << "mov [" << varName << "], eax\n";  // Store value in the variable
    }

    // Generate code for output (e.g., out << x)
    void generateOutput(const std::string& expr) {
        asmFile << "mov eax, " << expr << "\n";  // Load value into eax
        asmFile << "call print_int\n";  // Call a function to print the value
    }

    // Generate code for input (e.g., in >> x)
    void generateInput(const std::string& varName) {
        asmFile << "call read_int\n";  // Call a function to read an integer
        asmFile << "mov [" << varName << "], eax\n";  // Store the input in the variable
    }

    // Generate code for an if statement
    void generateIfStatement(const std::string& condition, const std::string& trueBlock, const std::string& falseBlock) {
        std::string endLabel = generateTempVar();
        asmFile << "cmp " << condition << ", 0\n";  // Compare the condition
        asmFile << "je " << endLabel << "\n";  // Jump to end if false
        asmFile << trueBlock;  // Generate code for the true block
        asmFile << "jmp " << endLabel << "\n";  // Jump to end after true block
        asmFile << endLabel << ":\n";  // Label for the end
        asmFile << falseBlock;  // Generate code for the false block
    }

public:
    CodeGenerator(const std::string& filename) {
        asmFile.open(filename);
        if (!asmFile.is_open()) {
            throw std::runtime_error("Failed to open ASM file.");
        }
    }

    ~CodeGenerator() {
        if (asmFile.is_open()) {
            asmFile.close();
        }
    }

    // Generate the entire assembly code for a program
    void generateCode(const std::vector<std::string>& statements) {
        asmFile << "section .data\n";  // Data section for variables
        for (const auto& stmt : statements) {
            if (stmt == "intbox x = 5;") {
                generateVariableDeclaration("x", "intbox");
                generateAssignment("x", "5");
            }
            // Add more cases for other statements...
        }

        asmFile << "section .text\n";
        asmFile << "global _start\n";
        asmFile << "_start:\n";

        for (const auto& stmt : statements) {
            if (stmt == "out << x;") {
                generateOutput("x");
            }
            // Add more cases for other statements...
        }
    }
};
