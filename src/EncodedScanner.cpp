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
#include "HelperFunctions.h"
#include <optional>
#include <string>
#include <istream>
#include <iostream>

EncodedScanner::EncodedScanner(std::istream &istream,
            const std::string &defaultEncoding)
    : istream_(istream)
    , defaultEncoding_(defaultEncoding)
{
}

std::optional<TypedCodepoint> EncodedScanner::GetNext(unsigned &size)
{
    auto CreateResultFct = [&](unsigned size){
        TypedCodepoint result;
        result.codepoint = GetUtf8CharacterFromBuffer(size);
        auto category = u_charType(result.codepoint);
        result.type = (category == U_CONTROL_CHAR) ? EncodingType::Control :
            ((result.codepoint < Codepoint(0x7FU)) ?
             EncodingType::Ascii : EncodingType::Unicode);
        buffer_.erase(buffer_.begin(), buffer_.begin() + size);

        return result;
    };
    size = 0U;

    if (IsUtf8CharacterInBuffer(size))
    {
        return CreateResultFct(size);
    }

    if (!istream_.eof() && !istream_.fail())
    {
        for (auto i = buffer_.size(); i <= 4; ++i)
        {
            auto data = istream_.get();
            ++filePosition_;
            if (istream_.eof() || istream_.fail())
            {
                break;
            }
            buffer_.push_back(static_cast<uint8_t>(data));
            if (IsUtf8CharacterInBuffer(size))
            {
                return CreateResultFct(size);
            }
        }
    }

    if (!buffer_.empty())
    {
        TypedCodepoint result;
        result.codepoint = buffer_.front();
        result.type = GetCodepointType(result.codepoint, defaultEncoding_);
        buffer_.erase(buffer_.begin());
        size = 1U;
        return result;
    }

    return std::nullopt;
}

bool EncodedScanner::IsUtf8CharacterInBuffer(unsigned &size) const
{
    bool isUtf8 = false;

    for (size = 1U; size <= static_cast<unsigned>(buffer_.size()); ++size)
    {
        switch (size)
        {
            default:
                [[fallthrough]];
            case 0U:
                isUtf8 = false;
                break;

            case 1U:
                isUtf8 = ((buffer_[0] & 0x80) == 0x00);
                break;

            case 2U:
                isUtf8 = ((buffer_[0] & 0xE0) == 0xC0 &&
                          (buffer_[1] & 0xC0) == 0x80);
                break;

            case 3U:
                isUtf8 = ((buffer_[0] & 0xF0) == 0xE0 &&
                          (buffer_[1] & 0xC0) == 0x80 &&
                          (buffer_[2] & 0xC0) == 0x80);
                break;

            case 4U:
                isUtf8 = ((buffer_[0] & 0xF8) == 0xF0 &&
                          (buffer_[1] & 0xC0) == 0x80 &&
                          (buffer_[2] & 0xC0) == 0x80 &&
                          (buffer_[3] & 0xC0) == 0x80);
                break;
        }

        if (isUtf8)
        {
            return true;
        }
    }

    return false;
}

Codepoint EncodedScanner::GetUtf8CharacterFromBuffer(unsigned size) const
{
    switch (size)
    {
        default:
            [[fallthrough]];
        case 0U:
            [[fallthrough]];
        case 1U:
            return (static_cast<uint32_t>(buffer_[0] & 0x7FU));

        case 2U:
            return (static_cast<uint32_t>(buffer_[0]) & 0x1FU) << 6U |
                    (static_cast<uint32_t>(buffer_[1]) & 0x3FU);

        case 3U:
            return (static_cast<uint32_t>(buffer_[0]) & 0x0FU) << 12U |
                    (static_cast<uint32_t>(buffer_[1]) & 0x3FU) << 6U |
                    (static_cast<uint32_t>(buffer_[2]) & 0x3FU);

        case 4U:
            return (static_cast<uint32_t>(buffer_[0]) & 0x07U) << 18U |
                    (static_cast<uint32_t>(buffer_[1]) & 0x3FU) << 12U |
                    (static_cast<uint32_t>(buffer_[2]) & 0x3FU) << 6U |
                    (static_cast<uint32_t>(buffer_[3]) & 0x3FU);
    }
}

