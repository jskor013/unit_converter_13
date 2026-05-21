#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "../Converter.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

constexpr double kCubitRatioToMeter = 0.4572;
constexpr double kMeterToFeet = 3.28084;

const ConversionResult& requireUnitResult(const std::vector<ConversionResult>& results, const std::string& unit) {
    const auto result = std::find_if(results.begin(), results.end(), [&unit](const ConversionResult& candidate) {
        return candidate.unit == unit;
    });

    REQUIRE(result != results.end());
    return *result;
}

} // namespace

TEST_CASE("감쇠 단위 변환", "[bonus]") {
    using Catch::Approx;

    SECTION("BT-01 registerUnit 후 cubit to meter 변환") {
        registerUnit("cubit", kCubitRatioToMeter);

        REQUIRE(convert("cubit", 1.0, "meter") == Approx(0.4572).epsilon(1e-5));
    }

    SECTION("BT-02 registerUnit 후 meter to cubit 역변환") {
        registerUnit("cubit", kCubitRatioToMeter);

        REQUIRE(convert("meter", 1.0, "cubit") == Approx(1.0 / kCubitRatioToMeter).epsilon(1e-5));
    }

    SECTION("BT-03 registerUnit 후 cubit to feet 교차 변환") {
        registerUnit("cubit", kCubitRatioToMeter);

        REQUIRE(convert("cubit", 1.0, "feet") == Approx(kCubitRatioToMeter * kMeterToFeet).epsilon(1e-5));
    }

    SECTION("BT-04 음수 비율 등록 시 invalid_argument 발생") {
        REQUIRE_THROWS_AS(registerUnit("negative-cubit", -0.4572), std::invalid_argument);
    }

    SECTION("BT-05 registerUnit 후 convertAll cubit 값은 전 단위를 반환") {
        registerUnit("cubit", kCubitRatioToMeter);

        const auto results = convertAll("cubit", 1.0);

        REQUIRE(results.size() == 4);
        REQUIRE(requireUnitResult(results, "meter").value == Approx(0.4572).epsilon(1e-5));
        REQUIRE(requireUnitResult(results, "feet").value == Approx(kCubitRatioToMeter * kMeterToFeet).epsilon(1e-5));
        REQUIRE(requireUnitResult(results, "yard").value == Approx(0.5).epsilon(1e-5));
        REQUIRE(requireUnitResult(results, "cubit").value == Approx(1.0).epsilon(1e-5));
    }

    SECTION("BT-06 기존 meter to feet 결과 불변") {
        registerUnit("cubit", kCubitRatioToMeter);

        REQUIRE(convert("meter", 1.0, "feet") == Approx(kMeterToFeet).epsilon(1e-5));
    }
}
