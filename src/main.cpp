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
#include <optional>
#include <utility>
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
        bool printLineNumber = false;
        bool printFilename = false;
        bool printCategories = false;
        bool isPrintUnicodeCodepoints = false;
        std::string colorModeString;
        std::string outputBomString;
        std::string excludeUnicodeCodepointsString;
        std::string filter;
        std::optional<fs::path> optOutputFilename;
        po::options_description desc("Supported options", 80, 58);
        desc.add_options()
            ("help,h", "Output a usage message and exit")
            ("version,V", "Output the version number and exit")
            ("encoding,e",
             po::value<std::string>()->default_value("none"),
             "Fallback encoding hint (in addition to UTF-8):\n"
             "'none' means no fallback encoding.\n"
             "'list' lists available single byte encodings and exit.\n"
             "'list-a' same as 'list' with aliases.\n"
             "'list-standards' lists available standards and exit.\n"
             "'list-<standard>' lists available single byte encodings\n"
             " for given <standard> and exit.\n"
             "'list-<standard>-a' same as 'list-<standard>' with "
             " aliases.\n"
             "Examples:\n"
             "ea -e iso-8859-1; ea -e standards; ea -e list-iana-a")
            ("count,c", po::bool_switch(&printSummary),
             "Suppress normal output, instead print character counts per "
             "category")
            ("statistics,s", po::bool_switch(&printStatistics),
             "Suppress normal output, instead print character counts per "
             "character within each category")
            ("color", po::value<std::string>(&colorModeString)
                        ->default_value("auto")
                        ->implicit_value("auto"),
             "Set color mode: 'always', 'never', 'auto'; 'auto' only "
             "emits color codes when standard output is connected to a "
             "terminal")
            ("filter,f",
             po::value<std::string>(&filter)->default_value("c,a,u,f,i,e"),
             "Filter output by categories (comma-separated list of "
             "each category's first letter); Available: control,ascii,"
             "unicode,fallback,indeterminate,empty. "
             "'empty' is a pseudo-category that matches blank lines.\n"
             "Example: Print lines containing control, unicode or fallback: "
             "ea -f c,u,f")
            ("with-filename,H", po::bool_switch(&printFilename),
             "Print filename with output lines of file content or statistics")
            ("line-number,n", po::bool_switch(&printLineNumber),
             "Print line numbers with output lines of file content")
            ("categories-per-line,C", po::bool_switch(&printCategories),
             "Print categories found per line of file content")
            ("output-file,o", po::value<std::string>(),
             "Write a converted UTF-8 file")
            ("output-bom,b", po::value<std::string>(&outputBomString)
                        ->default_value("as_input")
                        ->implicit_value("yes"),
             "Output file BOM mode: 'yes', 'no', 'as_input';\n"
             "'as_input' uses the BOM of the first input file, if present")
            ("exclude-unicode",
             po::value<std::string>(&excludeUnicodeCodepointsString),
             "Exclude a comma-separated list of Unicode codepoints from "
             "Unicode treatment; treat them instead as fallback or "
             "indeterminate. If the argument is 'list', print all "
             "found Unicode codepoints on one line and exit.\n"
             "Examples:\n"
             "ea --exclude-unicode U+FC,U+58A,U+2665,U+1d191\n"
             "ea --exclude-unicode list")
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
            std::cout << "ea " << PROJECT_VERSION_FULL << "\n";
            return 0;
        }

        if (vm.count("help"))
        {
            std::cout <<
                "Usage: ea [OPTIONS]... [FILE]...\n"
                "Analyze each FILE or standard input for character encoding "
                "categories:\n"
                "- CONTROL characters (excluding newlines)\n"
                "- ASCII encoded characters\n"
                "- UNICODE encoded characters\n"
                "- (optionally) Characters encoded in a FALLBACK encoding\n"
                "- Characters with INDETERMINATE encoding\n"
                "\n";

            std::cout << desc;
            return 0;
        }

        const auto encoding = vm["encoding"].as<std::string>();
        std::string invalidCategory;
        const auto categoryFilter = ToCategories(filter, invalidCategory);
        isPrintUnicodeCodepoints = (excludeUnicodeCodepointsString == "list");
        std::string invalidUnicodeExcludes;
        const auto excludeUnicodeCodepoints = ToUnicodeCodepoints(
                excludeUnicodeCodepointsString, invalidUnicodeExcludes);
        if (vm.count("output-file"))
        {
            optOutputFilename = vm["output-file"].as<std::string>();
        }

        if (!optOutputFilename.has_value() && !vm["output-bom"].defaulted())
        {
            throw std::runtime_error("--output-bom only can be used "
                "together with --output-file");
        }
        const auto outputBomMode = ToOutputBomMode(outputBomString);

        if (!invalidCategory.empty())
        {
            throw std::runtime_error("Undefined filter category '" +
                    invalidCategory + "'");
        }
        if (!invalidUnicodeExcludes.empty())
        {
            throw std::runtime_error("Got invalid unicode excludes '" +
                    invalidUnicodeExcludes + "'");
        }
        const auto colorMode = ToMode(colorModeString);
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

        if (isPrintUnicodeCodepoints)
        {
            printSummary = false;
            printStatistics = false;
            printLines = false;
        }
        else if (printSummary || printStatistics)
        {
            using Type = std::vector<std::pair<bool, const char *>>;

            static const bool filterNotDefaulted = !vm["filter"].defaulted();
            static const Type stateWithOption{
                { printCategories, "--categories-per-line" },
                { filterNotDefaulted, "--filter" },
                { printLineNumber, "--line-number" },
                { printFilename, "--with-filename" },
            };

            for (const auto &item : stateWithOption)
            {
                if (item.first)
                {
                    throw std::runtime_error(std::string("Option ") +
                        item.second + " cannot be used "
                        "with --count or --statistics");
                }
            }

            printLines = false;
        }

        std::string error;
        if (boost::to_lower_copy(encoding) != "none")
        {
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

                std::ifstream istream(filePath, std::ios::binary);
                if (!istream.is_open())
                {
                    std::stringstream messageStream;

                    messageStream << "Input file " << filePath <<
                        " cannot be opened";
                    throw std::runtime_error(messageStream.str());
                }

                EncodingApplication app(istream,
                        encoding, printLines, printSummary, printStatistics,
                        printFilename, printLineNumber, printCategories,
                        isPrintUnicodeCodepoints,
                        colorMode, categoryFilter, outputBomMode,
                        std::move(excludeUnicodeCodepoints),
                        filePath.filename().string(),
                        optOutputFilename);

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
                printFilename, printLineNumber, printCategories,
                isPrintUnicodeCodepoints,
                colorMode, categoryFilter, outputBomMode,
                std::move(excludeUnicodeCodepoints),
                std::nullopt, optOutputFilename);

        return app.Run();

    } catch (const std::exception& e)
    {
        std::cerr << "*** Error: " << e.what() << ".\n";
        return 1;
    }
}
