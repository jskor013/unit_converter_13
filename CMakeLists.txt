cmake_minimum_required(VERSION 3.16)

project(UnitConverter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(converter_core
    Converter.cpp
    ConverterBoundary.cpp
    src/DecayingUnit.cpp
)

add_executable(UnitConverter UnitConverter.cpp)
target_link_libraries(UnitConverter PRIVATE converter_core)

include(CTest)

if(BUILD_TESTING)
    find_package(Catch2 3 QUIET)

    if(NOT Catch2_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.8.1
        )
        FetchContent_MakeAvailable(Catch2)
    endif()

    add_executable(unit_converter_contract_tests
        tests/unit_converter_contract_tests.cpp
    )
    target_link_libraries(unit_converter_contract_tests PRIVATE Catch2::Catch2WithMain)

    add_executable(test_converter
        tests/test_converter.cpp
    )
    target_link_libraries(test_converter PRIVATE converter_core Catch2::Catch2WithMain)

    add_executable(test_decaying_unit
        tests/test_decaying_unit.cpp
    )
    target_link_libraries(test_decaying_unit PRIVATE converter_core Catch2::Catch2WithMain)

    add_test(NAME unit_converter_contract_tests COMMAND unit_converter_contract_tests)
    add_test(NAME test_converter COMMAND test_converter)
    add_test(NAME test_decaying_unit COMMAND test_decaying_unit)
endif()
