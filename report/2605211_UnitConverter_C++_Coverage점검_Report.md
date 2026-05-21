# UnitConverter C++ Coverage 점검 Report

## 1. 작업 개요

| 항목 | 내용 |
|---|---|
| 프로젝트 | `E:\dev\unit_converter_13` |
| 브랜치 | `REFACTOR` |
| 날짜 | 2026-05-21 |
| 작업자 | Cursor AI Agent (GPT-5.5) |
| 단계 | Coverage 점검 및 리팩토링 회고 |

이번 작업은 UnitConverter C++ 프로젝트의 RED-GREEN-REFACTOR 흐름 이후 현재 테스트 커버리지를 점검하고, Domain/Boundary 레이어별 미달 원인과 후속 테스트 보강 항목을 정리하는 것이다. 구현 코드는 추가하지 않았고, 커버리지 측정을 위해 별도 `build-coverage` 디렉터리에서 coverage 빌드와 테스트를 실행했다.

## 2. 완료된 To-Do 항목 요약 (Phase 6 To-Do 기준)

| 구분 | 완료 항목 | 상태 |
|---|---|---|
| Domain / Logic | `meter -> feet`, `meter -> yard`, `feet -> meter`, `yard -> meter` 변환 테스트 및 GREEN 구현 | 완료 |
| Domain / Logic | `yard -> feet` meter hub 변환 보완 | 완료 |
| Domain / Logic | `convertAll("meter", 1.0)` 기본 단위 전체 반환 | 완료 |
| Domain / Logic | `registerUnit("cubit", 0.4572)` 후 변환 가능 | 완료 |
| Domain / Logic | 유효 JSON 설정 로드 후 `fathom` 비율 적용 | 완료 |
| Boundary / UI | 정상 입력 `meter:2.5` 변환 포맷 반환 | 완료 |
| Boundary / UI | 콜론 없는 입력, 음수 입력, 없는 단위, malformed decimal 예외 처리 | 완료 |
| Boundary / UI | 원 입력 값과 단위 보존 포맷 검증 | 완료 |
| Refactor | Boundary 파싱/검증/포맷과 Domain 변환 계산 분리 | 완료 |
| Refactor | `converter_core` 라이브러리 타깃 도입 | 완료 |
| Coverage | gcov/lcov 기반 파일별 커버리지 점검 | 완료 |

## 3. RED 단계 결과

RED 단계에서는 `tests/unit_converter_contract_tests.cpp`에 Catch2 스켈레톤 테스트를 작성했다. 각 테스트 본문은 RED 원칙에 맞춰 `FAIL("RED");` 한 줄만 포함한다.

작성된 RED 스켈레톤 테스트:

| Track | 테스트 |
|---|---|
| Domain | `convert_meter_to_feet_returns_correct_ratio` |
| Domain | `convert_meter_to_yard_returns_correct_ratio` |
| Domain | `convert_feet_to_meter_returns_correct_ratio` |
| Domain | `convert_yard_to_meter_returns_correct_ratio` |
| Domain | `convert_all_returns_all_registered_units` |
| Domain | `register_unit_allows_new_unit_conversion` |
| Config | `load_config_json_applies_unit_ratios` |
| Config | `load_config_yaml_applies_unit_ratios` |
| Config | `load_config_invalid_path_keeps_default_units` |
| Boundary | `parse_input_meter_value_returns_formatted_conversion` |
| Boundary | `parse_input_without_colon_throws_invalid_argument` |
| Boundary | `parse_input_negative_value_throws_invalid_argument` |
| Boundary | `parse_input_unknown_unit_throws_invalid_argument` |
| Boundary | `format_output_preserves_original_value_and_unit` |
| Boundary / JSON | `format_json_returns_documented_schema` |

실패 확인 여부:

- `unit_converter_contract_tests`는 스켈레톤 RED 테스트로 유지했다.
- 이후 GREEN/REFACTOR/coverage 검증에서는 사용자 지시에 따라 `ctest -E unit_converter_contract_tests`로 제외했다.

## 4. GREEN 단계 결과

GREEN 단계에서는 `tests/test_converter.cpp`를 중심으로 focused 테스트를 작성하고, 최소 구현으로 `test_converter` 통과를 확인했다.

통과한 주요 테스트:

| Track | 통과 테스트 |
|---|---|
| Domain | `TC-B-01 convert meter to feet returns correct ratio` |
| Domain | `TC-B-02 convert meter to yard returns correct ratio` |
| Domain | `TC-B-03 convert feet to meter returns correct ratio` |
| Domain | `TC-B-04 convert yard to meter returns correct ratio` |
| Domain | `TC-B-04 convertAll meter returns all default unit conversions` |
| Domain | `TC-B-05 register cubit enables conversion to meter` |
| Domain | `TC-B-06 loadConfig valid path applies unit ratio` |
| Domain | `TC-B-08 convert yard to feet uses meter hub ratio` |
| Boundary | `TC-A-01 convertInput normal meter input returns formatted conversion` |
| Boundary | `TC-A-02 convertInput without colon throws invalid_argument` |
| Boundary | `TC-A-03 convertInput negative value throws invalid_argument` |
| Boundary | `TC-A-04 convertInput unknown unit throws invalid_argument` |
| Boundary | `TC-A-05 convertInput malformed decimal throws invalid_argument` |
| Boundary | `TC-A-06 convertInput preserves original feet value and unit` |

관련 커밋 메시지:

- `refactor(converter): split boundary and meter hub logic`
- `docs(refactor): record dual track backup result`

## 5. Refactoring 결과

| 선택 항목 | 변경 내용 | 변경 파일 | 회귀 테스트 |
|---|---|---|---|
| Boundary 분리 | `convertInput()`의 파싱, 검증, 포맷 생성을 Boundary Track으로 분리 | `ConverterBoundary.cpp` | 통과 |
| Domain 분리 | `ConversionRule` 기반 meter hub 변환 공식으로 if-else 변환 분기 제거 | `Converter.cpp` | 통과 |
| 공개 API 정리 | `ConversionResult`, `convert`, `convertAll`, `registerUnit`, `loadConfig`, `convertInput` 선언을 헤더로 추출 | `Converter.h`, `tests/test_converter.cpp` | 통과 |
| 내부 API 정리 | 단위 존재 여부 조회용 내부 헤더 추가 | `ConverterInternal.h` | 통과 |
| 앱 재사용 | 콘솔 앱이 자체 변환 비율/분기 대신 `convertAll()` 재사용 | `UnitConverter.cpp` | 통과 |
| 빌드 구조 | `converter_core` 라이브러리 타깃으로 앱과 테스트가 같은 변환 코어 링크 | `CMakeLists.txt` | 통과 |

회귀 테스트 결과:

- `cmake --build build`: 통과
- `ctest --test-dir build -E unit_converter_contract_tests --output-on-failure`: 통과
- `meter:2.5` 콘솔 스모크: 통과

## 6. 커버리지 현황

커버리지 실행:

```text
cmake -S . -B build-coverage -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage -O0 -g" -DCMAKE_EXE_LINKER_FLAGS="--coverage"
cmake --build build-coverage
ctest --test-dir build-coverage -E unit_converter_contract_tests --output-on-failure
lcov --capture --directory build-coverage --output-file build-coverage\coverage.info
lcov --extract build-coverage\coverage.info "*UnitConverter.cpp" "*Converter.cpp" "*ConverterBoundary.cpp" "*tests/test_converter.cpp" --output-file build-coverage\project.info
lcov --list build-coverage\project.info
gcov -b -c build-coverage\CMakeFiles\UnitConverter.dir\UnitConverter.cpp.gcno
gcov -b -c build-coverage\CMakeFiles\converter_core.dir\Converter.cpp.gcno
gcov -b -c build-coverage\CMakeFiles\converter_core.dir\ConverterBoundary.cpp.gcno
```

파일별 라인 커버리지:

| 파일 / 레이어 | Stmts | Miss | Cover | 판정 |
|---|---:|---:|---:|---|
| `Converter.cpp` / Domain Logic | 39 | 1 | 97.4% | Domain 95% 이상 충족 |
| `ConverterBoundary.cpp` / Boundary | 24 | 0 | 100.0% | Boundary 85% 이상 충족 |
| `UnitConverter.cpp` / Console Boundary | 24 | 8 | 66.7% | 앱 실패 경로 미달 |
| `tests/test_converter.cpp` / Test | 56 | 0 | 100.0% | 참고 지표 |
| 전체 대상 | 143 | 9 | 93.7% | 전체 95%에는 미달 |

커버되지 않은 주요 invariant 분기:

| 파일 | 미커버 분기 | 영향 | 추가 테스트 제안 |
|---|---|---|---|
| `Converter.cpp` | `convert()`의 최종 fallback `return 0.0` | 없는 변환 조합 방어 경로 직접 검증 없음 | `convert("parsec", 1.0, "feet") == 0.0`, `convert("meter", 1.0, "parsec") == 0.0` |
| `UnitConverter.cpp` | 콜론 없는 입력 | 콘솔 invalid format 경로 미커버 | stdin `2.5` 실행 시 exit code 1 및 `Invalid format` 확인 |
| `UnitConverter.cpp` | 숫자 파싱 실패 | 콘솔 invalid number 경로 미커버 | stdin `meter:abc` 실행 시 exit code 1 및 `Invalid number` 확인 |
| `UnitConverter.cpp` | 없는 단위 처리 | 콘솔 unknown unit 경로 미커버 | stdin `parsec:1.0` 실행 시 exit code 1 및 `Unknown unit` 확인 |
| `UnitConverter.cpp` | 음수 입력 정책 | 앱 레벨 음수 거부 정책이 현재 `main()`에 없음 | 콘솔 앱도 `convertInput()` 경로를 재사용하도록 리팩터링하거나, 정책을 문서화 후 계약 테스트 추가 |

## 7. 미완료 항목 및 다음 단계 제안

| 항목 | 상태 | 제안 |
|---|---|---|
| `unit_converter_contract_tests` | 스켈레톤 RED 유지 | 실제 공개 API 기반 계약 테스트로 전환할지 결정 |
| 콘솔 앱 커버리지 | 66.7% | stdin 기반 앱 계약 테스트 추가 |
| 콘솔 앱 음수 정책 | 불명확 | `UnitConverter.cpp`가 `convertInput()` Boundary 정책을 재사용하도록 후속 리팩터링 검토 |
| 전체 커버리지 | 93.7% | 앱 실패 경로 테스트 추가 후 95% 재측정 |
| JSON/YAML 계약 | 일부 스켈레톤만 존재 | 기능 범위 승인 후 RED 테스트를 실제 구현 대상으로 전환 |
| build 산출물 | `build/`, `build-coverage/`, `*.gcov` 생성 | 커밋 제외 유지 또는 필요 시 정리 |

## 8. 발견된 이슈 및 해결 방법

| 이슈 | 원인 | 해결 / 대응 |
|---|---|---|
| 기존 `gcov UnitConverter.cpp` 실패 | 기본 `build`가 coverage 플래그 없이 생성되어 `.gcno/.gcda` 없음 | 별도 `build-coverage` 디렉터리를 coverage 플래그로 생성 |
| `lcov`가 Catch2 내부 파일 경고 출력 | FetchContent로 받은 Catch2 경로와 lcov 소스 경로 해석 불일치 | 프로젝트 파일만 `lcov --extract`로 필터링 |
| 전체 CTest 실패 가능성 | `unit_converter_contract_tests`가 의도적 RED 스켈레톤 | 사용자 지시대로 `-E unit_converter_contract_tests` 사용 |
| IDE clang 진단 | Catch2 include 경로를 IDE clangd가 찾지 못함 | CMake 빌드/CTest 기준으로는 정상. 필요 시 compile commands 설정 점검 |
| 콘솔 앱 coverage 낮음 | 정상 경로 스모크만 실행됨 | 실패 경로 stdin 테스트 추가 필요 |

## 9. 생성형 AI 활용 회고

### 도움이 된 순간

- 반복적인 TDD 절차에서 RED/GREEN/REFACTOR 범위를 분리하고, 스켈레톤 RED 테스트를 제외한 검증 명령을 일관되게 유지하는 데 도움이 되었다.
- `Converter.cpp`의 if-else 변환 분기를 meter hub 공식으로 정리하고, Boundary/Domain 파일 분리를 작은 단계로 나누는 데 효과적이었다.
- 커버리지 수집 실패 원인을 빠르게 분리해 coverage 전용 빌드 디렉터리를 구성할 수 있었다.

### 한계

- IDE linter와 CMake 빌드의 include 경로 인식 차이처럼 도구별 진단 차이는 사람이 최종 판단해야 했다.
- `unit_converter_contract_tests`가 스켈레톤 RED로 남아 있어 전체 `ctest`를 GREEN 기준으로 사용할 수 없었다.
- 콘솔 앱의 음수 입력 정책처럼 구현과 문서 계약이 미묘하게 어긋날 수 있는 지점은 명시적 요구사항 확인이 필요하다.

### TC 작성 팁

- Domain 테스트는 `convert(from, value, to)`와 `convertAll()`처럼 순수 계산 API를 직접 검증한다.
- Boundary 테스트는 입력 문자열, 예외 타입, 원 입력 보존 출력 포맷을 한 케이스에 하나씩 고정한다.
- 콘솔 앱 테스트는 stdin/stdout/stderr와 exit code를 함께 확인해야 한다.
- RED 스켈레톤은 학습용 산출물로 유용하지만, coverage와 회귀 보호 기준에서는 실제 assertion 기반 테스트로 전환해야 한다.

## 부록: 현재 권장 추가 테스트

| 우선순위 | 테스트 제안 | 목적 |
|---|---|---|
| 높음 | 콘솔 `2.5` 입력 invalid format 테스트 | `UnitConverter.cpp` line 18-19 커버 |
| 높음 | 콘솔 `meter:abc` 입력 invalid number 테스트 | `UnitConverter.cpp` line 27-29 커버 |
| 높음 | 콘솔 `parsec:1.0` 입력 unknown unit 테스트 | `UnitConverter.cpp` line 32-34 커버 |
| 중간 | `convert("parsec", 1.0, "feet") == 0.0` | Domain fallback line 83 커버 |
| 중간 | 콘솔 `meter:-1.0` 정책 테스트 | 앱 Boundary와 `convertInput()` 정책 일치 여부 확인 |
