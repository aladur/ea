<p align="center">
 <svg width="128" height="128" viewBox="0 0 128 128" xmlns="http://www.w3.org/2000/svg">
  <rect width="i128" height="128" fill="white"/>
  <image href="https://raw.githubusercontent.com/aladur/ea/main/ea.ico" width="128" height="128"/>
 </svg>
</p>

# ea - Character Encoding Analyzer

[![Windows build](https://github.com/aladur/ea/actions/workflows/windows-build.yml/badge.svg?branch=main)](https://github.com/aladur/ea/actions/workflows/windows-build.yml)
[![Linux build](https://github.com/aladur/ea/actions/workflows/linux-build.yml/badge.svg?branch=main)](https://github.com/aladur/ea/actions/workflows/linux-build.yml)
[![MacOS build](https://github.com/aladur/ea/actions/workflows/macos-build.yml/badge.svg?branch=main)](https://github.com/aladur/ea/actions/workflows/macos-build.yml)

![github commit activity](https://img.shields.io/github/commit-activity/y/aladur/ea?style=plastic)
[![Total commits](https://img.shields.io/github/commit-activity/t/aladur/ea?label=total%20commits)](https://github.com/aladur/ea/commits)
![github last commit](https://img.shields.io/github/last-commit/aladur/ea?style=plastic&color=blue)
[![license](https://img.shields.io/github/license/aladur/ea?style=plastic&color=blue)](https://github.com/aladur/ea/blob/master/LICENSE)
[![github top language](https://img.shields.io/github/languages/top/aladur/ea?style=plastic)](https://github.com/search?q=repo%3Aaladur%2Fea++language%3AC%2B%2B&type=code)
![github repo size](https://img.shields.io/github/repo-size/aladur/ea?style=plastic)
![made for fun](https://img.shields.io/badge/made_with-fun-blue?style=plastic)
![github created](https://img.shields.io/github/created-at/aladur/ea?color=blue&style=plastic)

## Overview

Analyze a text file or stream for the following character encoding categories

- CONTROL characters (except for newline).
- ASCII encoded characters.
- UNICODE encoded characters.
- Characters encoded in a FALLBACK encoding.
- Characters with INDETERMINATE encoding.

Command line options

- Print file content with different colors for each category.
- Optionally print character count for each category.
- Optionally print character count for each character in each category.
- Optionally on each line print the filename and line number.
- Optionally a category filter can be set which line to print.
- Optionally write an UTF-8 converted file.

For now `ea` is focused on UTF-8 encoded files (or streams) optionally mixed with any second single-byte encoding, the so called fallback encoding, like Windows-1252 or ISO-8859-1. A well encoded UTF-8 codepoint always has priority over the fallback encoding.

`ea` is not able to resolve [Mojibake](https://en.wikipedia.org/wiki/Mojibake).
