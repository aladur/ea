import sys
from helper_functions import Run, GetTestFiles, GetExamplesPath, \
    GetExpectedPath, CompareWithFile, IsPrepare, EqualsPattern


sys.stdout.reconfigure(encoding='utf-8') # type: ignore[attr-defined]


def test_categories_per_line():
    for item in GetTestFiles():
        path = GetExamplesPath() / (item.stem + ".txt")
        expected = GetExpectedPath() / "categories" / (item.stem + "_expected.txt")
        params = [
            "--color=never",
            "--categories-per-line",
            f"--encoding={item.encoding}",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)


def test_category_filter():
    categories = [
        "ascii",
        "control",
        "unicode",
        "fallback",
        "indeterminate",
        "empty",
        "ascii,unicode,indeterminate",
        "control,fallback,empty",
    ]
    stem = "example_all_categories"
    encoding = "cp1252"
    path = GetExamplesPath() / (stem + ".txt")

    for category in categories:
        catstr = result = "".join(part[0] for part in category.split(","))
        filename = stem + "_" + catstr + "_expected.txt"
        expected = GetExpectedPath() / "categories" / filename
        params = [
            "--color=never",
            "--categories-per-line",
            f"--filter={category}",
            f"--encoding={encoding}",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)

