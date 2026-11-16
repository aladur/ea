import sys
from helper_functions import Run, GetTestFiles, GetExamplesPath, \
    GetExpectedPath, CompareWithFile, IsPrepare, EqualsPattern


sys.stdout.reconfigure(encoding='utf-8') # type: ignore[attr-defined]
subdir = "statistics"


def test_statistics_encoded():

    for item in GetTestFiles():
        path = GetExamplesPath() / (item.stem + ".txt")
        expected = GetExpectedPath() / subdir / (item.stem + "_encoded_expected.txt")
        params = [
            "--color=never",
            "--statistics",
            "--statistics-mode=brief",
            f"--encoding={item.encoding}",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)


def test_statistics_filter_encoded():

    for item in GetTestFiles():
        path = GetExamplesPath() / (item.stem + ".txt")
        expected = GetExpectedPath() / subdir / (item.stem + "_filter_encoded_expected.txt")
        params = [
            "--color=never",
            "--statistics",
            "--statistics-mode=brief",
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


def test_statistics():

    for item in GetTestFiles():
        path = GetExamplesPath() / (item.stem + ".txt")
        expected = GetExpectedPath() / subdir / (item.stem + "_noenc_expected.txt")
        params = [
            "--color=never",
            "--statistics",
            "--statistics-mode=brief",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)


def test_statistics_modes():

    statisticsModes = [
        "brief",
        "normal",
        "full",
    ]
    stem = "example_all_categories";
    path = GetExamplesPath() / (stem + ".txt")
    for statisticsMode in statisticsModes:
        filename = f"{stem}_{statisticsMode}_expected.txt"
        expected = GetExpectedPath() / subdir / filename
        params = [
            "--color=never",
            "--statistics",
            f"--statistics-mode={statisticsMode}",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)

