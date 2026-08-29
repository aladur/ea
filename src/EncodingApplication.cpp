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

#include "Definitions.h"
#include "EncodingApplication.h"
#include "EncodedScanner.h"
#include "CharacterStatistics.h"
#include "TypeDefinitions.h"
#include "Codepoint.h"
#include "HelperFunctions.h"
#include "RichLinePrinter.h"
#include <boost/program_options.hpp>
#include <unicode/uchar.h>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <string>
#include <iostream>

EncodingApplication::EncodingApplication(
        std::istream &is,
        const std::string &fallbackEncoding,
        bool printLines,
        bool printSummary,
        bool printStatistics,
        bool printFilename,
        bool printLineNumber,
        ColorMode colorMode,
        std::optional<std::string> optFilename)
    : istream_(is)
    , fallbackEncoding_(fallbackEncoding)
    , printLines_(printLines)
    , printSummary_(printSummary)
    , printStatistics_(printStatistics)
    , printFilename_(printFilename)
    , printLineNumber_(printLineNumber)
    , colorMode_(colorMode)
    , optFilename_(optFilename)
    , linePrinter_(std::cout, printFilename_, printLineNumber_,
                   optFilename)
{
    if (!istream_.good())
    {
        std::stringstream messageStream;

        messageStream << "Input stream is not in good state";
        throw std::runtime_error(messageStream.str());
    }
}


int EncodingApplication::Run()
{
    assert(GetEncodingNames().size() == GetEncodingTypes().size());
    EncodedScanner scanner(istream_, fallbackEncoding_);
    std::unordered_map<EncodingType, CharacterStatistics> statistics;
    std::optional<TypedCodepoint> optResult;
    std::stringstream lineStream;
    bool isFirst = true;
    BomType bomType{};
    bool withColor = (colorMode_ == ColorMode::Always ||
                      (colorMode_ == ColorMode::Auto && IS_ATTY(STDOUT_FD)));

    for (const auto encodingType : GetEncodingTypes())
    {
        statistics[encodingType] = CharacterStatistics();
    }

    do
    {
        unsigned size{};

        optResult = scanner.GetNext(size);

        if (optResult.has_value())
        {
            const auto encodingType = optResult.value().type;
            const auto &codepoint = optResult.value().codepoint;
            statistics[encodingType].Add(codepoint);
            if (encodingType == EncodingType::Control &&
                codepoint == Codepoint(0x0A))
            {
                if (printLines_)
                {
                    linePrinter_.PrintLine(withColor);
                }
                continue;
            }

            if (printLines_)
            {
                if (withColor)
                {
                    UpdateColor(encodingType);
                }
                linePrinter_ << AsUtf8String(optResult.value());
            }
            if (isFirst)
            {
                bomType = EvaluateBomType(optResult.value().codepoint, size);
            }
        }
        isFirst = false;

    } while (optResult.has_value());

    if (printLines_)
    {
        linePrinter_.PrintLine(withColor);
    }

    if (printSummary_)
    {
        std::cout << "BOM: " << AsHumanReadable(bomType) << "\n";
        for (int i = 0; i < static_cast<int>(GetEncodingNames().size()); ++i)
        {
            const auto encodingType = GetEncodingTypes()[i];
            std::cout << statistics[encodingType].GetCount() << " " <<
                GetEncodingNames()[i] << " characters\n";
        }
    }

    if (printStatistics_)
    {
        std::cout << "\n";

        for (int i = 0; i < static_cast<int>(GetEncodingNames().size()); ++i)
        {
            const auto encodingType = GetEncodingTypes()[i];
            const auto &stat = statistics[encodingType];

            std::cout << "List of " << GetEncodingNames()[i] << " characters\n";
            for (const auto codepoint : stat.GetCodepoints())
            {
                const auto count = stat.GetCount(codepoint);
                const std::string utf8Char =
                    (encodingType == EncodingType::Default) ?
                        ToUtf8(codepoint, fallbackEncoding_) :
                        ((encodingType == EncodingType::Control ||
                          encodingType == EncodingType::Unknown) ?
                        "" : ToUtf8(codepoint));

                std::cout << std::dec << count << " ";
                if (encodingType == EncodingType::Unicode)
                {
                    std::cout << codepoint << " " << utf8Char << "\n";
                }
                else
                {
                    std::cout << std::uppercase << std::setw(2) <<
                        std::setfill('0') << std::hex << codepoint.Value() <<
                        " " << utf8Char << "\n";
                }
            }
        }
    }

    return 0;
}

std::string EncodingApplication::AsUtf8String(const TypedCodepoint &tcp) const
{
    switch (tcp.type)
    {
        case EncodingType::Control:
            [[fallthrough]];
        case EncodingType::Unknown:
            return AsControlCharacter(tcp.codepoint);

        case EncodingType::Ascii:
            [[fallthrough]];
        case EncodingType::Unicode:
            return ToUtf8(tcp.codepoint);

        case EncodingType::Default:
            return ToUtf8(tcp.codepoint, fallbackEncoding_);
    }

    return {};
}

void EncodingApplication::UpdateColor(EncodingType encodingType)
{
    const char *color = nullptr;

    if (encodingType == EncodingType::Ascii)
    {
        color = Color::RESET;
    }
    else if (encodingType == EncodingType::Unicode)
    {
        color = Color::GREEN;
    }
    else if (encodingType == EncodingType::Control)
    {
        color = Color::CYAN;
    }
    else if (encodingType == EncodingType::Default)
    {
        color = Color::YELLOW;
    }
    else if (encodingType == EncodingType::Unknown)
    {
        color = Color::RED;
    }

    if (color != nullptr)
    {
        linePrinter_.UpdateColor(color);
    }
}

const std::vector<EncodingType> &EncodingApplication::GetEncodingTypes()
{
    static const std::vector<EncodingType> encodingTypes{
        EncodingType::Control,
        EncodingType::Ascii,
        EncodingType::Unicode,
        EncodingType::Default,
        EncodingType::Unknown,
    };

    return encodingTypes;
}

const std::vector<std::string> &EncodingApplication::GetEncodingNames()
{
    static const std::vector<std::string> encodingNames{
        "CONTROL",
        "ASCII",
        "UNICODE",
        fallbackEncoding_.c_str(),
        "UNKNOWN",
    };

    return encodingNames;
}

BomType EncodingApplication::EvaluateBomType(Codepoint &cp, unsigned size)
{
    if (cp == Codepoint(0xFEFFU))
    {
        switch (size)
        {
            case 3U:
                return BomType::Utf8;
                break;
            case 2U:
                return BomType::Utf16LE;
                break;
        }
    }
    else if (cp == Codepoint(0xFFFEU))
    {
        switch (size)
        {
            case 2U:
                return BomType::Utf16BE;
                break;
        }
    }

    return BomType::None;
}

