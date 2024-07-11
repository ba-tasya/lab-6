#include "Something.h"

namespace omfl {
    Something* Array::CreateIdenticalObject() const {
        auto* new_arr = new Array();
        for (const auto& cur_value : value) {
            new_arr->AddToArray(cur_value->CreateIdenticalObject());
        }

        return new_arr;
    }

    //copy constructor
    Array::Array(const Array& other) : Something(other) {
        for (const auto& cur_value : other.value) {
            this->value.push_back(cur_value->CreateIdenticalObject());
        }
    }

    Array::~Array() {
        for (auto& cur_value : value) {
            delete cur_value;
        }
    }

    const Something& Array::operator[](int ind) const {
        if (ind >= value.size()) {
            return *new Empty();
        }

        return *value[ind];
    }

    Array& Array::operator=(const Array& rhs) {
        if (this == &rhs) {
            return *this;
        }
        for (auto& cur_value : value) {
            delete cur_value;
        }
        value.clear();
        for (const auto& cur_value : rhs.value) {
            value.push_back(cur_value->CreateIdenticalObject());
        }

        return *this;
    }

    Something* Section::CreateIdenticalObject() const {
        auto* new_sect = new Section();
        for (const auto& cur_value : value) {
            new_sect->AddToSection(cur_value.first, cur_value.second->CreateIdenticalObject());
        }

        return new_sect;
    }

    Section::Section(const Section& original) : Something(original) {
        for (const auto& cur_value : original.value) {
            value[cur_value.first] = cur_value.second->CreateIdenticalObject();
        }
    }

    const Something& Section::Get(const std::string& key) const {
        int end_section = key.find('.');
        if (end_section == -1) {
            auto cur_value = value.find(key);
            if (cur_value == value.end()) {
                return *new Empty();
            }
            return *cur_value->second;
        }

        std::string section = key.substr(0, end_section);
        std::string subsection = key.substr(end_section + 1);
        auto cur_section = value.find(section);
        if (cur_section == value.end()) {
            return *new Empty();
        }

        return cur_section->second->Get(subsection);
    }

    Section::~Section() {
        for (auto& cur_value : value) {
            delete cur_value.second;
        }
    }

    Section& Section::operator=(const Section& rhs) {
        if (this == &rhs) {
            return *this;
        }
        for (auto& cur_value : value) {
            delete cur_value.second;
        }
        value.clear();
        for (const auto& cur_value : rhs.value) {
            value[cur_value.first] = cur_value.second->CreateIdenticalObject();
        }

        return *this;
    }

    Something* Section::AddToSection(const std::string& key, Something* value) {
        if (this->value.find(key) != this->value.end()) {
            this->validity = false;
        }

        this->value[key] = value;
        return this->value[key];
    }

    Something* Section::GetSection(const std::string& key) {
        auto section = this->value.find(key);
        if (section == this->value.end()) {
            this->value[key] = new Section();
            return this->value[key];
        }

        return section->second;
    }
}