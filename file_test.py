import os
import sys
from pathlib import Path
from charset_normalizer import from_bytes


def main():
    sys.stdout.reconfigure(encoding='utf-8')
    path = Path(__file__).resolve().parents[0] / "test" / "expected" / "default" / "example_utf8_expected.txt"
    match = from_bytes(path.read_bytes()).best()
    if match is None:
        print("Could not determine an encoding")
    else:
        print(f"guess={match.encoding}")

    data = path.read_bytes()

    try:
        data.decode("utf-8", errors="strict")
        print("Valid UTF-8")
    except UnicodeDecodeError:
        print("Not valid UTF-8")

    with open(path, encoding="utf-8") as f:
        file_lines = [line.rstrip("\n\r") for line in f]

    if len(file_lines) > 0:
        print(f"line[0]={file_lines[0]}")

    for offset in range(0, len(data), 16):
        chunk = data[offset:offset + 16]
        hex_bytes = " ".join(f"{byte:02x}" for byte in chunk)
        print(f"{offset:08x}  {hex_bytes}")


if __name__ == "__main__":
    main()
