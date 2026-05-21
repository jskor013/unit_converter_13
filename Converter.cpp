#include <string>

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

double convert(const std::string& fromUnit, double value, const std::string& toUnit) {
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
