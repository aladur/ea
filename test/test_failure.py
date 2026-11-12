import sys
from helper_functions import Run, GetExamplesPath, \
    IsPrepare, EqualsPattern, ContainsPattern


sys.stdout.reconfigure(encoding='utf-8') # type: ignore[attr-defined]


def test_categories_per_line_failure():
    stem = "example_utf8"
    path = GetExamplesPath() / (stem + ".txt")
    options = [ "--count", "--statistics" ]

    for option in options:
        result = Run([ "--categories-per-line", f"{option}", str(path)], 1)
        EqualsPattern(result.stdout, "")
        ContainsPattern(result.stderr, "cannot be used with")


def test_line_number_failure():
    stem = "example_utf8"
    path = GetExamplesPath() / (stem + ".txt")
    options = [ "--count", "--statistics" ]

    for option in options:
        result = Run([ "--line-number", f"{option}", str(path)], 1)
        EqualsPattern(result.stdout, "")
        ContainsPattern(result.stderr, "cannot be used with")


def test_output_bom_failure():
    stem = "example_utf8"
    path = GetExamplesPath() / (stem + ".txt")
    options = [ "--output-bom", "--output-bom=no", "--output-bom=yes", "--output-bom=as_input" ]

    for option in options:
        result = Run([ f"{option}", str(path)], 1)
        EqualsPattern(result.stdout, "")
        ContainsPattern(result.stderr, "only can be used together with")

    result = Run([ "--output-file=o.txt", "--output-bom=invalid", str(path)], 1)
    EqualsPattern(result.stdout, "")
    ContainsPattern(result.stderr, "Undefined")
