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

#ifndef ENCODINGAPPLICATION_INCLUDED
#define ENCODINGAPPLICATION_INCLUDED

#include "TypeDefinitions.h"
#include "RichLinePrinter.h"
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <optional>
#include <string>
#include <vector>
#include <istream>

class EncodingApplication
{
public:
    EncodingApplication(
            std::istream &is,
            const std::string &fallbackEncoding,
            bool printLines, bool printSummary, bool printStatistics,
            bool printFilename, bool printLineNumber,
            ColorMode colorMode,
            std::optional<std::string> optFilename = std::nullopt);
    EncodingApplication() = delete;

    int Run();

protected:
    std::string AsUtf8String(const TypedCodepoint &tcp) const;
    const std::vector<EncodingType> &GetEncodingTypes();
    const std::vector<std::string> &GetEncodingNames();
    void UpdateColor(EncodingType encodingType);
    BomType EvaluateBomType(Codepoint &cp, unsigned size);

private:
    std::istream &istream_;
    std::string fallbackEncoding_;
    bool printLines_;
    bool printSummary_;
    bool printStatistics_;
    bool printFilename_;
    bool printLineNumber_;
    ColorMode colorMode_;
    std::optional<std::string> optFilename_;
    RichLinePrinter linePrinter_;
};

#endif

