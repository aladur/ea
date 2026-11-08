import sys
from helper_functions import Run, GetTestFiles, GetExamplesPath, \
    GetExpectedPath, CompareWithFile, IsPrepare, EqualsPattern, \
    ContainsPattern


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


def test_encoding_list():
    result = Run(["--encoding=list"], 0)
    result_a = Run(["--encoding=list-a"], 0)

    ContainsPattern(result.stderr, "")
    ContainsPattern(result_a.stderr, "")
    assert(len(result.stdout.splitlines()) == len(result_a.stdout.splitlines()))
    ContainsPattern(result.stdout, "ISO-8859-1")
    ContainsPattern(result.stdout, "US-ASCII")
    ContainsPattern(result.stdout, "macos-0_2-10.2") # Mac OS Roman
    ContainsPattern(result.stdout, "ibm-1089_P100-1995") # Latin/Arabic
    ContainsPattern(result_a.stdout, "ISO-8859-1")
    ContainsPattern(result_a.stdout, "ISO-8859-6")
    ContainsPattern(result_a.stdout, "ISO-8859-15")


def test_encoding_list_standards():
    result = Run(["--encoding=list-standards"], 0)

    ContainsPattern(result.stderr, "")
    ContainsPattern(result.stdout, "IANA")
    ContainsPattern(result.stdout, "IBM")
    ContainsPattern(result.stdout, "JAVA")
    ContainsPattern(result.stdout, "MIME")
    ContainsPattern(result.stdout, "UTR22")
    ContainsPattern(result.stdout, "WINDOWS")


def test_encoding_list_iana():
    encoding = "iana"
    result = Run([f"--encoding=list-{encoding}"], 0)
    result_a = Run([f"--encoding=list-{encoding}-a"], 0)

    ContainsPattern(result.stderr, "")
    assert(len(result.stdout.splitlines()) == len(result_a.stdout.splitlines()))
    ContainsPattern(result.stdout, "ISO_8859-6")
    ContainsPattern(result.stdout, "IBM437")
    ContainsPattern(result.stdout, "windows-1252")
    ContainsPattern(result.stdout, "hp-roman8")
    ContainsPattern(result.stdout, "macintosh")


def test_encoding_list_ibm():
    encoding = "ibm"
    result = Run([f"--encoding=list-{encoding}"], 0)
    result_a = Run([f"--encoding=list-{encoding}-a"], 0)

    ContainsPattern(result.stderr, "")
    assert(len(result.stdout.splitlines()) == len(result_a.stdout.splitlines()))
    for line in result.stdout.splitlines():
        ContainsPattern(line, "ibm-")


def test_encoding_list_java():
    encoding = "java"
    result = Run([f"--encoding=list-{encoding}"], 0)
    result_a = Run([f"--encoding=list-{encoding}-a"], 0)

    ContainsPattern(result.stderr, "")
    assert(len(result.stdout.splitlines()) == len(result_a.stdout.splitlines()))
    ContainsPattern(result.stdout, "ISO-8859-6")
    ContainsPattern(result.stdout, "ASCII")
    ContainsPattern(result.stdout, "windows-1252")
    ContainsPattern(result.stdout, "x-macroman")



def test_encoding_list_mime():
    encoding = "mime"
    result = Run([f"--encoding=list-{encoding}"], 0)
    result_a = Run([f"--encoding=list-{encoding}-a"], 0)

    ContainsPattern(result.stderr, "")
    assert(len(result.stdout.splitlines()) == len(result_a.stdout.splitlines()))
    ContainsPattern(result.stdout, "US-ASCII")
    ContainsPattern(result.stdout, "KOI8-R")
    ContainsPattern(result.stdout, "IBM860")
    ContainsPattern(result.stdout, "ISO-8859-15")
    ContainsPattern(result.stdout, "macintosh")


def test_encoding_list_utr22():
    encoding = "utr22"
    result = Run([f"--encoding=list-{encoding}"], 0)
    result_a = Run([f"--encoding=list-{encoding}-a"], 0)

    ContainsPattern(result.stderr, "")
    assert(len(result.stdout.splitlines()) == len(result_a.stdout.splitlines()))
    ContainsPattern(result.stdout, "ibm-437_P100-1995")
    ContainsPattern(result.stdout, "macos-0_2-10.2")
    ContainsPattern(result.stdout, "windows-874-2000")


def test_encoding_list_windows():
    encoding = "windows"
    result = Run([f"--encoding=list-{encoding}"], 0)
    result_a = Run([f"--encoding=list-{encoding}-a"], 0)

    ContainsPattern(result.stderr, "")
    assert(len(result.stdout.splitlines()) == len(result_a.stdout.splitlines()))
    for line in result.stdout.splitlines():
        ContainsPattern(line, "windows-")
    ContainsPattern(result.stdout, "windows-437")
    ContainsPattern(result.stdout, "windows-850")
    ContainsPattern(result.stdout, "windows-1252")


def test_encoding_list_aliases():
    standards=[
        "IANA",
        "IBM",
        "JAVA",
        "MIME",
        "UTR22",
        "WINDOWS",
    ]

    for standard in standards:
        result = Run([f"--encoding=list-{standard}-a"], 0)
        ContainsPattern(result.stderr, "")
        for line in result.stdout.splitlines():
            ContainsPattern(line, ";")
