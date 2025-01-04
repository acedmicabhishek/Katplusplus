#include <iostream>
#include <fstream>
#include <filesystem>
#include "tokenstore.hpp"
#include "parser.hpp"

int main(int argc, char* argv[]) {
    std::cout << "Compiler started\n";

    // Check if a .kat file was provided
    if (argc < 2) {
        std::cerr << "Usage: kat_compiler <file.kat>" << std::endl;
        return 1;
    }

    std::filesystem::path katFile = argv[1];
    if (katFile.extension() != ".kat") {
        std::cerr << "Error: Input file must have a .kat extension." << std::endl;
        return 1;
    }

    // Open the file
    std::ifstream file(katFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << katFile << std::endl;
        return 1;
    }

    // Read the entire source code from the file
    std::string sourceCode((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    std::cout << "Source code loaded successfully.\n";

    try {
        // Tokenize the source code
        TokenStore tokenStore;
        tokenStore.tokenize(sourceCode);

        std::cout << "Tokenization completed successfully. Tokens:\n";
        tokenStore.printTokens();

        // Parse the tokens
        Parser parser(tokenStore.getTokens());  // Pass tokens to the parser
        parser.parse();  // Start parsing

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
