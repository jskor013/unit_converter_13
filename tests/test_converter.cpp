#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <string>

double convert(const std::string& fromUnit, double value, const std::string& toUnit);
void registerUnit(const std::string& name, double ratioToMeter);

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
