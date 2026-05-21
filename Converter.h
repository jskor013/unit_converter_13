#pragma once

#include <string>
#include <vector>

struct ConversionResult {
    std::string unit;
    double value{};
};

double convert(const std::string& fromUnit, double value, const std::string& toUnit);
std::vector<ConversionResult> convertAll(const std::string& fromUnit, double value);
void registerUnit(const std::string& name, double ratioToMeter);
void loadConfig(const std::string& path);
std::string convertInput(const std::string& input, const std::string& toUnit);
