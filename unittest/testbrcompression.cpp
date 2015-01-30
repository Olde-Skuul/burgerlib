/***************************************

	Unit tests for the Integer Math library

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrcompression.h"
#include "common.h"
#include "brcompresslbmrle.h"
#include "brdecompresslbmrle.h"
#include "brcompresslzss.h"
#include "brdecompresslzss.h"
#include "brcompressdeflate.h"
#include "brdecompressdeflate.h"
#include "brstringfunctions.h"
#include "brfixedpoint.h"
#include "brnumberstringhex.h"
#include "brmemoryansi.h"

using namespace Burger;

static const Word8 RawData[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
	0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,
	0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
	0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E,0x7F,
	0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,
	0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,
	0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,
	0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
	0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
	0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF,
	0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,
	0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,
	0x00,0x00,0x01,0x01,0x02,0x02,0x03,0x03,0x04,0x04,0x05,0x05,0x06,0x06,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00
};

static const Word8 ILBMCompressed[] = {
	0x80,0x00,0xE1,0x00,0x7F,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,
	0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,
	0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,
	0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,
	0x3C,0x3D,0x3E,0x3F,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,
	0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,
	0x5C,0x5D,0x5E,0x5F,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,
	0x6C,0x6D,0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,
	0x7C,0x7D,0x7E,0x7F,0x80,0x7F,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,
	0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,
	0x9B,0x9C,0x9D,0x9E,0x9F,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,
	0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,
	0xBB,0xBC,0xBD,0xBE,0xBF,0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,
	0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,
	0xDB,0xDC,0xDD,0xDE,0xDF,0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,
	0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,
	0xFB,0xFC,0xFD,0xFE,0xFF,0x00,0x00,0x00,0xFF,0x01,0xFF,0x02,0xFF,0x03,0xFF,0x04,
	0xFF,0x05,0xFF,0x06,0xFA,0x00
};

static const Word8 LZSSCompressed[] = {
	0x01,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFC,0xFF,0xFF,0xFE,0xDF,0x01,0x02,0x03,0x04,0x05,0x06,0xFF,0x07,0x08,0x09,0x0A,
	0x0B,0x0C,0x0D,0x0E,0xFF,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0xFF,0x17,0x18,
	0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0xFF,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0xFF,
	0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0xFF,0x2F,0x30,0x31,0x32,0x33,0x34,0x35,
	0x36,0xFF,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0xFF,0x3F,0x40,0x41,0x42,0x43,
	0x44,0x45,0x46,0xFF,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0xFF,0x4F,0x50,0x51,
	0x52,0x53,0x54,0x55,0x56,0xFF,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0xFF,0x5F,
	0x60,0x61,0x62,0x63,0x64,0x65,0x66,0xFF,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,
	0xFF,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0xFF,0x77,0x78,0x79,0x7A,0x7B,0x7C,
	0x7D,0x7E,0xFF,0x7F,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0xFF,0x87,0x88,0x89,0x8A,
	0x8B,0x8C,0x8D,0x8E,0xFF,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0xFF,0x97,0x98,
	0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0xFF,0x9F,0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xFF,
	0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xFF,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,
	0xB6,0xFF,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xFF,0xBF,0xC0,0xC1,0xC2,0xC3,
	0xC4,0xC5,0xC6,0xFF,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xFF,0xCF,0xD0,0xD1,
	0xD2,0xD3,0xD4,0xD5,0xD6,0xFF,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xFF,0xDF,
	0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xFF,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,
	0xFF,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xFF,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,
	0xFD,0xFE,0xFD,0xFF,0xFF,0x0E,0x01,0x02,0x02,0x03,0x03,0x04,0x1F,0x04,0x05,0x05,
	0x06,0x06,0xE1,0x4E
};

static const Word8 DeflateCompressed[] = {
	0x78,0xDA,0x63,0x60,0x18,0xE4,0x80,0x91,0x89,0x99,0x85,0x95,0x8D,0x9D,0x83,0x93,
	0x8B,0x9B,0x87,0x97,0x8F,0x5F,0x40,0x50,0x48,0x58,0x44,0x54,0x4C,0x5C,0x42,0x52,
	0x4A,0x5A,0x46,0x56,0x4E,0x5E,0x41,0x51,0x49,0x59,0x45,0x55,0x4D,0x5D,0x43,0x53,
	0x4B,0x5B,0x47,0x57,0x4F,0xDF,0xC0,0xD0,0xC8,0xD8,0xC4,0xD4,0xCC,0xDC,0xC2,0xD2,
	0xCA,0xDA,0xC6,0xD6,0xCE,0xDE,0xC1,0xD1,0xC9,0xD9,0xC5,0xD5,0xCD,0xDD,0xC3,0xD3,
	0xCB,0xDB,0xC7,0xD7,0xCF,0x3F,0x20,0x30,0x28,0x38,0x24,0x34,0x2C,0x3C,0x22,0x32,
	0x2A,0x3A,0x26,0x36,0x2E,0x3E,0x21,0x31,0x29,0x39,0x25,0x35,0x2D,0x3D,0x23,0x33,
	0x2B,0x3B,0x27,0x37,0x2F,0xBF,0xA0,0xB0,0xA8,0xB8,0xA4,0xB4,0xAC,0xBC,0xA2,0xB2,
	0xAA,0xBA,0xA6,0xB6,0xAE,0xBE,0xA1,0xB1,0xA9,0xB9,0xA5,0xB5,0xAD,0xBD,0xA3,0xB3,
	0xAB,0xBB,0xA7,0xB7,0xAF,0x7F,0xC2,0xC4,0x49,0x93,0xA7,0x4C,0x9D,0x36,0x7D,0xC6,
	0xCC,0x59,0xB3,0xE7,0xCC,0x9D,0x37,0x7F,0xC1,0xC2,0x45,0x8B,0x97,0x2C,0x5D,0xB6,
	0x7C,0xC5,0xCA,0x55,0xAB,0xD7,0xAC,0x5D,0xB7,0x7E,0xC3,0xC6,0x4D,0x9B,0xB7,0x6C,
	0xDD,0xB6,0x7D,0xC7,0xCE,0x5D,0xBB,0xF7,0xEC,0xDD,0xB7,0xFF,0xC0,0xC1,0x43,0x87,
	0x8F,0x1C,0x3D,0x76,0xFC,0xC4,0xC9,0x53,0xA7,0xCF,0x9C,0x3D,0x77,0xFE,0xC2,0xC5,
	0x4B,0x97,0xAF,0x5C,0xBD,0x76,0xFD,0xC6,0xCD,0x5B,0xB7,0xEF,0xDC,0xBD,0x77,0xFF,
	0xC1,0xC3,0x47,0x8F,0x9F,0x3C,0x7D,0xF6,0xFC,0xC5,0xCB,0x57,0xAF,0xDF,0xBC,0x7D,
	0xF7,0xFE,0xC3,0xC7,0x4F,0x9F,0xBF,0x7C,0xFD,0xF6,0xFD,0xC7,0xCF,0x5F,0xBF,0xFF,
	0xFC,0xFD,0xF7,0x1F,0x18,0x00,0x8C,0x4C,0x4C,0xCC,0xCC,0x2C,0x2C,0xAC,0xAC,0x6C,
	0x6C,0xD0,0x30,0x01,0x00,0x26,0xC1,0x7F,0xAB
};

//
// Convert a hex string dump to a text string
//

static void DumpTheHex(char *pOutput,const Word8 *pInput,WordPtr uCount)
{
	pOutput[0] = 0;
	if (uCount) {
		do {
			NumberStringHex Hex(static_cast<Word32>(pInput[0]),LEADINGZEROS+2);
			StringConcatenate(pOutput,Hex.GetPtr());
			if (uCount!=1) {
				StringConcatenate(pOutput," ");
			}
			++pInput;
		} while (--uCount);
	}
}

//
// Perform verification for decompression
//

static Word ReportDecompress(Word8 *pBuffer,const Word8 *pOriginal,WordPtr uOriginalSize,const char *pFunction)
{
	char Before[256];
	char After[256];
	Word uFailure = FALSE;
	if (MemoryCompare(pBuffer,pOriginal,uOriginalSize)) {
		WordPtr uIndex = 0;
		do {
			if (pBuffer[uIndex]!=pOriginal[uIndex]) {
				break;
			}
		} while (++uIndex<uOriginalSize);
		WordPtr uMax = uOriginalSize-uIndex;
		if (uMax>8) {
			uMax = 8;
		}
		DumpTheHex(Before,pOriginal+uIndex,uMax);
		DumpTheHex(After,pBuffer+uIndex,uMax);
		ReportFailure("%s = Data mismatch at index %04X, Expected %s, Found %s",TRUE,pFunction,uIndex,Before,After);

		uFailure = TRUE;
	}
	if (!uFailure) {
		Word i = 80;
		const Word8 *pTest = pBuffer+uOriginalSize;
		do {
			if (pTest[0]!=0xD5) {
				ReportFailure("%s = Buffer overrun",TRUE,pFunction);
				uFailure = TRUE;
				break;
			}
			++pTest;
		} while (--i);
	}
	MemoryFill(pBuffer,0xD5,uOriginalSize+80);
	return uFailure;
}

//
// Test the ILBM Decompression
//

static Word TestILBMDecompress(void)
{
	Word uFailure = FALSE;
	Word8 Buffer[sizeof(RawData)+80];

	// Perform a simple decompression test and test for buffer overrun
	DecompressILBMRLE Tester;

	MemoryFill(Buffer,0xD5,sizeof(Buffer));
	Decompress::eError Error = Tester.Process(Buffer,sizeof(RawData),ILBMCompressed,sizeof(ILBMCompressed));
	if (Error!=Decompress::DECOMPRESS_OKAY) {
		ReportFailure("DecompressILBMRLE::Process(Buffer,sizeof(RawData),ILBMCompressed,sizeof(ILBMCompressed)) = %d, expected Decompress::DECOMPRESS_OKAY",TRUE,Error);
		uFailure = TRUE;
	}
	uFailure |= ReportDecompress(Buffer,RawData,sizeof(RawData),"DecompressILBMRLE::Process(Buffer,sizeof(RawData),ILBMCompressed,sizeof(ILBMCompressed))");

	// Perform test where the decompression is bisected by decompression buffer

	WordPtr uSplit = 0;
	do {
		Tester.Reset();
		Error = Tester.Process(Buffer,uSplit,ILBMCompressed,sizeof(ILBMCompressed));
		WordPtr uBytesProcessed = Tester.GetProcessedInputSize();
		if (Error!=Decompress::DECOMPRESS_OUTPUTOVERRUN) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("DecompressILBMRLE::Process(Buffer,0x%s,ILBMCompressed,sizeof(ILBMCompressed)) = %d, expected Decompress::DECOMPRESS_OUTPUTOVERRUN",TRUE,Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		Error = Tester.Process(Buffer+uSplit,sizeof(RawData)-uSplit,ILBMCompressed+uBytesProcessed,sizeof(ILBMCompressed)-uBytesProcessed);
		if (Error!=Decompress::DECOMPRESS_OKAY) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("DecompressILBMRLE::Process(Buffer+0x%s,sizeof(RawData)-0x%s,ILBMCompressed+uBytesProcessed,sizeof(ILBMCompressed)-uBytesProcessed) = %d, expected Decompress::DECOMPRESS_OKAY",TRUE,Hex.GetPtr(),Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		uFailure |= ReportDecompress(Buffer,RawData,sizeof(RawData),"DecompressILBMRLE::Process(Buffer,sizeof(RawData),ILBMCompressed,sizeof(ILBMCompressed))");
	} while (++uSplit<sizeof(RawData));

	// Perform test where compressed data is bisected

	uSplit = 0;
	do {
		Tester.Reset();
		Error = Tester.Process(Buffer,sizeof(RawData),ILBMCompressed,uSplit);
		WordPtr uBytesProcessed = Tester.GetProcessedOutputSize();
		if (Error!=Decompress::DECOMPRESS_OUTPUTUNDERRUN) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("DecompressILBMRLE::Process(Buffer,sizeof(RawData),ILBMCompressed,0x%s) = %d, expected Decompress::DECOMPRESS_OUTPUTUNDERRUN",TRUE,Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		Error = Tester.Process(Buffer+uBytesProcessed,sizeof(RawData)-uBytesProcessed,ILBMCompressed+uSplit,sizeof(ILBMCompressed)-uSplit);
		if (Error!=Decompress::DECOMPRESS_OKAY) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("DecompressILBMRLE::Process(Buffer+uBytesProcessed,sizeof(RawData)-uBytesProcessed,ILBMCompressed+0x%s,sizeof(ILBMCompressed)-0x%s) = %d, expected Decompress::DECOMPRESS_OKAY",TRUE,Hex.GetPtr(),Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		uFailure |= ReportDecompress(Buffer,RawData,sizeof(RawData),"DecompressILBMRLE::Process(Buffer,sizeof(RawData),ILBMCompressed,sizeof(ILBMCompressed))");
	} while (++uSplit<sizeof(ILBMCompressed));

	// Perform single byte output
	uSplit = 0;
	Tester.Reset();
	WordPtr uStepper = 0;
	do {
		Error = Tester.Process(Buffer+uSplit,1,ILBMCompressed+uStepper,sizeof(ILBMCompressed)-uStepper);
		if (uSplit==(sizeof(RawData)-1)) {
			if (Error!=Decompress::DECOMPRESS_OKAY) {
				ReportFailure("DecompressILBMRLE::Process(Buffer,1,ILBMCompressed,uStepper) = %d, expected Decompress::DECOMPRESS_OKAY",TRUE,Error);
				uFailure = TRUE;
			}
		} else {
			if (Error!=Decompress::DECOMPRESS_OUTPUTOVERRUN) {
				ReportFailure("DecompressILBMRLE::Process(Buffer,1,ILBMCompressed,uStepper) = %d, expected Decompress::DECOMPRESS_OUTPUTOVERRUN",TRUE,Error);
				uFailure = TRUE;
			}

		}
		uStepper += Tester.GetProcessedInputSize();
	} while (++uSplit<sizeof(RawData));
	uFailure |= ReportDecompress(Buffer,RawData,sizeof(RawData),"DecompressILBMRLE::Process(Buffer,1,ILBMCompressed,sizeof(ILBMCompressed))");

	// Perform single byte input
	uSplit = 0;
	Tester.Reset();
	uStepper = 0;
	do {
		Error = Tester.Process(Buffer+uStepper,sizeof(RawData)-uStepper,ILBMCompressed+uSplit,1);
		if (uSplit==(sizeof(ILBMCompressed)-1)) {
			if (Error!=Decompress::DECOMPRESS_OKAY) {
				ReportFailure("DecompressILBMRLE::Process(Buffer,uStepper,ILBMCompressed,1) = %d, expected Decompress::DECOMPRESS_OKAY",TRUE,Error);
				uFailure = TRUE;
			}
		} else {
			if (Error!=Decompress::DECOMPRESS_OUTPUTUNDERRUN) {
				ReportFailure("DecompressILBMRLE::Process(Buffer,uStepper,ILBMCompressed,1) = %d, expected Decompress::DECOMPRESS_OUTPUTUNDERRUN",TRUE,Error);
				uFailure = TRUE;
			}

		}
		uStepper += Tester.GetProcessedOutputSize();
	} while (++uSplit<sizeof(ILBMCompressed));
	uFailure |= ReportDecompress(Buffer,RawData,sizeof(RawData),"DecompressILBMRLE::Process(Buffer,sizeof(RawData),ILBMCompressed,1)");

	return uFailure;
}

//
// Test the ILBM Compression
//

static Word TestILBMCompress(void)
{
	Word uFailure = FALSE;

	// Perform a simple Compression test and test for buffer overrun
	CompressILBMRLE Tester;

	Compress::eError Error = Tester.Process(RawData,sizeof(RawData));
	if (Error!=Compress::COMPRESS_OKAY) {
		ReportFailure("CompressILBMRLE::Process(RawData,sizeof(RawData)) = %d, expected Compress::COMPRESS_OKAY",TRUE,Error);
		uFailure = TRUE;
	}
	Error = Tester.Finalize();
	if (Error!=Compress::COMPRESS_OKAY) {
		ReportFailure("CompressILBMRLE::Finalize() = %d, expected Compress::COMPRESS_OKAY",TRUE,Error);
		uFailure = TRUE;
	}
	if (Tester.GetOutputSize()!=sizeof(ILBMCompressed)) {
		ReportFailure("CompressILBMRLE::GetOutputSize() = %u, expected %u",TRUE,static_cast<Word>(Tester.GetOutputSize()),static_cast<Word>(sizeof(ILBMCompressed)));
		uFailure = TRUE;
	}
	OutputMemoryStream *pOutput = Tester.GetOutput();
	if (pOutput->Compare(ILBMCompressed,sizeof(ILBMCompressed))) {
		ReportFailure("CompressILBMRLE::GetOutput() data mismatch",TRUE);
		uFailure = TRUE;
	}

	// Perform test where the Compression is bisected by decompression buffer

	WordPtr uSplit = 0;
	do {
		Tester.Init();
		Error = Tester.Process(RawData,uSplit);
		if (Error!=Compress::COMPRESS_OKAY) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("CompressILBMRLE::Process(RawData,0x%s) = %d, expected Compress::COMPRESS_OKAY",TRUE,Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		Error = Tester.Process(RawData+uSplit,sizeof(RawData)-uSplit);
		if (Error!=Compress::COMPRESS_OKAY) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("CompressILBMRLE::Process(RawData+0x%s,sizeof(RawData)-0x%s) = %d, expected Compress::COMPRESS_OKAY",TRUE,Hex.GetPtr(),Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		Error = Tester.Finalize();
		if (Error!=Compress::COMPRESS_OKAY) {
			ReportFailure("CompressILBMRLE::Finalize(uSplit %04X) = %d, expected Compress::COMPRESS_OKAY",TRUE,static_cast<Word32>(uSplit),Error);
			uFailure = TRUE;
		}
		if (Tester.GetOutputSize()!=sizeof(ILBMCompressed)) {
			ReportFailure("CompressILBMRLE::GetOutputSize(uSplit %04X) = %u, expected %u",TRUE,static_cast<Word32>(uSplit),static_cast<Word>(Tester.GetOutputSize()),static_cast<Word>(sizeof(ILBMCompressed)));
			uFailure = TRUE;
		}
		pOutput = Tester.GetOutput();
		if (pOutput->Compare(ILBMCompressed,sizeof(ILBMCompressed))) {
			ReportFailure("CompressILBMRLE::GetOutput(uSplit %04X) data mismatch",TRUE,static_cast<Word32>(uSplit));
			uFailure = TRUE;
		}
	} while (++uSplit<sizeof(RawData));

	// Perform single byte output

	uSplit = 0;
	Tester.Init();
	do {
		Error = Tester.Process(RawData+uSplit,1);
		if (Error!=Compress::COMPRESS_OKAY) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("CompressILBMRLE::Process(RawData+0x%s,1) = %d, expected Compress::COMPRESS_OKAY",TRUE,Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
	} while (++uSplit<sizeof(RawData));
	Error = Tester.Finalize();
	if (Error!=Compress::COMPRESS_OKAY) {
		ReportFailure("CompressILBMRLE::Finalize(uSplit %04X) = %d, expected Compress::COMPRESS_OKAY",TRUE,static_cast<Word32>(uSplit),Error);
		uFailure = TRUE;
	}
	if (Tester.GetOutputSize()!=sizeof(ILBMCompressed)) {
		ReportFailure("CompressILBMRLE::GetOutputSize(uSplit %04X) = %u, expected %u",TRUE,static_cast<Word32>(uSplit),static_cast<Word>(Tester.GetOutputSize()),static_cast<Word>(sizeof(ILBMCompressed)));
		uFailure = TRUE;
	}
	pOutput = Tester.GetOutput();
	if (pOutput->Compare(ILBMCompressed,sizeof(ILBMCompressed))) {
		ReportFailure("CompressILBMRLE::GetOutput(uSplit %04X) data mismatch",TRUE,static_cast<Word32>(uSplit));
		uFailure = TRUE;
	}

	return uFailure;
}


//
// Test the LZSS Decompression
//

static Word TestLZSSDecompress(void)
{
	Word uFailure = FALSE;
	Word8 Buffer[sizeof(RawData)+80];

	// Perform a simple decompression test and test for buffer overrun
	DecompressLZSS Tester;

	MemoryFill(Buffer,0xD5,sizeof(Buffer));
	Decompress::eError Error = Tester.Process(Buffer,sizeof(RawData),LZSSCompressed,sizeof(LZSSCompressed));
	if (Error!=Decompress::DECOMPRESS_OKAY) {
		ReportFailure("DecompressLZSS::Process(Buffer,sizeof(RawData),LZSSCompressed,sizeof(LZSSCompressed)) = %d, expected Decompress::DECOMPRESS_OKAY",TRUE,Error);
		uFailure = TRUE;
	}
	uFailure |= ReportDecompress(Buffer,RawData,sizeof(RawData),"DecompressLZSS::Process(Buffer,sizeof(RawData),LZSSCompressed,sizeof(LZSSCompressed))");

	// Perform test where the decompression is bisected by decompression buffer

	WordPtr uSplit = 0;
	do {
		Tester.Reset();
		Error = Tester.Process(Buffer,uSplit,LZSSCompressed,sizeof(LZSSCompressed));
		WordPtr uBytesProcessed = Tester.GetProcessedInputSize();
		if (Error!=Decompress::DECOMPRESS_OUTPUTOVERRUN) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("DecompressLZSS::Process(Buffer,0x%s,LZSSCompressed,sizeof(LZSSCompressed)) = %d, expected Decompress::DECOMPRESS_OUTPUTOVERRUN",TRUE,Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		Error = Tester.Process(Buffer+uSplit,sizeof(RawData)-uSplit,LZSSCompressed+uBytesProcessed,sizeof(LZSSCompressed)-uBytesProcessed);
		if (Error!=Decompress::DECOMPRESS_OKAY) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("DecompressLZSS::Process(Buffer+0x%s,sizeof(RawData)-0x%s,LZSSCompressed+uBytesProcessed,sizeof(LZSSCompressed)-uBytesProcessed) = %d, expected Decompress::DECOMPRESS_OKAY",TRUE,Hex.GetPtr(),Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		uFailure |= ReportDecompress(Buffer,RawData,sizeof(RawData),"DecompressLZSS::Process(Buffer,sizeof(RawData),LZSSCompressed,sizeof(LZSSCompressed))");
	} while (++uSplit<sizeof(RawData));

	// Perform test where compressed data is bisected

	uSplit = 0;
	do {
		Tester.Reset();
		Error = Tester.Process(Buffer,sizeof(RawData),LZSSCompressed,uSplit);
		WordPtr uBytesProcessed = Tester.GetProcessedOutputSize();
		if (Error!=Decompress::DECOMPRESS_OUTPUTUNDERRUN) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("DecompressLZSS::Process(Buffer,sizeof(RawData),LZSSCompressed,0x%s) = %d, expected Decompress::DECOMPRESS_OUTPUTUNDERRUN",TRUE,Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		Error = Tester.Process(Buffer+uBytesProcessed,sizeof(RawData)-uBytesProcessed,LZSSCompressed+uSplit,sizeof(LZSSCompressed)-uSplit);
		if (Error!=Decompress::DECOMPRESS_OKAY) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("DecompressLZSS::Process(Buffer+uBytesProcessed,sizeof(RawData)-uBytesProcessed,LZSSCompressed+0x%s,sizeof(LZSSCompressed)-0x%s) = %d, expected Decompress::DECOMPRESS_OKAY",TRUE,Hex.GetPtr(),Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		uFailure |= ReportDecompress(Buffer,RawData,sizeof(RawData),"DecompressLZSS::Process(Buffer,sizeof(RawData),LZSSCompressed,sizeof(LZSSCompressed))");
	} while (++uSplit<sizeof(LZSSCompressed));

	// Perform single byte output
	uSplit = 0;
	Tester.Reset();
	WordPtr uStepper = 0;
	do {
		Error = Tester.Process(Buffer+uSplit,1,LZSSCompressed+uStepper,sizeof(LZSSCompressed)-uStepper);
		if (uSplit==(sizeof(RawData)-1)) {
			if (Error!=Decompress::DECOMPRESS_OKAY) {
				ReportFailure("DecompressLZSS::Process(Buffer,1,LZSSCompressed,uStepper) = %d, expected Decompress::DECOMPRESS_OKAY",TRUE,Error);
				uFailure = TRUE;
			}
		} else {
			if (Error!=Decompress::DECOMPRESS_OUTPUTOVERRUN) {
				ReportFailure("DecompressLZSS::Process(Buffer,1,LZSSCompressed,uStepper) = %d, expected Decompress::DECOMPRESS_OUTPUTOVERRUN",TRUE,Error);
				uFailure = TRUE;
			}

		}
		uStepper += Tester.GetProcessedInputSize();
	} while (++uSplit<sizeof(RawData));
	uFailure |= ReportDecompress(Buffer,RawData,sizeof(RawData),"DecompressLZSS::Process(Buffer,1,LZSSCompressed,sizeof(LZSSCompressed))");

	// Perform single byte input
	uSplit = 0;
	Tester.Reset();
	uStepper = 0;
	do {
		Error = Tester.Process(Buffer+uStepper,sizeof(RawData)-uStepper,LZSSCompressed+uSplit,1);
		if (uSplit==(sizeof(LZSSCompressed)-1)) {
			if (Error!=Decompress::DECOMPRESS_OKAY) {
				ReportFailure("DecompressLZSS::Process(Buffer,uStepper,LZSSCompressed,1) = %d, expected Decompress::DECOMPRESS_OKAY",TRUE,Error);
				uFailure = TRUE;
			}
		} else {
			if (Error!=Decompress::DECOMPRESS_OUTPUTUNDERRUN) {
				ReportFailure("DecompressLZSS::Process(Buffer,uStepper,LZSSCompressed,1) = %d, expected Decompress::DECOMPRESS_OUTPUTUNDERRUN",TRUE,Error);
				uFailure = TRUE;
			}

		}
		uStepper += Tester.GetProcessedOutputSize();
	} while (++uSplit<sizeof(LZSSCompressed));
	uFailure |= ReportDecompress(Buffer,RawData,sizeof(RawData),"DecompressLZSS::Process(Buffer,sizeof(RawData),LZSSCompressed,1)");

	return uFailure;
}


//
// Test the LZSS Compression
//

static Word TestLZSSCompress(void)
{
	Word uFailure = FALSE;

	// Perform a simple Compression test and test for buffer overrun
	CompressLZSS Tester;

	Compress::eError Error = Tester.Process(RawData,sizeof(RawData));
	if (Error!=Compress::COMPRESS_OKAY) {
		ReportFailure("CompressLZSS::Process(RawData,sizeof(RawData)) = %d, expected Compress::COMPRESS_OKAY",TRUE,Error);
		uFailure = TRUE;
	}
	Error = Tester.Finalize();
	if (Error!=Compress::COMPRESS_OKAY) {
		ReportFailure("CompressLZSS::Finalize() = %d, expected Compress::COMPRESS_OKAY",TRUE,Error);
		uFailure = TRUE;
	}
	if (Tester.GetOutputSize()!=sizeof(LZSSCompressed)) {
		ReportFailure("CompressLZSS::GetOutputSize() = %u, expected %u",TRUE,static_cast<Word>(Tester.GetOutputSize()),static_cast<Word>(sizeof(LZSSCompressed)));
		uFailure = TRUE;
	}
	OutputMemoryStream *pOutput = Tester.GetOutput();
	if (pOutput->Compare(LZSSCompressed,sizeof(LZSSCompressed))) {
		ReportFailure("CompressLZSS::GetOutput() data mismatch",TRUE);
		uFailure = TRUE;
	}

	// Perform test where the Compression is bisected by decompression buffer

	WordPtr uSplit = 0;
	do {
		Tester.Init();
		Error = Tester.Process(RawData,uSplit);
		if (Error!=Compress::COMPRESS_OKAY) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("CompressLZSS::Process(RawData,0x%s) = %d, expected Compress::COMPRESS_OKAY",TRUE,Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		Error = Tester.Process(RawData+uSplit,sizeof(RawData)-uSplit);
		if (Error!=Compress::COMPRESS_OKAY) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("CompressLZSS::Process(RawData+0x%s,sizeof(RawData)-0x%s) = %d, expected Compress::COMPRESS_OKAY",TRUE,Hex.GetPtr(),Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		Error = Tester.Finalize();
		if (Error!=Compress::COMPRESS_OKAY) {
			ReportFailure("CompressLZSS::Finalize(uSplit %04X) = %d, expected Compress::COMPRESS_OKAY",TRUE,static_cast<Word32>(uSplit),Error);
			uFailure = TRUE;
		}
		if (Tester.GetOutputSize()!=sizeof(LZSSCompressed)) {
			ReportFailure("CompressLZSS::GetOutputSize(uSplit %04X) = %u, expected %u",TRUE,static_cast<Word32>(uSplit),static_cast<Word>(Tester.GetOutputSize()),static_cast<Word>(sizeof(LZSSCompressed)));
			uFailure = TRUE;
		}
		pOutput = Tester.GetOutput();
		if (pOutput->Compare(LZSSCompressed,sizeof(LZSSCompressed))) {
			ReportFailure("CompressLZSS::GetOutput(uSplit %04X) data mismatch",TRUE,static_cast<Word32>(uSplit));
			uFailure = TRUE;
		}
	} while (++uSplit<sizeof(RawData));

	// Perform single byte output

	uSplit = 0;
	Tester.Init();
	do {
		Error = Tester.Process(RawData+uSplit,1);
		if (Error!=Compress::COMPRESS_OKAY) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("CompressLZSS::Process(RawData+0x%s,1) = %d, expected Compress::COMPRESS_OKAY",TRUE,Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
	} while (++uSplit<sizeof(RawData));
	Error = Tester.Finalize();
	if (Error!=Compress::COMPRESS_OKAY) {
		ReportFailure("CompressLZSS::Finalize(uSplit %04X) = %d, expected Compress::COMPRESS_OKAY",TRUE,static_cast<Word32>(uSplit),Error);
		uFailure = TRUE;
	}
	if (Tester.GetOutputSize()!=sizeof(LZSSCompressed)) {
		ReportFailure("CompressLZSS::GetOutputSize(uSplit %04X) = %u, expected %u",TRUE,static_cast<Word32>(uSplit),static_cast<Word>(Tester.GetOutputSize()),static_cast<Word>(sizeof(LZSSCompressed)));
		uFailure = TRUE;
	}
	pOutput = Tester.GetOutput();
	if (pOutput->Compare(LZSSCompressed,sizeof(LZSSCompressed))) {
		ReportFailure("CompressLZSS::GetOutput(uSplit %04X) data mismatch",TRUE,static_cast<Word32>(uSplit));
		uFailure = TRUE;
	}

	return uFailure;
}


//
// Test the Deflate Decompression
//

static Word TestDeflateDecompress(void)
{
	Word uFailure = FALSE;
	Word8 Buffer[sizeof(RawData)+80];

	// Perform a simple decompression test and test for buffer overrun
	DecompressDeflate Tester;

	MemoryFill(Buffer,0xD5,sizeof(Buffer));
	Decompress::eError Error = Tester.Process(Buffer,sizeof(RawData),DeflateCompressed,sizeof(DeflateCompressed));
	if (Error!=Decompress::DECOMPRESS_OKAY) {
		ReportFailure("DecompressDeflate::Process(Buffer,sizeof(RawData),DeflateCompressed,sizeof(DeflateCompressed)) = %d, expected Decompress::DECOMPRESS_OKAY",TRUE,Error);
		uFailure = TRUE;
	}
	uFailure |= ReportDecompress(Buffer,RawData,sizeof(RawData),"DecompressDeflate::Process(Buffer,sizeof(RawData),DeflateCompressed,sizeof(DeflateCompressed))");

	// Perform test where the decompression is bisected by decompression buffer

	WordPtr uSplit = 0;
	do {
		Tester.Reset();
		Error = Tester.Process(Buffer,uSplit,DeflateCompressed,sizeof(DeflateCompressed));
		WordPtr uBytesProcessed = Tester.GetProcessedInputSize();
		if (Error!=Decompress::DECOMPRESS_OUTPUTOVERRUN) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("DecompressDeflate::Process(Buffer,0x%s,DeflateCompressed,sizeof(DeflateCompressed)) = %d, expected Decompress::DECOMPRESS_OUTPUTOVERRUN",TRUE,Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		Error = Tester.Process(Buffer+uSplit,sizeof(RawData)-uSplit,DeflateCompressed+uBytesProcessed,sizeof(DeflateCompressed)-uBytesProcessed);
		if (Error!=Decompress::DECOMPRESS_OKAY) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("DecompressDeflate::Process(Buffer+0x%s,sizeof(RawData)-0x%s,DeflateCompressed+uBytesProcessed,sizeof(DeflateCompressed)-uBytesProcessed) = %d, expected Decompress::DECOMPRESS_OKAY",TRUE,Hex.GetPtr(),Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		uFailure |= ReportDecompress(Buffer,RawData,sizeof(RawData),"DecompressDeflate::Process(Buffer,sizeof(RawData),DeflateCompressed,sizeof(DeflateCompressed))");
	} while (++uSplit<sizeof(RawData));

	// Perform test where compressed data is bisected

	uSplit = 0;
	do {
		Tester.Reset();
		Error = Tester.Process(Buffer,sizeof(RawData),DeflateCompressed,uSplit);
		WordPtr uBytesProcessed = Tester.GetProcessedOutputSize();
		if (Error!=Decompress::DECOMPRESS_OUTPUTUNDERRUN) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("DecompressDeflate::Process(Buffer,sizeof(RawData),DeflateCompressed,0x%s) = %d, expected Decompress::DECOMPRESS_OUTPUTUNDERRUN",TRUE,Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		Error = Tester.Process(Buffer+uBytesProcessed,sizeof(RawData)-uBytesProcessed,DeflateCompressed+uSplit,sizeof(DeflateCompressed)-uSplit);
		if (Error!=Decompress::DECOMPRESS_OKAY) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("DecompressDeflate::Process(Buffer+uBytesProcessed,sizeof(RawData)-uBytesProcessed,DeflateCompressed+0x%s,sizeof(DeflateCompressed)-0x%s) = %d, expected Decompress::DECOMPRESS_OKAY",TRUE,Hex.GetPtr(),Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		uFailure |= ReportDecompress(Buffer,RawData,sizeof(RawData),"DecompressDeflate::Process(Buffer,sizeof(RawData),DeflateCompressed,sizeof(DeflateCompressed))");
	} while (++uSplit<sizeof(DeflateCompressed));

	// Perform single byte output
	uSplit = 0;
	Tester.Reset();
	WordPtr uStepper = 0;
	do {
		Error = Tester.Process(Buffer+uSplit,1,DeflateCompressed+uStepper,sizeof(DeflateCompressed)-uStepper);
		if (uSplit==(sizeof(RawData)-1)) {
			if (Error!=Decompress::DECOMPRESS_OKAY) {
				ReportFailure("DecompressDeflate::Process(Buffer,1,DeflateCompressed,uStepper) = %d, expected Decompress::DECOMPRESS_OKAY",TRUE,Error);
				uFailure = TRUE;
			}
		} else {
			if (Error!=Decompress::DECOMPRESS_OUTPUTOVERRUN) {
				ReportFailure("DecompressDeflate::Process(Buffer,1,DeflateCompressed,uStepper) = %d, expected Decompress::DECOMPRESS_OUTPUTOVERRUN",TRUE,Error);
				uFailure = TRUE;
			}

		}
		uStepper += Tester.GetProcessedInputSize();
	} while (++uSplit<sizeof(RawData));
	uFailure |= ReportDecompress(Buffer,RawData,sizeof(RawData),"DecompressDeflate::Process(Buffer,1,DeflateCompressed,sizeof(DeflateCompressed))");

	// Perform single byte input
	uSplit = 0;
	Tester.Reset();
	uStepper = 0;
	do {
		Error = Tester.Process(Buffer+uStepper,sizeof(RawData)-uStepper,DeflateCompressed+uSplit,1);
		if (uSplit==(sizeof(DeflateCompressed)-1)) {
			if (Error!=Decompress::DECOMPRESS_OKAY) {
				ReportFailure("DecompressDeflate::Process(Buffer,uStepper,DeflateCompressed,1) = %d, expected Decompress::DECOMPRESS_OKAY",TRUE,Error);
				uFailure = TRUE;
			}
		} else {
			if (Error!=Decompress::DECOMPRESS_OUTPUTUNDERRUN) {
				ReportFailure("DecompressDeflate::Process(Buffer,uStepper,DeflateCompressed,1) = %d, expected Decompress::DECOMPRESS_OUTPUTUNDERRUN",TRUE,Error);
				uFailure = TRUE;
			}

		}
		uStepper += Tester.GetProcessedOutputSize();
	} while (++uSplit<sizeof(DeflateCompressed));
	uFailure |= ReportDecompress(Buffer,RawData,sizeof(RawData),"DecompressDeflate::Process(Buffer,sizeof(RawData),DeflateCompressed,1)");

	return uFailure;
}


//
// Test the Deflate Compression
// Note: Due to the size of the CompressDeflate class (384K),
// it needs to be allocated at runtime to prevent stack overflow
//

static Word TestDeflateCompress(void)
{
	Word uFailure = FALSE;

	// Perform a simple Compression test and test for buffer overrun
	CompressDeflate *pTester = New<CompressDeflate>();

	Compress::eError Error = pTester->Process(RawData,sizeof(RawData));
	if (Error!=Compress::COMPRESS_OKAY) {
		ReportFailure("CompressDeflate::Process(RawData,sizeof(RawData)) = %d, expected Compress::COMPRESS_OKAY",TRUE,Error);
		uFailure = TRUE;
	}
	Error = pTester->Finalize();
	if (Error!=Compress::COMPRESS_OKAY) {
		ReportFailure("CompressDeflate::Finalize() = %d, expected Compress::COMPRESS_OKAY",TRUE,Error);
		uFailure = TRUE;
	}
	if (pTester->GetOutputSize()!=sizeof(DeflateCompressed)) {
		ReportFailure("CompressDeflate::GetOutputSize() = %u, expected %u",TRUE,static_cast<Word>(pTester->GetOutputSize()),static_cast<Word>(sizeof(DeflateCompressed)));
		uFailure = TRUE;
	}
	OutputMemoryStream *pOutput = pTester->GetOutput();
	if (pOutput->Compare(DeflateCompressed,sizeof(DeflateCompressed))) {
		ReportFailure("CompressDeflate::GetOutput() data mismatch",TRUE);
		uFailure = TRUE;
	}

	// Perform test where the Compression is bisected by decompression buffer

	WordPtr uSplit = 0;
	do {
		pTester->Init();
		Error = pTester->Process(RawData,uSplit);
		if (Error!=Compress::COMPRESS_OKAY) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("CompressDeflate::Process(RawData,0x%s) = %d, expected Compress::COMPRESS_OKAY",TRUE,Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		Error = pTester->Process(RawData+uSplit,sizeof(RawData)-uSplit);
		if (Error!=Compress::COMPRESS_OKAY) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("CompressDeflate::Process(RawData+0x%s,sizeof(RawData)-0x%s) = %d, expected Compress::COMPRESS_OKAY",TRUE,Hex.GetPtr(),Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
		Error = pTester->Finalize();
		if (Error!=Compress::COMPRESS_OKAY) {
			ReportFailure("CompressDeflate::Finalize(uSplit %04X) = %d, expected Compress::COMPRESS_OKAY",TRUE,static_cast<Word32>(uSplit),Error);
			uFailure = TRUE;
		}
		if (pTester->GetOutputSize()!=sizeof(DeflateCompressed)) {
			ReportFailure("CompressDeflate::GetOutputSize(uSplit %04X) = %u, expected %u",TRUE,static_cast<Word32>(uSplit),static_cast<Word>(pTester->GetOutputSize()),static_cast<Word>(sizeof(DeflateCompressed)));
			uFailure = TRUE;
		}
		pOutput = pTester->GetOutput();
		if (pOutput->Compare(DeflateCompressed,sizeof(DeflateCompressed))) {
			ReportFailure("CompressDeflate::GetOutput(uSplit %04X) data mismatch",TRUE,static_cast<Word32>(uSplit));
			uFailure = TRUE;
		}
	} while (++uSplit<sizeof(RawData));

	// Perform single byte output

	uSplit = 0;
	pTester->Init();
	do {
		Error = pTester->Process(RawData+uSplit,1);
		if (Error!=Compress::COMPRESS_OKAY) {
			NumberStringHex Hex(static_cast<Word32>(uSplit),LEADINGZEROS+4);
			ReportFailure("CompressDeflate::Process(RawData+0x%s,1) = %d, expected Compress::COMPRESS_OKAY",TRUE,Hex.GetPtr(),Error);
			uFailure = TRUE;
		}
	} while (++uSplit<sizeof(RawData));
	Error = pTester->Finalize();
	if (Error!=Compress::COMPRESS_OKAY) {
		ReportFailure("CompressDeflate::Finalize(uSplit %04X) = %d, expected Compress::COMPRESS_OKAY",TRUE,static_cast<Word32>(uSplit),Error);
		uFailure = TRUE;
	}
	if (pTester->GetOutputSize()!=sizeof(DeflateCompressed)) {
		ReportFailure("CompressDeflate::GetOutputSize(uSplit %04X) = %u, expected %u",TRUE,static_cast<Word32>(uSplit),static_cast<Word>(pTester->GetOutputSize()),static_cast<Word>(sizeof(DeflateCompressed)));
		uFailure = TRUE;
	}
	pOutput = pTester->GetOutput();
	if (pOutput->Compare(DeflateCompressed,sizeof(DeflateCompressed))) {
		ReportFailure("CompressDeflate::GetOutput(uSplit %04X) data mismatch",TRUE,static_cast<Word32>(uSplit));
		uFailure = TRUE;
	}
	Delete(pTester);
	return uFailure;
}

//
// Test compression code
//

int BURGER_API TestBrcompression(void)
{
	MemoryManagerGlobalANSI Memory;
	Message("Running Compression tests");
	Word uResult = TestILBMDecompress();
	uResult |= TestILBMCompress();
	uResult |= TestLZSSDecompress();
	uResult |= TestLZSSCompress();
	uResult |= TestDeflateDecompress();
	uResult |= TestDeflateCompress();
	return static_cast<int>(uResult);
}
