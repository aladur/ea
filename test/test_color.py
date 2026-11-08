import sys
from helper_functions import Run, GetExamplesPath, \
    GetExpectedPath, CompareWithFile, IsPrepare, EqualsPattern


sys.stdout.reconfigure(encoding='utf-8') # type: ignore[attr-defined]


def test_color():
    stem = "example_all_categories"
    encoding="cp1252"
    path = GetExamplesPath() / (stem + ".txt")
    expected = GetExpectedPath() / "color" / (stem + "_expected.txt")
    params = [
        "--color=always",
        "--categories-per-line",
        "--line-number",
        "--with-filename",
        f"--encoding={encoding}",
        path,
    ]
    result = Run(params, 0)
    if IsPrepare() and not expected.exists():
        with open(expected, "w", encoding="utf-8") as f:
            f.write(result.stdout)

    EqualsPattern(result.stderr, "")
    CompareWithFile(result.stdout, expected)

