/*
MIT License

Copyright (c) 2026 Wolfgang Schwotzer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef HELPERFUNCTIONS_INCLUDED
#define HELPERFUNCTIONS_INCLUDED

#include "Codepoint.h"
#include "TypeDefinitions.h"
#include <string>
#include <ostream>

extern std::string to_string(EncodingType type);
extern std::ostream& operator<<(std::ostream& os, EncodingType type);
extern std::string to_string(BomType type);
extern std::ostream& operator<<(std::ostream& os, BomType type);
extern std::string AsHumanReadable(BomType type);
extern ColorMode ToMode(std::string colorString);
extern bool IsValidEncoding(const std::string &encoding, std::string &error);
extern bool IsValidColorMode(const std::string &colorString);
extern EncodingType GetCodepointType(const Codepoint &cp,
        const std::string &encoding);
extern bool IsSingleByteEncoding(const std::string &encoding);
extern std::string ToUtf8(const Codepoint &cp);
extern std::string ToUtf8(const Codepoint &cp, const std::string &encoding);
extern std::string AsControlCharacter(const Codepoint &cp);
extern Codepoint GetUtf8Character(const char *data, int size);
extern void PrepareConsoleForUtf8();
#endif

