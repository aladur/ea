import sys
import tempfile
from pathlib import Path
from helper_functions import Run, GetExamplesPath, \
    GetExpectedPath, CompareWithFile, IsPrepare, ContainsPattern


sys.stdout.reconfigure(encoding='utf-8') # type: ignore[attr-defined]


def test_output_file():
    stem = "example_all_categories"
    encoding="cp1252"
    path = GetExamplesPath() / (stem + ".txt")
    expected = GetExpectedPath() / "output_file" / (stem + "_output_expected.txt")
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

    ContainsPattern(result.stderr, "REPLACEMENT CHARACTER")
    CompareWithFile(content, expected)


def test_output_file_with_bom():
    stem = "example_all_categories"
    encoding="cp1252"
    path = GetExamplesPath() / (stem + ".txt")
    expected = GetExpectedPath() / "output_file" / (stem + "_output_bom_expected.txt")
    output_file = Path(tempfile.gettempdir()) / "output.txt"
    params = [
        "--color=never",
        f"--encoding={encoding}",
        "--output-bom=yes",
        f"--output-file={str(output_file)}",
        path,
    ]
    result = Run(params, 0)
    if IsPrepare() and not expected.exists():
        expected.write_bytes(output_file.read_bytes())
    with open(output_file, encoding="utf-8") as f:
        content = f.read()

    ContainsPattern(result.stderr, "REPLACEMENT CHARACTER")
    CompareWithFile(content, expected)


def test_output_file_no_bom():
    stem = "example_all_categories"
    encoding="cp1252"
    path = GetExamplesPath() / (stem + ".txt")
    expected = GetExpectedPath() / "output_file" / (stem + "_output_expected.txt")
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

    ContainsPattern(result.stderr, "REPLACEMENT CHARACTER")
    CompareWithFile(content, expected)


def test_output_file_bom_as_input():
    stem = "example_all_categories"
    encoding="cp1252"
    path = GetExamplesPath() / (stem + ".txt")
    expected = GetExpectedPath() / "output_file" / (stem + "_output_expected.txt")
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

    ContainsPattern(result.stderr, "REPLACEMENT CHARACTER")
    CompareWithFile(content, expected)
