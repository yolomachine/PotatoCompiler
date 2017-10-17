#include <iostream>
#include "LexicalAnalyzer.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "\n\tPascal Compiler\n\tAlexander Gomeniuk, Far Eastern FU -- B8303a, 2017\n";
    }
    
    LexicalAnalyzer la;
    la.enableStoringTokens();
    la.open("test.txt");
    Token t;
    while (!la.eof()) t = la.nextToken();
    getc(stdin);
}