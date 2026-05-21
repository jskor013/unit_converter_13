#include "Converter.h"
#include "ConverterInternal.h"

#include <iostream>
#include <string>

int main() {
    std::cout << "Insert value for converting (ex: meter:2.5): ";

    std::string input;
    std::getline(std::cin, input);

    std::string unit;
    double value = 0.0;

    std::size_t pos = input.find(':');
    if (pos == std::string::npos) {
        std::cerr << "Invalid format. Use unit:value (ex: meter:2.5)" << std::endl;
        return 1;
    }

    unit = input.substr(0, pos);
    std::string valueStr = input.substr(pos + 1);

    try {
        value = std::stod(valueStr);
    } catch (...) {
        std::cerr << "Invalid number: " << valueStr << std::endl;
        return 1;
    }

    if (!isKnownUnit(unit)) {
        std::cerr << "Unknown unit: " << unit << std::endl;
        return 1;
    }

    for (const auto& result : convertAll(unit, value)) {
        std::cout << value << " " << unit << " = " << result.value << " " << result.unit << std::endl;
    }

    return 0;
}
