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

#include "IcuExtensions.h"
#include "Codepoint.h"
#include <cstdint>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <unicode/unistr.h>
#include <unicode/ucnv.h>
#include <unicode/utypes.h>

// Parameter encoding has to be the standard name as returned by
// ucnv_getName().
std::unordered_set<Codepoint> GetInvalidCodepoints(const std::string &encoding)
{
    // Commented out encodings do not have invalid codepoints.
    enum class Encoding : uint8_t
    {
        IBM290, // EBCDIC Japanese Katakana
        IBM420, // EBCDIC Arabic
        IBM424, // EBCDIC Hebrew
        CP720, // Arabic (MS-DOS)
        IBM803, // EBCDIC Hebrew Character Set A
        IBM813, // Greek
        IBM851, // Greek (MS-DOS)
        IBM852, // Greek, DOS-Greek 1 (MS-DOS)
        IBM856, // Hebrew (MS-DOS)
        CP857, // Turkish (MS-DOS)
        IBM864, // Arabic (MS-DOS)
        IBM867, // Hebrew (MS-DOS)
        IBM868, // Pakistan, Urdu (MS-DOS)
        CP869, // Modern Greek (MS-DOS)
        IBM874, // ibm-874, Thai (MS-DOS)
        IBM875, // EBCDIC Greek
        IBM916, // Hebrew
        IBM1051, // HP roman-8
        IBM1098, // Farsi, Persian
        IBM1132, // EBCDIC Lao
        IBM1133, // Lao
        IBM1137, // Devangari, Hindi, Sanskrit, Marathi 
        IBM1162, // Thai
        IBM1253, // Greek
        WIN1253, // Windows Greek
        IBM1255, // Hebrew
        WIN1255, // Windows Hebrew
        IBM1256, // Arabic
        IBM1257, // IBM Baltic
        WIN1257, // Windows Baltic
        IBM1276, // Adobe PostScript
        IBM4517, // EBCDIC Arabic French
        IBM4899, // EBCDIC Hebrew + Euro
        IBM4909, // ISO-8, Greek and Latin + Euro
        IBM4971, // EBCDIC Greek
        IBM5123, // EBCDIC Japanese + Euro
        IBM5351, // Hebrew + Euro
        IBM5352, // Arabic + Euro
        IBM5353, // Baltic + Euro
        IBM8482, // EBCDIC Japanese Katakana
        IBM9067, // EBCDIC Greek 2005 + Euro
        IBM12712, // EBCDIC Hebrew + Euro
        IBM16804, // EBCDIC Arabic + Euro
        ISO_8859_3, // ISO-8859-3, South European
        ISO_8859_6, // ISO-8859-6, Arabic
        ISO_8859_7, // ISO-8859-7, Greek
        ISO_8859_8, // ISO-8859-8, Hebrew
        ISO_8859_11, // ISO-8859-11, Thai
    };
    using Type = std::unordered_map<std::string, Encoding>;

    static const Type invalidCodepointsForEncoding{
        { "ibm-290_P100-1995", Encoding::IBM290 },
        { "ibm-420_X120-1999", Encoding::IBM420 },
        { "ibm-424_P100-1995", Encoding::IBM424 },
        { "ibm-720_P100-1997", Encoding::CP720 },
        { "ibm-803_P100-1999", Encoding::IBM803 },
        { "ibm-813_P100-1995", Encoding::IBM813 },
        { "ibm-851_P100-1995", Encoding::IBM851 },
        { "ibm-852_P100-1995", Encoding::IBM852 },
        { "ibm-856_P100-1995", Encoding::IBM856 },
        { "ibm-857_P100-1995", Encoding::CP857 },
        { "ibm-864_X110-1999", Encoding::IBM864 },
        { "ibm-867_P100-1998", Encoding::IBM867 },
        { "ibm-868_P100-1995", Encoding::IBM868 },
        { "ibm-869_P100-1995", Encoding::CP869 },
        { "ibm-874_P100-1995", Encoding::IBM874 },
        { "ibm-875_P100-1995", Encoding::IBM875 },
        { "ibm-913_P100-2000", Encoding::ISO_8859_3 },
        { "ibm-916_P100-1995", Encoding::IBM916 },
        { "ibm-1051_P100-1995", Encoding::IBM1051 },
        { "ibm-1089_P100-1995", Encoding::ISO_8859_6 },
        { "ibm-1098_P100-1995", Encoding::IBM1098 },
        { "ibm-1132_P100-1998", Encoding::IBM1132 },
        { "ibm-1133_P100-1997", Encoding::IBM1133 },
        { "ibm-1137_P100-1999", Encoding::IBM1137 },
        { "ibm-1162_P100-1999", Encoding::IBM1162 },
        { "ibm-1253_P100-1995", Encoding::IBM1253 },
        { "ibm-1255_P100-1995", Encoding::IBM1255 },
        { "ibm-1256_P110-1997", Encoding::IBM1256 },
        { "ibm-1257_P100-1995", Encoding::IBM1257 },
        { "ibm-1276_P100-1995", Encoding::IBM1276 },
        { "ibm-4517_P100-2005", Encoding::IBM4517 },
        { "ibm-4899_P100-1998", Encoding::IBM4899 },
        { "ibm-4909_P100-1999", Encoding::IBM4909 },
        { "ibm-4971_P100-1999", Encoding::IBM4971 },
        { "ibm-5012_P100-1999", Encoding::ISO_8859_8 },
        { "ibm-5123_P100-1999", Encoding::IBM5123 },
        { "ibm-5349_P100-1998", Encoding::WIN1253 },
        { "ibm-5351_P100-1998", Encoding::IBM5351 },
        { "ibm-5352_P100-1998", Encoding::IBM5352 },
        { "ibm-5353_P100-1998", Encoding::IBM5353 },
        { "ibm-8482_P100-1999", Encoding::IBM8482 },
        { "ibm-9005_X110-2007", Encoding::ISO_8859_7 },
        { "ibm-9067_X100-2005", Encoding::IBM9067 },
        { "ibm-9447_P100-2002", Encoding::WIN1255 },
        { "ibm-9449_P100-2002", Encoding::WIN1257 },
        { "ibm-12712_P100-1998", Encoding::IBM12712 },
        { "ibm-16804_X110-1999", Encoding::IBM16804 },
        { "iso-8859_11-2001", Encoding::ISO_8859_11 },
    };

    const auto iter = invalidCodepointsForEncoding.find(encoding);
    if (iter == invalidCodepointsForEncoding.cend())
    {
        return {};
    }

    switch(iter->second)
    {
        case Encoding::IBM290:
            return {
                0x57, 0x59, 0x6A, 0x9C, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
                0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE1, 0xEA, 0xEB, 0xEC,
                0xED, 0xEE, 0xEF, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE};

        case Encoding::IBM420:
            return {0x53, 0x54, 0xB6, 0xB7, 0xCC, 0xCE, 0xE1, 0xEC, 0xFA};

        case Encoding::IBM424:
            return {
                0x70, 0x72, 0x73, 0x75, 0x76, 0x77, 0x80, 0x8C, 0x8D, 0x8E,
                0x9A, 0x9B, 0x9C, 0x9E, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xCB,
                0xCC, 0xCD, 0xCE, 0xCF, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xEB,
                0xEC, 0xED, 0xEE, 0xEF, 0xFB, 0xFC, 0xFD, 0xFE};

        case Encoding::CP720:
            return {0x80, 0x81, 0x84, 0x86, 0x8D, 0x8E, 0x8F, 0x90};

        case Encoding::IBM803:
            return {
                0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x51,
                0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x62, 0x63,
                0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x70, 0x71, 0x72,
                0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x80, 0x8A, 0x8B,
                0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E,
                0x9F, 0xA0, 0xA1, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0,
                0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA,
                0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xCA, 0xCB, 0xCC, 0xCD,
                0xCE, 0xCF, 0xD0, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0,
                0xE1, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xFA, 0xFB, 0xFC,
                0xFD, 0xFE};

        case Encoding::IBM813:
            return {0xA4, 0xA5, 0xAA, 0xAE, 0xD2, 0xFF};

        case Encoding::IBM851:
            return {0x91};

        case Encoding::IBM852:
            return {0xAA};

        case Encoding::IBM856:
            return {
                0x9B, 0x9D, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
                0xA7, 0xA8, 0xAD, 0xB5, 0xB6, 0xB7, 0xC6, 0xC7, 0xD0, 0xD1,
                0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xDE, 0xE0, 0xE1,
                0xE2, 0xE3, 0xE4, 0xE5, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC,
                0xED};

        case Encoding::CP857:
            return {0xD5, 0xE7, 0xF2};

        case Encoding::IBM864:
            return {0x9B, 0x9C, 0xA6, 0xA7, 0xFF};

        case Encoding::IBM867:
            return {0x9E, 0xA7, 0xA8};

        case Encoding::IBM868:
            return {0xFD};

        case Encoding::CP869:
            return {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x87, 0x93, 0x94};

        case Encoding::IBM874:
            return {
                0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
                0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93,
                0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D,
                0x9E, 0x9F};

        case Encoding::IBM875:
            return {0xDC, 0xE1, 0xEC, 0xED, 0xFC, 0xFD};

        case Encoding::IBM916:
            return {
                0xA1, 0xBF, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
                0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1,
                0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB,
                0xDC, 0xDD, 0xDE, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF};

        case Encoding::IBM1051:
            return {0xA0, 0xFF};

        case Encoding::IBM1098:
            return {0x80, 0x81, 0xCF};

        case Encoding::IBM1132:
            return {
                0x51, 0x71, 0x78, 0x80, 0x8A, 0x8B, 0x90, 0xA0, 0xAE, 0xAF,
                0xBA, 0xCA, 0xDC, 0xDF, 0xE1, 0xEA, 0xEB, 0xEC, 0xED, 0xEE,
                0xEF, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE};

        case Encoding::IBM1133:
            return {
                0xA0, 0xBC, 0xBD, 0xBE, 0xCD, 0xCE, 0xCF, 0xDC, 0xE0, 0xE1,
                0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB,
                0xEC, 0xED, 0xEE, 0xEF, 0xFA, 0xFB};

        case Encoding::IBM1137:
            return {0xCE, 0xCF};

        case Encoding::IBM1162:
            return {0xDB, 0xDC, 0xDD, 0xDE, 0xFC, 0xFD, 0xFE, 0xFF};

        case Encoding::IBM1253:
            [[fallthrough]];
        case Encoding::WIN1253:
            return {0xD2, 0xFF};

        case Encoding::IBM1255:
            return {
                0xA1, 0xAA, 0xB8, 0xBA, 0xBF, 0xCA, 0xD7, 0xD8, 0xD9, 0xDA,
                0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xFB, 0xFC, 0xFF};

        case Encoding::WIN1255:
            return {
                0xCA, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xFB, 0xFC,
                0xFF};

        case Encoding::IBM1256:
            return {0xAA, 0xC0, 0xFF};

        case Encoding::IBM1257:
            [[fallthrough]];
        case Encoding::IBM5353:
            return {0xA1, 0xA5, 0xB4, 0xFF};

        case Encoding::WIN1257:
            return {0xA1, 0xA5};

        case Encoding::IBM1276:
            return {
                0xA0, 0xB0, 0xB5, 0xBE, 0xC0, 0xC9, 0xCC, 0xD1, 0xD2, 0xD3,
                0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD,
                0xDE, 0xDF, 0xE0, 0xE2, 0xE4, 0xE5, 0xE6, 0xE7, 0xEC, 0xED,
                0xEE, 0xEF, 0xF0, 0xF2, 0xF3, 0xF4, 0xF6, 0xF7, 0xFC, 0xFD,
                0xFE, 0xFF};

        case Encoding::IBM4517:
            return {
                0x53, 0x54, 0xB6, 0xB7, 0xCC, 0xCE, 0xDF, 0xEC, 0xFA, 0xFB,
                0xFC, 0xFD, 0xFE};

        case Encoding::IBM4899:
            return {
                0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x51,
                0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x62, 0x63,
                0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x70, 0x71, 0x72,
                0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x80, 0x8A, 0x8B,
                0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x9A, 0x9B, 0x9D, 0x9F, 0xA0,
                0xA1, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2,
                0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC,
                0xBD, 0xBE, 0xBF, 0xC0, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
                0xD0, 0xDA, 0xDE, 0xDF, 0xE0, 0xE1, 0xEA, 0xEB, 0xEC, 0xED,
                0xEE, 0xEF, 0xFA};

        case Encoding::IBM4909:
            return {0xA5, 0xAA, 0xAE, 0xD2, 0xFF};

        case Encoding::IBM4971:
            return {0xDC, 0xE1, 0xEC, 0xED, 0xFD};

        case Encoding::IBM5123:
            return {
                0x41, 0x6A, 0x80, 0x90, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
                0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xEA, 0xEB, 0xEC, 0xED,
                0xEE, 0xEF, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE};

        case Encoding::IBM5351:
            return {
                0xA1, 0xAA, 0xB8, 0xBA, 0xBF, 0xCA, 0xD7, 0xD8, 0xD9, 0xDA,
                0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xFB, 0xFC, 0xFF};

        case Encoding::IBM5352:
            return {0xAA, 0xC0, 0xFF};

        case Encoding::IBM8482:
            return {
                0x57, 0x59, 0x6A, 0x9C, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
                0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xEA, 0xEB, 0xEC, 0xED,
                0xEE, 0xEF, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE};

        case Encoding::IBM9067:
            return {0xDC, 0xED, 0xFD};

        case Encoding::IBM12712:
            return {
                0x70, 0x72, 0x73, 0x75, 0x76, 0x77, 0x80, 0x8C, 0x8D, 0x8E,
                0x9A, 0x9B, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xCB, 0xCC, 0xCD,
                0xCE, 0xCF, 0xDE, 0xDF, 0xEB, 0xEC, 0xED, 0xEE, 0xEF};

        case Encoding::IBM16804:
            return {0x53, 0x54, 0xB6, 0xB7, 0xCC, 0xCE, 0xEC};

        case Encoding::ISO_8859_3:
            return {0xA5, 0xAE, 0xBE, 0xC3, 0xD0, 0xE3, 0xF0};

        case Encoding::ISO_8859_6:
            return {0xAE, 0xD2, 0xFF};

        case Encoding::ISO_8859_7:
            return {
                0xA1, 0xA2, 0xA3, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB,
                0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
                0xB8, 0xB9, 0xBA, 0xBC, 0xBD, 0xBE, 0xC0, 0xDB, 0xDC, 0xDD,
                0xDE, 0xDF, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA,
                0xFB, 0xFC, 0xFD, 0xFE, 0xFF};

        case Encoding::ISO_8859_8:
            return {0xA1, 0xFB, 0xFC, 0xFF};

        case Encoding::ISO_8859_11:
            return {0xDB, 0xDC, 0xDD, 0xDE, 0xFC, 0xFD, 0xFE, 0xFF};
    }

    return {};
}

// Parameter encoding has to be the standard name as returned by
// ucnv_getName().
bool HasC1ControlsDefined(const std::string &encoding)
{
    static const std::unordered_set<std::string> encodingsWithC1Controls{
        "ISO-8859-1", // ISO-8859-1
        "ibm-912_P100-1995", // ISO-8859-2
        "ibm-913_P100-2000", // ISO-8859-3
        "ibm-914_P100-1995", // ISO-8859-4
        "ibm-915_P100-1995", // ISO-8859-5
        "ibm-1089_P100-1995", // ISO-8859-6
        "ibm-9005_X110-2007", // ISO-8859-7
        "ibm-5012_P100-1999", // ISO-8859-8
        "ibm-920_P100-1995", // ISO-9859-9
        "iso-8859_10-1998", // ISO-9859-10
        "iso-8859_11-2001", // ISO-9859-11
        "ibm-921_P100-1995", // ISO-9859-13
        "iso-8859_14-1998", // ISO-9859-14
        "ibm-923_P100-1998", // ISO-9859-15
    };

    return encodingsWithC1Controls.find(encoding) !=
           encodingsWithC1Controls.end();
}

std::string GetFamily(const std::string &encoding)
{
    static const char * const EBCDIC = "EBCDIC";
    static const char * const ASCII = "ASCII";

    using Type = std::unordered_map<std::string, const char *>;

    Type familyForEncoding{
        { "ebcdic-xml-us", EBCDIC },
        { "ibm-37_P100-1995", EBCDIC },
        { "gsm-03.38-2009", ASCII },
        { "ibm-273_P100-1995", EBCDIC },
        { "ibm-277_P100-1995", EBCDIC },
        { "ibm-278_P100-1995", EBCDIC },
        { "ibm-280_P100-1995", EBCDIC },
        { "ibm-284_P100-1995", EBCDIC },
        { "ibm-285_P100-1995", EBCDIC },
        { "ibm-290_P100-1995", EBCDIC },
        { "ibm-297_P100-1995", EBCDIC },
        { "ibm-420_X120-1999", EBCDIC },
        { "ibm-424_P100-1995", EBCDIC },
        { "ibm-437_P100-1995", ASCII },
        { "ibm-500_P100-1995", EBCDIC },
        { "ibm-720_P100-1997", ASCII },
        { "ibm-737_P100-1997", ASCII },
        { "ibm-775_P100-1996", ASCII },
        { "ibm-803_P100-1999", EBCDIC },
        { "ibm-813_P100-1995", ASCII },
        { "ibm-838_P100-1995", EBCDIC },
        { "ibm-850_P100-1995", ASCII },
        { "ibm-851_P100-1995", ASCII },
        { "ibm-852_P100-1995", ASCII },
        { "ibm-855_P100-1995", ASCII },
        { "ibm-856_P100-1995", ASCII },
        { "ibm-857_P100-1995", ASCII },
        { "ibm-858_P100-1997", ASCII },
        { "ibm-860_P100-1995", ASCII },
        { "ibm-861_P100-1995", ASCII },
        { "ibm-862_P100-1995", ASCII },
        { "ibm-863_P100-1995", ASCII },
        { "ibm-864_X110-1999", ASCII },
        { "ibm-865_P100-1995", ASCII },
        { "ibm-866_P100-1995", ASCII },
        { "ibm-867_P100-1998", ASCII },
        { "ibm-868_P100-1995", ASCII },
        { "ibm-869_P100-1995", ASCII },
        { "ibm-870_P100-1995", EBCDIC },
        { "ibm-871_P100-1995", EBCDIC },
        { "ibm-874_P100-1995", ASCII },
        { "ibm-875_P100-1995", EBCDIC },
        { "ibm-878_P100-1996", ASCII },
        { "ibm-901_P100-1999", ASCII },
        { "ibm-902_P100-1999", ASCII },
        { "ibm-912_P100-1995", ASCII },
        { "ibm-913_P100-2000", ASCII },
        { "ibm-914_P100-1995", ASCII },
        { "ibm-915_P100-1995", ASCII },
        { "ibm-916_P100-1995", ASCII },
        { "ibm-918_P100-1995", EBCDIC },
        { "ibm-920_P100-1995", ASCII },
        { "ibm-921_P100-1995", ASCII },
        { "ibm-922_P100-1999", ASCII },
        { "ibm-923_P100-1998", ASCII },
        { "ibm-1006_P100-1995", ASCII },
        { "ibm-1025_P100-1995", EBCDIC },
        { "ibm-1026_P100-1995", EBCDIC },
        { "ibm-1047_P100-1995", EBCDIC },
        { "ibm-1051_P100-1995", ASCII },
        { "ibm-1089_P100-1995", ASCII },
        { "ibm-1097_P100-1995", EBCDIC },
        { "ibm-1098_P100-1995", ASCII },
        { "ibm-1112_P100-1995", EBCDIC },
        { "ibm-1122_P100-1999", EBCDIC },
        { "ibm-1123_P100-1995", EBCDIC },
        { "ibm-1124_P100-1996", ASCII },
        { "ibm-1125_P100-1997", ASCII },
        { "ibm-1129_P100-1997", ASCII },
        { "ibm-1130_P100-1997", EBCDIC },
        { "ibm-1131_P100-1997", ASCII },
        { "ibm-1132_P100-1998", EBCDIC },
        { "ibm-1133_P100-1997", ASCII },
        { "ibm-1137_P100-1999", EBCDIC },
        { "ibm-1140_P100-1997", EBCDIC },
        { "ibm-1141_P100-1997", EBCDIC },
        { "ibm-1142_P100-1997", EBCDIC },
        { "ibm-1143_P100-1997", EBCDIC },
        { "ibm-1144_P100-1997", EBCDIC },
        { "ibm-1145_P100-1997", EBCDIC },
        { "ibm-1146_P100-1997", EBCDIC },
        { "ibm-1147_P100-1997", EBCDIC },
        { "ibm-1148_P100-1997", EBCDIC },
        { "ibm-1149_P100-1997", EBCDIC },
        { "ibm-1153_P100-1999", EBCDIC },
        { "ibm-1154_P100-1999", EBCDIC },
        { "ibm-1155_P100-1999", EBCDIC },
        { "ibm-1156_P100-1999", EBCDIC },
        { "ibm-1157_P100-1999", EBCDIC },
        { "ibm-1158_P100-1999", EBCDIC },
        { "ibm-1160_P100-1999", EBCDIC },
        { "ibm-1162_P100-1999", ASCII },
        { "ibm-1164_P100-1999", EBCDIC },
        { "ibm-1168_P100-2002", ASCII },
        { "ibm-1250_P100-1995", ASCII },
        { "ibm-1251_P100-1995", ASCII },
        { "ibm-1252_P100-2000", ASCII },
        { "ibm-1253_P100-1995", ASCII },
        { "ibm-1254_P100-1995", ASCII },
        { "ibm-1255_P100-1995", ASCII },
        { "ibm-1256_P110-1997", ASCII },
        { "ibm-1257_P100-1995", ASCII },
        { "ibm-1258_P100-1997", ASCII },
        { "ibm-1276_P100-1995", ASCII },
        { "ibm-4517_P100-2005", EBCDIC },
        { "ibm-4899_P100-1998", EBCDIC },
        { "ibm-4909_P100-1999", ASCII },
        { "ibm-4971_P100-1999", EBCDIC },
        { "ibm-5012_P100-1999", ASCII },
        { "ibm-5123_P100-1999", EBCDIC },
        { "ibm-5346_P100-1998", ASCII },
        { "ibm-5347_P100-1998", ASCII },
        { "ibm-5348_P100-1997", ASCII },
        { "ibm-5349_P100-1998", ASCII },
        { "ibm-5350_P100-1998", ASCII },
        { "ibm-5351_P100-1998", ASCII },
        { "ibm-5352_P100-1998", ASCII },
        { "ibm-5353_P100-1998", ASCII },
        { "ibm-5354_P100-1998", ASCII },
        { "ibm-8482_P100-1999", EBCDIC },
        { "ibm-9005_X110-2007", ASCII },
        { "ibm-9067_X100-2005", EBCDIC },
        { "ibm-9447_P100-2002", ASCII },
        { "ibm-9448_X100-2005", ASCII },
        { "ibm-9449_P100-2002", ASCII },
        { "ibm-12712_P100-1998", EBCDIC },
        { "ibm-16804_X110-1999", EBCDIC },
        { "iso-8859_10-1998", ASCII },
        { "iso-8859_11-2001", ASCII },
        { "iso-8859_14-1998", ASCII },
        { "macos-0_2-10.2", ASCII },
        { "macos-29-10.2", ASCII },
        { "macos-35-10.2", ASCII },
        { "macos-6_2-10.4", ASCII },
        { "macos-7_3-10.2", ASCII },
        { "windows-874-2000", ASCII },
    };

    const auto iter = familyForEncoding.find(encoding);
    return iter == familyForEncoding.end() ?  "" : iter->second;
}
