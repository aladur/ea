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

#include "HelperFunctions.h"
#include "TypeDefinitions.h"
#include "Codepoint.h"
#include <string>
#include <array>
#include <sstream>
#include <ostream>
#include <iostream>
#include <algorithm>
#include <boost/algorithm/string/case_conv.hpp>
#include <unicode/unistr.h>
#include <unicode/ucnv.h>
#include <unicode/utypes.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace balgo = boost::algorithm;

std::string to_string(EncodingType type)
{
    switch (type)
    {
        case EncodingType::Control:
            return "EncodingType::Control";
        case EncodingType::Ascii:
            return "EncodingType::Ascii";
        case EncodingType::Default:
            return "EncodingType::Default";
        case EncodingType::Unicode:
            return "EncodingType::Unicode";
        case EncodingType::Unknown:
            return "EncodingType::Unknown";
        default:
            return "<unknown>";
    }
}

std::ostream& operator<<(std::ostream& os, EncodingType type)
{
    os << to_string(type);

    return os;
}

std::string to_string(BomType type)
{
    switch (type)
    {
        case BomType::None:
            return "EncodingType::None";
        case BomType::Utf8:
            return "EncodingType::Utf8";
        case BomType::Utf16BE:
            return "EncodingType::Utf16BE";
        case BomType::Utf16LE:
            return "EncodingType::Utf16LE";
        case BomType::Utf32BE:
            return "EncodingType::Utf32BE";
        case BomType::Utf32LE:
            return "EncodingType::Utf32LE";
        default:
            return "<unknown>";
    }
}

std::ostream& operator<<(std::ostream& os, BomType type)
{
    os << to_string(type);

    return os;
}

std::string AsHumanReadable(BomType type)
{
    switch (type)
    {
        case BomType::None:
            return "None";
        case BomType::Utf8:
            return "UTF-8";
        case BomType::Utf16BE:
            return "UTF-16BE";
        case BomType::Utf16LE:
            return "UTF-16LE";
        case BomType::Utf32BE:
            return "UTF-32BE";
        case BomType::Utf32LE:
            return "UTF-32LE";
        default:
            return "<unknown>";
    }
}

ColorMode ToMode(std::string colorString)
{
    balgo::to_lower(colorString);

    if (colorString == "auto")
    {
        return ColorMode::Auto;
    }
    else if (colorString == "always")
    {
        return ColorMode::Always;
    }
    else if (colorString == "never")
    {
        return ColorMode::Never;
    }

    return ColorMode::Never;
}

bool IsValidEncoding(const std::string &encoding, std::string &error)
{
    UErrorCode status = U_ZERO_ERROR;
    UConverter* conv = ucnv_open(encoding.c_str(), &status);

    if (U_SUCCESS(status) && conv != NULL)
    {
        ucnv_close(conv);
        return true;
    }

    error = u_errorName(status);
    return false;
}

bool IsValidColorMode(const std::string &colorString)
{
    const static std::array<const char *, 3> colorModes{
        "never", "auto", "always"
    };

    return std::find(colorModes.cbegin(), colorModes.cend(),
            balgo::to_lower_copy(colorString)) != colorModes.cend();
}

EncodingType GetCodepointType(const Codepoint &cp,
        const std::string &encoding)
{
    UErrorCode status = U_ZERO_ERROR;
    std::array<char, 2> source{};
    std::array<char, 8> target{};

    source[0] = static_cast<char>(cp.Value());
    const auto byteCount = ucnv_convert(
        "UTF-8",
        encoding.c_str(),
        target.data(),
        static_cast<int32_t>(target.size()),
        source.data(),
        static_cast<int32_t>(source.size()),
        &status
    );

    if (byteCount == 0 || U_FAILURE(status))
    {
        return EncodingType::Unknown;
    }

    const auto codepoint = GetUtf8Character(target.data(), byteCount - 1);
    return (u_charType(codepoint) == U_CONTROL_CHAR) ?
        EncodingType::Control : EncodingType::Default;
}

bool IsSingleByteEncoding(const std::string &encoding)
{
    UErrorCode status = U_ZERO_ERROR;
    UConverter* conv = ucnv_open(encoding.c_str(), &status);

    if (U_FAILURE(status) || conv == NULL)
    {
        return false;
    }

    const auto minSize = ucnv_getMinCharSize(conv);
    const auto maxSize = ucnv_getMaxCharSize(conv);
    ucnv_close(conv);

    return (minSize == maxSize) && (minSize == 1);
}

std::string ToUtf8(const Codepoint &cp)
{
    if (cp.Value() > 0x10FFFF)
    {
        return {};
    }

    icu::UnicodeString unicodeString(static_cast<UChar32>(cp.Value()));
    std::string result;
    unicodeString.toUTF8String(result);

    return result;
}

std::string ToUtf8(const Codepoint &cp, const std::string &encoding)
{
    std::array<char, 2> source{};
    std::array<char, 8> target{};
    UErrorCode status = U_ZERO_ERROR;

    source[0] = static_cast<char>(cp.Value());
    const auto targetLen = ucnv_convert(
        "UTF-8",
        encoding.c_str(),
        target.data(),
        static_cast<int32_t>(target.size()),
        source.data(),
        static_cast<int32_t>(source.size()),
        &status
    );

    if (U_SUCCESS(status))
    {
        if (targetLen < static_cast<int>(target.size()))
        {
            target[targetLen] = '\0';
        }

        return std::string(target.data());
    }

    return {};
}

std::string AsControlCharacter(const Codepoint &cp)
{
    if (cp.Value() > 0x10FFFF)
    {
        return {};
    }

    int width = 6;

    if (cp.Value() <= 0xFF)
    {
        width = 2;
    }
    else if (cp.Value() <= 0xFFFF)
    {
        width = 4;
    }

    std::stringstream result;

    result <<
        "\\x" << std::uppercase << std::setw(width) <<
        std::setfill('0') << std::hex << cp.Value();

    return result.str();
}

Codepoint GetUtf8Character(const char *data, int size)
{
    switch (size)
    {
        default:
            [[fallthrough]];
        case 0U:
            [[fallthrough]];
        case 1U:
            return (static_cast<uint32_t>(data[0] & 0x7FU));

        case 2U:
            return (static_cast<uint32_t>(data[0]) & 0x1FU) << 6U |
                    (static_cast<uint32_t>(data[1]) & 0x3FU);

        case 3U:
            return (static_cast<uint32_t>(data[0]) & 0x0FU) << 12U |
                    (static_cast<uint32_t>(data[1]) & 0x3FU) << 6U |
                    (static_cast<uint32_t>(data[2]) & 0x3FU);

        case 4U:
            return (static_cast<uint32_t>(data[0]) & 0x07U) << 18U |
                    (static_cast<uint32_t>(data[1]) & 0x3FU) << 12U |
                    (static_cast<uint32_t>(data[2]) & 0x3FU) << 6U |
                    (static_cast<uint32_t>(data[3]) & 0x3FU);
    }
}

void PrepareConsoleForUtf8()
{
#ifdef _WIN32
    auto hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hOut != INVALID_HANDLE_VALUE)
    {
        DWORD mode = 0;

        if (GetConsoleMode(hOut, &mode))
        {
            // Prepare console to support ESC sequences to set color etc.
            SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }

    // Prepare console to use UTF-8 encoding.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

