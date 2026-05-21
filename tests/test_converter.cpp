#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <string>

double convert(const std::string& fromUnit, double value, const std::string& toUnit);
void registerUnit(const std::string& name, double ratioToMeter);
void loadConfig(const std::string& path);
std::string convertInput(const std::string& input, const std::string& toUnit);

TEST_CASE("TC-B-01 convert meter to feet returns correct ratio", "[domain]") {
    using Catch::Approx;

    REQUIRE(convert("meter", 2.5, "feet") == Approx(8.20210).epsilon(1e-5));
}

TEST_CASE("TC-B-02 convert meter to yard returns correct ratio", "[domain]") {
    using Catch::Approx;

    REQUIRE(convert("meter", 2.5, "yard") == Approx(2.734025).epsilon(1e-5));
}

TEST_CASE("TC-B-03 convert feet to meter returns correct ratio", "[domain]") {
    using Catch::Approx;

    REQUIRE(convert("feet", 3.28084, "meter") == Approx(1.0).epsilon(1e-5));
}

TEST_CASE("TC-B-04 convert yard to meter returns correct ratio", "[domain]") {
    using Catch::Approx;

    REQUIRE(convert("yard", 1.09361, "meter") == Approx(1.0).epsilon(1e-5));
}

TEST_CASE("TC-B-05 register cubit enables conversion to meter", "[domain]") {
    using Catch::Approx;

    registerUnit("cubit", 0.4572);

    REQUIRE(convert("cubit", 1.0, "meter") == Approx(0.4572).epsilon(1e-5));
}

TEST_CASE("TC-B-06 loadConfig valid path applies unit ratio", "[domain]") {
    using Catch::Approx;

    const auto path = std::filesystem::temp_directory_path() / "unit_converter_tc_b_06.json";
    std::ofstream file(path);
    REQUIRE(file.is_open());
    file << R"json({"units":[{"name":"fathom","ratio_to_meter":1.8288}]})json";
    file.close();

    loadConfig(path.string());

    REQUIRE(convert("fathom", 1.0, "meter") == Approx(1.8288).epsilon(1e-5));
}

TEST_CASE("TC-A-01 convertInput normal meter input returns formatted conversion", "[boundary]") {
    REQUIRE(convertInput("meter:2.5", "feet") == "2.5 meter = 8.202100 feet");
}

TEST_CASE("TC-A-02 convertInput without colon throws invalid_argument", "[boundary]") {
    REQUIRE_THROWS_AS(convertInput("2.5", "feet"), std::invalid_argument);
}

TEST_CASE("TC-A-03 convertInput negative value throws invalid_argument", "[boundary]") {
    REQUIRE_THROWS_AS(convertInput("meter:-1.0", "feet"), std::invalid_argument);
}
