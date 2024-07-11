#pragma once

#include "Something.h"

#include <filesystem>
#include <istream>
#include <fstream>
#include <sstream>


namespace omfl {

class OMFL_parser {
public:
    bool validity = true;
    Section* root = new Section();

    void ParseAll(std::istream& input);
    Something *ParseValue(const std::string &str);
    std::string ParseString(const std::string &str);

    [[nodiscard]] bool valid() const;
    [[nodiscard]] const Something& Get(const std::string& key) const {
        return root->Get(key);
    }

    explicit OMFL_parser(std::istream& input);
};

OMFL_parser parse(const std::filesystem::path& path);
OMFL_parser parse(const std::string& str);


}// namespace