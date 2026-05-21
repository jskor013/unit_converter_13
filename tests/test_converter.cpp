#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <string>

double convert(const std::string& fromUnit, double value, const std::string& toUnit);

TEST_CASE("TC-B-01 convert meter to feet returns correct ratio", "[domain]") {
    using Catch::Approx;

    REQUIRE(convert("meter", 2.5, "feet") == Approx(8.20210).epsilon(1e-5));
}
