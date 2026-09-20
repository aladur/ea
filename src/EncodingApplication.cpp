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
#include "EncodedOutput.h"
#include <boost/program_options.hpp>
#include <unicode/uchar.h>
#include <cassert>
#include <cstddef>
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
        bool printCategories,
        ColorMode colorMode,
        EncodingTypes categoryFilter,
        std::optional<std::string> optFilename)
    : istream_(is)
    , fallbackEncoding_(fallbackEncoding)
    , printLines_(printLines)
    , printSummary_(printSummary)
    , printStatistics_(printStatistics)
    , printFilename_(printFilename)
    , printLineNumber_(printLineNumber)
    , printCategories_(printCategories)
    , colorMode_(colorMode)
    , categoryFilter_(categoryFilter)
    , optFilename_(optFilename)
    , output_(std::cout, printFilename_, printLineNumber_,
                   printCategories_,
                   categoryFilter_, optFilename)
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
    auto categories = EncodingTypes::None;
    bool isCR = false;

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
            const bool isNewLine = encodingType == EncodingType::Control &&
                codepoint == Codepoint(0x0A);
            bool wasSingleCR = (isCR && !isNewLine);
            isCR = encodingType == EncodingType::Control &&
                codepoint == Codepoint(0x0D);
            if (!isCR && !isNewLine)
            {
                categories |= ToEncodingTypes(encodingType);
            }

            if (wasSingleCR)
            {
                // Previous codepoint was a CR without LF.
                const TypedCodepoint typed{Codepoint(0x0D),
                                           EncodingType::Control};

                categories |= ToEncodingTypes(EncodingType::Control);
                statistics[typed.type].Add(typed.codepoint);

                if (printLines_)
                {
                    if (withColor)
                    {
                        UpdateColor(encodingType);
                    }
                    output_ << AsUtf8String(typed);
                }
            }

            if (isCR)
            {
                continue;
            }

            if (isNewLine)
            {
                if (printLines_)
                {
                    output_.PrintLine(withColor, categories);
                    categories = EncodingTypes::None;
                }
                continue;
            }
            else
            {
                statistics[encodingType].Add(codepoint);
            }

            if (printLines_)
            {
                if (withColor)
                {
                    UpdateColor(encodingType);
                }
                output_ << AsUtf8String(optResult.value());
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
        output_.PrintLine(withColor, categories);
    }
    categories = EncodingTypes::None;

    if (printSummary_)
    {
        std::size_t sum = 0U;

        std::cout << "BOM: " << AsHumanReadable(bomType) << "\n";
        for (int i = 0; i < static_cast<int>(GetEncodingNames().size()); ++i)
        {
            const auto encodingType = GetEncodingTypes()[i];
            std::cout << statistics[encodingType].GetCount() << " " <<
                GetEncodingNames()[i] << " characters\n";
            sum += statistics[encodingType].GetCount();
        }

        std::cout << sum << " characters in total" << "\n";
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
                    (encodingType == EncodingType::Fallback) ?
                        ToUtf8(codepoint, fallbackEncoding_) :
                        ((encodingType == EncodingType::Control ||
                          encodingType == EncodingType::Indeterminate) ?
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
    else if (encodingType == EncodingType::Fallback)
    {
        color = Color::YELLOW;
    }
    else if (encodingType == EncodingType::Indeterminate)
    {
        color = Color::RED;
    }

    if (color != nullptr)
    {
        output_.UpdateColor(color);
    }
}

const std::vector<EncodingType> &EncodingApplication::GetEncodingTypes()
{
    static const std::vector<EncodingType> encodingTypes{
        EncodingType::Control,
        EncodingType::Ascii,
        EncodingType::Unicode,
        EncodingType::Fallback,
        EncodingType::Indeterminate,
    };

    return encodingTypes;
}

const std::vector<std::string> &EncodingApplication::GetEncodingNames()
{
    static const std::vector<std::string> encodingNames{
        "CONTROL",
        "ASCII",
        "UNICODE",
        "FALLBACK(" + fallbackEncoding_ + ")",
        "INDETERMINATE",
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

