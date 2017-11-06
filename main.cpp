#include "LexicalAnalyzer.hpp"
#include "Parser.hpp"
#include <locale>
#include <codecvt>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "\nPascal Compiler\nAlexander Gomeniuk, Far Eastern FU -- B8303a, 2017\n\n";
        std::cout << "usage: PascalCompiler [-l] File\n";
        std::cout << "-l\tlexical analysis\n";
    };

    for (int i = 0; i < argc; ++i)
        if (std::string(argv[i]) == "-l")
            LexicalAnalyzer(argv[i + 1]).log(std::ofstream("tokens.log"));

    const std::locale utf8_locale = std::locale(std::locale(), new std::codecvt_utf8<wchar_t>());
    std::wofstream stream("syntax.log");
    stream.imbue(utf8_locale);

    Parser p("input.txt");
    p.log(stream);
}