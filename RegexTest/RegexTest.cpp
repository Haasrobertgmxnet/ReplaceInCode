#include <iostream>
#include <regex>
#include <string>

int main() {
    std::string codeLine = R"(data::Load("../../../data/cifar-10_test.csv", dataset, true);)";

    // Raw String korrekt gesetzt (R"(...)")
    std::regex pattern(R"(data::Load\(\s*\"(.*?)/([^/]+\.(csv|xml))\",\s*(\w+),\s*true\s*\);)");

    std::smatch matches;
    if (std::regex_match(codeLine, matches, pattern)) {
        std::cout << "Pfad: " << matches[1].str() << std::endl;
        std::cout << "Dateiname: " << matches[2].str() << std::endl;
        std::cout << "Endung: " << matches[3].str() << std::endl;
        std::cout << "Datensatzvariable: " << matches[4].str() << std::endl;
    }
    else {
        std::cout << "Kein Treffer." << std::endl;
    }

    return 0;
}
