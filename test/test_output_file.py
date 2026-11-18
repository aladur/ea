import sys
import tempfile
from pathlib import Path
from helper_functions import Run, GetExamplesPath, \
    GetExpectedPath, CompareWithFile, IsPrepare, ContainsPattern


sys.stdout.reconfigure(encoding='utf-8') # type: ignore[attr-defined]
subdir = "output_file"


def test_output_file():
    stem = "example_all_categories"
    encoding="cp1252"
    path = GetExamplesPath() / (stem + ".txt")
    expected = GetExpectedPath() / subdir / (stem + "_output_expected.txt")
    output_file = Path(tempfile.gettempdir()) / "output.txt"
    params = [
        "--color=never",
        f"--encoding={encoding}",
        f"--output-file={str(output_file)}",
        path,
    ]
    result = Run(params, 0)
    with open(output_file, encoding="utf-8") as f:
        content = f.read()
    if IsPrepare() and not expected.exists():
        with open(expected, "w", encoding="utf-8") as f:
            f.write(content)

    ContainsPattern(result.stderr, "REPLACEMENT CHARACTER")
    CompareWithFile(content, expected)


def test_output_file_with_bom():
    stem = "example_all_categories"
    encoding="cp1252"
    path = GetExamplesPath() / (stem + ".txt")
    expected = GetExpectedPath() / subdir / (stem + "_output_bom_expected.txt")
    output_file = Path(tempfile.gettempdir()) / "output.txt"
    params = [
        "--color=never",
        f"--encoding={encoding}",
        "--output-bom=yes",
        f"--output-file={str(output_file)}",
        path,
    ]
    result = Run(params, 0)
    with open(output_file, encoding="utf-8") as f:
        content = f.read()
    if IsPrepare() and not expected.exists():
        with open(expected, "w", encoding="utf-8") as f:
            f.write(content)

    ContainsPattern(result.stderr, "REPLACEMENT CHARACTER")
    CompareWithFile(content, expected)


def test_output_file_no_bom():
    stem = "example_all_categories"
    encoding="cp1252"
    path = GetExamplesPath() / (stem + ".txt")
    expected = GetExpectedPath() / subdir / (stem + "_output_no_bom_expected.txt")
    output_file = Path(tempfile.gettempdir()) / "output.txt"
    params = [
        "--color=never",
        f"--encoding={encoding}",
        "--output-bom=no",
        f"--output-file={str(output_file)}",
        path,
    ]
    result = Run(params, 0)
    with open(output_file, encoding="utf-8") as f:
        content = f.read()
    if IsPrepare() and not expected.exists():
        with open(expected, "w", encoding="utf-8") as f:
            f.write(content)

    ContainsPattern(result.stderr, "REPLACEMENT CHARACTER")
    CompareWithFile(content, expected)


def test_output_file_bom_as_input():
    stem = "example_all_categories"
    encoding="cp1252"
    path = GetExamplesPath() / (stem + ".txt")
    expected = GetExpectedPath() / subdir / (stem + "_output_as_input_expected.txt")
    output_file = Path(tempfile.gettempdir()) / "output.txt"
    params = [
        "--color=never",
        f"--encoding={encoding}",
        "--output-bom=as_input",
        f"--output-file={str(output_file)}",
        path,
    ]
    result = Run(params, 0)
    with open(output_file, encoding="utf-8") as f:
        content = f.read()
    if IsPrepare() and not expected.exists():
        with open(expected, "w", encoding="utf-8") as f:
            f.write(content)

    ContainsPattern(result.stderr, "REPLACEMENT CHARACTER")
    CompareWithFile(content, expected)
