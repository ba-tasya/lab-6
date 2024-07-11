#include "parser.h"

#include <iostream>

namespace omfl {
    bool OMFL_parser::valid() const {
        return this->validity && root->validity;
    }

    OMFL_parser::OMFL_parser(std::istream& input) {
        ParseAll(input);
    }

    OMFL_parser parse(const std::string& str) {
        auto stream = std::istringstream(str);
        return OMFL_parser(stream);
    }

    OMFL_parser parse(const std::filesystem::path& str) {
        auto stream = std::ifstream(str);
        return OMFL_parser(stream);
    }

    bool checkValidity(const std::string& str, bool is_section) {
        for (char i : str) {
            if (i >= 'a' && i <= 'z') {
                continue;
            }
            if (i >= 'A' && i <= 'Z') {
                continue;
            }
            if (i >= '0' && i <= '9') {
                continue;
            }
            if (i == '-' || i == '_' || i == '.') {
                continue;
            }
            if (is_section) {
                return false;
            }
            if (i == '+' || i == '[' || i == ']' || i == '=' || i == '#' || i == ',' || i == '"') {
                continue;
            }
        }
        return true;
    }

    bool checkValidity(const char& c) {
        if (c >= 'a' && c <= 'z') {
            return true;
        }
        if (c >= 'A' && c <= 'Z') {
            return true;
        }
        if (c >= '0' && c <= '9') {
            return true;
        }
        if (c == '-' || c == '_' || c == '.') {
            return true;
        }
        if (c == '+' || c == '[' || c == ']' || c == '=' || c == '#' || c == ',' || c == '"') {
            return true;
        }
        return false;
    }

    bool max_spaces_between_symbols = false;

    std::string OMFL_parser::ParseString(const std::string& str) {
        std::string real_str;
        if (str[0] == '[') {
            if (str[str.size() - 1] != ']') {
                validity = false;
                return "";
            }
            real_str = str.substr(1, str.size() - 2);
            bool is_section = true;
            if (!checkValidity(real_str, is_section) || real_str[0] == '.' || real_str[real_str.size() - 1] == '.') {
                validity = false;
                return "";
            }

            return '[' + real_str + ']';
        }
        int level = 0;
        bool is_string_now = false;
        int last_symbol = 0;
        int cur_ind = 0;
        bool was_not_space = false;
        max_spaces_between_symbols = false;
        std::string is_bool = "truefals";
        for (char i : str) {
            if (i == '"') {
                is_string_now = !is_string_now;
            }
            if (is_string_now) {
                real_str += i;
                continue;
            }
            if (!checkValidity(i) && !std::isspace(i)) {
                validity = false;
                return "";
            }
            if (i == '#') {
                break;
            }
            if (i == '[') {
                real_str += i;
                level++;
            } else if (i == ']') {
                real_str += i;
                level--;
            } else if (i == ',') {
                if (level == 0) {
                    level--;
                    break;
                } else {
                    real_str += i;
                }
            } else if (!std::isspace(i)) {
                real_str += i;
            } else if (std::isspace(i) && is_bool.find(real_str[real_str.size() - 1]) != -1) {
                max_spaces_between_symbols = true;
            }
            cur_ind++;
        }

        if (level != 0 || is_string_now) {
            validity = false;
            return "";
        }

        return real_str;
    }

    bool is_number(const std::string& str) {
        for (char i : str) {
            if (i >= '0' && i <= '9' || i == '+' || i == '-') {
                continue;
            }
            return false;
        }
        return true;
    }

    Something* OMFL_parser::ParseValue(const std::string& str) {
        if (str.empty()) {
            return nullptr;
        }
        if (str[0] == '[') {
            if (str.size() == 2) {
                return new Array();
            }
            Something* new_arr = new Array();
            std::string real_arr = str.substr(1, str.size() - 2);
            size_t array_begin = 0;
            size_t array_end = 0;
            size_t level = 0;
            int cur_ind = 0;
            for (char i : real_arr) {
                if (i == '[') {
                    level++;
                } else if (i == ']') {
                    level--;
                } else if (i == ',' && level == 0) {
                    array_end = cur_ind;
                    Something* value = ParseValue(real_arr.substr(array_begin, array_end - array_begin));
                    if (value != nullptr) {
                        new_arr->AddToArray(value);
                    } else {
                        validity = false;
                        return nullptr;
                    }
                    array_begin = cur_ind + 1;
                }
                cur_ind++;
            }

            Something* value = ParseValue(real_arr.substr(array_begin, real_arr.size() - array_begin));
            if (value != nullptr) {
                new_arr->AddToArray(value);
            } else {
                validity = false;
                return nullptr;
            }
            return new_arr;
        }

        if (str[0] == '"' && str[str.size() - 1] == '"' && str.size() > 1 && std::count(str.begin(), str.end(), '"') == 2) {
            return new String(str.substr(1, str.size() - 2));
        }

        if (str == "true" || str == "false") {
            if (max_spaces_between_symbols) {
                return nullptr;
            }
            return new Bool(str == "true");
        }

        if (str.find('.') != -1 && (str.rfind('+') == 0 || str.rfind('+') == std::string::npos) && (str.rfind('-') == 0 || str.rfind('-') == std::string::npos)) {
                if (str[str.size() - 1] == '.' || str[0] == '.' || str[0] == '+' && str[1] == '.' || str[0] == '-' && str[1] == '.') {
                    validity = false;
                    return nullptr;
                }
                return new Float(std::stof(std::string(str)));
        }

        if ((str.rfind('+') == 0 || str.rfind('+') == std::string::npos) && (str.rfind('-') == 0 || str.rfind('-') == std::string::npos) && is_number(str) && str[str.size() - 1] != '+' && str[str.size() - 1] != '-') {
                return new Int(std::stoi(std::string(str)));
        }

        return nullptr;
    }

    void OMFL_parser::ParseAll(std::istream& input) {
        std::string str;
        Something* section = root;

        while (std::getline(input, str)) {
            str = ParseString(str);
            if (str.empty()) {
                continue;
            }

            if (str[0] == '[') {
                if (str[str.size() - 1] != ']' || str.size() < 3 || str[1] == '.' || str[str.size() - 2] == '.') {
                    validity = false;
                    return;
                }
                std::string subsection = str.substr(1, str.size() - 2);
                // split with '.' separator
                std::istringstream split(subsection);
                std::string sub_subsection;
                section = root;
                while (std::getline(split, sub_subsection, '.')) {
                    if (sub_subsection.empty()) {
                        validity = false;
                        return;
                    }
                    if (section->IsSection()) {
                        section = section->GetSection(sub_subsection);
                    } else {
                        validity = false;
                        return;
                    }
                }
            } else {
                int separator = str.find('=');
                if (separator == -1) {
                    validity = false;
                    return;
                }
                std::string key = str.substr(0, separator);
                std::string value = str.substr(separator + 1, str.size() - separator - 1);
                bool is_section = false;
                if (key.empty() || value.empty() || !checkValidity(key, is_section) || key.find('.') != -1) {
                    validity = false;
                }

                Something* real_value = ParseValue(value);
                if (real_value == nullptr) {
                    validity = false;
                    return;
                }

                section->AddToSection(key, real_value);
            }
        }
    }
}