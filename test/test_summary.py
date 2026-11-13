import sys
from helper_functions import Run, GetTestFiles, GetExamplesPath, \
    GetExpectedPath, CompareWithFile, IsPrepare, EqualsPattern


sys.stdout.reconfigure(encoding='utf-8') # type: ignore[attr-defined]
subdir = "summary"

def test_summary_encoded():

    for item in GetTestFiles():
        path = GetExamplesPath() / (item.stem + ".txt")
        expected = GetExpectedPath() / subdir / (item.stem + "_encoded_expected.txt")
        params = [
            "--color=never",
            "--count",
            f"--encoding={item.encoding}",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)


def test_summary_filter_encoded():

    for item in GetTestFiles():
        path = GetExamplesPath() / (item.stem + ".txt")
        expected = GetExpectedPath() / subdir / (item.stem + "_filter_encoded_expected.txt")
        params = [
            "--color=never",
            "--count",
            "--filter=u,f",
            f"--encoding={item.encoding}",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)


def test_summary():

    for item in GetTestFiles():
        path = GetExamplesPath() / (item.stem + ".txt")
        expected = GetExpectedPath() / subdir / (item.stem + "_noenc_expected.txt")
        params = [
            "--color=never",
            "--count",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)


def test_summary_total_encoded():

    for item in GetTestFiles():
        path = GetExamplesPath() / (item.stem + ".txt")
        expected = GetExpectedPath() / subdir / (item.stem + "_total_encoded_expected.txt")
        params = [
            "--color=never",
            "--count",
            "--total-sum",
            f"--encoding={item.encoding}",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)


def test_summary_total():

    for item in GetTestFiles():
        path = GetExamplesPath() / (item.stem + ".txt")
        expected = GetExpectedPath() / subdir / (item.stem + "_total_noenc_expected.txt")
        params = [
            "--color=never",
            "--count",
            "--total-sum",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)
