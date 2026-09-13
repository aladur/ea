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

#include "EncodingApplication.h"
#include "HelperFunctions.h"
#include "Definitions.h"
#include "version.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <unicode/unistr.h>
#include <unicode/ustream.h>

namespace po = boost::program_options;
namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    PrepareConsoleForUtf8();

    try
    {
        bool printSummary = false;
        bool printStatistics = false;
        bool printLines = true;
        bool printAll = false;
        bool printLineNumber = false;
        bool printFilename = false;
        std::string colorModeString{"auto"};
        po::options_description desc("Supported options", 80, 58);
        desc.add_options()
            ("help,h", "Output a usage message and exit")
            ("version,V", "Output the version number and exit")
            ("encoding,e",
             po::value<std::string>()->default_value("ISO-8859-1"),
             "Fallback encoding hint (in addition to UTF-8).\n"
             "'list' lists available single byte encodings and exits.\n"
             "'list-a' same as 'list' with aliases.\n"
             "'list-standards' lists available standards and exits.\n"
             "'list-<standard>' lists available single byte encodings\n"
             " for given <standard> and exits.\n"
             "'list-<standard>-a' same as 'list-<standard>' with\n"
             " aliases.\n"
             "Examples:\n"
             "ea -e iso-8859-1\n"
             "ea -e standards\n"
             "ea -e list-iana-a")
            ("count,c", po::bool_switch(&printSummary),
             "Print character count per category")
            ("statistics,s", po::bool_switch(&printStatistics),
             "Print character count for each character in each category")
            ("all,a", po::bool_switch(&printAll),
             "Print file contents, character count and statistics")
            ("color", po::value<std::string>(&colorModeString)
                        ->default_value("never")
                        ->implicit_value("auto"),
             "Set color mode: auto, always, never")
            ("filter,f",
             po::value<std::string>()->default_value("as,un,co,fa,un"),
             "Filter output for list of categories")
            ("with-filename,H", po::bool_switch(&printFilename),
             "Print filename for each line of file content")
            ("line-number,n", po::bool_switch(&printLineNumber),
             "Print line number for each line of file content")
            ("FILE", po::value<std::vector<fs::path>>(),
             "Input file(s) to process");

        po::positional_options_description pos_desc;
        pos_desc.add("FILE", -1);

        po::variables_map vm;
        po::store(
            po::command_line_parser(argc, argv)
                .options(desc)
                .positional(pos_desc)
                .run(),
            vm
        );
        po::notify(vm);

        if (vm.count("version"))
        {
            std::cout << "ea " << PROJECT_VERSION << "\n";
            return 0;
        }

        if (vm.count("help"))
        {
            std::cout <<
                "Usage: ea [OPTIONS]... [FILE]...\n"
                "\n"
                "Analyze a text file for the following character encoding\n"
                "categories\n"
                "\n"
                "- CONTROL characters\n"
                "- ASCII encoded characters\n"
                "- UNICODE encoded characters\n"
                "- Characters encoded in a fallback encoding\n"
                "- Characters with UNKNOWN encoding\n"
                "\n"
                "- Print file content with (optionally) different colors for "
                "each category.\n"
                "- Optionally print character count for each category\n"
                "- Optionally print character count for each character in "
                "each category\n"
                "\n"
                "By applying a filter output is only processed for a subset "
                "of categories.\n"
                "Available categories:\n"
                "\n"
                "    control,ascii,unicode,fallback,unknown\n"
                "\n"
                "The first two characters of a category are sufficient. "
                "For example to filter\n"
                "output for ASCII, UNICODE and fallback encoding, use "
                "--filter as,fa,un.\n"
                "\n";

            std::cout << desc << "\n";
            return 0;
        }

        const auto encoding = vm["encoding"].as<std::string>();
        const auto lcEncoding = boost::to_lower_copy(encoding);
        if (lcEncoding.substr(0, 4) == "list")
        {
            if (lcEncoding == "list-standards")
            {
                OutputAvailableStandards(std::cout);
                return 0;
            }

            std::string standard;
            const bool withAliases =
                (lcEncoding.substr(lcEncoding.size() - 2U, 2U) == "-a");
            if (encoding.size() > 6U)
            {
                const auto offset = withAliases ? 7U : 0U;
                standard =
                    lcEncoding.substr(5U, lcEncoding.size() - offset);
            }
            if (!IsValidStandard(standard))
            {
                std::stringstream messageStream;

                messageStream << "'" << standard << "' is not a valid standard";
                throw std::runtime_error(messageStream.str());
            }

            OutputAvailableEncodings(std::cout, standard, withAliases, true);
            return 0;
        }

        if (printAll)
        {
            printSummary = true;
            printStatistics = true;
            printLines = true;
        }
        else if (printSummary || printStatistics)
        {
            printLines = false;
        }

        std::string error;
        if (!IsValidEncoding(encoding, error))
        {
            std::stringstream messageStream;

            messageStream << "Encoding '" << encoding <<
                "' is unknown";
            throw std::runtime_error(messageStream.str());
        }

        if (!IsSingleByteEncoding(encoding))
        {
            throw std::runtime_error(
                    "Only single byte encodings like ISO-8859-x or "
                    "Windows-125x are supported");
        }

        if (!IsValidColorMode(colorModeString))
        {
            std::stringstream messageStream;

            messageStream << "Color '" << colorModeString <<
                "' is unsupported";
            throw std::runtime_error(messageStream.str());
        }


        if (vm.count("FILE") > 0)
        {
            const auto filePaths = vm["FILE"].as<std::vector<fs::path>>();
            for (const auto &filePath : filePaths)
            {
                const auto status = fs::status(filePath);
                if (!fs::exists(status) || !fs::is_regular_file(status))
                {
                    std::stringstream messageStream;

                    messageStream << "Input file " << filePath <<
                        " does not exist, or is no regular file";
                    throw std::runtime_error(messageStream.str());
                }

                std::ifstream istream(filePath);
                if (!istream.is_open())
                {
                    std::stringstream messageStream;

                    messageStream << "Input file " << filePath <<
                        " cannot be opened";
                    throw std::runtime_error(messageStream.str());
                }

                EncodingApplication app(istream,
                        encoding, printLines, printSummary, printStatistics,
                        printFilename, printLineNumber,
                        ToMode(colorModeString),
                        filePath.filename().string());

                const auto exitCode = app.Run();
                if (exitCode != 0)
                {
                    return exitCode;
                }
            }

            return 0;
        }

        EncodingApplication app(std::cin,
                encoding, printLines, printSummary, printStatistics,
                printFilename, printLineNumber, ToMode(colorModeString));

        return app.Run();

    } catch (const std::exception& e)
    {
        std::cerr << "*** Error: " << e.what() << ".\n";
        return 1;
    }
}
