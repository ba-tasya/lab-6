#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace omfl {
enum valueType {
    EmptyType,
    IntType,
    FloatType,
    StringType,
    ArrayType,
    SectionType,
    BoolType
};
class Something {
public:
    valueType type = valueType::EmptyType;
    bool validity = true;

    [[nodiscard]] bool IsEmpty() const {
        return type == valueType::EmptyType;
    }
    [[nodiscard]] bool IsInt() const {
        return type == valueType::IntType;
    }
    [[nodiscard]] bool IsFloat() const {
        return type == valueType::FloatType;
    }
    [[nodiscard]] bool IsString() const {
        return type == valueType::StringType;
    }
    [[nodiscard]] bool IsArray() const {
        return type == valueType::ArrayType;
    }
    [[nodiscard]] bool IsSection() const {
        return type == valueType::SectionType;
    }
    [[nodiscard]] bool IsBool() const {
        return type == valueType::BoolType;
    }

    [[nodiscard]] virtual int AsInt() const {
        // something went wrong
    }
    [[nodiscard]] virtual float AsFloat() const {
        // something went wrong
    }
    [[nodiscard]] virtual std::string AsString() const {
        // something went wrong
    }
    [[nodiscard]] virtual bool AsBool() const {
        // something went wrong
    }

    [[nodiscard]] virtual int AsIntOrDefault(int value) const {
        return value;
    }
    [[nodiscard]] virtual float AsFloatOrDefault(float value) const {
        return value;
    }
    [[nodiscard]] virtual std::string AsStringOrDefault(const std::string value) const {
        return value;
    }
    [[nodiscard]] virtual bool AsBoolOrDefault(bool value) const {
        return value;
    }

    [[nodiscard]] virtual const Something& Get(const std::string& key) const {
        // something went wrong
    }
    [[nodiscard]] virtual const Something& operator[](int ind) const {
        // something went wrong
    }

    [[nodiscard]] virtual Something* AddToSection(const std::string& key, Something* value) {
        // something went wrong
    }
    [[nodiscard]] virtual Something* AddToArray(Something* value) {
        // something went wrong
    }

    [[nodiscard]] virtual Something* GetSection(const std::string& key) {
        // something went wrong
    }

    [[nodiscard]] virtual Something* CreateIdenticalObject() const {
        return new Something(this->type);
    }

    explicit Something(valueType type) : type(type) {}

    // the rule of 3
    virtual ~Something() = default;
    Something& operator=(const Something& other) = default;
    Something(const Something& other) = default;
};

class Empty : public Something {
public:
    Empty() : Something(valueType::EmptyType) {}
};

class Int : public Something {
public:
    int value;

    [[nodiscard]] int AsInt() const override {
        return value;
    }
    [[nodiscard]] int AsIntOrDefault(int default_value) const override {
        return value;
    }

    [[nodiscard]] Something* CreateIdenticalObject() const override {
        return new Int(value);
    }

    explicit Int(int value) : Something(valueType::IntType), value(value) {}
};

class Float : public Something {
public:
    float value;

    [[nodiscard]] float AsFloat() const override {
        return value;
    }
    [[nodiscard]] float AsFloatOrDefault(float default_value) const override {
        return value;
    }

    [[nodiscard]] Something* CreateIdenticalObject() const override {
        return new Float(value);
    }

    explicit Float(float value) : Something(valueType::FloatType), value(value) {}
};

class String : public Something {
public:
    std::string value;

    [[nodiscard]] std::string AsString() const override {
        return value;
    }
    [[nodiscard]] std::string AsStringOrDefault(std::string default_value) const override {
        return value;
    }

    [[nodiscard]] Something* CreateIdenticalObject() const override {
        return new String(value);
    }

    explicit String(std::string value) : Something(valueType::StringType), value(value) {}
};

class Array : public Something {
public:
    std::vector<Something*> value;

    ~Array() override;
    Array& operator=(const Array& rhs);
    Array(const Array& original);

    [[nodiscard]] const Something& operator[](int ind) const override;
    Something* AddToArray(Something* value) override {
        this->value.push_back(value);
        return this->value.back();
    }
    [[nodiscard]] Something* CreateIdenticalObject() const override;

    Array() : Something(valueType::ArrayType) {}
};

class Section : public Something {
public:
    std::map<std::string, Something*> value;

    ~Section() override;
    Section& operator=(const Section& rhs);
    Section(const Section& original);

    Something* AddToSection(const std::string& key, Something* value) override;
    Something* GetSection(const std::string& key) override;
    [[nodiscard]] Something* CreateIdenticalObject() const override;
    [[nodiscard]] const Something& Get(const std::string& key) const override;

    Section() : Something(valueType::SectionType) {}
};

class Bool : public Something {
public:
    bool value;

    [[nodiscard]] bool AsBool() const override {
        return value;
    }
    [[nodiscard]] bool AsBoolOrDefault(bool default_value) const override {
        return value;
    }

    [[nodiscard]] Something* CreateIdenticalObject() const override {
        return new Bool(value);
    }

    explicit Bool(bool value) : Something(valueType::BoolType), value(value) {}
};
}