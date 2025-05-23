#include <filesystem>  
#include <print>  
#include <regex>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

const std::string searchPath{ "C:/Users/haasr/Documents/OpenSourceProjects/MLPack/examples_repo/examples/cpp" };  
// const std::regex pattern(R"(data::Load\(\s*\"(.*?)/([^/]+\.(csv|xml))\",\s*(\w+),\s*true\s*\);)");
// const std::regex pattern(R"(\s*data::Load\(\s*\"(.*?)/([^/]+\.(csv|xml))\",\s*(\w+),\s*true\s*\);)");
const std::regex pattern(R"((\s*)data::Load\(\s*\"(.*/(.*\.csv|xml|bin))\".*\);)");
// const std::regex pattern(R"(\s*data::Load\(\s*\"([^\"]+?\.(csv|xml|bin))\"\s*(,\s*[^)]+){1,3}\s*\))");
//const std::regex pattern(R"(\s*data::Load\(\s*\"(.*).(csv|xml|bin)\"(.*)\);)");

std::string readFileToString(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Datei konnte nicht geöffnet werden: " + filename);
    }

    std::ostringstream ss;
    ss << file.rdbuf();  // ganzer Stream -> String
    return ss.str();
}

std::vector<std::string> splitLines(const std::string& text) {
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string line;

    while (std::getline(stream, line)) {
        lines.push_back(line);
    }

    return lines;
}

std::string replaceLoadCall(const std::string& input) {
    std::smatch match;

    if (std::regex_search(input, match, pattern)) {
        std::string fullPath = match[1].str();  // z. B. "../../../data/cifar-10_test.csv"
        std::string fileName = match[2].str();  // z. B. "cifar-10_test.csv"

        std::string fnameLine = "fname = Helper::findFileAboveCurrentDirectory(\"" + fileName + "\").value();\n";
        std::string newLine = std::regex_replace(input, pattern, "data::Load(fname,");

        return fnameLine + newLine;
    }

    return input; // kein Match → Original zurückgeben
}

std::string replaceFirstArgWithFname(const std::string& line) {
    // Regex: Erster Parameter ist ein String (Pfad), danach folgt ein Komma
    std::regex pattern(R"(data::Load\(\s*\"[^\"]+\"\s*,)");
    return std::regex_replace(line, pattern, "data::Load(fname,");
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) return; // Endlosschleife vermeiden
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Weiterschieben, um Mehrfachtreffer zu finden
    }
}

void saveToFile(const std::string& content, const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Fehler beim Öffnen der Datei: " << filename << std::endl;
        return;
    }
    outFile << content;
    outFile.close();
}

int main() {
    try {
        for (const auto& entry : fs::recursive_directory_iterator(searchPath)) {

            auto hasFileContentChanged = bool{ false };
            if (!entry.is_regular_file()) {
                continue;
            }
            if (!(entry.path().extension() == ".cpp")) {
                continue;
            }
            std::cout << "Datei: " << entry.path().filename() << '\n';
            std::string content{ readFileToString(entry.path().string()) };

            // replace #include <mplpack.hpp>
            std::regex pattern{ R"(\s*#include\s*<\s*mlpack.hpp\s*>)" };
            std::regex_replace(content, pattern, "#inlcde <mlpack.hpp>\n#include \"Helper.h\"");

            // replace int main()
            pattern = R"(\s*int\s*main\s*\(\s*(\w*\s*\w*\s*,\s*\w*\s*\*\s*\w*\s*\[\s*\])\s*\)\s*\n*\s*\{)";
            std::smatch matches;
            if (std::regex_match(content, matches, pattern)) {
                std::cout << "Pfad: " << matches[2].str() << std::endl;
                std::regex_replace(content, pattern, std::string{ matches[1].str() + '\n' + '\t' + "Helper::Timer tim;" + '\n' + '\t' + "std::string fname{};"});
            }

            // replace data::Load
            auto splittedContent = splitLines(content);
            for(auto& line : splittedContent){
                if (line.find("data::Load") != std::string::npos &&
                    // line.find("//") != std::string::npos &&
                    line.find("//") < line.find("data::Load")) {
                    continue;
                }

                std::smatch matches;
                if (std::regex_match(line, matches, pattern)) {
                    std::cout << "Datei: " << entry.path().filename() << '\n';
                    std::cout << "Gelesene Zeile: " << line << std::endl;
                    std::cout << "Pfad: " << matches[2].str() << std::endl;
                    std::cout << "Dateiname: " << matches[3].str() << std::endl;
                    std::cout << std::endl;
                }
                else {
                    continue;
                }

                auto newline{ matches[1].str() + "fname = Helper::findFileAboveCurrentDirectory(\"" + matches[3].str() + "\").value();\n" + replaceFirstArgWithFname(line) + "\n//" + line};
                std::println("Line {}", line);
                std::println("Replaced by:\n{}", newline);
                std::cout << std::endl;


                replaceAll(content, line, newline);
                hasFileContentChanged = true;
            }

            if (!hasFileContentChanged) {
                std::println("Next File");
                continue;
            }
            //auto destPath = std::string{ entry.path().parent_path().string() + "/" + entry.path().stem().string() + "-mod.cpp"};
            //saveToFile(content, destPath );
            saveToFile(content, entry.path().string());
            std::println("Next File");
            
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Fehler: " << e.what() << '\n';
    }

    //auto dir = fs::directory_iterator(searchpath1);
    //for (auto&& item : dir) {
    //    std::cout << "Item: " << item << std::endl;
    //    //std::println("Item: {}", item);
    //}
    return 0;  
}