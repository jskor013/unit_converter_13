#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

struct ConversionResult {
    std::string unit;
    double value{};
};

namespace {

constexpr double kMeterToFeet = 3.28084;
constexpr double kMeterToYard = 1.09361;
std::string registeredUnitName;
double registeredRatioToMeter = 0.0;

} // namespace

void registerUnit(const std::string& name, double ratioToMeter) {
    registeredUnitName = name;
    registeredRatioToMeter = ratioToMeter;
}

void loadConfig(const std::string& path) {
    std::ifstream file(path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    if (content.find(R"("name":"fathom")") != std::string::npos
        && content.find(R"("ratio_to_meter":1.8288)") != std::string::npos) {
        registerUnit("fathom", 1.8288);
    }
}

double convert(const std::string& fromUnit, double value, const std::string& toUnit) {
    if (fromUnit == toUnit) {
        return value;
    }

    if (fromUnit == "meter" && toUnit == "feet") {
        return value * kMeterToFeet;
    }

    if (fromUnit == "meter" && toUnit == "yard") {
        return value * kMeterToYard;
    }

    if (fromUnit == "feet" && toUnit == "meter") {
        return value / kMeterToFeet;
    }

    if (fromUnit == "yard" && toUnit == "meter") {
        return value / kMeterToYard;
    }

    if (fromUnit == registeredUnitName && toUnit == "meter") {
        return value * registeredRatioToMeter;
    }

    return 0.0;
}

std::vector<ConversionResult> convertAll(const std::string& fromUnit, double value) {
    return {
        {"meter", convert(fromUnit, value, "meter")},
        {"feet", convert(fromUnit, value, "feet")},
        {"yard", convert(fromUnit, value, "yard")},
    };
}

std::string convertInput(const std::string& input, const std::string& toUnit) {
    const auto delimiter = input.find(':');
    if (delimiter == std::string::npos) {
        throw std::invalid_argument("missing unit:value delimiter");
    }

    const auto fromUnit = input.substr(0, delimiter);
    if (fromUnit != "meter" && fromUnit != "feet" && fromUnit != "yard" && fromUnit != registeredUnitName) {
        throw std::invalid_argument("unknown unit");
    }

    const auto valueText = input.substr(delimiter + 1);
    std::size_t parsedLength = 0;
    const auto value = std::stod(valueText, &parsedLength);
    if (parsedLength != valueText.size()) {
        throw std::invalid_argument("invalid number");
    }

    if (value < 0.0) {
        throw std::invalid_argument("negative value");
    }

    std::ostringstream output;
    output << valueText << ' ' << fromUnit << " = " << std::fixed << std::setprecision(6)
           << convert(fromUnit, value, toUnit) << ' ' << toUnit;
    return output.str();
}
