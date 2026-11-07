import sys
from helper_functions import Run, GetTestFiles, GetExamplesPath, \
    GetExpectedPath, CompareWithFile, IsPrepare


sys.stdout.reconfigure(encoding='utf-8')


def test_default():
    for item in GetTestFiles():
        path = GetExamplesPath() / (item.stem + ".txt")
        expected = GetExpectedPath() / "default" / (item.stem + "_expected.txt")
        params = [
            "--color=never",
            str(path),
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        CompareWithFile(result.stdout, expected)
