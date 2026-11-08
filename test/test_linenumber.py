import sys
from helper_functions import Run, GetExamplesPath, \
    GetExpectedPath, CompareWithFile, IsPrepare, EqualsPattern


sys.stdout.reconfigure(encoding='utf-8') # type: ignore[attr-defined]


def test_linenumbers():
    categories = [
        "ascii",
        "control",
        "unicode",
        "fallback",
        "indeterminate",
        "empty",
        "ascii,unicode,indeterminate",
        "control,fallback,empty",
        "ascii,control,unicode,fallback,indeterminate,empty",
    ]
    stem = "example_all_categories"
    encoding = "cp1252"
    path = GetExamplesPath() / (stem + ".txt")

    for category in categories:
        catstr = result = "".join(part[0] for part in category.split(","))
        filename = stem + "_" + catstr + "_expected.txt"
        expected = GetExpectedPath() / "linenumber" / filename
        params = [
            "--color=never",
            "--line-number",
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

