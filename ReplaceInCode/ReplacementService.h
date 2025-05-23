#pragma once
#include <regex>

struct ReplacementServiceItem {
	ReplacementServiceItem() = default;
	virtual ~ReplacementServiceItem();
	bool replaceOnce{};
	std::regex pattern{};
	virtual std::string getReplacement(const std::string& line) = 0;
};

struct ReplaceDataLoad : public ReplacementServiceItem {
	ReplaceDataLoad() {
		replaceOnce = false;
		pattern = R"((\s*)data::Load\(\s*\"(.*/(.*\.csv|xml|bin))\".*\);)";
	}
	virtual std::string getReplacement(const std::string& line) override {
		std::smatch match;

		if (std::regex_search(line, match, pattern)) {
			std::string fullPath = match[1].str();  // z. B. "../../../data/cifar-10_test.csv"
			std::string fileName = match[2].str();  // z. B. "cifar-10_test.csv"

			std::string fnameLine = "fname = Helper::findFileAboveCurrentDirectory(\"" + fileName + "\").value();\n";
			std::string newLine = std::regex_replace(line, pattern, "data::Load(fname,");

			return fnameLine + newLine;
		}
		return line;
	}
};

struct ReplaceInclude : public ReplacementServiceItem {
	ReplaceInclude() {
		replaceOnce = true;
		pattern = R"(\s*#include\s*<\s*mlpack.hpp\s*>)";
	}
	virtual std::string getReplacement(const std::string& line) override {
		std::smatch match;

		if (std::regex_search(line, match, pattern)) {
			return "#inlcde <mlpack.hpp>\n#include \"Helper.h\"";
		}
		return line;
	}
};

struct ReplaceMain : public ReplacementServiceItem {
	bool postPoneReplacement{};
	ReplaceMain() {
		replaceOnce = true;
		pattern = R"(\s*int\s*main\s*\(\s*(\w*\s*\w*\s*,\s*\w*\s*\*\s*\w*\s*\[\s*\])\s*\)\s*{)";
	}
	void preProcess(const std::string& line) {
		std::smatch match;
		auto pattern1 = std::regex{ R"(\s*int\s*main\s*\(\s*(\w*\s*\w*\s*,\s*\w*\s*\*\s*\w*\s*\[\s*\])\s*\)\s*)" };
		auto pattern2 = std::regex{ R"(\s*int\s*main\s*\(\s*(\w*\s*\w*\s*,\s*\w*\s*\*\s*\w*\s*\[\s*\])\s*\)\s*{)" };
		postPoneReplacement = (std::regex_search(line, match, pattern1)) && (std::regex_search(line, match, pattern2));

	}
	virtual std::string getReplacement(const std::string& line) override {
		std::smatch match;

		if (std::regex_search(line, match, pattern)) {
			return std::string{ match[1].str() + '\n' + '\t' + "Helper::Timer tim;" };
		}
		return line;
	}
};