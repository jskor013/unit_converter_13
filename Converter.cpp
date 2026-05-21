#include <string>

namespace {

constexpr double kMeterToFeet = 3.28084;

} // namespace

double convert(const std::string& fromUnit, double value, const std::string& toUnit) {
    if (fromUnit == "meter" && toUnit == "feet") {
        return value * kMeterToFeet;
    }

    return 0.0;
}
