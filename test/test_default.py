from pathlib import Path
from helper_functions import Run, GetExamplesPath, GetExpectedPath, \
    CompareWithFile, IsPrepare


def test_default():
    stems = [
        "example_utf8",
        "example_windows1252",
        "example_spam_mail",
        "example_all_categories",
        "example_c0_controls_with_del",
        "example_ebcdic_english_cp037",
        "single_byte_character_set_ascii",
        "single_byte_character_set_ebcdic",
        "UTF-8-demo",
    ]

    for stem in stems:
        path = GetExamplesPath() / (stem + ".txt")
        expected = GetExpectedPath() / "default" / (stem + "_expected.txt")
        print("path=" + str(path))
        result = Run(["--color=never"], path, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        CompareWithFile(result.stdout, expected)
