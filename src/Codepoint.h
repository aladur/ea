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

#ifndef CODEPOINT_INCLUDED
#define CODEPOINT_INCLUDED

#include <cstdint>
#include <iomanip>
#include <ostream>
#include <unicode/uchar.h>

class Codepoint
{
public:
    Codepoint(uint32_t value = 0U) noexcept
      : value_(value)
    {
    }

    constexpr uint32_t Value() const noexcept
    {
        return value_;
    }

    explicit constexpr operator uint32_t() const noexcept
    {
        return value_;
    }

    constexpr operator UChar32() const noexcept
    {
        return value_;
    }

    bool operator< (const Codepoint &cp) const noexcept
    {
        return value_ < cp.value_;
    }

    bool operator== (const Codepoint &cp) const noexcept
    {
        return value_ == cp.value_;
    }

private:
    uint32_t value_{};
};

inline std::ostream& operator<<(std::ostream& os, const Codepoint& cp)
{
    const auto width = (cp.Value() > 0xFFFFU) ? 6U : 4U;

    os << "U+" << std::uppercase << std::setw(width) << std::setfill('0') <<
        std::hex << cp.Value();

    return os;
}

namespace std {
    template <>
    struct hash<Codepoint> {
        std::size_t operator()(const Codepoint& cp) const noexcept {
            return std::hash<std::uint64_t>{}(cp.Value());
        }
    };
}

#endif

