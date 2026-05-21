# Test Plan: Meter to Feet Conversion

## 1. Purpose

This test plan defines the Catch2-based test scope for the selected sample feature: `meter -> feet` conversion.

The target business rule is:

```text
1 meter = 3.28084 feet
```

Primary sample:

```text
Input:  meter:2.5
Output: 2.5 meter = 8.202100 feet
```

## 2. Test Target

| Item | Description |
|---|---|
| Feature | `meter -> feet` conversion |
| Input contract | `<unit>:<amount>` |
| Source unit | `meter` |
| Target unit | `feet` |
| Conversion rule | `amount * 3.28084` |
| Floating-point tolerance | `0.000001` |
| Framework | Catch2 |
| Language / Build | C++17, CMake |

## 3. Catch2 Unit Test Scope and Priority

### P0: Core Domain Conversion

P0 tests protect the core business rule and must be executed on every change.

| Test Case | Input | Expected Result | Priority |
|---|---|---|---|
| Convert positive meter value to feet | `meter:2.5` | `8.202100 feet` within tolerance | P0 |
| Convert one meter to feet | `meter:1` | `3.280840 feet` within tolerance | P0 |
| Preserve source amount and unit in output | `meter:2.5` | output contains `2.5 meter` | P0 |

### P1: Boundary and Validation

P1 tests cover boundary values and input validation contracts.

| Test Case | Input | Expected Result | Priority |
|---|---|---|---|
| Zero value conversion | `meter:0` | `0.000000 feet` | P1 |
| Very large value conversion | `meter:1000000000000` | no overflow, finite converted value | P1 |
| Negative value rejection | `meter:-1` | `NEGATIVE_VALUE` | P1 |
| Non-numeric amount rejection | `meter:abc` | `INVALID_NUMBER` | P1 |
| Missing delimiter rejection | `meter` | `INVALID_FORMAT` | P1 |
| Unknown unit rejection | `parsec:1.0` | `UNKNOWN_UNIT` | P1 |

### P2: Formatting and Regression

P2 tests reduce regression risk around output representation.

| Test Case | Input | Expected Result | Priority |
|---|---|---|---|
| Console output format | `meter:2.5` | `<source_amount> <source_unit> = <converted_amount> <target_unit>` | P2 |
| Fixed precision compatibility | `meter:2.5` | converted feet value can be compared to `8.202100` | P2 |
| Re-run after adding new units | `meter:2.5` | existing meter to feet result remains unchanged | P2 |

## 4. Boundary Value Cases

| Boundary | Input | Expected Behavior | Layer |
|---|---|---|---|
| `value = 0` | `meter:0` | Accepted. Converted value is `0 feet`. | Domain / Boundary |
| Very large value | `meter:1000000000000` | Accepted only if parsed as a valid finite number and conversion result remains finite. | Domain |
| `value < 0` | `meter:-1` | Rejected before conversion with `NEGATIVE_VALUE`. | Boundary |
| Decimal parsing failure | `meter:abc` | Rejected with `INVALID_NUMBER`. | Boundary |
| Missing `:` delimiter | `meter` | Rejected with `INVALID_FORMAT`. | Boundary |
| Unknown unit | `parsec:1.0` | Rejected with `UNKNOWN_UNIT`. | Boundary / Control |

## 5. Exception and Special Cases

| Case | Example | Expected Policy |
|---|---|---|
| Empty input | `` | Reject as `INVALID_FORMAT`. |
| Empty unit | `:2.5` | Reject as `INVALID_FORMAT`. |
| Empty amount | `meter:` | Reject as `INVALID_FORMAT` or amount-specific validation error according to the boundary contract. |
| Multiple delimiters | `meter:2.5:extra` | Reject as `INVALID_FORMAT`. |
| Malformed decimal | `meter:2.5.1` | Reject as `INVALID_NUMBER`. |
| Whitespace around input | ` meter:2.5 ` | Define whether trimming is supported; if not supported, reject consistently. |
| Case mismatch | `Meter:2.5` | Treat as unknown unit unless case-insensitive units are explicitly supported. |
| Non-finite numeric value | `meter:inf`, `meter:nan` | Reject unless non-finite values are explicitly allowed. |

## 6. Coverage Goals

| Area | Goal | Notes |
|---|---|---|
| Domain | 95%+ line coverage | Conversion rule, unit ratio lookup, meter hub calculation, and floating-point comparison paths must be covered. |
| Boundary | 85%+ line coverage | Parsing, format validation, numeric validation, negative value rejection, and unknown unit handling must be covered. |
| Branch coverage | 85%+ recommended | Validation success and failure branches should both be exercised. |

## 7. gcov / lcov Measurement Strategy

### 7.1 CMake Debug Coverage Build

Use a dedicated coverage build directory so normal builds are not affected.

```bash
cmake -S . -B build-coverage -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage -O0 -g"
cmake --build build-coverage
ctest --test-dir build-coverage --output-on-failure
```

### 7.2 gcov File-Level Check

Run `gcov` against the generated object files for `UnitConverter.cpp`.

```bash
gcov UnitConverter.cpp
```

Review the generated `UnitConverter.cpp.gcov` file and confirm:

- Core conversion lines for `meter -> feet` are executed.
- Validation branches for invalid input are executed.
- Error paths do not execute conversion logic after validation failure.

### 7.3 lcov HTML Report

Collect and render coverage into an HTML report.

```bash
lcov --capture --directory build-coverage --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.filtered.info
genhtml coverage.filtered.info --output-directory coverage-report
```

Acceptance criteria:

- Domain coverage is at least `95%`.
- Boundary coverage is at least `85%`.
- `meter -> feet` conversion and listed boundary cases are visible in the report.

## 8. Exit Criteria

The test plan is considered satisfied when:

- The happy path `meter:2.5 -> 8.202100 feet` passes within `0.000001` tolerance.
- All listed boundary cases have Catch2 coverage.
- Invalid inputs return the expected error code before conversion.
- Coverage results meet Domain `95%+` and Boundary `85%+` goals.
- `gcov UnitConverter.cpp` and the lcov HTML report are generated successfully.
