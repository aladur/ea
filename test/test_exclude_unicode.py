import sys
from helper_functions import Run, GetExamplesPath, \
    GetExpectedPath, CompareWithFile, IsPrepare, EqualsPattern
from dataclasses import dataclass


sys.stdout.reconfigure(encoding='utf-8') # type: ignore[attr-defined] 


def test_exclude_unicode_ebcdic():
    @dataclass
    class FileStemWithExcludes:
        stem: str
        excludes: str

    items = [
        FileStemWithExcludes("example_ebcdic_english_cp037", "U+0085,U+0196"),
        FileStemWithExcludes("example_ebcdic_german_cp037",
                             "U+0053,U+0057,U+0085,U+0105,U+0193,U+01D9,U+0505,U+0707,U+0722,U+28C1"),
    ]

    for item in items:
        encoding="cp037"
        path = GetExamplesPath() / (item.stem + ".txt")
        expected = GetExpectedPath() / "exclude_unicode" / (item.stem + "_expected.txt")
        params = [
            "--color=never",
            f"--exclude-unicode={item.excludes}",
            f"--encoding={encoding}",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)


def test_exclude_unicode_ebcdic_list():
    @dataclass
    class FileStemWithCount:
        stem: str
        count: int

    items = [
        FileStemWithCount("example_ebcdic_english_cp037", 2),
        FileStemWithCount("example_ebcdic_german_cp037", 10),
    ]

    for item in items:
        encoding="cp037"
        path = GetExamplesPath() / (item.stem + ".txt")
        params = [
            f"--exclude-unicode=list",
            f"--encoding={encoding}",
            path,
        ]
        result = Run(params, 0)

        assert(result.stdout.count(",") == item.count)
