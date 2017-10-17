#include "LexicalAnalyzer.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "\nPascal Compiler\nAlexander Gomeniuk, Far Eastern FU -- B8303a, 2017\n\n";
        std::cout << "usage: PascalCompiler [-l] File\n";
        std::cout << "-l\tlexical analysis\n";
    };

    for (int i = 0; i < argc; ++i)
        if (std::string(argv[i]) == "-l")
            LexicalAnalyzer(argv[i + 1]).log(std::ofstream("tokens.log"));
}