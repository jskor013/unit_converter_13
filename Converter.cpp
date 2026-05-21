#include <string>

namespace {

constexpr double kMeterToFeet = 3.28084;
constexpr double kMeterToYard = 1.09361;

} // namespace

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

    return 0.0;
}
