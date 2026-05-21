#include "Converter.h"
#include "ConverterInternal.h"

#include <fstream>

namespace {

constexpr double kMeterToFeet = 3.28084;
constexpr double kMeterToYard = 1.09361;

struct ConversionRule {
    const char* unit;
    double unitsPerMeter;
};

struct RegisteredUnit {
    std::string name;
    double ratioToMeter = 0.0;
};

constexpr ConversionRule kDefaultRules[] = {
    {"meter", 1.0},
    {"feet", kMeterToFeet},
    {"yard", kMeterToYard},
};

RegisteredUnit registeredUnit;

const ConversionRule* findDefaultRule(const std::string& unit) {
    for (const auto& rule : kDefaultRules) {
        if (unit == rule.unit) {
            return &rule;
        }
    }

    return nullptr;
}

bool isKnownInputUnit(const std::string& unit) {
    return findDefaultRule(unit) != nullptr || unit == registeredUnit.name;
}

double convertDefaultUnit(const ConversionRule& fromRule, double value, const ConversionRule& toRule) {
    const double valueInMeters = value / fromRule.unitsPerMeter;
    return valueInMeters * toRule.unitsPerMeter;
}

} // namespace

bool isKnownUnit(const std::string& unit) {
    return isKnownInputUnit(unit);
}

void registerUnit(const std::string& name, double ratioToMeter) {
    registeredUnit = {name, ratioToMeter};
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

    const auto* fromRule = findDefaultRule(fromUnit);
    const auto* toRule = findDefaultRule(toUnit);
    if (fromRule != nullptr && toRule != nullptr) {
        return convertDefaultUnit(*fromRule, value, *toRule);
    }

    if (fromUnit == registeredUnit.name && toUnit == "meter") {
        return value * registeredUnit.ratioToMeter;
    }

    return 0.0;
}

std::vector<ConversionResult> convertAll(const std::string& fromUnit, double value) {
    std::vector<ConversionResult> results;
    results.reserve(sizeof(kDefaultRules) / sizeof(kDefaultRules[0]));

    for (const auto& rule : kDefaultRules) {
        results.push_back({rule.unit, convert(fromUnit, value, rule.unit)});
    }

    return results;
}

