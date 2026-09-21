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

#include "TypeDefinitions.h"
#include "HelperFunctions.h"
#include "EncodedOutput.h"
#include <optional>
#include <ios>
#include <ostream>
#include <utility>
#include <vector>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

EncodedOutput::EncodedOutput(
        std::ostream &os,
        bool printLines,
        bool printFilename,
        bool printLineNumber,
        bool printCategories,
        EncodingTypes categoryFilter,
        OutputBomMode outputBomMode,
        const std::string &fallbackEncoding,
        std::optional<std::string> optFilename,
        std::optional<fs::path> optOutputFilePath)
    : ostream_(os)
    , printLines_(printLines)
    , printFilename_(printFilename)
    , printLineNumber_(printLineNumber)
    , printCategories_(printCategories)
    , categoryFilter_(categoryFilter)
    , outputBomMode_(outputBomMode)
    , fallbackEncoding_(fallbackEncoding)
    , filename_(optFilename.value_or(std::string("(standard input)")))
    , optOutputFilePath_(optOutputFilePath)
    , currentColor_(Color::RESET)
{
    if (optOutputFilePath.has_value())
    {
        outputFileStream_ =
            std::ofstream(optOutputFilePath.value(), std::ios::binary);
        if (!outputFileStream_.is_open())
        {
            std::stringstream messageStream;

            messageStream << "Error opening " <<
                    optOutputFilePath.value().string();
            throw std::runtime_error(messageStream.str());
        }
    }
}

void EncodedOutput::Reset()
{
    lineStream_.clear();
    lineStream_.str("");
}

void EncodedOutput::PrintLine(bool withColor, EncodingTypes categories)
{
#ifdef _WIN32
    outputFileStream_ << "\r\n";
#else
    outputFileStream_ << "\n";
#endif

    if (!printLines_)
    {
        return;
    }

    if ((categoryFilter_ & categories) != EncodingTypes::None)
    {
        if (printFilename_)
        {
            if (withColor)
            {
                ostream_ << Color::MAGENTA << filename_ <<
                    Color::BLUE << ":" << Color::RESET;
            }
            else
            {
                ostream_ << filename_ << ":";
            }
        }

        if (printLineNumber_)
        {
            if (withColor)
            {
                ostream_ << Color::GREEN << lineNumber_ <<
                    Color::BLUE << ":" << Color::RESET;
            }
            else
            {
                ostream_ << lineNumber_ << ":";
            }
        }

        if (printCategories_)
        {
            PrintCategories(withColor, categories);
        }

        if (withColor)
        {
            UpdateColor(Color::RESET);
        }
        ostream_ << lineStream_.str() << "\n";
    }

    Reset();
    ++lineNumber_;
}

void EncodedOutput::UpdateColor(const char *color)
{
    if (currentColor_ != color)
    {
        currentColor_ = color;
        lineStream_ << currentColor_;
    }
}

void EncodedOutput::Output(const TypedCodepoint &tcp)
{
    if (printLines_)
    {
        lineStream_ << AsUtf8String(tcp);
    }

    if (isBomProcessed)
    {
        // A BOM in between is ignored.
        if (tcp.codepoint == Codepoint(0xFEFF))
        {
            return;
        }
    }
    else
    {
        isBomProcessed = true;

        switch (outputBomMode_)
        {
            case OutputBomMode::Yes:
                outputFileStream_ << '\xEF' << '\xBB' << '\xBF';
                [[fallthrough]];
            case OutputBomMode::No:
                if (tcp.codepoint == Codepoint(0xFEFF))
                {
                    return;
                }
                break;

            case OutputBomMode::AsInput:
                break;
        }
    }

    outputFileStream_ << ConvertToUtf8(tcp);

}

void EncodedOutput::PrintCategories(bool withColor, EncodingTypes categories)
{
    struct CategoryProps_t
    {
        char id;
        EncodingTypes type;
        const char *color;
    };
    static const std::vector<CategoryProps_t> categoryProperties{
        { 'a', EncodingTypes::Ascii, Color::RESET },
        { 'u', EncodingTypes::Unicode, Color::GREEN },
        { 'f', EncodingTypes::Fallback, Color::YELLOW },
        { 'i', EncodingTypes::Indeterminate, Color::RED },
        { 'c', EncodingTypes::Control, Color::CYAN },
    };

    for (const auto &props : categoryProperties)
    {
        const bool isSet = ((categories & props.type) != EncodingTypes::None);

        if (withColor)
        {
            ostream_ << (isSet ? props.color : Color::RESET);
        }

        ostream_ << (isSet ? props.id : '-');
    }

    if (withColor)
    {
        ostream_ << Color::BLUE << ':' << Color::RESET;
    }
    else
    {
        ostream_ << ':';
    }
}

std::string EncodedOutput::AsUtf8String(const TypedCodepoint &tcp) const
{
    switch (tcp.type)
    {
        case EncodingType::Control:
            [[fallthrough]];
        case EncodingType::Indeterminate:
            return AsControlCharacter(tcp.codepoint);

        case EncodingType::Ascii:
            [[fallthrough]];
        case EncodingType::Unicode:
            return ToUtf8(tcp.codepoint);

        case EncodingType::Fallback:
            return ToUtf8(tcp.codepoint, fallbackEncoding_);
    }

    return {};
}

std::string EncodedOutput::ConvertToUtf8(const TypedCodepoint &tcp)
{
    switch (tcp.type)
    {
        case EncodingType::Control:
        case EncodingType::Ascii:
            return std::string(1, tcp.codepoint & '\xFF');

        case EncodingType::Unicode:
            return ToUtf8(tcp.codepoint);

        case EncodingType::Fallback:
            return ToUtf8(tcp.codepoint, fallbackEncoding_);

        case EncodingType::Indeterminate:
            // For unknown codepoint use UTF-8 replacement character.
            hasReplacementCharacter_ = true;
            return std::string("\xEF\xBF\xBD");
    }

    return {};
}

bool EncodedOutput::HasReplacementCharacter() const
{
    return hasReplacementCharacter_;
}

const std::optional<fs::path> &EncodedOutput::GetOptOutputFilePath() const
{
    return optOutputFilePath_;
}
