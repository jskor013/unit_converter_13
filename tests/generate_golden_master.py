from __future__ import annotations

import difflib
import subprocess
import sys
from decimal import Decimal, ROUND_HALF_UP
from pathlib import Path


SCENARIOS = ("meter:2.5", "feet:1.0", "yard:1.0", "meter:0.0")
TARGET_UNITS = ("meter", "feet", "yard")
RATIOS_FROM_METER = {
    "meter": Decimal("1"),
    "feet": Decimal("3.28084"),
    "yard": Decimal("1.09361"),
}
SIX_DECIMALS = Decimal("0.000001")


def project_root() -> Path:
    return Path(__file__).resolve().parents[1]


def unit_converter_executable(root: Path) -> Path:
    exe_name = "UnitConverter.exe" if sys.platform.startswith("win") else "UnitConverter"
    return root / "build" / exe_name


def to_meter(unit: str, value: Decimal) -> Decimal:
    if unit == "meter":
        return value
    return value / RATIOS_FROM_METER[unit]


def from_meter(unit: str, value: Decimal) -> Decimal:
    if unit == "meter":
        return value
    return value * RATIOS_FROM_METER[unit]


def format_decimal(value: Decimal) -> str:
    return str(value.quantize(SIX_DECIMALS, rounding=ROUND_HALF_UP))


def render_section(scenario: str) -> str:
    source_unit, source_value_text = scenario.split(":", 1)
    source_value = Decimal(source_value_text)
    meter_value = to_meter(source_unit, source_value)

    lines = [f"[{scenario}]"]
    for target_unit in TARGET_UNITS:
        if target_unit == source_unit:
            continue
        converted = from_meter(target_unit, meter_value)
        lines.append(f"{source_value_text} {source_unit} = {format_decimal(converted)} {target_unit}")
    lines.append("---")
    return "\n".join(lines)


def capture_stdout(executable: Path, scenario: str, output_dir: Path) -> None:
    scenario_name = scenario.replace(":", "_").replace(".", "_")
    input_path = output_dir / f"{scenario_name}_input.txt"
    raw_output_path = output_dir / f"{scenario_name}_actual_raw.txt"

    input_path.write_text(f"{scenario}\n", encoding="utf-8")
    with input_path.open("r", encoding="utf-8") as stdin, raw_output_path.open("w", encoding="utf-8") as stdout:
        subprocess.run([str(executable)], stdin=stdin, stdout=stdout, check=True)


def render_actual(executable: Path, output_dir: Path) -> str:
    output_dir.mkdir(parents=True, exist_ok=True)
    sections = []
    for scenario in SCENARIOS:
        capture_stdout(executable, scenario, output_dir)
        sections.append(render_section(scenario))
    return "\n".join(sections) + "\n"


def main() -> int:
    root = project_root()
    executable = unit_converter_executable(root)
    expected_path = root / "tests" / "golden_master_expected.txt"
    output_dir = root / "build" / "golden_master"
    actual_path = output_dir / "actual.txt"

    if not executable.exists():
        print(f"UnitConverter executable not found: {executable}", file=sys.stderr)
        print("Run: cmake -S . -B build && cmake --build build", file=sys.stderr)
        return 2

    actual = render_actual(executable, output_dir)
    actual_path.write_text(actual, encoding="utf-8")

    if not expected_path.exists():
        expected_path.write_text(actual, encoding="utf-8")
        print(f"Created golden master: {expected_path}")
        print("Run: git add tests/golden_master_expected.txt")
        return 0

    expected = expected_path.read_text(encoding="utf-8")
    if actual == expected:
        print("Golden master approved.")
        return 0

    diff = difflib.unified_diff(
        expected.splitlines(keepends=True),
        actual.splitlines(keepends=True),
        fromfile=str(expected_path),
        tofile=str(actual_path),
    )
    print("Golden master mismatch:", file=sys.stderr)
    print("".join(diff), file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
