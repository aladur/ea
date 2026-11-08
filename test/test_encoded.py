import sys
from helper_functions import Run, GetTestFiles, GetExamplesPath, \
    GetExpectedPath, CompareWithFile, IsPrepare, EqualsPattern


sys.stdout.reconfigure(encoding='utf-8') # type: ignore[attr-defined]


def test_encoded():
    for item in GetTestFiles():
        path = GetExamplesPath() / (item.stem + ".txt")
        expected = GetExpectedPath() / "encoded" / (item.stem + "_expected.txt")
        params = [
            "--color=never",
            f"--encoding={item.encoding}",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)


def test_encoded_iso_8859_x():
    encodings = [
        "iso-8859-1",
        "iso-8859-2",
        "iso-8859-3",
        "iso-8859-4",
        "iso-8859-5",
        "iso-8859-6",
        "iso-8859-7",
        "iso-8859-8",
        "iso-8859-9",
        "iso-8859-10",
        "iso-8859-11",
        "iso-8859-13",
        "iso-8859-14",
        "iso-8859-15",
    ]
    stem = "single_byte_character_set_ascii"
    path = GetExamplesPath() / (stem + ".txt")

    for encoding in encodings:
        filename = stem + "_" + encoding + "_expected.txt"
        expected = GetExpectedPath() / "iso-8859-x" / filename
        params = [
            "--color=never",
            f"--encoding={encoding}",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)


def test_encoded_windows_125x():
    encodings = [
        "windows-1250",
        "windows-1251",
        "windows-1252",
        "windows-1253",
        "windows-1254",
        "windows-1255",
        "windows-1256",
        "windows-1257",
        "windows-1258",
    ]
    stem = "single_byte_character_set_ascii"
    path = GetExamplesPath() / (stem + ".txt")

    for encoding in encodings:
        filename = stem + "_" + encoding + "_expected.txt"
        expected = GetExpectedPath() / "windows-125x" / filename
        params = [
            "--color=never",
            f"--encoding={encoding}",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)


def test_encoded_ebcdic():
    encodings = [
        "ebcdic-xml-us",
        "ibm-37_P100-1995",
        "ibm-273_P100-1995",
        "ibm-277_P100-1995",
        "ibm-278_P100-1995",
        "ibm-280_P100-1995",
        "ibm-284_P100-1995",
        "ibm-285_P100-1995",
        "ibm-290_P100-1995",
        "ibm-297_P100-1995",
        "ibm-420_X120-1999",
        "ibm-424_P100-1995",
        "ibm-500_P100-1995",
        "ibm-803_P100-1999",
        "ibm-838_P100-1995",
        "ibm-870_P100-1995",
        "ibm-871_P100-1995",
        "ibm-875_P100-1995",
        "ibm-918_P100-1995",
        "ibm-1025_P100-1995",
        "ibm-1026_P100-1995",
        "ibm-1047_P100-1995",
        "ibm-1097_P100-1995",
        "ibm-1112_P100-1995",
        "ibm-1122_P100-1999",
        "ibm-1123_P100-1995",
        "ibm-1130_P100-1997",
        "ibm-1132_P100-1998",
        "ibm-1137_P100-1999",
        "ibm-1140_P100-1997",
        "ibm-1141_P100-1997",
        "ibm-1142_P100-1997",
        "ibm-1143_P100-1997",
        "ibm-1144_P100-1997",
        "ibm-1145_P100-1997",
        "ibm-1146_P100-1997",
        "ibm-1147_P100-1997",
        "ibm-1148_P100-1997",
        "ibm-1149_P100-1997",
        "ibm-1153_P100-1999",
        "ibm-1154_P100-1999",
        "ibm-1155_P100-1999",
        "ibm-1156_P100-1999",
        "ibm-1157_P100-1999",
        "ibm-1158_P100-1999",
        "ibm-1160_P100-1999",
        "ibm-1164_P100-1999",
        "ibm-4517_P100-2005",
        "ibm-4899_P100-1998",
        "ibm-4971_P100-1999",
        "ibm-5123_P100-1999",
        "ibm-8482_P100-1999",
        "ibm-9067_X100-2005",
        "ibm-12712_P100-1998",
        "ibm-16804_X110-1999",
    ]
    stem = "single_byte_character_set_ebcdic"
    path = GetExamplesPath() / (stem + ".txt")

    for encoding in encodings:
        filename = stem + "_" + encoding + "_expected.txt"
        expected = GetExpectedPath() / "ebcdic" / filename
        params = [
            "--color=never",
            f"--encoding={encoding}",
            path,
        ]
        result = Run(params, 0)
        if IsPrepare() and not expected.exists():
            with open(expected, "w", encoding="utf-8") as f:
                f.write(result.stdout)

        EqualsPattern(result.stderr, "")
        CompareWithFile(result.stdout, expected)
