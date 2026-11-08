import sys
from helper_functions import Run, GetTestFiles, GetExamplesPath, \
    GetExpectedPath, CompareWithFile, IsPrepare, EqualsPattern


sys.stdout.reconfigure(encoding='utf-8') # type: ignore[attr-defined]


def test_with_filename():
    base = GetExamplesPath()
    expected = GetExpectedPath() / "with_filename" / "files_expected.txt"
    params = [
        "--color=never",
        "--with-filename",
    ]
    for item in GetTestFiles():
        params.append(str(base / (item.stem + ".txt")))
    result = Run(params, 0)
    if IsPrepare() and not expected.exists():
        with open(expected, "w", encoding="utf-8") as f:
            f.write(result.stdout)

    EqualsPattern(result.stderr, "")
    CompareWithFile(result.stdout, expected)
