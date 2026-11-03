import os
import re
import subprocess
from pathlib import Path
from typing import Sequence


def _findBinary():
      root = Path(__file__).resolve().parents[1]
      candidates = []

      if os.name == "nt":
          candidates.extend([
              root / "install" / "bin" / "ea.exe",
              root / "ea_build" / "Release" / "ea.exe",
              root / "ea_build" / "src" / "Release" / "ea.exe",
          ])
      else:
          candidates.extend([
              root / "install" / "bin" / "ea",
              root / ".." / "ea_build" / "src" / "ea",
          ])

      for candidate in candidates:
          if candidate.exists():
              return str(candidate)

      raise FileNotFoundError(
          "Could not find built ea executable. "
          "Expected one of: " + ", ".join(str(p) for p in candidates)
      )


def IsPrepare() -> bool:
    """Only to be used for test preparation"""
    return False


def GetExamplesPath() -> Path:
    return Path(__file__).resolve().parents[1] / "examples"


def GetExpectedPath() -> Path:
    return Path(__file__).resolve().parents[0] / "expected"


def Run(
    programOptions: Sequence[str],
    exampleFile: str | os.PathLike,
    expectedExitCode: int,
) -> subprocess.CompletedProcess:
    command = [str(_findBinary()), *programOptions, str(exampleFile)]
    result = subprocess.run(
        command,
        capture_output=True,
        text=True,
        check=False,
    )

    if result.returncode != expectedExitCode:
        raise AssertionError(
            f"ea exited with {result.returncode}; expected {expectedExitCode}.\n"
            f"Command: {command!r}\n"
            f"stdout: {result.stdout!r}\n"
            f"stderr: {result.stderr!r}"
        )

    return result


def EqualsPattern(text: str, pattern: str):
    if text == pattern:
        return
    msg = f"Expected pattern not equal: {pattern!r}";
    assert False, msg


def ContainsPattern(text: str, pattern: str, **kwargs):
    lineno = kwargs.get('lineno', None)
    line_number = 0
    for line in text.replace("\r\n", "\n").replace("\r", "\n").split("\n"):
        line_number += 1
        if ((lineno is None) or lineno == line_number) and pattern in line:
            return True
    if lineno is None:
        msg = f"Expected pattern not found: {pattern!r}";
    else:
        msg = f"Expected pattern not found in line {lineno}: {pattern!r}";
    assert False, msg


def RegexMatches(text: str, pattern: str, **kwargs):
    lineno = kwargs.get('lineno', None)
    line_number = 0
    regex = re.compile(pattern)
    for line in text.replace("\r\n", "\n").replace("\r", "\n").split("\n"):
        line_number += 1
        if ((lineno is None) or lineno == line_number) and regex.search(line):
            return
    if lineno is None:
        msg = f"Expected regex pattern not found: {pattern!r}";
    else:
        msg = f"Expected regex pattern not found in line {lineno}: {pattern!r}";
    assert False, msg


def CompareWithFile(text: str, filepath: Path) -> None:
    """
    Compare a string against a file line by line.
    Raises AssertionError with the line number and differing content
    if any lines differ (or if the number of lines differs).
    """
    lines = text.splitlines()

    with open(filepath, encoding="utf-8") as f:
        file_lines = [line.rstrip("\n\r") for line in f]

    if len(lines) != len(file_lines):
        raise AssertionError(
            f"Number of lines differs: string has {len(lines)}, "
            f"file has {len(file_lines)}"
        )

    for lineno, (actual, expected) in enumerate(zip(lines, file_lines), start=1):
        if expected != actual:
            raise AssertionError(
                f"File {filepath} line {lineno} differs:\n"
                f"  expected: {expected!r}\n"
                f"  actual:   {actual!r}"
            )

