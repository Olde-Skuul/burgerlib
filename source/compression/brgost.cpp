/***************************************

	GOST hash manager

	Implemented following the documentation found in
	https://en.wikipedia.org/wiki/GOST_(hash_function)
	and http://tools.ietf.org/html/rfc5831

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brgost.h"
#include "brendian.h"
#include "brstringfunctions.h"
#include "brfixedpoint.h"

/*! ************************************

	\brief Test parameters S-Box, as copied from Wikipedia

	GOST test parameters for an S-Box. 
	Full documentation on this hash format can be found here
	https://en.wikipedia.org/wiki/GOST_(hash_function)

	\sa GOSTHasher_t

***************************************/

const Word32 Burger::GOSTHasher_t::g_TestParmsSBox[8][16] = {
	{4,10, 9, 2,13, 8, 0,14, 6,11, 1,12, 7,15, 5, 3},
	{14,11, 4,12, 6,13,15,10, 2, 3, 8, 1, 0, 7, 5, 9},
	{5, 8, 1,13,10, 3, 4, 2,14,15,12, 7, 6, 0, 9,11},
	{7,13,10, 1, 0, 8, 9,15,14, 4, 6,12,11, 2, 5, 3},
	{6,12, 7, 1, 5,15,13, 8, 4,10, 9,14, 0, 3,11, 2},
	{4,11,10, 0, 7, 2, 1,13, 3, 6, 8, 5, 9,12,15,14},
	{13,11, 4, 1, 3,15, 5, 9, 0,10,14, 7, 6, 8, 2,12},
	{1,15,13, 0, 5, 7,10, 4, 9, 2, 3,14, 6,11, 8,12}
};


/*! ************************************

	\brief Precalculated SBoxTable using g_TestParmsSBox input

	Full documentation on this hash format can be found here
	https://en.wikipedia.org/wiki/GOST_(hash_function)

	\sa GOSTHasher_t

***************************************/

const Word32 Burger::GOSTHasher_t::g_SBoxTable[4][256] = {
	{0x00072000,0x00075000,0x00074800,0x00071000,0x00076800,0x00074000,0x00070000,0x00077000,
	0x00073000,0x00075800,0x00070800,0x00076000,0x00073800,0x00077800,0x00072800,0x00071800,
	0x0005A000,0x0005D000,0x0005C800,0x00059000,0x0005E800,0x0005C000,0x00058000,0x0005F000,
	0x0005B000,0x0005D800,0x00058800,0x0005E000,0x0005B800,0x0005F800,0x0005A800,0x00059800,
	0x00022000,0x00025000,0x00024800,0x00021000,0x00026800,0x00024000,0x00020000,0x00027000,
	0x00023000,0x00025800,0x00020800,0x00026000,0x00023800,0x00027800,0x00022800,0x00021800,
	0x00062000,0x00065000,0x00064800,0x00061000,0x00066800,0x00064000,0x00060000,0x00067000,
	0x00063000,0x00065800,0x00060800,0x00066000,0x00063800,0x00067800,0x00062800,0x00061800,
	0x00032000,0x00035000,0x00034800,0x00031000,0x00036800,0x00034000,0x00030000,0x00037000,
	0x00033000,0x00035800,0x00030800,0x00036000,0x00033800,0x00037800,0x00032800,0x00031800,
	0x0006A000,0x0006D000,0x0006C800,0x00069000,0x0006E800,0x0006C000,0x00068000,0x0006F000,
	0x0006B000,0x0006D800,0x00068800,0x0006E000,0x0006B800,0x0006F800,0x0006A800,0x00069800,
	0x0007A000,0x0007D000,0x0007C800,0x00079000,0x0007E800,0x0007C000,0x00078000,0x0007F000,
	0x0007B000,0x0007D800,0x00078800,0x0007E000,0x0007B800,0x0007F800,0x0007A800,0x00079800,
	0x00052000,0x00055000,0x00054800,0x00051000,0x00056800,0x00054000,0x00050000,0x00057000,
	0x00053000,0x00055800,0x00050800,0x00056000,0x00053800,0x00057800,0x00052800,0x00051800,
	0x00012000,0x00015000,0x00014800,0x00011000,0x00016800,0x00014000,0x00010000,0x00017000,
	0x00013000,0x00015800,0x00010800,0x00016000,0x00013800,0x00017800,0x00012800,0x00011800,
	0x0001A000,0x0001D000,0x0001C800,0x00019000,0x0001E800,0x0001C000,0x00018000,0x0001F000,
	0x0001B000,0x0001D800,0x00018800,0x0001E000,0x0001B800,0x0001F800,0x0001A800,0x00019800,
	0x00042000,0x00045000,0x00044800,0x00041000,0x00046800,0x00044000,0x00040000,0x00047000,
	0x00043000,0x00045800,0x00040800,0x00046000,0x00043800,0x00047800,0x00042800,0x00041800,
	0x0000A000,0x0000D000,0x0000C800,0x00009000,0x0000E800,0x0000C000,0x00008000,0x0000F000,
	0x0000B000,0x0000D800,0x00008800,0x0000E000,0x0000B800,0x0000F800,0x0000A800,0x00009800,
	0x00002000,0x00005000,0x00004800,0x00001000,0x00006800,0x00004000,0x00000000,0x00007000,
	0x00003000,0x00005800,0x00000800,0x00006000,0x00003800,0x00007800,0x00002800,0x00001800,
	0x0003A000,0x0003D000,0x0003C800,0x00039000,0x0003E800,0x0003C000,0x00038000,0x0003F000,
	0x0003B000,0x0003D800,0x00038800,0x0003E000,0x0003B800,0x0003F800,0x0003A800,0x00039800,
	0x0002A000,0x0002D000,0x0002C800,0x00029000,0x0002E800,0x0002C000,0x00028000,0x0002F000,
	0x0002B000,0x0002D800,0x00028800,0x0002E000,0x0002B800,0x0002F800,0x0002A800,0x00029800,
	0x0004A000,0x0004D000,0x0004C800,0x00049000,0x0004E800,0x0004C000,0x00048000,0x0004F000,
	0x0004B000,0x0004D800,0x00048800,0x0004E000,0x0004B800,0x0004F800,0x0004A800,0x00049800},
	{0x03A80000,0x03C00000,0x03880000,0x03E80000,0x03D00000,0x03980000,0x03A00000,0x03900000,
	0x03F00000,0x03F80000,0x03E00000,0x03B80000,0x03B00000,0x03800000,0x03C80000,0x03D80000,
	0x06A80000,0x06C00000,0x06880000,0x06E80000,0x06D00000,0x06980000,0x06A00000,0x06900000,
	0x06F00000,0x06F80000,0x06E00000,0x06B80000,0x06B00000,0x06800000,0x06C80000,0x06D80000,
	0x05280000,0x05400000,0x05080000,0x05680000,0x05500000,0x05180000,0x05200000,0x05100000,
	0x05700000,0x05780000,0x05600000,0x05380000,0x05300000,0x05000000,0x05480000,0x05580000,
	0x00A80000,0x00C00000,0x00880000,0x00E80000,0x00D00000,0x00980000,0x00A00000,0x00900000,
	0x00F00000,0x00F80000,0x00E00000,0x00B80000,0x00B00000,0x00800000,0x00C80000,0x00D80000,
	0x00280000,0x00400000,0x00080000,0x00680000,0x00500000,0x00180000,0x00200000,0x00100000,
	0x00700000,0x00780000,0x00600000,0x00380000,0x00300000,0x00000000,0x00480000,0x00580000,
	0x04280000,0x04400000,0x04080000,0x04680000,0x04500000,0x04180000,0x04200000,0x04100000,
	0x04700000,0x04780000,0x04600000,0x04380000,0x04300000,0x04000000,0x04480000,0x04580000,
	0x04A80000,0x04C00000,0x04880000,0x04E80000,0x04D00000,0x04980000,0x04A00000,0x04900000,
	0x04F00000,0x04F80000,0x04E00000,0x04B80000,0x04B00000,0x04800000,0x04C80000,0x04D80000,
	0x07A80000,0x07C00000,0x07880000,0x07E80000,0x07D00000,0x07980000,0x07A00000,0x07900000,
	0x07F00000,0x07F80000,0x07E00000,0x07B80000,0x07B00000,0x07800000,0x07C80000,0x07D80000,
	0x07280000,0x07400000,0x07080000,0x07680000,0x07500000,0x07180000,0x07200000,0x07100000,
	0x07700000,0x07780000,0x07600000,0x07380000,0x07300000,0x07000000,0x07480000,0x07580000,
	0x02280000,0x02400000,0x02080000,0x02680000,0x02500000,0x02180000,0x02200000,0x02100000,
	0x02700000,0x02780000,0x02600000,0x02380000,0x02300000,0x02000000,0x02480000,0x02580000,
	0x03280000,0x03400000,0x03080000,0x03680000,0x03500000,0x03180000,0x03200000,0x03100000,
	0x03700000,0x03780000,0x03600000,0x03380000,0x03300000,0x03000000,0x03480000,0x03580000,
	0x06280000,0x06400000,0x06080000,0x06680000,0x06500000,0x06180000,0x06200000,0x06100000,
	0x06700000,0x06780000,0x06600000,0x06380000,0x06300000,0x06000000,0x06480000,0x06580000,
	0x05A80000,0x05C00000,0x05880000,0x05E80000,0x05D00000,0x05980000,0x05A00000,0x05900000,
	0x05F00000,0x05F80000,0x05E00000,0x05B80000,0x05B00000,0x05800000,0x05C80000,0x05D80000,
	0x01280000,0x01400000,0x01080000,0x01680000,0x01500000,0x01180000,0x01200000,0x01100000,
	0x01700000,0x01780000,0x01600000,0x01380000,0x01300000,0x01000000,0x01480000,0x01580000,
	0x02A80000,0x02C00000,0x02880000,0x02E80000,0x02D00000,0x02980000,0x02A00000,0x02900000,
	0x02F00000,0x02F80000,0x02E00000,0x02B80000,0x02B00000,0x02800000,0x02C80000,0x02D80000,
	0x01A80000,0x01C00000,0x01880000,0x01E80000,0x01D00000,0x01980000,0x01A00000,0x01900000,
	0x01F00000,0x01F80000,0x01E00000,0x01B80000,0x01B00000,0x01800000,0x01C80000,0x01D80000},
	{0x30000002,0x60000002,0x38000002,0x08000002,0x28000002,0x78000002,0x68000002,0x40000002,
	0x20000002,0x50000002,0x48000002,0x70000002,0x00000002,0x18000002,0x58000002,0x10000002,
	0xB0000005,0xE0000005,0xB8000005,0x88000005,0xA8000005,0xF8000005,0xE8000005,0xC0000005,
	0xA0000005,0xD0000005,0xC8000005,0xF0000005,0x80000005,0x98000005,0xD8000005,0x90000005,
	0x30000005,0x60000005,0x38000005,0x08000005,0x28000005,0x78000005,0x68000005,0x40000005,
	0x20000005,0x50000005,0x48000005,0x70000005,0x00000005,0x18000005,0x58000005,0x10000005,
	0x30000000,0x60000000,0x38000000,0x08000000,0x28000000,0x78000000,0x68000000,0x40000000,
	0x20000000,0x50000000,0x48000000,0x70000000,0x00000000,0x18000000,0x58000000,0x10000000,
	0xB0000003,0xE0000003,0xB8000003,0x88000003,0xA8000003,0xF8000003,0xE8000003,0xC0000003,
	0xA0000003,0xD0000003,0xC8000003,0xF0000003,0x80000003,0x98000003,0xD8000003,0x90000003,
	0x30000001,0x60000001,0x38000001,0x08000001,0x28000001,0x78000001,0x68000001,0x40000001,
	0x20000001,0x50000001,0x48000001,0x70000001,0x00000001,0x18000001,0x58000001,0x10000001,
	0xB0000000,0xE0000000,0xB8000000,0x88000000,0xA8000000,0xF8000000,0xE8000000,0xC0000000,
	0xA0000000,0xD0000000,0xC8000000,0xF0000000,0x80000000,0x98000000,0xD8000000,0x90000000,
	0xB0000006,0xE0000006,0xB8000006,0x88000006,0xA8000006,0xF8000006,0xE8000006,0xC0000006,
	0xA0000006,0xD0000006,0xC8000006,0xF0000006,0x80000006,0x98000006,0xD8000006,0x90000006,
	0xB0000001,0xE0000001,0xB8000001,0x88000001,0xA8000001,0xF8000001,0xE8000001,0xC0000001,
	0xA0000001,0xD0000001,0xC8000001,0xF0000001,0x80000001,0x98000001,0xD8000001,0x90000001,
	0x30000003,0x60000003,0x38000003,0x08000003,0x28000003,0x78000003,0x68000003,0x40000003,
	0x20000003,0x50000003,0x48000003,0x70000003,0x00000003,0x18000003,0x58000003,0x10000003,
	0x30000004,0x60000004,0x38000004,0x08000004,0x28000004,0x78000004,0x68000004,0x40000004,
	0x20000004,0x50000004,0x48000004,0x70000004,0x00000004,0x18000004,0x58000004,0x10000004,
	0xB0000002,0xE0000002,0xB8000002,0x88000002,0xA8000002,0xF8000002,0xE8000002,0xC0000002,
	0xA0000002,0xD0000002,0xC8000002,0xF0000002,0x80000002,0x98000002,0xD8000002,0x90000002,
	0xB0000004,0xE0000004,0xB8000004,0x88000004,0xA8000004,0xF8000004,0xE8000004,0xC0000004,
	0xA0000004,0xD0000004,0xC8000004,0xF0000004,0x80000004,0x98000004,0xD8000004,0x90000004,
	0x30000006,0x60000006,0x38000006,0x08000006,0x28000006,0x78000006,0x68000006,0x40000006,
	0x20000006,0x50000006,0x48000006,0x70000006,0x00000006,0x18000006,0x58000006,0x10000006,
	0xB0000007,0xE0000007,0xB8000007,0x88000007,0xA8000007,0xF8000007,0xE8000007,0xC0000007,
	0xA0000007,0xD0000007,0xC8000007,0xF0000007,0x80000007,0x98000007,0xD8000007,0x90000007,
	0x30000007,0x60000007,0x38000007,0x08000007,0x28000007,0x78000007,0x68000007,0x40000007,
	0x20000007,0x50000007,0x48000007,0x70000007,0x00000007,0x18000007,0x58000007,0x10000007},
	{0x000000E8,0x000000D8,0x000000A0,0x00000088,0x00000098,0x000000F8,0x000000A8,0x000000C8,
	0x00000080,0x000000D0,0x000000F0,0x000000B8,0x000000B0,0x000000C0,0x00000090,0x000000E0,
	0x000007E8,0x000007D8,0x000007A0,0x00000788,0x00000798,0x000007F8,0x000007A8,0x000007C8,
	0x00000780,0x000007D0,0x000007F0,0x000007B8,0x000007B0,0x000007C0,0x00000790,0x000007E0,
	0x000006E8,0x000006D8,0x000006A0,0x00000688,0x00000698,0x000006F8,0x000006A8,0x000006C8,
	0x00000680,0x000006D0,0x000006F0,0x000006B8,0x000006B0,0x000006C0,0x00000690,0x000006E0,
	0x00000068,0x00000058,0x00000020,0x00000008,0x00000018,0x00000078,0x00000028,0x00000048,
	0x00000000,0x00000050,0x00000070,0x00000038,0x00000030,0x00000040,0x00000010,0x00000060,
	0x000002E8,0x000002D8,0x000002A0,0x00000288,0x00000298,0x000002F8,0x000002A8,0x000002C8,
	0x00000280,0x000002D0,0x000002F0,0x000002B8,0x000002B0,0x000002C0,0x00000290,0x000002E0,
	0x000003E8,0x000003D8,0x000003A0,0x00000388,0x00000398,0x000003F8,0x000003A8,0x000003C8,
	0x00000380,0x000003D0,0x000003F0,0x000003B8,0x000003B0,0x000003C0,0x00000390,0x000003E0,
	0x00000568,0x00000558,0x00000520,0x00000508,0x00000518,0x00000578,0x00000528,0x00000548,
	0x00000500,0x00000550,0x00000570,0x00000538,0x00000530,0x00000540,0x00000510,0x00000560,
	0x00000268,0x00000258,0x00000220,0x00000208,0x00000218,0x00000278,0x00000228,0x00000248,
	0x00000200,0x00000250,0x00000270,0x00000238,0x00000230,0x00000240,0x00000210,0x00000260,
	0x000004E8,0x000004D8,0x000004A0,0x00000488,0x00000498,0x000004F8,0x000004A8,0x000004C8,
	0x00000480,0x000004D0,0x000004F0,0x000004B8,0x000004B0,0x000004C0,0x00000490,0x000004E0,
	0x00000168,0x00000158,0x00000120,0x00000108,0x00000118,0x00000178,0x00000128,0x00000148,
	0x00000100,0x00000150,0x00000170,0x00000138,0x00000130,0x00000140,0x00000110,0x00000160,
	0x000001E8,0x000001D8,0x000001A0,0x00000188,0x00000198,0x000001F8,0x000001A8,0x000001C8,
	0x00000180,0x000001D0,0x000001F0,0x000001B8,0x000001B0,0x000001C0,0x00000190,0x000001E0,
	0x00000768,0x00000758,0x00000720,0x00000708,0x00000718,0x00000778,0x00000728,0x00000748,
	0x00000700,0x00000750,0x00000770,0x00000738,0x00000730,0x00000740,0x00000710,0x00000760,
	0x00000368,0x00000358,0x00000320,0x00000308,0x00000318,0x00000378,0x00000328,0x00000348,
	0x00000300,0x00000350,0x00000370,0x00000338,0x00000330,0x00000340,0x00000310,0x00000360,
	0x000005E8,0x000005D8,0x000005A0,0x00000588,0x00000598,0x000005F8,0x000005A8,0x000005C8,
	0x00000580,0x000005D0,0x000005F0,0x000005B8,0x000005B0,0x000005C0,0x00000590,0x000005E0,
	0x00000468,0x00000458,0x00000420,0x00000408,0x00000418,0x00000478,0x00000428,0x00000448,
	0x00000400,0x00000450,0x00000470,0x00000438,0x00000430,0x00000440,0x00000410,0x00000460,
	0x00000668,0x00000658,0x00000620,0x00000608,0x00000618,0x00000678,0x00000628,0x00000648,
	0x00000600,0x00000650,0x00000670,0x00000638,0x00000630,0x00000640,0x00000610,0x00000660}
};

/*! ************************************

	\struct Burger::GOST_t
	\brief 32 byte array to contain a GOST hash
	
	Full documentation on this hash format can be found here
	https://en.wikipedia.org/wiki/GOST_(hash_function)

	\sa Hash(GOST_t *,const void *,WordPtr) and Burger::GOSTHasher_t

***************************************/

/*! ************************************

	\struct Burger::GOSTHasher_t
	\brief Multi-pass GOST hash generator

	Full documentation on this hash format can be found here
	https://en.wikipedia.org/wiki/GOST_(hash_function)

	This structure is needed to perform a multi-pass GOST hash
	and contains cached data and a running checksum.
	\code
		Burger::GOSTHasher_t Context;
		// Initialize
		Context.Init();
		// Process data in passes
		Context.Process(Buffer1,sizeof(Buffer1));
		Context.Process(Buffer2,sizeof(Buffer2));
		Context.Process(Buffer3,sizeof(Buffer3));
		// Wrap up the processing
		Context.Finalize();
		// Return the resulting hash
		MemoryCopy(pOutput,&Context.m_Hash,32);
	\endcode

	\sa Burger::GOST_t or Hash(GOST_t *,const void *,WordPtr)

***************************************/

/*! ************************************

	\brief Initialize the GOST hasher

	Call this function before any hashing is performed

	\sa Process(const void *,WordPtr) or Finalize(void)

***************************************/

void BURGER_API Burger::GOSTHasher_t::Init(void)
{
	// Load magic initialization constants.

	MemoryClear(&m_Hash,sizeof(m_Hash));
	MemoryClear(m_uNativeHash,sizeof(m_uNativeHash));
	MemoryClear(m_uSum,sizeof(m_uSum));
	m_uTotalCount = 0;
	m_uByteCount = 0;
}

/*! ************************************

	\brief Process a single 8 32-bit integer block of data

	GOST data is processed in 8 32-bit chunks. This function
	will process 32 bytes on input and update the hash and checksum

	\param pBlock Pointer to a buffer of 8 32 bit values in native endian to hash
	\sa Process(const void *,WordPtr), Finalize(void) or Init(void)

***************************************/

#if !defined(DOXYGEN)
#define GOST_ITERATE(val1,val2) \
	uTemp = val1 + uRight; \
	uLeft ^= g_SBoxTable[0][uTemp&0xFFU] ^ g_SBoxTable[1][(uTemp>>8U) & 0xFFU] ^ g_SBoxTable[2][(uTemp>>16U) & 0xFFU] ^ g_SBoxTable[3][uTemp>>24U]; \
	uTemp = val2 + uLeft; \
	uRight ^= g_SBoxTable[0][uTemp&0xFFU] ^ g_SBoxTable[1][(uTemp>>8U) & 0xFFU] ^ g_SBoxTable[2][(uTemp>>16U) & 0xFFU] ^ g_SBoxTable[3][uTemp>>24U]
#endif

void BURGER_API Burger::GOSTHasher_t::Process(const Word32 *pBlock)
{
	Word32 TempHash[8];
	Word32 TempBlock[8];

	// Copy to a temp buffer
	Word i = 0;
	do {
		TempHash[i] = m_uNativeHash[i];
		TempBlock[i] = pBlock[i];
	} while (++i<8);

	// Iterate over the data to generate a sample table

	Word32 Samples[8];
	i = 0;
	for (;;) {

		// w entries are the xor of the hash and new data
		Word32 w0 = TempHash[0]^TempBlock[0];
		Word32 w1 = TempHash[1]^TempBlock[1];
		Word32 w2 = TempHash[2]^TempBlock[2];
		Word32 w3 = TempHash[3]^TempBlock[3];
		Word32 w4 = TempHash[4]^TempBlock[4];
		Word32 w5 = TempHash[5]^TempBlock[5];
		Word32 w6 = TempHash[6]^TempBlock[6];
		Word32 w7 = TempHash[7]^TempBlock[7];

		// P-Transformation

		Word32 key0 = (w0&0xFF)|((w2&0xFF)<<8) | ((w4&0xFF)<<16)|(w6<<24);
		Word32 key1 = ((w0&0xFF00)>>8)|(w2&0xFF00)| ((w4&0xFF00)<<8)|((w6&0xFF00)<<16);
		Word32 key2 = ((w0&0xFF0000)>>16)|((w2&0xFF0000)>>8) | (w4&0xFF0000)|((w6&0xFF0000)<<8);
		Word32 key3 = (w0>>24)|((w2&0xFF000000)>>16) | ((w4&0xFF000000)>>8)|(w6&0xFF000000);
		Word32 key4 = (w1&0xFF)|((w3&0xFF)<<8) | ((w5&0xFF)<<16)|(w7<<24);
		Word32 key5 = ((w1&0xFF00)>>8)|(w3&0xFF00) | ((w5&0xFF00)<<8)|((w7&0xFF00)<<16);
		Word32 key6 = ((w1&0xFF0000)>>16)|((w3&0xFF0000)>>8) | (w5&0xFF0000)|((w7&0xFF0000)<<8);
		Word32 key7 = (w1>>24)|((w3&0xFF000000)>>16) | ((w5&0xFF000000)>>8)|(w7&0xFF000000);

		// Get the left and right values, and do the hash
		Word32 uRight = m_uNativeHash[i];
		Word32 uLeft = m_uNativeHash[i+1];
		Word32 uTemp;

		GOST_ITERATE(key0,key1); 
		GOST_ITERATE(key2,key3);
		GOST_ITERATE(key4,key5);
		GOST_ITERATE(key6,key7);
		GOST_ITERATE(key0,key1);
		GOST_ITERATE(key2,key3);
		GOST_ITERATE(key4,key5);
		GOST_ITERATE(key6,key7);
		GOST_ITERATE(key0,key1);
		GOST_ITERATE(key2,key3);
		GOST_ITERATE(key4,key5);
		GOST_ITERATE(key6,key7);
		GOST_ITERATE(key7,key6);
		GOST_ITERATE(key5,key4);
		GOST_ITERATE(key3,key2);
		GOST_ITERATE(key1,key0);

		// Store the samples
		Samples[i] = uLeft;
		Samples[i+1] = uRight;

		// Swap left and right
		uTemp = uRight;
		uRight = uLeft;
		uLeft = uTemp;

		// All done?
		if (i==6) {
			break;
		}

		// Rotate the temp hash table
		uLeft = TempHash[0]^TempHash[2];
		uRight = TempHash[1]^TempHash[3];
		TempHash[0] = TempHash[2];
		TempHash[1] = TempHash[3];
		TempHash[2] = TempHash[4];
		TempHash[3] = TempHash[5];
		TempHash[4] = TempHash[6];
		TempHash[5] = TempHash[7];
		TempHash[6] = uLeft;
		TempHash[7] = uRight;

		// On the first loop, insert a pattern of bit flips
		if (i==2) {
			TempHash[0] ^= 0xff00ff00;
			TempHash[1] ^= 0xff00ff00;
			TempHash[2] ^= 0x00ff00ff;
			TempHash[3] ^= 0x00ff00ff;
			TempHash[4] ^= 0x00ffff00;
			TempHash[5] ^= 0xff0000ff;
			TempHash[6] ^= 0x000000ff;
			TempHash[7] ^= 0xff00ffff;
		}

		// Rotate the temp block table
		uLeft = TempBlock[0];
		uRight = TempBlock[2];
		TempBlock[0] = TempBlock[4];
		TempBlock[2] = TempBlock[6];
		TempBlock[4] = uLeft ^ uRight;
		TempBlock[6] = TempBlock[0]^uRight;

		uLeft = TempBlock[1];
		uRight = TempBlock[3];
		TempBlock[1] = TempBlock[5];
		TempBlock[3] = TempBlock[7];
		TempBlock[5] = uLeft ^ uRight;
		TempBlock[7] = TempBlock[1]^uRight;
		i += 2;
	}

	// Blend the bits from the Samples with the original block

	{
		Word32 uTempHash0 = pBlock[0]^Samples[6];
		Word32 uTempHash1 = pBlock[1]^Samples[7];
		Word32 uTempHash2 = pBlock[2]^RotateLeft(Samples[0],16)^(Samples[0]&0xffff)^
			(Samples[1]&0xffff)^(Samples[1]>>16)^
			(Samples[2]<<16)^Samples[6]^(Samples[6]<<16)^
			(Samples[7]&0xffff0000)^(Samples[7]>>16);
		Word32 uTempHash3 = pBlock[3]^(Samples[0]&0xffff)^(Samples[0]<<16)^
			(Samples[1]&0xffff)^RotateLeft(Samples[1],16)^
			RotateLeft(Samples[2],16)^
			(Samples[3]<<16)^
			Samples[6]^RotateLeft(Samples[6],16)^
			(Samples[7]&0xffff)^RotateLeft(Samples[7],16);
		Word32 uTempHash4 = pBlock[4]^(Samples[0]&0xffff0000)^RotateLeft(Samples[0],16)^
			(Samples[1]&0xffff0000)^(Samples[1]>>16)^
			RotateLeft(Samples[2],16)^
			RotateLeft(Samples[3],16)^
			(Samples[4]<<16)^
			RotateLeft(Samples[6],16)^
			(Samples[7]&0xffff)^RotateLeft(Samples[7],16);
		Word32 uTempHash5 = pBlock[5]^RotateLeft(Samples[0],16)^(Samples[0]&0xffff0000)^
			(Samples[1]&0xffff)^
			Samples[2]^(Samples[2]>>16)^
			RotateLeft(Samples[3],16)^
			RotateLeft(Samples[4],16)^
			(Samples[5]<<16)^
			RotateLeft(Samples[6],16)^
			(Samples[7]&0xffff0000)^RotateLeft(Samples[7],16);
		Word32 uTempHash6 = pBlock[6]^Samples[0]^
			(Samples[1]>>16)^
			(Samples[2]<<16)^
			Samples[3]^(Samples[3]>>16)^
			RotateLeft(Samples[4],16)^
			RotateLeft(Samples[5],16)^
			Samples[6]^RotateLeft(Samples[6],16)^
			(Samples[7]<<16);
		Word32 uTempHash7 = pBlock[7]^(Samples[0]&0xffff0000)^(Samples[0]<<16)^
			(Samples[1]&0xffff)^(Samples[1]<<16)^
			(Samples[2]>>16)^
			(Samples[3]<<16)^
			Samples[4]^(Samples[4]>>16)^
			RotateLeft(Samples[5],16)^
			(Samples[6]>>16)^
			(Samples[7]&0xffff)^RotateLeft(Samples[7],16);

		/* 16 * 1 round of the LFSR and xor in H */

		Word32 uTempBlock0 = m_uNativeHash[0]^(uTempHash1<<16)^(uTempHash0>>16);
		Word32 uTempBlock1 = m_uNativeHash[1]^(uTempHash2<<16)^(uTempHash1>>16);
		Word32 uTempBlock2 = m_uNativeHash[2]^(uTempHash3<<16)^(uTempHash2>>16);
		Word32 uTempBlock3 = m_uNativeHash[3]^(uTempHash4<<16)^(uTempHash3>>16);
		Word32 uTempBlock4 = m_uNativeHash[4]^(uTempHash5<<16)^(uTempHash4>>16);
		Word32 uTempBlock5 = m_uNativeHash[5]^(uTempHash6<<16)^(uTempHash5>>16);
		Word32 uTempBlock6 = m_uNativeHash[6]^(uTempHash7<<16)^(uTempHash6>>16);
		Word32 uTempBlock7 = m_uNativeHash[7]^(uTempHash0&0xffff0000)^(uTempHash0<<16)^(uTempHash7>>16)^
			(uTempHash1&0xffff0000)^(uTempHash1<<16)^(uTempHash6<<16)^(uTempHash7&0xffff0000);

		// Update the final hash value

		m_uNativeHash[0] = (uTempBlock0&0xffff0000)^RotateLeft(uTempBlock0,16)^
			(uTempBlock1>>16)^(uTempBlock1&0xffff0000)^
			(uTempBlock2<<16)^
			(uTempBlock3>>16)^
			(uTempBlock4<<16)^
			(uTempBlock5>>16)^uTempBlock5^
			(uTempBlock6>>16)^
			RotateLeft(uTempBlock7,16)^(uTempBlock7&0xffff);
		m_uNativeHash[1] = RotateLeft(uTempBlock0,16)^(uTempBlock0&0xffff0000)^
			(uTempBlock1&0xffff)^
			uTempBlock2^(uTempBlock2>>16)^
			(uTempBlock3<<16)^
			(uTempBlock4>>16)^
			(uTempBlock5<<16)^
			(uTempBlock6<<16)^uTempBlock6^
			(uTempBlock7&0xffff0000)^(uTempBlock7>>16);
		m_uNativeHash[2] = (uTempBlock0&0xffff)^(uTempBlock0<<16)^
			RotateLeft(uTempBlock1,16)^(uTempBlock1&0xffff0000)^
			(uTempBlock2<<16)^
			(uTempBlock3>>16)^uTempBlock3^
			(uTempBlock4<<16)^
			(uTempBlock5>>16)^
			uTempBlock6^(uTempBlock6>>16)^
			(uTempBlock7&0xffff)^RotateLeft(uTempBlock7,16);
		m_uNativeHash[3] = RotateLeft(uTempBlock0,16)^(uTempBlock0&0xffff0000)^
			(uTempBlock1&0xffff0000)^(uTempBlock1>>16)^
			RotateLeft(uTempBlock2,16)^uTempBlock2^
			(uTempBlock3<<16)^
			(uTempBlock4>>16)^uTempBlock4^
			(uTempBlock5<<16)^
			(uTempBlock6<<16)^
			(uTempBlock7&0xffff)^(uTempBlock7>>16);
		m_uNativeHash[4] = (uTempBlock0>>16)^
			(uTempBlock1<<16)^uTempBlock1^
			(uTempBlock2>>16)^uTempBlock2^
			RotateLeft(uTempBlock3,16)^uTempBlock3^
			(uTempBlock4<<16)^
			(uTempBlock5>>16)^uTempBlock5^
			RotateLeft(uTempBlock6,16)^
			(uTempBlock7<<16);
		m_uNativeHash[5] = (uTempBlock0<<16)^(uTempBlock0&0xffff0000U)^
			RotateLeft(uTempBlock1,16)^(uTempBlock1&0xffff0000U)^
			(uTempBlock2<<16)^uTempBlock2^
			(uTempBlock3>>16)^uTempBlock3^
			RotateLeft(uTempBlock4,16)^uTempBlock4^
			(uTempBlock5<<16)^
			RotateLeft(uTempBlock6,16)^uTempBlock6^
			RotateLeft(uTempBlock7,16)^(uTempBlock7&0xffff0000U);
		m_uNativeHash[6] = uTempBlock0^
			uTempBlock2^(uTempBlock2>>16)^
			uTempBlock3^(uTempBlock3<<16)^
			uTempBlock4^(uTempBlock4>>16)^
			RotateLeft(uTempBlock5,16)^uTempBlock5^
			RotateLeft(uTempBlock6,16)^uTempBlock6^
			(uTempBlock7<<16)^uTempBlock7;
		m_uNativeHash[7] = uTempBlock0^(uTempBlock0>>16)^
			RotateLeft(uTempBlock1,16)^
			(uTempBlock2<<16)^
			(uTempBlock3>>16)^uTempBlock3^
			(uTempBlock4<<16)^uTempBlock4^
			(uTempBlock5>>16)^uTempBlock5^
			RotateLeft(uTempBlock6,16)^
			(uTempBlock7<<16)^uTempBlock7;
	}
}


/*! ************************************

	\brief Process a 32 byte block of data

	GOST data is processed in 8 32-bit chunks. This function
	will convert 32 bytes of data arranged as non-aligned
	array of 8 32 bit little endian integers and pass them to
	the actual function to do the data processing

	\param pBlock Pointer to a buffer of 32 byte values to hash
	\sa Process(const void *,WordPtr), Finalize(void) or Init(void)

***************************************/

void BURGER_API Burger::GOSTHasher_t::Process(const Word8 *pBlock)
{
	Word32 Buffer[8];

	// Adjust the sum

	Word32 uCarry = 0;
	Word i = 0;
	do {
		Word32 uTemp = LittleEndian::LoadAny(static_cast<const Word32 *>(static_cast<const void *>(pBlock)));
		pBlock += sizeof(Word32);
		Buffer[i] = uTemp;
		Word32 uPrevious = m_uSum[i];
		
		// Add, with carry
		uCarry = uTemp+uCarry+uPrevious;
		
		// Save the updated sum
		m_uSum[i] = uCarry;

		// Set carry if detected
		uCarry = ((uCarry < uTemp)||(uCarry < uPrevious)) ? 1U : 0;
	} while (++i<8);

	// Process the converted buffer
	Process(Buffer);
}

/*! ************************************

	\brief Process an arbitrary number of input bytes

	Process input data into the hash. If data chunks are not 
	a multiple of 64 bytes, the excess will be cached and 
	a future call will continue the hashing where it left
	off.
	
	\param pInput Pointer to a buffer of data to hash
	\param uLength Number of bytes to hash
	\sa Process(const Word8 *), Finalize(void)

***************************************/

void BURGER_API Burger::GOSTHasher_t::Process(const void *pInput,WordPtr uLength)
{
	// Compute number of bytes mod 32
	WordPtr uIndex = static_cast<WordPtr>(m_uByteCount) & 0x1FU;

	// Update number of bits (Perform a 32 bit add)
	m_uByteCount += uLength;

	WordPtr i = 32 - uIndex;

	// Transform as many times as possible.

	if (uLength >= i) {		// Should I copy or pack?

		MemoryCopy(&m_CacheBuffer[uIndex],pInput,i);
		Process(m_CacheBuffer);
		m_uTotalCount += 256;		// Bit count

		// Perform the checksum directly on the memory buffers

		if ((i+31)<uLength) {
			do {
	 			Process(static_cast<const Word8 *>(pInput)+i);
				m_uTotalCount += 256;		// Bit count
				i += 32;
			} while ((i+31) < uLength);
		}
		uIndex = 0;
	} else {
		i = 0;
	}

	// Buffer remaining input
	MemoryCopy(&m_CacheBuffer[uIndex],static_cast<const Word8 *>(pInput)+i,uLength-i);
}

/*! ************************************

	\brief Finalize the hashing

	When multi-pass hashing is performed, this call is necessary to
	finalize the hash so that the generated checksum can
	be applied into the hash

	\sa Init(void), Process(const void *,WordPtr)

***************************************/

void BURGER_API Burger::GOSTHasher_t::Finalize(void)
{
	// If there was any remaining data, hash it

	WordPtr uByteCount = static_cast<WordPtr>(m_uByteCount)&0x1FU;
	if (uByteCount) {
		MemoryClear(&m_CacheBuffer[uByteCount],32-uByteCount);
		Process(m_CacheBuffer);
		m_uTotalCount += uByteCount<<3;
	}

	// Hash in the data size count and the data sum

	Word32 Temp[8];
	MemoryClear(Temp,sizeof(Temp));
	Temp[0] = static_cast<Word32>(m_uTotalCount);
	Temp[1] = static_cast<Word32>(m_uTotalCount>>32);
	Process(Temp);
	Process(m_uSum);

	// Save off the hash in Little Endian format

#if defined(BURGER_LITTLEENDIAN)
	MemoryCopy(&m_Hash,m_uNativeHash,sizeof(m_Hash));
#else
	ConvertEndian(static_cast<Word32 *>(static_cast<void *>(&m_Hash)),m_uNativeHash,BURGER_ARRAYSIZE(m_uNativeHash));
#endif
}

/*! ************************************

	\brief Quickly create a GOST key
	
	Given a buffer of data, generate the GOST hash key

	\param pOutput Pointer to an unitialized GOST_t structure
	\param pInput Pointer to a buffer of data to hash
	\param uLength Number of bytes to hash

	\sa Burger::GOSTHasher_t

***************************************/

void BURGER_API Burger::Hash(GOST_t *pOutput,const void *pInput,WordPtr uLength)
{
	GOSTHasher_t Context;
	// Initialize
	Context.Init();
	// Process all of the data
	Context.Process(pInput,uLength);
	// Wrap up the processing
	Context.Finalize();
	// Return the resulting hash
	MemoryCopy(pOutput,&Context.m_Hash,32);
}

/*! ************************************

	\brief Calculate a GOST polynomial table

	Calculates a 1024 entry shift index table

	Implemented following the documentation found in
	https://en.wikipedia.org/wiki/GOST_(hash_function)

	\note This function was used to generate an internal
	table.

	\param pOutput Pointer to a buffer to have the generated table store
	\param pSBoxTable Pointer to the 4 bit definition table

***************************************/

void BURGER_API Burger::GenerateGOSTTable(Word32 *pOutput,const Word32 pSBoxTable[8][16])
{
	// Create the Shift Box
	
	Word i = 0;
	Word a = 0;
	do {
		Word32 uTemp1 = pSBoxTable[1][a]<<15;
		Word32 uTemp2 = pSBoxTable[3][a]<<23;
		Word32 uTemp3 = Burger::RotateLeft(pSBoxTable[5][a],31);
		Word32 uTemp4 = pSBoxTable[7][a]<<7;
		Word b = 0;
		do {
			pOutput[i] = uTemp1|(pSBoxTable[0][b]<<11);
			pOutput[i+256] = uTemp2|(pSBoxTable[2][b]<<19);
			pOutput[i+512] = uTemp3|(pSBoxTable[4][b]<<27);
			pOutput[i+768] = uTemp4|(pSBoxTable[6][b]<<3);
			++i;
		} while (++b<16);
	} while (++a<16);
}