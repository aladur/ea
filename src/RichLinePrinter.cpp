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
#include "RichLinePrinter.h"
#include <ostream>
#include <optional>

RichLinePrinter::RichLinePrinter(
        std::ostream &os,
        bool printFilename,
        bool printLineNumber,
        std::optional<std::string> optFilename)
    : ostream_(os)
    , printFilename_(printFilename)
    , printLineNumber_(printLineNumber)
    , filename_(optFilename.value_or(std::string("(standard input)")))
    , currentColor_(Color::RESET)
{
}

void RichLinePrinter::Reset()
{
    lineStream_.clear();
    lineStream_.str("");
}

void RichLinePrinter::PrintLine(bool withColor)
{
    if (true) // TODO filter
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

        if (withColor)
        {
            UpdateColor(Color::RESET);
        }
        ostream_ << lineStream_.str() << "\n";
    }

    Reset();
    ++lineNumber_;

    //hasNonAscii = false;
}

void RichLinePrinter::UpdateColor(const char *color)
{
    if (currentColor_ != color)
    {
        currentColor_ = color;
        lineStream_ << currentColor_;
    }
}
