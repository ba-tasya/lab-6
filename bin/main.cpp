#include "lib/parser.h"

int main(int, char**) {
    const auto test = omfl::parse(std::string("key = \"value\""));
    std::cout << test.validity;
    return 0;
}
