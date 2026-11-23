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

#include "Codepoint.h"
#include "EncodedScanner.h"
#include <optional>
#include <utility>
#include <string>
#include <unordered_set>
#include <istream>
#include <iostream>
#include <algorithm>

EncodedScanner::EncodedScanner(std::istream &istream,
            const std::string &fallbackEncoding,
            std::unordered_set<Codepoint> excludeUnicodeCodepoints)
    : istream_(istream)
    , fallbackEncoding_(fallbackEncoding)
    , excludeUnicodeCodepoints_(std::move(excludeUnicodeCodepoints))
    , hasExcludes(!excludeUnicodeCodepoints_.empty())
{
}

std::optional<TypedCodepoint> EncodedScanner::GetNext(unsigned &size)
{
    auto diff = size_ - offset_;
    size = 0U;

    if (diff < 4U)
    {
        if (diff > 0U)
        {
            std::copy(buffer_.begin() + offset_, buffer_.begin() + size_,
                      buffer_.begin());
        }
        offset_ = 0U;
        size_ = diff;
        if (!istream_.eof() && !istream_.fail())
        {
            istream_.read(buffer_.data() + diff, buffer_.size() - diff);
            size_ += istream_.gcount();
        }
    }

    // Check for UTF-8 encoded character (with min. 2-byte size)
    const auto optCodepoint = TryGetUtf8CharacterFromBuffer(size, false);
    if (optCodepoint.has_value())
    {
        const auto &codepoint = optCodepoint.value();

        if (!hasExcludes ||
            excludeUnicodeCodepoints_.find(codepoint) ==
            excludeUnicodeCodepoints_.cend())
        {
            offset_ += size;
            return TypedCodepoint{codepoint, EncodingType::Unicode};
        }
    }

    if (offset_ < size_)
    {
        size = 1U;
        return TypedCodepoint{
                Codepoint{static_cast<uint32_t>(buffer_[offset_++] & 0xFFU)},
                EncodingType::Fallback};
    }

    return std::nullopt;
}

std::optional<Codepoint> EncodedScanner::TryGetUtf8CharacterFromBuffer(
        unsigned &size, bool withSingleByte)
{
    const auto diff = size_ - offset_;
    const auto ch1 = diff >= 1U ? buffer_[offset_] : '\0';

    if (withSingleByte && diff >= 1U && (ch1 & 0x80) == 0x00)
    {
        size = 1U;
        return (Codepoint(ch1 & 0x7FU));
    }

    const auto ch2 = diff >= 2U ? buffer_[offset_ + 1U] : '\0';

    if ((ch1 & 0xE0) == 0xC0 && (ch2 & 0xC0) == 0x80)
    {
        size = 2U;
        return Codepoint((static_cast<uint32_t>(ch1) & 0x1FU) << 6U |
                         (static_cast<uint32_t>(ch2) & 0x3FU));
    }

    const auto ch3 = diff >= 3U ? buffer_[offset_ + 2U] : '\0';

    if ((ch1 & 0xF0) == 0xE0 && (ch2 & 0xC0) == 0x80 && (ch3 & 0xC0) == 0x80)
    {
        size = 3U;
        return Codepoint((static_cast<uint32_t>(ch1) & 0x1FU) << 12U |
                         (static_cast<uint32_t>(ch2) & 0x3FU) << 6U |
                         (static_cast<uint32_t>(ch3) & 0x3FU));
    }

    const auto ch4 = diff >= 4U ? buffer_[offset_ + 3U] : '\0';

    if ((ch1 & 0xF8) == 0xF0 && (ch2 & 0xC0) == 0x80 && (ch3 & 0xC0) == 0x80 &&
        (ch4 & 0xC0) == 0x80)
    {
        size = 4U;
        const auto codepoint =
               Codepoint((static_cast<uint32_t>(ch1) & 0x1FU) << 18U |
                         (static_cast<uint32_t>(ch2) & 0x3FU) << 12U |
                         (static_cast<uint32_t>(ch3) & 0x3FU) << 6U |
                         (static_cast<uint32_t>(ch4) & 0x3FU));
        if (codepoint <= Unicode::MAX)
        {
            return codepoint;
        }
    }

    return std::nullopt;
}

