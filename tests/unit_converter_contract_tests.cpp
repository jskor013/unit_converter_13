#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

constexpr double kTolerance = 0.000001;
constexpr double kMeterToFeet = 3.28084;
constexpr double kMeterToYard = 1.09361;
constexpr double kCubitToMeter = 0.4572;
constexpr double kFathomToMeter = 1.8288;

struct ConversionResult {
    std::string sourceUnit;
    double sourceAmount{};
    std::string targetUnit;
    double convertedAmount{};
};

enum class OutputFormat {
    Json,
    Csv,
    Table,
};

class UnitConverterContractError : public std::runtime_error {
public:
    UnitConverterContractError(std::string errorCode, const std::string& message)
        : std::runtime_error(message)
        , code(std::move(errorCode)) {}

    std::string code;
};

class UnitConverterContractDriver {
public:
    ConversionResult convert(const std::string&, double, const std::string&) {
        throwMissingPublicApi();
    }

    ConversionResult convertInput(const std::string&, const std::string&) {
        throwMissingPublicApi();
    }

    void registerUnit(const std::string&, double) {
        throwMissingPublicApi();
    }

    void loadConfig(const std::string&) {
        throwMissingPublicApi();
    }

    std::string format(const std::string&, OutputFormat) {
        throwMissingPublicApi();
    }

private:
    [[noreturn]] static void throwMissingPublicApi() {
        throw UnitConverterContractError(
            "PUBLIC_API_MISSING",
            "No production public API is available for Catch2 contract tests. "
            "Expose parse, convert, registerUnit, loadConfig, and format contracts.");
    }
};

UnitConverterContractDriver makeDriver() {
    return UnitConverterContractDriver{};
}

template <typename Action>
void requireContractError(Action action, const std::string& expectedCode) {
    try {
        action();
    } catch (const UnitConverterContractError& error) {
        INFO("Observed error code: " << error.code);
        INFO("Observed message: " << error.what());
        REQUIRE(error.code == expectedCode);
        return;
    } catch (const std::exception& error) {
        FAIL("Expected UnitConverterContractError with code " << expectedCode
             << " but caught std::exception: " << error.what());
    }

    FAIL("Expected UnitConverterContractError with code " << expectedCode);
}

std::filesystem::path writeTempConfig(const std::string& fileName, const std::string& content) {
    const auto path = std::filesystem::temp_directory_path() / fileName;
    std::ofstream file(path);
    REQUIRE(file.is_open());
    file << content;
    return path;
}

bool contains(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}

} // namespace

TEST_CASE("test_meter_to_feet_one_meter_returns_readme_ratio", "[conversion][normal]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: converting one meter to feet.
    const auto result = driver.convert("meter", 1.0, "feet");

    // Then: the README meter-to-feet ratio is preserved.
    REQUIRE(result.sourceUnit == "meter");
    REQUIRE(result.sourceAmount == Catch::Approx(1.0).margin(kTolerance));
    REQUIRE(result.targetUnit == "feet");
    REQUIRE(result.convertedAmount == Catch::Approx(kMeterToFeet).margin(kTolerance));
}

TEST_CASE("test_meter_to_feet_decimal_amount_returns_scaled_ratio", "[conversion][normal]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: converting 2.5 meters to feet.
    const auto result = driver.convert("meter", 2.5, "feet");

    // Then: the amount is multiplied by the meter-to-feet ratio.
    REQUIRE(result.convertedAmount == Catch::Approx(2.5 * kMeterToFeet).margin(kTolerance));
}

TEST_CASE("test_meter_to_yard_one_meter_returns_readme_ratio", "[conversion][normal]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: converting one meter to yard.
    const auto result = driver.convert("meter", 1.0, "yard");

    // Then: the README meter-to-yard ratio is preserved.
    REQUIRE(result.targetUnit == "yard");
    REQUIRE(result.convertedAmount == Catch::Approx(kMeterToYard).margin(kTolerance));
}

TEST_CASE("test_feet_to_meter_one_foot_returns_reverse_ratio", "[conversion][normal]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: converting one foot back to meter.
    const auto result = driver.convert("feet", 1.0, "meter");

    // Then: the reverse conversion divides by the meter-to-feet ratio.
    REQUIRE(result.targetUnit == "meter");
    REQUIRE(result.convertedAmount == Catch::Approx(1.0 / kMeterToFeet).margin(kTolerance));
}

TEST_CASE("test_feet_to_yard_hub_conversion_returns_meter_based_ratio", "[conversion][normal]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: converting feet to yard.
    const auto result = driver.convert("feet", kMeterToFeet, "yard");

    // Then: conversion passes through the meter hub and returns one meter in yards.
    REQUIRE(result.convertedAmount == Catch::Approx(kMeterToYard).margin(kTolerance));
}

TEST_CASE("test_yard_to_feet_hub_conversion_returns_meter_based_ratio", "[conversion][normal]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: converting yard to feet.
    const auto result = driver.convert("yard", kMeterToYard, "feet");

    // Then: conversion passes through the meter hub and returns one meter in feet.
    REQUIRE(result.convertedAmount == Catch::Approx(kMeterToFeet).margin(kTolerance));
}

TEST_CASE("test_meter_to_feet_zero_value_returns_zero", "[boundary][conversion]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: converting zero meters to feet.
    const auto result = driver.convert("meter", 0.0, "feet");

    // Then: zero is accepted and preserved as zero.
    REQUIRE(result.convertedAmount == Catch::Approx(0.0).margin(kTolerance));
}

TEST_CASE("test_feet_to_yard_zero_value_returns_zero", "[boundary][conversion]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: converting zero feet to yard.
    const auto result = driver.convert("feet", 0.0, "yard");

    // Then: zero remains zero through the meter hub.
    REQUIRE(result.convertedAmount == Catch::Approx(0.0).margin(kTolerance));
}

TEST_CASE("test_meter_to_feet_large_value_returns_finite_result", "[boundary][conversion]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: converting a very large meter value to feet.
    const auto result = driver.convert("meter", 1'000'000'000'000.0, "feet");

    // Then: the result is finite and ratio-correct.
    REQUIRE(std::isfinite(result.convertedAmount));
    REQUIRE(result.convertedAmount == Catch::Approx(1'000'000'000'000.0 * kMeterToFeet).margin(1.0));
}

TEST_CASE("test_meter_to_yard_six_decimal_input_returns_precise_result", "[boundary][conversion]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: converting a six-decimal meter value to yard.
    const auto result = driver.convert("meter", 1.234567, "yard");

    // Then: the result remains within the required 0.000001 tolerance.
    REQUIRE(result.convertedAmount == Catch::Approx(1.234567 * kMeterToYard).margin(kTolerance));
}

TEST_CASE("test_feet_to_meter_six_decimal_input_returns_precise_result", "[boundary][conversion]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: converting a six-decimal feet value to meter.
    const auto result = driver.convert("feet", 3.280840, "meter");

    // Then: the reverse ratio remains within the required 0.000001 tolerance.
    REQUIRE(result.convertedAmount == Catch::Approx(1.0).margin(kTolerance));
}

TEST_CASE("test_input_parse_missing_colon_throws_invalid_format", "[exception][parsing]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: parsing input without the required colon.
    auto act = [&driver] { driver.convertInput("meter", "feet"); };

    // Then: the input contract fails with INVALID_FORMAT before conversion.
    requireContractError(act, "INVALID_FORMAT");
}

TEST_CASE("test_input_parse_empty_unit_throws_invalid_format", "[exception][parsing]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: parsing input with no source unit.
    auto act = [&driver] { driver.convertInput(":1", "feet"); };

    // Then: missing unit is rejected as INVALID_FORMAT.
    requireContractError(act, "INVALID_FORMAT");
}

TEST_CASE("test_input_parse_empty_amount_throws_invalid_format", "[exception][parsing]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: parsing input with no amount.
    auto act = [&driver] { driver.convertInput("meter:", "feet"); };

    // Then: missing amount is rejected as INVALID_FORMAT.
    requireContractError(act, "INVALID_FORMAT");
}

TEST_CASE("test_input_parse_alpha_amount_throws_invalid_number", "[exception][parsing]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: parsing an alphabetic amount.
    auto act = [&driver] { driver.convertInput("meter:abc", "feet"); };

    // Then: number parsing fails with INVALID_NUMBER.
    requireContractError(act, "INVALID_NUMBER");
}

TEST_CASE("test_input_parse_malformed_decimal_throws_invalid_number", "[exception][parsing]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: parsing a malformed decimal amount.
    auto act = [&driver] { driver.convertInput("meter:2.5.1", "feet"); };

    // Then: malformed decimal input fails with INVALID_NUMBER.
    requireContractError(act, "INVALID_NUMBER");
}

TEST_CASE("test_input_parse_negative_amount_throws_negative_value", "[exception][parsing]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: parsing a negative length.
    auto act = [&driver] { driver.convertInput("meter:-1", "feet"); };

    // Then: negative values fail with NEGATIVE_VALUE before conversion.
    requireContractError(act, "NEGATIVE_VALUE");
}

TEST_CASE("test_input_parse_unknown_unit_throws_unknown_unit", "[exception][parsing]") {
    // Given: ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: parsing a unit that is not registered.
    auto act = [&driver] { driver.convertInput("parsec:1", "feet"); };

    // Then: unsupported units fail with UNKNOWN_UNIT.
    requireContractError(act, "UNKNOWN_UNIT");
}

TEST_CASE("test_dynamic_registration_cubit_then_meter_to_cubit_returns_registered_ratio", "[dynamic_registration]") {
    // Given: default ratios are 1 meter = 3.28084 feet, 1 meter = 1.09361 yard, and 1 cubit = 0.4572 meter.
    auto driver = makeDriver();
    driver.registerUnit("cubit", kCubitToMeter);

    // When: converting meter to the dynamically registered cubit unit.
    const auto result = driver.convert("meter", 1.0, "cubit");

    // Then: the new unit uses its meter ratio without changing default units.
    REQUIRE(result.convertedAmount == Catch::Approx(1.0 / kCubitToMeter).margin(kTolerance));
}

TEST_CASE("test_dynamic_registration_cubit_then_cubit_to_meter_returns_registered_ratio", "[dynamic_registration]") {
    // Given: default ratios are 1 meter = 3.28084 feet, 1 meter = 1.09361 yard, and 1 cubit = 0.4572 meter.
    auto driver = makeDriver();
    driver.registerUnit("cubit", kCubitToMeter);

    // When: converting the dynamically registered cubit unit to meter.
    const auto result = driver.convert("cubit", 1.0, "meter");

    // Then: the new unit converts through its meter ratio.
    REQUIRE(result.convertedAmount == Catch::Approx(kCubitToMeter).margin(kTolerance));
}

TEST_CASE("test_dynamic_registration_fathom_then_fathom_to_feet_returns_meter_hub_ratio", "[dynamic_registration]") {
    // Given: default ratios are 1 meter = 3.28084 feet, 1 meter = 1.09361 yard, and 1 fathom = 1.8288 meter.
    auto driver = makeDriver();
    driver.registerUnit("fathom", kFathomToMeter);

    // When: converting the dynamically registered fathom unit to feet.
    const auto result = driver.convert("fathom", 1.0, "feet");

    // Then: the new unit converts through the meter hub.
    REQUIRE(result.convertedAmount == Catch::Approx(kFathomToMeter * kMeterToFeet).margin(kTolerance));
}

TEST_CASE("test_dynamic_registration_new_unit_keeps_meter_to_feet_regression", "[dynamic_registration]") {
    // Given: default ratios are 1 meter = 3.28084 feet, 1 meter = 1.09361 yard, and 1 cubit = 0.4572 meter.
    auto driver = makeDriver();
    driver.registerUnit("cubit", kCubitToMeter);

    // When: converting meter to feet after registering cubit.
    const auto result = driver.convert("meter", 1.0, "feet");

    // Then: the README meter-to-feet ratio is unchanged.
    REQUIRE(result.convertedAmount == Catch::Approx(kMeterToFeet).margin(kTolerance));
}

TEST_CASE("test_dynamic_registration_new_unit_keeps_meter_to_yard_regression", "[dynamic_registration]") {
    // Given: default ratios are 1 meter = 3.28084 feet, 1 meter = 1.09361 yard, and 1 cubit = 0.4572 meter.
    auto driver = makeDriver();
    driver.registerUnit("cubit", kCubitToMeter);

    // When: converting meter to yard after registering cubit.
    const auto result = driver.convert("meter", 1.0, "yard");

    // Then: the README meter-to-yard ratio is unchanged.
    REQUIRE(result.convertedAmount == Catch::Approx(kMeterToYard).margin(kTolerance));
}

TEST_CASE("test_dynamic_registration_duplicate_unit_throws_duplicate_unit", "[dynamic_registration]") {
    // Given: default ratios are 1 meter = 3.28084 feet, 1 meter = 1.09361 yard, and 1 cubit = 0.4572 meter.
    auto driver = makeDriver();
    driver.registerUnit("cubit", kCubitToMeter);

    // When: registering cubit again.
    auto act = [&driver] { driver.registerUnit("cubit", kCubitToMeter); };

    // Then: duplicate unit registration is rejected.
    requireContractError(act, "DUPLICATE_UNIT");
}

TEST_CASE("test_config_json_valid_units_loads_meter_feet_yard_ratios", "[configuration][json]") {
    // Given: JSON ratios define 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    const auto path = writeTempConfig(
        "unit_converter_valid_units.json",
        R"json({"units":[{"name":"meter","toMeterRatio":1.0},{"name":"feet","toMeterRatio":0.3047999902464},{"name":"yard","toMeterRatio":0.9144027578387}]})json");
    auto driver = makeDriver();

    // When: loading the JSON configuration.
    driver.loadConfig(path.string());
    const auto result = driver.convert("meter", 1.0, "feet");

    // Then: configured ratios preserve the README feet contract.
    REQUIRE(result.convertedAmount == Catch::Approx(kMeterToFeet).margin(kTolerance));
}

TEST_CASE("test_config_yaml_valid_units_loads_meter_feet_yard_ratios", "[configuration][yaml]") {
    // Given: YAML ratios define 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    const auto path = writeTempConfig(
        "unit_converter_valid_units.yaml",
        "units:\n"
        "  - name: meter\n"
        "    toMeterRatio: 1.0\n"
        "  - name: feet\n"
        "    toMeterRatio: 0.3047999902464\n"
        "  - name: yard\n"
        "    toMeterRatio: 0.9144027578387\n");
    auto driver = makeDriver();

    // When: loading the YAML configuration.
    driver.loadConfig(path.string());
    const auto result = driver.convert("meter", 1.0, "yard");

    // Then: configured ratios preserve the README yard contract.
    REQUIRE(result.convertedAmount == Catch::Approx(kMeterToYard).margin(kTolerance));
}

TEST_CASE("test_config_missing_file_throws_config_file_not_found", "[configuration][failure]") {
    // Given: default ratios are 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    const auto path = std::filesystem::temp_directory_path() / "unit_converter_missing_units.json";
    auto driver = makeDriver();

    // When: loading a configuration path that does not exist.
    auto act = [&driver, &path] { driver.loadConfig(path.string()); };

    // Then: missing configuration is reported before conversion starts.
    requireContractError(act, "CONFIG_FILE_NOT_FOUND");
}

TEST_CASE("test_config_json_malformed_file_throws_config_parse_error", "[configuration][failure]") {
    // Given: malformed JSON should not override 1 meter = 3.28084 feet or 1 meter = 1.09361 yard.
    const auto path = writeTempConfig("unit_converter_malformed_units.json", R"json({"units":[)json");
    auto driver = makeDriver();

    // When: loading malformed JSON.
    auto act = [&driver, &path] { driver.loadConfig(path.string()); };

    // Then: configuration parsing fails with CONFIG_PARSE_ERROR.
    requireContractError(act, "CONFIG_PARSE_ERROR");
}

TEST_CASE("test_config_yaml_malformed_file_throws_config_parse_error", "[configuration][failure]") {
    // Given: malformed YAML should not override 1 meter = 3.28084 feet or 1 meter = 1.09361 yard.
    const auto path = writeTempConfig("unit_converter_malformed_units.yaml", "units:\n  - name: meter\n    toMeterRatio:");
    auto driver = makeDriver();

    // When: loading malformed YAML.
    auto act = [&driver, &path] { driver.loadConfig(path.string()); };

    // Then: configuration parsing fails with CONFIG_PARSE_ERROR.
    requireContractError(act, "CONFIG_PARSE_ERROR");
}

TEST_CASE("test_config_schema_missing_unit_name_throws_config_schema_error", "[configuration][failure]") {
    // Given: schema errors must not override 1 meter = 3.28084 feet or 1 meter = 1.09361 yard.
    const auto path = writeTempConfig(
        "unit_converter_missing_name.json",
        R"json({"units":[{"toMeterRatio":1.0}]})json");
    auto driver = makeDriver();

    // When: loading a unit definition without name.
    auto act = [&driver, &path] { driver.loadConfig(path.string()); };

    // Then: schema validation fails with CONFIG_SCHEMA_ERROR.
    requireContractError(act, "CONFIG_SCHEMA_ERROR");
}

TEST_CASE("test_config_ratio_zero_throws_config_ratio_error", "[configuration][failure]") {
    // Given: invalid ratios must not override 1 meter = 3.28084 feet or 1 meter = 1.09361 yard.
    const auto path = writeTempConfig(
        "unit_converter_zero_ratio.json",
        R"json({"units":[{"name":"meter","toMeterRatio":0.0}]})json");
    auto driver = makeDriver();

    // When: loading a zero meter ratio.
    auto act = [&driver, &path] { driver.loadConfig(path.string()); };

    // Then: ratio validation fails with CONFIG_RATIO_ERROR.
    requireContractError(act, "CONFIG_RATIO_ERROR");
}

TEST_CASE("test_output_json_source_fields_preserve_original_input", "[output][json]") {
    // Given: output uses ratios 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: formatting meter input as JSON.
    const auto output = driver.format("meter:5.0", OutputFormat::Json);

    // Then: JSON preserves source amount, source unit, and format fields.
    REQUIRE(contains(output, R"("format":"json")"));
    REQUIRE(contains(output, R"("amount":5.0)"));
    REQUIRE(contains(output, R"("unit":"meter")"));
}

TEST_CASE("test_output_json_results_include_readme_conversion_ratios", "[output][json]") {
    // Given: output uses ratios 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: formatting one meter as JSON.
    const auto output = driver.format("meter:1.0", OutputFormat::Json);

    // Then: JSON includes feet and yard targets.
    REQUIRE(contains(output, R"("targetUnit":"feet")"));
    REQUIRE(contains(output, R"("targetUnit":"yard")"));
}

TEST_CASE("test_output_csv_header_returns_documented_column_order", "[output][csv]") {
    // Given: output uses ratios 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: formatting meter input as CSV.
    const auto output = driver.format("meter:5.0", OutputFormat::Csv);

    // Then: CSV starts with the documented header order.
    REQUIRE(contains(output, "source_amount,source_unit,target_unit,converted_amount"));
}

TEST_CASE("test_output_csv_rows_preserve_original_input", "[output][csv]") {
    // Given: output uses ratios 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: formatting meter input as CSV.
    const auto output = driver.format("meter:5.0", OutputFormat::Csv);

    // Then: CSV rows preserve the original amount and unit.
    REQUIRE(contains(output, "5.0,meter"));
}

TEST_CASE("test_output_table_header_returns_documented_columns", "[output][table]") {
    // Given: output uses ratios 1 meter = 3.28084 feet and 1 meter = 1.09361 yard.
    auto driver = makeDriver();

    // When: formatting meter input as a table.
    const auto output = driver.format("meter:5.0", OutputFormat::Table);

    // Then: table output exposes the documented columns.
    REQUIRE(contains(output, "Source Amount"));
    REQUIRE(contains(output, "Source Unit"));
    REQUIRE(contains(output, "Target Unit"));
    REQUIRE(contains(output, "Converted Amount"));
}

