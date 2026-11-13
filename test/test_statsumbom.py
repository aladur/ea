import sys
from helper_functions import Run, GetTestFiles, GetExamplesPath, \
    GetExpectedPath, CompareWithFile, IsPrepare, EqualsPattern


sys.stdout.reconfigure(encoding='utf-8') # type: ignore[attr-defined]
subdir = "statsumbom"

def test_statistics_summary_bom_encoded():

    for item in GetTestFiles():
        path = GetExamplesPath() / (item.stem + ".txt")
        expected = GetExpectedPath() / subdir / (item.stem + "_encoded_expected.txt")
        params = [
            "--color=never",
            "--statistics",
            "--count",
            "--bom",
            f"--encoding={item.encoding}",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)


def test_statistics_summary_bom():

    for item in GetTestFiles():
        path = GetExamplesPath() / (item.stem + ".txt")
        expected = GetExpectedPath() / subdir / (item.stem + "_noenc_expected.txt")
        params = [
            "--color=never",
            "--statistics",
            "--count",
            "--bom",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)

