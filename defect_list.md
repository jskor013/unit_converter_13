# Defect List

검증 명령:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

현재 결과: Catch2 계약 테스트 36개 실패, 공통 실제값은 `PUBLIC_API_MISSING` 예외입니다.

| ID | Severity | 변환 타입 | 재현 절차 | 기대값 | 실제값 | 근본 원인 | 수정 요약 |
|---|---|---|---|---|---|---|---|
| DEF-001 | Critical | meter->feet | `test_meter_to_feet_one_meter_returns_readme_ratio`, `test_meter_to_feet_decimal_amount_returns_scaled_ratio` 실행 | `1 meter = 3.28084 feet`, `2.5 meter = 8.202100 feet` | `PUBLIC_API_MISSING` 예외 | 변환 로직이 `main()` 콘솔 흐름 안에만 있고 테스트 가능한 `convert()` 공개 계약이 없다. | 변환 유스케이스/엔티티 API를 분리하고 `meter` 기준 비율 `3.28084`를 등록해 `convert("meter", value, "feet")`를 제공한다. |
| DEF-002 | Critical | meter->yard | `test_meter_to_yard_one_meter_returns_readme_ratio`, `test_meter_to_yard_six_decimal_input_returns_precise_result` 실행 | `1 meter = 1.09361 yard`, 6자리 소수 입력은 `0.000001` 허용 오차 내 변환 | `PUBLIC_API_MISSING` 예외 | `yard` 변환을 검증할 재사용 가능한 공개 변환 계약이 없다. | `convert("meter", value, "yard")`를 제공하고 `1 meter = 1.09361 yard` 비율을 단위 레지스트리에 고정한다. |
| DEF-003 | Critical | feet->meter / feet<->yard | `test_feet_to_meter_one_foot_returns_reverse_ratio`, `test_feet_to_yard_hub_conversion_returns_meter_based_ratio`, `test_yard_to_feet_hub_conversion_returns_meter_based_ratio` 실행 | `1 feet = 1 / 3.28084 meter`, feet/yard는 meter 허브 경유 변환 | `PUBLIC_API_MISSING` 예외 | 역변환과 단위 간 허브 변환을 호출할 도메인 API가 없다. | 모든 단위를 meter 기준 비율로 정규화한 뒤 대상 단위 비율로 나누는 공통 변환 규칙을 구현한다. |
| DEF-004 | High | boundary value conversion | `test_meter_to_feet_zero_value_returns_zero`, `test_meter_to_feet_large_value_returns_finite_result`, 6자리 소수 정밀도 테스트 실행 | `0`은 `0`, 매우 큰 수는 finite, 6자리 소수는 `0.000001` 허용 오차 내 결과 | `PUBLIC_API_MISSING` 예외 | 경계값 정책과 정밀도 보장을 검증할 변환 계약이 없다. | 음수가 아닌 `double` 값에 대해 overflow/finite 확인과 명시 허용 오차 기반 변환 결과를 반환한다. |
| DEF-005 | High | input parsing / exception | `test_input_parse_missing_colon_throws_invalid_format`, `test_input_parse_alpha_amount_throws_invalid_number`, `test_input_parse_negative_amount_throws_negative_value`, `test_input_parse_unknown_unit_throws_unknown_unit` 실행 | `INVALID_FORMAT`, `INVALID_NUMBER`, `NEGATIVE_VALUE`, `UNKNOWN_UNIT` | `PUBLIC_API_MISSING` 예외 | 입력 파싱과 오류 코드 매핑이 `main()`의 출력/종료 코드에 묶여 있고 `convertInput()` 계약이 없다. | Boundary 파서를 분리하고 실패 유형별 안정 오류 코드를 반환 또는 throw하도록 구현한다. |
| DEF-006 | High | dynamic registration | `test_dynamic_registration_cubit_then_meter_to_cubit_returns_registered_ratio`, `test_dynamic_registration_duplicate_unit_throws_duplicate_unit` 실행 | `registerUnit("cubit", 0.4572)` 후 변환 가능, 중복 등록은 `DUPLICATE_UNIT` | `PUBLIC_API_MISSING` 예외 | 단위 등록 책임이 없고 `meter`, `feet`, `yard`가 if-else 체인에 고정되어 있다. | `UnitRegistry`를 도입하고 양수 meter 비율 신규 단위 등록 및 중복 검증을 제공한다. |
| DEF-007 | High | JSON/YAML config load | `test_config_json_valid_units_loads_meter_feet_yard_ratios`, `test_config_yaml_valid_units_loads_meter_feet_yard_ratios`, 설정 실패 테스트 실행 | JSON/YAML 정상 로드, 실패 시 `CONFIG_FILE_NOT_FOUND`, `CONFIG_PARSE_ERROR`, `CONFIG_SCHEMA_ERROR`, `CONFIG_RATIO_ERROR` | `PUBLIC_API_MISSING` 예외 | 설정 파일을 읽는 data adapter와 `loadConfig()` 공개 계약이 없다. | JSON/YAML 로더를 Data 계층에 추가하고 스키마/비율 검증 후 단위 레지스트리에 반영한다. |
| DEF-008 | Medium | output formatting | `test_output_json_source_fields_preserve_original_input`, `test_output_csv_header_returns_documented_column_order`, `test_output_table_header_returns_documented_columns` 실행 | JSON/CSV/Table 출력이 원 입력과 변환 결과 필드를 문서 계약대로 포함 | `PUBLIC_API_MISSING` 예외 | 출력 포맷팅이 콘솔 출력에 고정되어 있고 JSON/CSV/Table formatter 계약이 없다. | Boundary formatter를 분리해 `format(input, OutputFormat)`에서 JSON/CSV/Table 문자열을 반환한다. |
