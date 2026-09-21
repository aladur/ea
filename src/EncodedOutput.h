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

#ifndef RICHLINEPRINTER_INCLUDED
#define RICHLINEPRINTER_INCLUDED

#include "TypeDefinitions.h"
#include <utility>
#include <string>
#include <sstream>
#include <ostream>
#include <fstream>
#include <optional>
#include <filesystem>

namespace fs = std::filesystem;

class EncodedOutput
{
public:
    EncodedOutput() = delete;
    EncodedOutput(
            std::ostream &os,
            bool printLines,
            bool printFilename,
            bool printLineNumber,
            bool printCategories,
            EncodingTypes categoryFilter,
            OutputBomMode outputBomMode,
            const std::string &fallbackEncoding,
            std::optional<std::string> optFilename = std::nullopt,
            std::optional<fs::path> optOutputFilePath = std::nullopt);
    EncodedOutput(std::ostream&& stream) = delete;

    void Reset();
    void PrintLine(bool withColor, EncodingTypes categories);
    void UpdateColor(const char *color);
    void Output(const TypedCodepoint &typedCodepoint);
    bool HasReplacementCharacter() const;
    const std::optional<fs::path> &GetOptOutputFilePath() const;

protected:
    void PrintCategories(bool withColor, EncodingTypes categories);
    std::string AsUtf8String(const TypedCodepoint &tcp) const;
    std::string ConvertToUtf8(const TypedCodepoint &tcp);

private:
    std::ostream &ostream_;
    std::stringstream lineStream_;
    std::ofstream outputFileStream_;
    bool printLines_{};
    bool printFilename_{};
    bool printLineNumber_{};
    bool printCategories_{};
    EncodingTypes categoryFilter_{EncodingTypes::None};
    OutputBomMode outputBomMode_{OutputBomMode::AsInput};
    std::string fallbackEncoding_;
    std::string filename_;
    std::optional<fs::path> optOutputFilePath_;
    std::size_t lineNumber_{1U};
    const char *currentColor_{};
    bool hasReplacementCharacter_{};
    bool isBomProcessed{};
};

#endif

