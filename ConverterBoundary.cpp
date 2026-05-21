#include "Converter.h"
#include "ConverterInternal.h"

#include <iomanip>
#include <stdexcept>
#include <sstream>

namespace {

struct ParsedInput {
    std::string unit;
    std::string valueText;
    double value;
};

constexpr const char* kMissingDelimiterMessage = "missing unit:value delimiter";
constexpr const char* kUnknownUnitMessage = "unknown unit";
constexpr const char* kInvalidNumberMessage = "invalid number";
constexpr const char* kNegativeValueMessage = "negative value";

ParsedInput parseInput(const std::string& input) {
    const auto delimiter = input.find(':');
    if (delimiter == std::string::npos) {
        throw std::invalid_argument(kMissingDelimiterMessage);
    }

    const auto fromUnit = input.substr(0, delimiter);
    if (!isKnownUnit(fromUnit)) {
        throw std::invalid_argument(kUnknownUnitMessage);
    }

    const auto valueText = input.substr(delimiter + 1);
    std::size_t parsedLength = 0;
    const auto value = std::stod(valueText, &parsedLength);
    if (parsedLength != valueText.size()) {
        throw std::invalid_argument(kInvalidNumberMessage);
    }

    if (value < 0.0) {
        throw std::invalid_argument(kNegativeValueMessage);
    }

    return {fromUnit, valueText, value};
}

std::string formatConversion(const ParsedInput& input, const std::string& toUnit) {
    std::ostringstream output;
    output << input.valueText << ' ' << input.unit << " = " << std::fixed << std::setprecision(6)
           << convert(input.unit, input.value, toUnit) << ' ' << toUnit;
    return output.str();
}

} // namespace

std::string convertInput(const std::string& input, const std::string& toUnit) {
    return formatConversion(parseInput(input), toUnit);
}
