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

#include <utility>
#include <string>
#include <sstream>
#include <ostream>
#include <optional>

class RichLinePrinter
{
public:
    RichLinePrinter() = delete;
    RichLinePrinter(
            std::ostream &os,
            bool printFilename,
            bool printLineNumber,
            std::optional<std::string> optFilename = std::nullopt);
    RichLinePrinter(std::ostream&& stream) = delete;

    void Reset();
    void PrintLine(bool withColor);
    void UpdateColor(const char *color);

    template<typename T>
    RichLinePrinter& operator<<(T&& value)
    {
        lineStream_ << std::forward<T>(value);
        return *this;
    }

private:
    std::ostream &ostream_;
    std::stringstream lineStream_;
    bool printFilename_{};
    bool printLineNumber_{};
    std::string filename_;
    std::size_t lineNumber_{1U};
    const char *currentColor_{};
};

#endif

