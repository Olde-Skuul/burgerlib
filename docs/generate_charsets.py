#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Generate the charset tables for Burgerlib documentation.
"""

# pylint: disable=unused-argument
# pylint: disable=consider-using-f-string

from __future__ import absolute_import, print_function, unicode_literals

import sys
import os

from burger import create_folder_if_needed, save_text_file_if_newer, \
    is_string, escape_xml_cdata

#
# Directory this script resides in
#

WORKING_DIRECTORY = os.path.dirname(os.path.abspath(__file__))

#
# Unicode descriptions for all valid characters
#

UNICODE_DESCRIPTIONS = {
    0x0000: ("NULL", "NUL"),
    0x0001: ("START OF HEADING", "SOH"),
    0x0002: ("START OF TEXT", "STX"),
    0x0003: ("END OF TEXT", "ETX"),
    0x0004: ("END OF TRANSMISSION", "EOT"),
    0x0005: ("ENQUIRY", "ENQ"),
    0x0006: ("ACKNOWLEDGE", "ACK"),
    0x0007: ("BELL", "BEL"),
    0x0008: ("BACKSPACE", "BS"),
    0x0009: ("CHARACTER TABULATION", "HT"),
    0x000A: ("LINE FEED (LF)", "LF"),
    0x000B: ("LINE TABULATION", "VT"),
    0x000C: ("FORM FEED (FF)", "FF"),
    0x000D: ("CARRIAGE RETURN (CR)", "CR"),
    0x000E: ("SHIFT OUT", "SO"),
    0x000F: ("SHIFT IN", "SI"),

    0x0010: ("DATA LINK ESCAPE", "DLE"),
    0x0011: ("DEVICE CONTROL ONE", "DC1"),
    0x0012: ("DEVICE CONTROL TWO", "DC2"),
    0x0013: ("DEVICE CONTROL THREE", "DC3"),
    0x0014: ("DEVICE CONTROL FOUR", "DC4"),
    0x0015: ("NEGATIVE ACKNOWLEDGE", "NAK"),
    0x0016: ("SYNCHRONOUS IDLE", "SYN"),
    0x0017: ("END OF TRANSMISSION BLOCK", "ETB"),
    0x0018: ("CANCEL", "CAN"),
    0x0019: ("END OF MEDIUM", "EM"),
    0x001A: ("SUBSTITUTE", "SUB"),
    0x001B: ("ESCAPE", "ESC"),
    0x001C: ("INFORMATION SEPARATOR FOUR", "FS"),
    0x001D: ("INFORMATION SEPARATOR THREE", "GS"),
    0x001E: ("INFORMATION SEPARATOR TWO", "RS"),
    0x001F: ("INFORMATION SEPARATOR ONE", "US"),

    0x0020: "SPACE",
    0x0021: "EXCLAMATION MARK",
    0x0022: "QUOTATION MARK",
    0x0023: "NUMBER SIGN",
    0x0024: "DOLLAR SIGN",
    0x0025: "PERCENT SIGN",
    0x0026: "AMPERSAND",
    0x0027: "APOSTROPHE",
    0x0028: "LEFT PARENTHESIS",
    0x0029: "RIGHT PARENTHESIS",
    0x002A: "ASTERISK",
    0x002B: "PLUS SIGN",
    0x002C: "COMMA",
    0x002D: "HYPHEN-MINUS",
    0x002E: "FULL STOP",
    0x002F: "SOLIDUS",

    0x0030: "DIGIT ZERO",
    0x0031: "DIGIT ONE",
    0x0032: "DIGIT TWO",
    0x0033: "DIGIT THREE",
    0x0034: "DIGIT FOUR",
    0x0035: "DIGIT FIVE",
    0x0036: "DIGIT SIX",
    0x0037: "DIGIT SEVEN",
    0x0038: "DIGIT EIGHT",
    0x0039: "DIGIT NINE",
    0x003A: "COLON",
    0x003B: "SEMICOLON",
    0x003C: "LESS-THAN SIGN",
    0x003D: "EQUALS SIGN",
    0x003E: "GREATER-THAN SIGN",
    0x003F: "QUESTION MARK",

    0x0040: "COMMERCIAL AT",
    0x0041: "LATIN CAPITAL LETTER A",
    0x0042: "LATIN CAPITAL LETTER B",
    0x0043: "LATIN CAPITAL LETTER C",
    0x0044: "LATIN CAPITAL LETTER D",
    0x0045: "LATIN CAPITAL LETTER E",
    0x0046: "LATIN CAPITAL LETTER F",
    0x0047: "LATIN CAPITAL LETTER G",
    0x0048: "LATIN CAPITAL LETTER H",
    0x0049: "LATIN CAPITAL LETTER I",
    0x004A: "LATIN CAPITAL LETTER J",
    0x004B: "LATIN CAPITAL LETTER K",
    0x004C: "LATIN CAPITAL LETTER L",
    0x004D: "LATIN CAPITAL LETTER M",
    0x004E: "LATIN CAPITAL LETTER N",
    0x004F: "LATIN CAPITAL LETTER O",

    0x0050: "LATIN CAPITAL LETTER P",
    0x0051: "LATIN CAPITAL LETTER Q",
    0x0052: "LATIN CAPITAL LETTER R",
    0x0053: "LATIN CAPITAL LETTER S",
    0x0054: "LATIN CAPITAL LETTER T",
    0x0055: "LATIN CAPITAL LETTER U",
    0x0056: "LATIN CAPITAL LETTER V",
    0x0057: "LATIN CAPITAL LETTER W",
    0x0058: "LATIN CAPITAL LETTER X",
    0x0059: "LATIN CAPITAL LETTER Y",
    0x005A: "LATIN CAPITAL LETTER Z",
    0x005B: "LEFT SQUARE BRACKET",
    0x005C: "REVERSE SOLIDUS",
    0x005D: "RIGHT SQUARE BRACKET",
    0x005E: "CIRCUMFLEX ACCENT",
    0x005F: "LOW LINE",

    0x0060: "GRAVE ACCENT",
    0x0061: "LATIN SMALL LETTER A",
    0x0062: "LATIN SMALL LETTER B",
    0x0063: "LATIN SMALL LETTER C",
    0x0064: "LATIN SMALL LETTER D",
    0x0065: "LATIN SMALL LETTER E",
    0x0066: "LATIN SMALL LETTER F",
    0x0067: "LATIN SMALL LETTER G",
    0x0068: "LATIN SMALL LETTER H",
    0x0069: "LATIN SMALL LETTER I",
    0x006A: "LATIN SMALL LETTER J",
    0x006B: "LATIN SMALL LETTER K",
    0x006C: "LATIN SMALL LETTER L",
    0x006D: "LATIN SMALL LETTER M",
    0x006E: "LATIN SMALL LETTER N",
    0x006F: "LATIN SMALL LETTER O",

    0x0070: "LATIN SMALL LETTER P",
    0x0071: "LATIN SMALL LETTER Q",
    0x0072: "LATIN SMALL LETTER R",
    0x0073: "LATIN SMALL LETTER S",
    0x0074: "LATIN SMALL LETTER T",
    0x0075: "LATIN SMALL LETTER U",
    0x0076: "LATIN SMALL LETTER V",
    0x0077: "LATIN SMALL LETTER W",
    0x0078: "LATIN SMALL LETTER X",
    0x0079: "LATIN SMALL LETTER Y",
    0x007A: "LATIN SMALL LETTER Z",
    0x007B: "LEFT CURLY BRACKET",
    0x007C: "VERTICAL LINE",
    0x007D: "RIGHT CURLY BRACKET",
    0x007E: "TILDE",
    0x007F: ("DELETE", "DEL"),

    0x0080: ("PADDING CHARACTER", "PAD"),
    0x0081: ("HIGH OCTET PRESET", "HOP"),
    0x0082: ("BREAK PERMITTED HERE", "BPH"),
    0x0083: ("NO BREAK HERE", "NBH"),
    0x0084: ("INDEX", "IND"),
    0x0085: ("NEXT LINE", "NEL"),
    0x0086: ("START OF SELECTED AREA", "SSA"),
    0x0087: ("END OF SELECTED AREA", "ESA"),
    0x0088: ("CHARACTER TABULATION SET", "HTS"),
    0x0089: ("CHARACTER TABULATION WITH JUSTIFICATION", "HTJ"),
    0x008A: ("LINE TABULATION SET", "VTS"),
    0x008B: ("PARTIAL LINE FORWARD", "PLD"),
    0x008C: ("PARTIAL LINE BACKWARD", "PLU"),
    0x008D: ("REVERSE LINE FEED", "RI"),
    0x008E: ("SINGLE SHIFT TWO", "SS2"),
    0x008F: ("SINGLE SHIFT THREE", "SS3"),

    0x0090: ("DEVICE CONTROL STRING", "DCS"),
    0x0091: ("PRIVATE USE ONE", "PU1"),
    0x0092: ("PRIVATE USE TWO", "PU2"),
    0x0093: ("SET TRANSMIT STATE", "STS"),
    0x0094: ("CANCEL CHARACTER", "CCH"),
    0x0095: ("MESSAGE WAITING", "MW"),
    0x0096: ("START OF PROTECTED AREA", "SPA"),
    0x0097: ("END OF PROTECTED AREA", "EPA"),
    0x0098: ("START OF STRING", "SOS"),
    0x0099: ("SINGLE GRAPHIC CHARACTER INTRODUCER", "SGCI"),
    0x009A: ("SINGLE CHARACTER INTRODUCER", "SCI"),
    0x009B: ("CONTROL SEQUENCE INTRODUCER", "CSI"),
    0x009C: ("STRING TERMINATOR", "ST"),
    0x009D: ("OPERATING SYSTEM COMMAND", "OSC"),
    0x009E: ("PRIVATE MESSAGE", "PM"),
    0x009F: ("APPLICATION PROGRAM COMMAND", "APC"),

    0x00A0: "NO-BREAK SPACE",
    0x00A1: "INVERTED EXCLAMATION MARK",
    0x00A2: "CENT SIGN",
    0x00A3: "POUND SIGN",
    0x00A4: "CURRENCY SIGN",
    0x00A5: "YEN SIGN",
    0x00A6: "BROKEN BAR",
    0x00A7: "SECTION SIGN",
    0x00A8: "DIAERESIS",
    0x00A9: "COPYRIGHT SIGN",
    0x00AA: "FEMININE ORDINAL INDICATOR",
    0x00AB: "LEFT-POINTING DOUBLE ANGLE QUOTATION MARK",
    0x00AC: "NOT SIGN",
    0x00AD: "SOFT HYPHEN",
    0x00AE: "REGISTERED SIGN",
    0x00AF: "MACRON",

    0x00B0: "DEGREE SIGN",
    0x00B1: "PLUS-MINUS SIGN",
    0x00B2: "SUPERSCRIPT TWO",
    0x00B3: "SUPERSCRIPT THREE",
    0x00B4: "ACUTE ACCENT",
    0x00B5: "MICRO SIGN",
    0x00B6: "PILCROW SIGN",
    0x00B7: "MIDDLE DOT",
    0x00B8: "CEDILLA",
    0x00B9: "SUPERSCRIPT ONE",
    0x00BA: "MASCULINE ORDINAL INDICATOR",
    0x00BB: "RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK",
    0x00BC: "VULGAR FRACTION ONE QUARTER",
    0x00BD: "VULGAR FRACTION ONE HALF",
    0x00BE: "VULGAR FRACTION THREE QUARTERS",
    0x00BF: "INVERTED QUESTION MARK",

    0x00C0: "LATIN CAPITAL LETTER A WITH GRAVE",
    0x00C1: "LATIN CAPITAL LETTER A WITH ACUTE",
    0x00C2: "LATIN CAPITAL LETTER A WITH CIRCUMFLEX",
    0x00C3: "LATIN CAPITAL LETTER A WITH TILDE",
    0x00C4: "LATIN CAPITAL LETTER A WITH DIAERESIS",
    0x00C5: "LATIN CAPITAL LETTER A WITH RING ABOVE",
    0x00C6: "LATIN CAPITAL LETTER AE",
    0x00C7: "LATIN CAPITAL LETTER C WITH CEDILLA",
    0x00C8: "LATIN CAPITAL LETTER E WITH GRAVE",
    0x00C9: "LATIN CAPITAL LETTER E WITH ACUTE",
    0x00CA: "LATIN CAPITAL LETTER E WITH CIRCUMFLEX",
    0x00CB: "LATIN CAPITAL LETTER E WITH DIAERESIS",
    0x00CC: "LATIN CAPITAL LETTER I WITH GRAVE",
    0x00CD: "LATIN CAPITAL LETTER I WITH ACUTE",
    0x00CE: "LATIN CAPITAL LETTER I WITH CIRCUMFLEX",
    0x00CF: "LATIN CAPITAL LETTER I WITH DIAERESIS",

    0x00D0: "LATIN CAPITAL LETTER ETH",
    0x00D1: "LATIN CAPITAL LETTER N WITH TILDE",
    0x00D2: "LATIN CAPITAL LETTER O WITH GRAVE",
    0x00D3: "LATIN CAPITAL LETTER O WITH ACUTE",
    0x00D4: "LATIN CAPITAL LETTER O WITH CIRCUMFLEX",
    0x00D5: "LATIN CAPITAL LETTER O WITH TILDE",
    0x00D6: "LATIN CAPITAL LETTER O WITH DIAERESIS",
    0x00D7: "MULTIPLICATION SIGN",
    0x00D8: "LATIN CAPITAL LETTER O WITH STROKE",
    0x00D9: "LATIN CAPITAL LETTER U WITH GRAVE",
    0x00DA: "LATIN CAPITAL LETTER U WITH ACUTE",
    0x00DB: "LATIN CAPITAL LETTER U WITH CIRCUMFLEX",
    0x00DC: "LATIN CAPITAL LETTER U WITH DIAERESIS",
    0x00DD: "LATIN CAPITAL LETTER Y WITH ACUTE",
    0x00DE: "LATIN CAPITAL LETTER THORN",
    0x00DF: "LATIN SMALL LETTER SHARP S",

    0x00E0: "LATIN SMALL LETTER A WITH GRAVE",
    0x00E1: "LATIN SMALL LETTER A WITH ACUTE",
    0x00E2: "LATIN SMALL LETTER A WITH CIRCUMFLEX",
    0x00E3: "LATIN SMALL LETTER A WITH TILDE",
    0x00E4: "LATIN SMALL LETTER A WITH DIAERESIS",
    0x00E5: "LATIN SMALL LETTER A WITH RING ABOVE",
    0x00E6: "LATIN SMALL LETTER AE",
    0x00E7: "LATIN SMALL LETTER C WITH CEDILLA",
    0x00E8: "LATIN SMALL LETTER E WITH GRAVE",
    0x00E9: "LATIN SMALL LETTER E WITH ACUTE",
    0x00EA: "LATIN SMALL LETTER E WITH CIRCUMFLEX",
    0x00EB: "LATIN SMALL LETTER E WITH DIAERESIS",
    0x00EC: "LATIN SMALL LETTER I WITH GRAVE",
    0x00ED: "LATIN SMALL LETTER I WITH ACUTE",
    0x00EE: "LATIN SMALL LETTER I WITH CIRCUMFLEX",
    0x00EF: "LATIN SMALL LETTER I WITH DIAERESIS",

    0x00F0: "LATIN SMALL LETTER ETH",
    0x00F1: "LATIN SMALL LETTER N WITH TILDE",
    0x00F2: "LATIN SMALL LETTER O WITH GRAVE",
    0x00F3: "LATIN SMALL LETTER O WITH ACUTE",
    0x00F4: "LATIN SMALL LETTER O WITH CIRCUMFLEX",
    0x00F5: "LATIN SMALL LETTER O WITH TILDE",
    0x00F6: "LATIN SMALL LETTER O WITH DIAERESIS",
    0x00F7: "DIVISION SIGN",
    0x00F8: "LATIN SMALL LETTER O WITH STROKE",
    0x00F9: "LATIN SMALL LETTER U WITH GRAVE",
    0x00FA: "LATIN SMALL LETTER U WITH ACUTE",
    0x00FB: "LATIN SMALL LETTER U WITH CIRCUMFLEX",
    0x00FC: "LATIN SMALL LETTER U WITH DIAERESIS",
    0x00FD: "LATIN SMALL LETTER Y WITH ACUTE",
    0x00FE: "LATIN SMALL LETTER THORN",
    0x00FF: "LATIN SMALL LETTER Y WITH DIAERESIS",

    0x011E: "LATIN CAPITAL LETTER G WITH BREVE",
    0x011F: "LATIN SMALL LETTER G WITH BREVE",
    0x0130: "LATIN CAPITAL LETTER I WITH DOT ABOVE",
    0x0131: "LATIN SMALL LETTER DOTLESS I",

    0x0152: "LATIN CAPITAL LIGATURE OE",
    0x0153: "LATIN SMALL LIGATURE OE",
    0x015E: "LATIN CAPITAL LETTER S WITH CEDILLA",
    0x015F: "LATIN SMALL LETTER S WITH CEDILLA",

    0x0160: "LATIN CAPITAL LETTER S WITH CARON",
    0x0161: "LATIN SMALL LETTER S WITH CARON",
    0x0178: "LATIN CAPITAL LETTER Y WITH DIAERESIS",
    0x017D: "LATIN CAPITAL LETTER Z WITH CARON",
    0x017E: "LATIN SMALL LETTER Z WITH CARON",
    0x0192: "LATIN SMALL LETTER F WITH HOOK",

    0x02C6: "MODIFIER LETTER CIRCUMFLEX ACCENT",
    0x02C7: "CARON",

    0x02D8: "BREVE",
    0x02D9: "DOT ABOVE",
    0x02DA: "RING ABOVE",
    0x02DB: "OGONEK",
    0x02DC: "SMALL TILDE",
    0x02DD: "DOUBLE ACUTE ACCENT",

    0x0393: "GREEK CAPITAL LETTER GAMMA",
    0x0398: "GREEK CAPITAL LETTER THETA",
    0x03A3: "GREEK CAPITAL LETTER SIGMA",
    0x03A6: "GREEK CAPITAL LETTER PHI",
    0x03A9: "GREEK CAPITAL LETTER OMEGA",
    0x03B1: "GREEK SMALL LETTER ALPHA",
    0x03B4: "GREEK SMALL LETTER DELTA",
    0x03B5: "GREEK SMALL LETTER EPSILON",
    0x03C0: "GREEK SMALL LETTER PI",
    0x03C3: "GREEK SMALL LETTER SIGMA",
    0x03C4: "GREEK SMALL LETTER TAU",
    0x03C6: "GREEK SMALL LETTER PHI",

    0x2013: "EN DASH",
    0x2014: "EM DASH",
    0x2018: "LEFT SINGLE QUOTATION MARK",
    0x2019: "RIGHT SINGLE QUOTATION MARK",
    0x201A: "SINGLE LOW-9 QUOTATION MARK",

    0x201C: "LEFT DOUBLE QUOTATION MARK",
    0x201D: "RIGHT DOUBLE QUOTATION MARK",
    0x201E: "DOUBLE LOW-9 QUOTATION MARK",
    0x2021: "DOUBLE DAGGER",
    0x2030: "PER MILLE SIGN",

    0x2020: "DAGGER",
    0x2022: "BULLET",
    0x2026: "HORIZONTAL ELLIPSIS",

    0x2039: "LEFT GUILLEMET",
    0x203A: "RIGHT GUILLEMET",

    0x2044: "FRACTION SLASH",

    0x207F: "SUPERSCRIPT LATIN SMALL LETTER N",

    0x20A7: "PESETA SIGN",
    0x20AC: "EURO SIGN",

    0x2122: "TRADE MARK SIGN",

    0x2202: "PARTIAL DIFFERENTIAL",
    0x2206: "INCREMENT",
    0x220F: "N-ARY PRODUCT",
    0x2211: "N-ARY SUMMATION",
    0x2219: "BULLET POINT",
    0x221A: "SQUARE ROOT",
    0x221E: "INFINITY",
    0x2229: "INTERSECTION",
    0x222B: "INTEGRAL",
    0x2248: "ALMOST EQUAL TO",
    0x2260: "NOT EQUAL TO",
    0x2261: "TRIPLE BAR",
    0x2264: "LESS-THAN OR EQUAL TO",
    0x2265: "GREATER-THAN OR EQUAL TO",
    0x2310: "NEGATION",
    0x2320: "INTEGRAL TOP",
    0x2321: "INTEGRAL BOTTOM",

    0x2500: "BOX DRAWINGS LIGHT HORIZONTAL",
    0x2502: "BOX DRAWINGS LIGHT VERTICAL",
    0x250C: "BOX DRAWINGS LIGHT DOWN AND RIGHT",
    0x2510: "BOX DRAWINGS LIGHT DOWN AND LEFT",
    0x2514: "BOX DRAWINGS LIGHT UP AND RIGHT",
    0x2518: "BOX DRAWINGS LIGHT UP AND LEFT",
    0x251C: "BOX DRAWINGS LIGHT VERTICAL AND RIGHT",
    0x2524: "BOX DRAWINGS LIGHT VERTICAL AND LEFT",
    0x252C: "BOX DRAWINGS LIGHT DOWN AND HORIZONTAL",
    0x2534: "BOX DRAWINGS LIGHT UP AND HORIZONTAL",
    0x253C: "BOX DRAWINGS LIGHT VERTICAL AND HORIZONTAL",
    0x2550: "BOX DRAWINGS DOUBLE HORIZONTAL",
    0x2551: "BOX DRAWINGS DOUBLE VERTICAL",
    0x2552: "BOX DRAWINGS DOWN SINGLE AND RIGHT DOUBLE",
    0x2553: "BOX DRAWINGS DOWN DOUBLE AND RIGHT SINGLE",
    0x2554: "BOX DRAWINGS DOUBLE DOWN AND RIGHT",
    0x2555: "BOX DRAWINGS DOWN SINGLE AND LEFT DOUBLE",
    0x2556: "BOX DRAWINGS DOWN DOUBLE AND LEFT SINGLE",
    0x2557: "BOX DRAWINGS DOUBLE DOWN AND LEFT",
    0x2558: "BOX DRAWINGS UP SINGLE AND RIGHT DOUBLE",
    0x2559: "BOX DRAWINGS UP DOUBLE AND RIGHT SINGLE",
    0x255A: "BOX DRAWINGS DOUBLE UP AND RIGHT",
    0x255B: "BOX DRAWINGS UP SINGLE AND LEFT DOUBLE",
    0x255C: "BOX DRAWINGS UP DOUBLE AND LEFT SINGLE",
    0x255D: "BOX DRAWINGS DOUBLE UP AND LEFT",
    0x255E: "BOX DRAWINGS VERTICAL SINGLE AND RIGHT DOUBLE",
    0x255F: "BOX DRAWINGS VERTICAL DOUBLE AND RIGHT SINGLE",
    0x2560: "BOX DRAWINGS DOUBLE VERTICAL AND RIGHT",
    0x2561: "BOX DRAWINGS VERTICAL SINGLE AND LEFT DOUBLE",
    0x2562: "BOX DRAWINGS VERTICAL DOUBLE AND LEFT SINGLE",
    0x2563: "BOX DRAWINGS DOUBLE VERTICAL AND LEFT",
    0x2564: "BOX DRAWINGS DOWN SINGLE AND HORIZONTAL DOUBLE",
    0x2565: "BOX DRAWINGS DOWN DOUBLE AND HORIZONTAL SINGLE",
    0x2566: "BOX DRAWINGS DOUBLE DOWN AND HORIZONTAL",
    0x2567: "BOX DRAWINGS UP SINGLE AND HORIZONTAL DOUBLE",
    0x2568: "BOX DRAWINGS UP DOUBLE AND HORIZONTAL SINGLE",
    0x2569: "BOX DRAWINGS DOUBLE UP AND HORIZONTAL",
    0x256A: "BOX DRAWINGS VERTICAL SINGLE AND HORIZONTAL DOUBLE",
    0x256B: "BOX DRAWINGS VERTICAL DOUBLE AND HORIZONTAL SINGLE",
    0x256C: "BOX DRAWINGS DOUBLE VERTICAL AND HORIZONTAL",

    0x2580: "UPPER HALF BLOCK",
    0x2584: "LOWER HALF BLOCK",
    0x2588: "FULL BLOCK",
    0x258C: "LEFT HALF BLOCK",
    0x2590: "RIGHT HALF BLOCK",
    0x2591: "LIGHT SHADE",
    0x2592: "MEDIUM SHADE",
    0x2593: "DARK SHADE",
    0x25A0: "BLACK SQUARE",
    0x25CA: "LOZENGE",

    0xF8A0: "SMALL NUMBER ONE-F8A0",
    0xF8FF: "SOLID APPLE-F8FF",
    0xFB01: "LATIN SMALL LIGATURE FI",
    0xFB02: "LATIN SMALL LIGATURE FL"
}

#
# ISO Latin to unicode lookup table, taken from ISOLatin1::ToUTF16Table
#

ISO_LATIN1 = (
    0x0080,  # 0x80
    0x0081,  # 0x81
    0x0082,  # 0x82
    0x0083,  # 0x83
    0x0084,  # 0x84
    0x0085,  # 0x85
    0x0086,  # 0x86
    0x0087,  # 0x87
    0x0088,  # 0x88
    0x0089,  # 0x89
    0x008A,  # 0x8A
    0x008B,  # 0x8B
    0x008C,  # 0x8C
    0x008D,  # 0x8D
    0x008E,  # 0x8E
    0x008F,  # 0x8F
    0x0090,  # 0x90
    0x0091,  # 0x91
    0x0092,  # 0x92
    0x0093,  # 0x93
    0x0094,  # 0x94
    0x0095,  # 0x95
    0x0096,  # 0x96
    0x0097,  # 0x97
    0x0098,  # 0x98
    0x0099,  # 0x99
    0x009A,  # 0x9A
    0x009B,  # 0x9B
    0x009C,  # 0x9C
    0x009D,  # 0x9D
    0x009E,  # 0x9E
    0x009F,  # 0x9F
    0x00A0,  # 0xA0
    0x00A1,  # 0xA1
    0x00A2,  # 0xA2
    0x00A3,  # 0xA3
    0x00A4,  # 0xA4
    0x00A5,  # 0xA5
    0x00A6,  # 0xA6
    0x00A7,  # 0xA7
    0x00A8,  # 0xA8
    0x00A9,  # 0xA9
    0x00AA,  # 0xAA
    0x00AB,  # 0xAB
    0x00AC,  # 0xAC
    0x00AD,  # 0xAD
    0x00AE,  # 0xAE
    0x00AF,  # 0xAF
    0x00B0,  # 0xB0
    0x00B1,  # 0xB1
    0x00B2,  # 0xB2
    0x00B3,  # 0xB3
    0x00B4,  # 0xB4
    0x00B5,  # 0xB5
    0x00B6,  # 0xB6
    0x00B7,  # 0xB7
    0x00B8,  # 0xB8
    0x00B9,  # 0xB9
    0x00BA,  # 0xBA
    0x00BB,  # 0xBB
    0x00BC,  # 0xBC
    0x00BD,  # 0xBD
    0x00BE,  # 0xBE
    0x00BF,  # 0xBF
    0x00C0,  # 0xC0
    0x00C1,  # 0xC1
    0x00C2,  # 0xC2
    0x00C3,  # 0xC3
    0x00C4,  # 0xC4
    0x00C5,  # 0xC5
    0x00C6,  # 0xC6
    0x00C7,  # 0xC7
    0x00C8,  # 0xC8
    0x00C9,  # 0xC9
    0x00CA,  # 0xCA
    0x00CB,  # 0xCB
    0x00CC,  # 0xCC
    0x00CD,  # 0xCD
    0x00CE,  # 0xCE
    0x00CF,  # 0xCF
    0x00D0,  # 0xD0
    0x00D1,  # 0xD1
    0x00D2,  # 0xD2
    0x00D3,  # 0xD3
    0x00D4,  # 0xD4
    0x00D5,  # 0xD5
    0x00D6,  # 0xD6
    0x00D7,  # 0xD7
    0x00D8,  # 0xD8
    0x00D9,  # 0xD9
    0x00DA,  # 0xDA
    0x00DB,  # 0xDB
    0x00DC,  # 0xDC
    0x00DD,  # 0xDD
    0x00DE,  # 0xDE
    0x00DF,  # 0xDF
    0x00E0,  # 0xE0
    0x00E1,  # 0xE1
    0x00E2,  # 0xE2
    0x00E3,  # 0xE3
    0x00E4,  # 0xE4
    0x00E5,  # 0xE5
    0x00E6,  # 0xE6
    0x00E7,  # 0xE7
    0x00E8,  # 0xE8
    0x00E9,  # 0xE9
    0x00EA,  # 0xEA
    0x00EB,  # 0xEB
    0x00EC,  # 0xEC
    0x00ED,  # 0xED
    0x00EE,  # 0xEE
    0x00EF,  # 0xEF
    0x00F0,  # 0xF0
    0x00F1,  # 0xF1
    0x00F2,  # 0xF2
    0x00F3,  # 0xF3
    0x00F4,  # 0xF4
    0x00F5,  # 0xF5
    0x00F6,  # 0xF6
    0x00F7,  # 0xF7
    0x00F8,  # 0xF8
    0x00F9,  # 0xF9
    0x00FA,  # 0xFA
    0x00FB,  # 0xFB
    0x00FC,  # 0xFC
    0x00FD,  # 0xFD
    0x00FE,  # 0xFE
    0x00FF  # 0xFF
)

#
# Mac Roman US to unicode lookup table, taken from MacRomanUS::ToUTF16Table
#

MAC_ROMAN_US = (
    0x00C4,  # 0x80
    0x00C5,  # 0x81
    0x00C7,  # 0x82
    0x00C9,  # 0x83
    0x00D1,  # 0x84
    0x00D6,  # 0x85
    0x00DC,  # 0x86
    0x00E1,  # 0x87
    0x00E0,  # 0x88
    0x00E2,  # 0x89
    0x00E4,  # 0x8A
    0x00E3,  # 0x8B
    0x00E5,  # 0x8C
    0x00E7,  # 0x8D
    0x00E9,  # 0x8E
    0x00E8,  # 0x8F
    0x00EA,  # 0x90
    0x00EB,  # 0x91
    0x00ED,  # 0x92
    0x00EC,  # 0x93
    0x00EE,  # 0x94
    0x00EF,  # 0x95
    0x00F1,  # 0x96
    0x00F3,  # 0x97
    0x00F2,  # 0x98
    0x00F4,  # 0x99
    0x00F6,  # 0x9A
    0x00F5,  # 0x9B
    0x00FA,  # 0x9C
    0x00F9,  # 0x9D
    0x00FB,  # 0x9E
    0x00FC,  # 0x9F
    0x2020,  # 0xA0
    0x00B0,  # 0xA1
    0x00A2,  # 0xA2
    0x00A3,  # 0xA3
    0x00A7,  # 0xA4
    0x2022,  # 0xA5
    0x00B6,  # 0xA6
    0x00DF,  # 0xA7
    0x00AE,  # 0xA8
    0x00A9,  # 0xA9
    0x2122,  # 0xAA
    0x00B4,  # 0xAB
    0x00A8,  # 0xAC
    0x2260,  # 0xAD
    0x00C6,  # 0xAE
    0x00D8,  # 0xAF
    0x221E,  # 0xB0
    0x00B1,  # 0xB1
    0x2264,  # 0xB2
    0x2265,  # 0xB3
    0x00A5,  # 0xB4
    0x00B5,  # 0xB5
    0x2202,  # 0xB6
    0x2211,  # 0xB7
    0x220F,  # 0xB8
    0x03C0,  # 0xB9
    0x222B,  # 0xBA
    0x00AA,  # 0xBB
    0x00BA,  # 0xBC
    0x03A9,  # 0xBD
    0x00E6,  # 0xBE
    0x00F8,  # 0xBF
    0x00BF,  # 0xC0
    0x00A1,  # 0xC1
    0x00AC,  # 0xC2
    0x221A,  # 0xC3
    0x0192,  # 0xC4
    0x2248,  # 0xC5
    0x2206,  # 0xC6
    0x00AB,  # 0xC7
    0x00BB,  # 0xC8
    0x2026,  # 0xC9
    0x00A0,  # 0xCA
    0x00C0,  # 0xCB
    0x00C3,  # 0xCC
    0x00D5,  # 0xCD
    0x0152,  # 0xCE
    0x0153,  # 0xCF
    0x2013,  # 0xD0
    0x2014,  # 0xD1
    0x201C,  # 0xD2
    0x201D,  # 0xD3
    0x2018,  # 0xD4
    0x2019,  # 0xD5
    0x00F7,  # 0xD6
    0x25CA,  # 0xD7
    0x00FF,  # 0xD8
    0x0178,  # 0xD9
    0x2044,  # 0xDA
    0x20AC,  # 0xDB
    0x2039,  # 0xDC
    0x203A,  # 0xDD
    0xFB01,  # 0xDE
    0xFB02,  # 0xDF
    0x2021,  # 0xE0
    0x00B7,  # 0xE1
    0x201A,  # 0xE2
    0x201E,  # 0xE3
    0x2030,  # 0xE4
    0x00C2,  # 0xE5
    0x00CA,  # 0xE6
    0x00C1,  # 0xE7
    0x00CB,  # 0xE8
    0x00C8,  # 0xE9
    0x00CD,  # 0xEA
    0x00CE,  # 0xEB
    0x00CF,  # 0xEC
    0x00CC,  # 0xED
    0x00D3,  # 0xEE
    0x00D4,  # 0xEF
    0xF8FF,  # 0xF0
    0x00D2,  # 0xF1
    0x00DA,  # 0xF2
    0x00DB,  # 0xF3
    0x00D9,  # 0xF4
    0x0131,  # 0xF5
    0x02C6,  # 0xF6
    0x02DC,  # 0xF7
    0x00AF,  # 0xF8
    0x02D8,  # 0xF9
    0x02D9,  # 0xFA
    0x02DA,  # 0xFB
    0x00B8,  # 0xFC
    0x02DD,  # 0xFD
    0x02DB,  # 0xFE
    0x02C7  # 0xFF
)

#
# Windows page 437 to unicode lookup table, taken from Win437::ToUTF16Table
#

WINDOWS_437 = (
    0x00C7,  # 0x80
    0x00FC,  # 0x81
    0x00E9,  # 0x82
    0x00E2,  # 0x83
    0x00E4,  # 0x84
    0x00E0,  # 0x85
    0x00E5,  # 0x86
    0x00E7,  # 0x87
    0x00EA,  # 0x88
    0x00EB,  # 0x89
    0x00E8,  # 0x8A
    0x00EF,  # 0x8B
    0x00EE,  # 0x8C
    0x00EC,  # 0x8D
    0x00C4,  # 0x8E
    0x00C5,  # 0x8F
    0x00C9,  # 0x90
    0x00E6,  # 0x91
    0x00C6,  # 0x92
    0x00F4,  # 0x93
    0x00F6,  # 0x94
    0x00F2,  # 0x95
    0x00FB,  # 0x96
    0x00F9,  # 0x97
    0x00FF,  # 0x98
    0x00D6,  # 0x99
    0x00DC,  # 0x9A
    0x00A2,  # 0x9B
    0x00A3,  # 0x9C
    0x00A5,  # 0x9D
    0x20A7,  # 0x9E
    0x0192,  # 0x9F
    0x00E1,  # 0xA0
    0x00ED,  # 0xA1
    0x00F3,  # 0xA2
    0x00FA,  # 0xA3
    0x00F1,  # 0xA4
    0x00D1,  # 0xA5
    0x00AA,  # 0xA6
    0x00BA,  # 0xA7
    0x00BF,  # 0xA8
    0x2310,  # 0xA9
    0x00AC,  # 0xAA
    0x00BD,  # 0xAB
    0x00BC,  # 0xAC
    0x00A1,  # 0xAD
    0x00AB,  # 0xAE
    0x00BB,  # 0xAF
    0x2591,  # 0xB0
    0x2592,  # 0xB1
    0x2593,  # 0xB2
    0x2502,  # 0xB3
    0x2524,  # 0xB4
    0x2561,  # 0xB5
    0x2562,  # 0xB6
    0x2556,  # 0xB7
    0x2555,  # 0xB8
    0x2563,  # 0xB9
    0x2551,  # 0xBA
    0x2557,  # 0xBB
    0x255D,  # 0xBC
    0x255C,  # 0xBD
    0x255B,  # 0xBE
    0x2510,  # 0xBF
    0x2514,  # 0xC0
    0x2534,  # 0xC1
    0x252C,  # 0xC2
    0x251C,  # 0xC3
    0x2500,  # 0xC4
    0x253C,  # 0xC5
    0x255E,  # 0xC6
    0x255F,  # 0xC7
    0x255A,  # 0xC8
    0x2554,  # 0xC9
    0x2569,  # 0xCA
    0x2566,  # 0xCB
    0x2560,  # 0xCC
    0x2550,  # 0xCD
    0x256C,  # 0xCE
    0x2567,  # 0xCF
    0x2568,  # 0xD0
    0x2564,  # 0xD1
    0x2565,  # 0xD2
    0x2559,  # 0xD3
    0x2558,  # 0xD4
    0x2552,  # 0xD5
    0x2553,  # 0xD6
    0x256B,  # 0xD7
    0x256A,  # 0xD8
    0x2518,  # 0xD9
    0x250C,  # 0xDA
    0x2588,  # 0xDB
    0x2584,  # 0xDC
    0x258C,  # 0xDD
    0x2590,  # 0xDE
    0x2580,  # 0xDF
    0x03B1,  # 0xE0
    0x00DF,  # 0xE1
    0x0393,  # 0xE2
    0x03C0,  # 0xE3
    0x03A3,  # 0xE4
    0x03C3,  # 0xE5
    0x00B5,  # 0xE6
    0x03C4,  # 0xE7
    0x03A6,  # 0xE8
    0x0398,  # 0xE9
    0x03A9,  # 0xEA
    0x03B4,  # 0xEB
    0x221E,  # 0xEC
    0x03C6,  # 0xED
    0x03B5,  # 0xEE
    0x2229,  # 0xEF
    0x2261,  # 0xF0
    0x00B1,  # 0xF1
    0x2265,  # 0xF2
    0x2264,  # 0xF3
    0x2320,  # 0xF4
    0x2321,  # 0xF5
    0x00F7,  # 0xF6
    0x2248,  # 0xF7
    0x00B0,  # 0xF8
    0x2219,  # 0xF9
    0x00B7,  # 0xFA
    0x221A,  # 0xFB
    0x207F,  # 0xFC
    0x00B2,  # 0xFD
    0x25A0,  # 0xFE
    0x00A0  # 0xFF
)

#
# Windows page 1252 to unicode lookup table, taken from Win1252::ToUTF16Table
#

WINDOWS_1252 = (
    0x20AC,  # 0x80
    0x0081,  # 0x81
    0x201A,  # 0x82
    0x0192,  # 0x83
    0x201E,  # 0x84
    0x2026,  # 0x85
    0x2020,  # 0x86
    0x2021,  # 0x87
    0x02C6,  # 0x88
    0x2030,  # 0x89
    0x0160,  # 0x8A
    0x2039,  # 0x8B
    0x0152,  # 0x8C
    0x008D,  # 0x8D
    0x017D,  # 0x8E
    0x008F,  # 0x8F
    0x0090,  # 0x90
    0x2018,  # 0x91
    0x2019,  # 0x92
    0x201C,  # 0x93
    0x201D,  # 0x94
    0x2022,  # 0x95
    0x2013,  # 0x96
    0x2014,  # 0x97
    0x02DC,  # 0x98
    0x2122,  # 0x99
    0x0161,  # 0x9A
    0x203A,  # 0x9B
    0x0153,  # 0x9C
    0x009D,  # 0x9D
    0x017E,  # 0x9E
    0x0178,  # 0x9F
    0x00A0,  # 0xA0
    0x00A1,  # 0xA1
    0x00A2,  # 0xA2
    0x00A3,  # 0xA3
    0x00A4,  # 0xA4
    0x00A5,  # 0xA5
    0x00A6,  # 0xA6
    0x00A7,  # 0xA7
    0x00A8,  # 0xA8
    0x00A9,  # 0xA9
    0x00AA,  # 0xAA
    0x00AB,  # 0xAB
    0x00AC,  # 0xAC
    0x00AD,  # 0xAD
    0x00AE,  # 0xAE
    0x00AF,  # 0xAF
    0x00B0,  # 0xB0
    0x00B1,  # 0xB1
    0x00B2,  # 0xB2
    0x00B3,  # 0xB3
    0x00B4,  # 0xB4
    0x00B5,  # 0xB5
    0x00B6,  # 0xB6
    0x00B7,  # 0xB7
    0x00B8,  # 0xB8
    0x00B9,  # 0xB9
    0x00BA,  # 0xBA
    0x00BB,  # 0xBB
    0x00BC,  # 0xBC
    0x00BD,  # 0xBD
    0x00BE,  # 0xBE
    0x00BF,  # 0xBF
    0x00C0,  # 0xC0
    0x00C1,  # 0xC1
    0x00C2,  # 0xC2
    0x00C3,  # 0xC3
    0x00C4,  # 0xC4
    0x00C5,  # 0xC5
    0x00C6,  # 0xC6
    0x00C7,  # 0xC7
    0x00C8,  # 0xC8
    0x00C9,  # 0xC9
    0x00CA,  # 0xCA
    0x00CB,  # 0xCB
    0x00CC,  # 0xCC
    0x00CD,  # 0xCD
    0x00CE,  # 0xCE
    0x00CF,  # 0xCF
    0x00D0,  # 0xD0
    0x00D1,  # 0xD1
    0x00D2,  # 0xD2
    0x00D3,  # 0xD3
    0x00D4,  # 0xD4
    0x00D5,  # 0xD5
    0x00D6,  # 0xD6
    0x00D7,  # 0xD7
    0x00D8,  # 0xD8
    0x00D9,  # 0xD9
    0x00DA,  # 0xDA
    0x00DB,  # 0xDB
    0x00DC,  # 0xDC
    0x00DD,  # 0xDD
    0x00DE,  # 0xDE
    0x00DF,  # 0xDF
    0x00E0,  # 0xE0
    0x00E1,  # 0xE1
    0x00E2,  # 0xE2
    0x00E3,  # 0xE3
    0x00E4,  # 0xE4
    0x00E5,  # 0xE5
    0x00E6,  # 0xE6
    0x00E7,  # 0xE7
    0x00E8,  # 0xE8
    0x00E9,  # 0xE9
    0x00EA,  # 0xEA
    0x00EB,  # 0xEB
    0x00EC,  # 0xEC
    0x00ED,  # 0xED
    0x00EE,  # 0xEE
    0x00EF,  # 0xEF
    0x00F0,  # 0xF0
    0x00F1,  # 0xF1
    0x00F2,  # 0xF2
    0x00F3,  # 0xF3
    0x00F4,  # 0xF4
    0x00F5,  # 0xF5
    0x00F6,  # 0xF6
    0x00F7,  # 0xF7
    0x00F8,  # 0xF8
    0x00F9,  # 0xF9
    0x00FA,  # 0xFA
    0x00FB,  # 0xFB
    0x00FC,  # 0xFC
    0x00FD,  # 0xFD
    0x00FE,  # 0xFE
    0x00FF  # 0xFF
)

TABLE_NAMES = (
    ("ISOLatin1", "isolatin1", ISO_LATIN1),
    ("MacRoman US", "macromanus", MAC_ROMAN_US),
    ("Windows 437", "windows437", WINDOWS_437),
    ("Windows 1252", "windows1252", WINDOWS_1252)
)

########################################


def create_header(lines, table):
    """
    Create the HTML header and HEAD record
    """
    lines.append("<!DOCTYPE HTML PUBLIC "
        "\"-//W3C//DTD HTML 4.01 Transitional//EN\"")
    lines.append("   \"http://www.w3.org/TR/html4/loose.dtd\">")
    lines.append("<html lang=\"en-US\">")
    lines.append("<head>")
    lines.append("<title>" + table[0] + " Unicode table</title>")
    lines.append("<meta name=\"Author\" content=\"Rebecca Heineman\">")
    lines.append("<meta name=\"description\" content=\"" + table[0] +
        " Unicode table\">")
    lines.append("<meta name=\"keywords\" content=\"burgerlib, xbox, ps3, ps2, "
        "wii, ds, ipod, mac, vista, xp, c, c++, assembly, arm, ppc, intel, "
        "amd, nvidia, ati, sony, microsoft, nintendo, sega, playstation, "
        "ps4, ps5\">")
    lines.append("<meta http-equiv=\"Content-Type\" content=\"text/html; "
        "charset=utf-8\">")
    lines.append("</head>")

########################################


def create_top(lines, table):
    """
    Create the beginning of the body
    """

    lines.append("<body bgcolor=\"#FFFFFF\">")
    lines.append("<center><h1>" + table[0] + " Unicode table</h1></center>")
    lines.append(
        "<center><h3>Generated with <i>generate_charsets.py</i></h3></center>")
    lines.append("<table width=\"100%\" border=\"1\" cellspacing=\"0\" "
        "cellpadding=\"2\" summary=\"This is the Unicode mapping of " +
        table[0] + "\">")
    lines.append("<tr bgcolor=\"#555555\">")
    lines.append("<td></td>")
    for i in range(16):
        lines.append("<th><font color=\"white\">" + "%02X" % i +
        "</font></th>")
    lines.append("<td></td>")
    lines.append("</tr>")

########################################


def create_bottom(lines, table):
    """
    Create the bottom row of the table
    """
    lines.append("<tr bgcolor=\"#555555\">")
    lines.append("<td></td>")
    for i in range(16):
        lines.append("<th><font color=\"white\">" + "%02X" % i + "</font></th>")
    lines.append("<td></td>")
    lines.append("</tr>")

########################################


def create_entry(lines, utf32):
    """
    Generate the HTML for a single table cell
    """

    hexstring = "%04X" % utf32
    description = UNICODE_DESCRIPTIONS.get(utf32, None)
    if description:
        if is_string(description):
            title = description
            desc = chr(utf32)
        else:
            title = description[0]
            desc = description[1]
    else:
        print("Code " + hex(utf32) + " is not found")
        title = "UNKNOWN"
        desc = "NULL"

    if len(desc) == 1:
        entry = (
            "<td align=\"center\" title=\"{}\"><font size=\"+2\">{}</font><br>"
            "<font size=\"-2\">{}</font></td>").format(
            title, escape_xml_cdata(desc), hexstring)
    else:
        entry = ("<td align=\"center\" title=\"{}\">{}<br><font size=\"-2\">"
            "{}</font></td>").format(title, desc, hexstring)
    lines.append(entry)

########################################


def create_table(lines, table):
    """
    Create an HTML table of all the characters in this charset
    """
    create_top(lines, table)
    for i in range(16):
        if (i % 2) == 0:
            lines.append("<tr>")
        else:
            lines.append("<tr bgcolor=\"#dddddd\">")
        lines.append("<th bgcolor=\"#555555\"><font color=\"white\">" +
            "%X0" % i + "</font></th>")
        for j in range(16):
            index = (i * 16) + j
            if index < 128:
                create_entry(lines, index)
            else:
                create_entry(lines, table[2][index - 128])
        lines.append("<th bgcolor=\"#555555\"><font color=\"white\">" +
            "%X0" % i + "</font></th>")
        lines.append("</tr>")

    create_bottom(lines, table)

########################################


def create_footer(lines, table):
    """
    Close up the HTML file
    """
    lines.append("</table>")
    lines.append("</body>")
    lines.append("</html>")


########################################


def main(working_directory):
    """
    Main entry point for charset doc generator.

    Args:
        working_directory
            Directory this script resides in.

    Returns:
        0
    """

    dest_folder = os.path.join(
        working_directory, "temp", "charsets")
    create_folder_if_needed(dest_folder)

    # Prepare to output the tables
    for table in TABLE_NAMES:
        lines = []
        create_header(lines, table)
        create_table(lines, table)
        create_footer(lines, table)

        dest_file = os.path.join(dest_folder, table[1] + ".htm")
        save_text_file_if_newer(dest_file, lines,
            bom=True, perforce=False, verbose=True)


########################################


# If called as a command line and not a class, perform the build
if __name__ == "__main__":
    sys.exit(main(WORKING_DIRECTORY))
