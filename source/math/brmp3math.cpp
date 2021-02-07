/***************************************

    Math functions for MP3 support

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brmp3math.h"
#include "brmp3.h"

/*! ************************************

	\brief Array of cosine points

	Array of pointers to cosine tables used for MP3 decoding
	in the order of 16 entry, 8 entry, 4 entry, 2 entry
	and 1 entry in that order.

***************************************/

#if !defined(DOXYGEN)

static const Burger::uint32_float_t g_MP3Cosine64[16] = {
	{0x3F002785},{0x3F01668B},{0x3F03F45B},{0x3F07F268},
	{0x3F0D9838},{0x3F153B3A},{0x3F1F5C6E},{0x3F2CC03D},
	{0x3F3E99EE},{0x3F56DF9E},{0x3F78FA3B},{0x3F95B035},
	{0x3FBDF91B},{0x4003B2AF},{0x405A1642},{0x41230A46}
};

static const Burger::uint32_float_t g_MP3Cosine32[8] = {
	{0x3F009E8D},{0x3F05C278},{0x3F11233F},{0x3F25961D},
	{0x3F49C480},{0x3F87C449},{0x3FDC7926},{0x40A33C9C}
};

static const Burger::uint32_float_t g_MP3Cosine16[4] = {
	{0x3F0281F7},{0x3F19F1BD},{0x3F6664D7},{0x402406CF}
};

static const Burger::uint32_float_t g_MP3Cosine8[2] = {
	{0x3F0A8BD4},{0x3FA73D75}
};

static const Burger::uint32_float_t g_MP3Cosine4[1] = {
	{0x3F3504F3}
};

#endif

const Burger::uint32_float_t *Burger::g_pMP3CosinePoints[5] = { g_MP3Cosine64, g_MP3Cosine32, g_MP3Cosine16, g_MP3Cosine8, g_MP3Cosine4 };

/*! ************************************

	\brief 4 pairs of FHT cosine/sine values for MP3 encoding
	\sa MP3FHT(float *,WordPtr)

***************************************/

const Burger::uint32_float_t Burger::g_MP3FHTCosines[4][2] = {
{
	{0x3F6C835E},{0x3EC3EF15}
},{
	{0x3F7EC46D},{0x3DC8BD36}
},{
	{0x3F7FEC43},{0x3CC90AB0}
},{
	{0x3F7FFEC4},{0x3BC90F88}
}};

/*! ************************************

	\brief MP3 Layer 3 table for MP3DCT36
	
	The first 9 entries are cos((BURGER_PI / 18.0) * i)
	The second 9 entries are 0.5 / cos((Burger::g_dPi * (i*2+1)) / 36.0)

	\sa MP3DCT36(float *,float *,float *,const float *,const float *)
	
***************************************/

const Burger::uint32_float_t Burger::g_MP3DCT36Constants[18] = {
	{0x3F800000},{0x3F7C1C5C},{0x3F708FB2},{0x3F5DB3D7},
	{0x3F441B7D},{0x3F248DBB},{0x3F000000},{0x3EAF1D44},
	{0x3E31D0D4},
	{0x3F007D2B},{0x3F0483EE},{0x3F0D3B7D},{0x3F1C4257},
	{0x3F3504F3},{0x3F5F2944},{0x3F976FD9},{0x3FF746EA},
	{0x40B79454}
};

/*! ************************************

	\brief MP3 Layer 3 table for 0.5 / cos((Burger::g_dPi * (i*2+1)) / 12.0)
	\sa MP3DCT12(float *,float *,float *,const float *,const float *)
	
***************************************/

const Burger::uint32_float_t Burger::g_MP3DCT12Constants[3] = {
	{0x3F0483EE},{0x3F3504F3},{0x3FF746EA}
};

/*! ************************************

	\brief MP3 Discrete cosine transform function

	Discrete cosine transform applied to an 8*8
	2 dimensional array for decoding audio for MP3.

	The output arrays are written at a stride of 16 entries

	\param pOutput1 Pointer to a buffer with a minimum of 257 entries
	\param pOutput2 Pointer to a buffer with a minimum of 257 entries
	\param pInput Pointer to a 32 entry array of audio samples

***************************************/

void BURGER_API Burger::MP3DCT64(float *pOutput1,float *pOutput2,const float *pInput)
{
	float TempBuffer[8*8];

	// 16 entry table
	const float *pCosineTable = &g_pMP3CosinePoints[0]->f;

	TempBuffer[ 0] = pInput[ 0] + pInput[31];
	TempBuffer[31] = (pInput[ 0] - pInput[31]) * pCosineTable[0];

	TempBuffer[ 1] = pInput[ 1] + pInput[30];
	TempBuffer[30] = (pInput[ 1] - pInput[30]) * pCosineTable[1];

	TempBuffer[ 2] = pInput[ 2] + pInput[29];
	TempBuffer[29] = (pInput[ 2] - pInput[29]) * pCosineTable[2];

	TempBuffer[ 3] = pInput[ 3] + pInput[28];
	TempBuffer[28] = (pInput[ 3] - pInput[28]) * pCosineTable[3];

	TempBuffer[ 4] = pInput[ 4] + pInput[27];
	TempBuffer[27] = (pInput[ 4] - pInput[27]) * pCosineTable[4];

	TempBuffer[ 5] = pInput[ 5] + pInput[26];
	TempBuffer[26] = (pInput[ 5] - pInput[26]) * pCosineTable[5];

	TempBuffer[ 6] = pInput[ 6] + pInput[25];
	TempBuffer[25] = (pInput[ 6] - pInput[25]) * pCosineTable[6];

	TempBuffer[ 7] = pInput[ 7] + pInput[24];
	TempBuffer[24] = (pInput[ 7] - pInput[24]) * pCosineTable[7];

	TempBuffer[ 8] = pInput[ 8] + pInput[23];
	TempBuffer[23] = (pInput[ 8] - pInput[23]) * pCosineTable[8];

	TempBuffer[ 9] = pInput[ 9] + pInput[22];
	TempBuffer[22] = (pInput[ 9] - pInput[22]) * pCosineTable[9];

	TempBuffer[10] = pInput[10] + pInput[21];
	TempBuffer[21] = (pInput[10] - pInput[21]) * pCosineTable[10];

	TempBuffer[11] = pInput[11] + pInput[20];
	TempBuffer[20] = (pInput[11] - pInput[20]) * pCosineTable[11];

	TempBuffer[12] = pInput[12] + pInput[19];
	TempBuffer[19] = (pInput[12] - pInput[19]) * pCosineTable[12];

	TempBuffer[13] = pInput[13] + pInput[18];
	TempBuffer[18] = (pInput[13] - pInput[18]) * pCosineTable[13];

	TempBuffer[14] = pInput[14] + pInput[17];
	TempBuffer[17] = (pInput[14] - pInput[17]) * pCosineTable[14];

	TempBuffer[15] = pInput[15] + pInput[16];
	TempBuffer[16] = (pInput[15] - pInput[16]) * pCosineTable[15];

	// 8 entry table
	pCosineTable = &g_pMP3CosinePoints[1]->f;

	TempBuffer[32+ 0] = TempBuffer[ 0] + TempBuffer[15];
	TempBuffer[32+15] = (TempBuffer[ 0] - TempBuffer[15]) * pCosineTable[0];
	TempBuffer[32+ 1] = TempBuffer[ 1] + TempBuffer[14];
	TempBuffer[32+14] = (TempBuffer[ 1] - TempBuffer[14]) * pCosineTable[1];
	TempBuffer[32+ 2] = TempBuffer[ 2] + TempBuffer[13];
	TempBuffer[32+13] = (TempBuffer[ 2] - TempBuffer[13]) * pCosineTable[2];
	TempBuffer[32+ 3] = TempBuffer[ 3] + TempBuffer[12];
	TempBuffer[32+12] = (TempBuffer[ 3] - TempBuffer[12]) * pCosineTable[3];
	TempBuffer[32+ 4] = TempBuffer[ 4] + TempBuffer[11];
	TempBuffer[32+11] = (TempBuffer[ 4] - TempBuffer[11]) * pCosineTable[4];
	TempBuffer[32+ 5] = TempBuffer[ 5] + TempBuffer[10];
	TempBuffer[32+10] = (TempBuffer[ 5] - TempBuffer[10]) * pCosineTable[5];
	TempBuffer[32+ 6] = TempBuffer[ 6] + TempBuffer[ 9];
	TempBuffer[32+ 9] = (TempBuffer[ 6] - TempBuffer[ 9]) * pCosineTable[6];
	TempBuffer[32+ 7] = TempBuffer[ 7] + TempBuffer[ 8];
	TempBuffer[32+ 8] = (TempBuffer[ 7] - TempBuffer[ 8]) * pCosineTable[7];

	TempBuffer[32+16] = TempBuffer[16] + TempBuffer[31];
	TempBuffer[32+31] = (TempBuffer[31] - TempBuffer[16]) * pCosineTable[0];
	TempBuffer[32+17] = TempBuffer[17] + TempBuffer[30];
	TempBuffer[32+30] = (TempBuffer[30] - TempBuffer[17]) * pCosineTable[1];
	TempBuffer[32+18] = TempBuffer[18] + TempBuffer[29];
	TempBuffer[32+29] = (TempBuffer[29] - TempBuffer[18]) * pCosineTable[2];
	TempBuffer[32+19] = TempBuffer[19] + TempBuffer[28];
	TempBuffer[32+28] = (TempBuffer[28] - TempBuffer[19]) * pCosineTable[3];
	TempBuffer[32+20] = TempBuffer[20] + TempBuffer[27];
	TempBuffer[32+27] = (TempBuffer[27] - TempBuffer[20]) * pCosineTable[4];
	TempBuffer[32+21] = TempBuffer[21] + TempBuffer[26];
	TempBuffer[32+26] = (TempBuffer[26] - TempBuffer[21]) * pCosineTable[5];
	TempBuffer[32+22] = TempBuffer[22] + TempBuffer[25];
	TempBuffer[32+25] = (TempBuffer[25] - TempBuffer[22]) * pCosineTable[6];
	TempBuffer[32+23] = TempBuffer[23] + TempBuffer[24];
	TempBuffer[32+24] = (TempBuffer[24] - TempBuffer[23]) * pCosineTable[7];

	// 4 entry table

	pCosineTable = &g_pMP3CosinePoints[2]->f;

	TempBuffer[ 0] = TempBuffer[32+ 0] + TempBuffer[32+ 7];
	TempBuffer[ 7] = (TempBuffer[32+ 0] - TempBuffer[32+ 7]) * pCosineTable[0];
	TempBuffer[ 1] = TempBuffer[32+ 1] + TempBuffer[32+ 6];
	TempBuffer[ 6] = (TempBuffer[32+ 1] - TempBuffer[32+ 6]) * pCosineTable[1];
	TempBuffer[ 2] = TempBuffer[32+ 2] + TempBuffer[32+ 5];
	TempBuffer[ 5] = (TempBuffer[32+ 2] - TempBuffer[32+ 5]) * pCosineTable[2];
	TempBuffer[ 3] = TempBuffer[32+ 3] + TempBuffer[32+ 4];
	TempBuffer[ 4] = (TempBuffer[32+ 3] - TempBuffer[32+ 4]) * pCosineTable[3];

	TempBuffer[ 8] = TempBuffer[32+ 8] + TempBuffer[32+15];
	TempBuffer[15] = (TempBuffer[32+15] - TempBuffer[32+ 8]) * pCosineTable[0];
	TempBuffer[ 9] = TempBuffer[32+ 9] + TempBuffer[32+14];
	TempBuffer[14] = (TempBuffer[32+14] - TempBuffer[32+ 9]) * pCosineTable[1];
	TempBuffer[10] = TempBuffer[32+10] + TempBuffer[32+13];
	TempBuffer[13] = (TempBuffer[32+13] - TempBuffer[32+10]) * pCosineTable[2];
	TempBuffer[11] = TempBuffer[32+11] + TempBuffer[32+12];
	TempBuffer[12] = (TempBuffer[32+12] - TempBuffer[32+11]) * pCosineTable[3];

	TempBuffer[16] = TempBuffer[32+16] + TempBuffer[32+23];
	TempBuffer[23] = (TempBuffer[32+16] - TempBuffer[32+23]) * pCosineTable[0];
	TempBuffer[17] = TempBuffer[32+17] + TempBuffer[32+22];
	TempBuffer[22] = (TempBuffer[32+17] - TempBuffer[32+22]) * pCosineTable[1];
	TempBuffer[18] = TempBuffer[32+18] + TempBuffer[32+21];
	TempBuffer[21] = (TempBuffer[32+18] - TempBuffer[32+21]) * pCosineTable[2];
	TempBuffer[19] = TempBuffer[32+19] + TempBuffer[32+20];
	TempBuffer[20] = (TempBuffer[32+19] - TempBuffer[32+20]) * pCosineTable[3];

	TempBuffer[24] = TempBuffer[32+24] + TempBuffer[32+31];
	TempBuffer[31] = (TempBuffer[32+31] - TempBuffer[32+24]) * pCosineTable[0];
	TempBuffer[25] = TempBuffer[32+25] + TempBuffer[32+30];
	TempBuffer[30] = (TempBuffer[32+30] - TempBuffer[32+25]) * pCosineTable[1];
	TempBuffer[26] = TempBuffer[32+26] + TempBuffer[32+29];
	TempBuffer[29] = (TempBuffer[32+29] - TempBuffer[32+26]) * pCosineTable[2];
	TempBuffer[27] = TempBuffer[32+27] + TempBuffer[32+28];
	TempBuffer[28] = (TempBuffer[32+28] - TempBuffer[32+27]) * pCosineTable[3];

	// 2 entry table

	pCosineTable = &g_pMP3CosinePoints[3]->f;

	{
		float const fCosine0 = pCosineTable[0];
		float const fCosine1 = pCosineTable[1];

		TempBuffer[32+ 0] = TempBuffer[ 0] + TempBuffer[ 3];
		TempBuffer[32+ 3] = (TempBuffer[ 0] - TempBuffer[ 3]) * fCosine0;
		TempBuffer[32+ 1] = TempBuffer[ 1] + TempBuffer[ 2];
		TempBuffer[32+ 2] = (TempBuffer[ 1] - TempBuffer[ 2]) * fCosine1;

		TempBuffer[32+ 4] = TempBuffer[ 4] + TempBuffer[ 7];
		TempBuffer[32+ 7] = (TempBuffer[ 7] - TempBuffer[ 4]) * fCosine0;
		TempBuffer[32+ 5] = TempBuffer[ 5] + TempBuffer[ 6];
		TempBuffer[32+ 6] = (TempBuffer[ 6] - TempBuffer[ 5]) * fCosine1;

		TempBuffer[32+ 8] = TempBuffer[ 8] + TempBuffer[11];
		TempBuffer[32+11] = (TempBuffer[ 8] - TempBuffer[11]) * fCosine0;
		TempBuffer[32+ 9] = TempBuffer[ 9] + TempBuffer[10];
		TempBuffer[32+10] = (TempBuffer[ 9] - TempBuffer[10]) * fCosine1;

		TempBuffer[32+12] = TempBuffer[12] + TempBuffer[15];
		TempBuffer[32+15] = (TempBuffer[15] - TempBuffer[12]) * fCosine0;
		TempBuffer[32+13] = TempBuffer[13] + TempBuffer[14];
		TempBuffer[32+14] = (TempBuffer[14] - TempBuffer[13]) * fCosine1;

		TempBuffer[32+16] = TempBuffer[16] + TempBuffer[19];
		TempBuffer[32+19] = (TempBuffer[16] - TempBuffer[19]) * fCosine0;
		TempBuffer[32+17] = TempBuffer[17] + TempBuffer[18];
		TempBuffer[32+18] = (TempBuffer[17] - TempBuffer[18]) * fCosine1;

		TempBuffer[32+20] = TempBuffer[20] + TempBuffer[23];
		TempBuffer[32+23] = (TempBuffer[23] - TempBuffer[20]) * fCosine0;
		TempBuffer[32+21] = TempBuffer[21] + TempBuffer[22];
		TempBuffer[32+22] = (TempBuffer[22] - TempBuffer[21]) * fCosine1;

		TempBuffer[32+24] = TempBuffer[24] + TempBuffer[27];
		TempBuffer[32+27] = (TempBuffer[24] - TempBuffer[27]) * fCosine0;
		TempBuffer[32+25] = TempBuffer[25] + TempBuffer[26];
		TempBuffer[32+26] = (TempBuffer[25] - TempBuffer[26]) * fCosine1;

		TempBuffer[32+28] = TempBuffer[28] + TempBuffer[31];
		TempBuffer[32+31] = (TempBuffer[31] - TempBuffer[28]) * fCosine0;
		TempBuffer[32+29] = TempBuffer[29] + TempBuffer[30];
		TempBuffer[32+30] = (TempBuffer[30] - TempBuffer[29]) * fCosine1;
	}

	// 1 entry table

	{
		float const fCosine4 = g_pMP3CosinePoints[4][0];

		TempBuffer[ 0] = TempBuffer[32+ 0] + TempBuffer[32+ 1];
		TempBuffer[ 1] = (TempBuffer[32+ 0] - TempBuffer[32+ 1]) * fCosine4;
		TempBuffer[ 2] = TempBuffer[32+ 2] + TempBuffer[32+ 3];
		TempBuffer[ 3] = (TempBuffer[32+ 3] - TempBuffer[32+ 2]) * fCosine4;
		TempBuffer[ 2] += TempBuffer[ 3];

		TempBuffer[ 4] = TempBuffer[32+ 4] + TempBuffer[32+ 5];
		TempBuffer[ 5] = (TempBuffer[32+ 4] - TempBuffer[32+ 5]) * fCosine4;
		TempBuffer[ 6] = TempBuffer[32+ 6] + TempBuffer[32+ 7];
		TempBuffer[ 7] = (TempBuffer[32+ 7] - TempBuffer[32+ 6]) * fCosine4;
		TempBuffer[ 6] += TempBuffer[ 7];
		TempBuffer[ 4] += TempBuffer[ 6];
		TempBuffer[ 6] += TempBuffer[ 5];
		TempBuffer[ 5] += TempBuffer[ 7];

		TempBuffer[ 8] = TempBuffer[32+ 8] + TempBuffer[32+ 9];
		TempBuffer[ 9] = (TempBuffer[32+ 8] - TempBuffer[32+ 9]) * fCosine4;
		TempBuffer[10] = TempBuffer[32+10] + TempBuffer[32+11];
		TempBuffer[11] = (TempBuffer[32+11] - TempBuffer[32+10]) * fCosine4;
		TempBuffer[10] += TempBuffer[11];

		TempBuffer[12] = TempBuffer[32+12] + TempBuffer[32+13];
		TempBuffer[13] = (TempBuffer[32+12] - TempBuffer[32+13]) * fCosine4;
		TempBuffer[14] = TempBuffer[32+14] + TempBuffer[32+15];
		TempBuffer[15] = (TempBuffer[32+15] - TempBuffer[32+14]) * fCosine4;
		TempBuffer[14] += TempBuffer[15];
		TempBuffer[12] += TempBuffer[14];
		TempBuffer[14] += TempBuffer[13];
		TempBuffer[13] += TempBuffer[15];

		TempBuffer[16] = TempBuffer[32+16] + TempBuffer[32+17];
		TempBuffer[17] = (TempBuffer[32+16] - TempBuffer[32+17]) * fCosine4;
		TempBuffer[18] = TempBuffer[32+18] + TempBuffer[32+19];
		TempBuffer[19] = (TempBuffer[32+19] - TempBuffer[32+18]) * fCosine4;
		TempBuffer[18] += TempBuffer[19];

		TempBuffer[20] = TempBuffer[32+20] + TempBuffer[32+21];
		TempBuffer[21] = (TempBuffer[32+20] - TempBuffer[32+21]) * fCosine4;
		TempBuffer[22] = TempBuffer[32+22] + TempBuffer[32+23];
		TempBuffer[23] = (TempBuffer[32+23] - TempBuffer[32+22]) * fCosine4;
		TempBuffer[22] += TempBuffer[23];
		TempBuffer[20] += TempBuffer[22];
		TempBuffer[22] += TempBuffer[21];
		TempBuffer[21] += TempBuffer[23];

		TempBuffer[24] = TempBuffer[32+24] + TempBuffer[32+25];
		TempBuffer[25] = (TempBuffer[32+24] - TempBuffer[32+25]) * fCosine4;
		TempBuffer[26] = TempBuffer[32+26] + TempBuffer[32+27];
		TempBuffer[27] = (TempBuffer[32+27] - TempBuffer[32+26]) * fCosine4;
		TempBuffer[26] += TempBuffer[27];

		TempBuffer[28] = TempBuffer[32+28] + TempBuffer[32+29];
		TempBuffer[29] = (TempBuffer[32+28] - TempBuffer[32+29]) * fCosine4;
		TempBuffer[30] = TempBuffer[32+30] + TempBuffer[32+31];
		TempBuffer[31] = (TempBuffer[32+31] - TempBuffer[32+30]) * fCosine4;
		TempBuffer[30] += TempBuffer[31];
		TempBuffer[28] += TempBuffer[30];
		TempBuffer[30] += TempBuffer[29];
		TempBuffer[29] += TempBuffer[31];
	}

	// Now apply the final results

	pOutput1[16 * 16] = TempBuffer[ 0];
	pOutput1[16 * 12] = TempBuffer[ 4];
	pOutput1[16 * 8] = TempBuffer[ 2];
	pOutput1[16 * 4] = TempBuffer[ 6];
	pOutput1[16 * 0] = TempBuffer[ 1];

	pOutput2[16 * 0] = TempBuffer[ 1];
	pOutput2[16 * 4] = TempBuffer[ 5];
	pOutput2[16 * 8] = TempBuffer[ 3];
	pOutput2[16 * 12] = TempBuffer[ 7];

	TempBuffer[ 8] += TempBuffer[12];
	pOutput1[16 * 14] = TempBuffer[ 8];
	TempBuffer[12] += TempBuffer[10];
	pOutput1[16 * 10] = TempBuffer[12];
	TempBuffer[10] += TempBuffer[14];
	pOutput1[16 * 6] = TempBuffer[10];
	TempBuffer[14] += TempBuffer[ 9];
	pOutput1[16 * 2] = TempBuffer[14];

	TempBuffer[ 9] += TempBuffer[13];
	pOutput2[16 * 2] = TempBuffer[ 9];
	TempBuffer[13] += TempBuffer[11];
	pOutput2[16 * 6] = TempBuffer[13];
	TempBuffer[11] += TempBuffer[15];
	pOutput2[16 * 10] = TempBuffer[11];
	pOutput2[16 * 14] = TempBuffer[15];

	TempBuffer[24] += TempBuffer[28];
	pOutput1[16 * 15] = TempBuffer[16] + TempBuffer[24];
	pOutput1[16 * 13] = TempBuffer[24] + TempBuffer[20];
	TempBuffer[28] += TempBuffer[26];
	pOutput1[16 * 11] = TempBuffer[20] + TempBuffer[28];
	pOutput1[16 * 9] = TempBuffer[28] + TempBuffer[18];
	TempBuffer[26] += TempBuffer[30];
	pOutput1[16 * 7] = TempBuffer[18] + TempBuffer[26];
	pOutput1[16 * 5] = TempBuffer[26] + TempBuffer[22];
	TempBuffer[30] += TempBuffer[25];
	pOutput1[16 * 3] = TempBuffer[22] + TempBuffer[30];
	pOutput1[16 * 1] = TempBuffer[30] + TempBuffer[17];

	TempBuffer[25] += TempBuffer[29];
	pOutput2[16 * 1] = TempBuffer[17] + TempBuffer[25];
	pOutput2[16 * 3] = TempBuffer[25] + TempBuffer[21];
	TempBuffer[29] += TempBuffer[27];
	pOutput2[16 * 5] = TempBuffer[21] + TempBuffer[29];
	pOutput2[16 * 7] = TempBuffer[29] + TempBuffer[19];
	TempBuffer[27] += TempBuffer[31];
	pOutput2[16 * 9] = TempBuffer[19] + TempBuffer[27];
	pOutput2[16 * 11] = TempBuffer[27] + TempBuffer[23];
	pOutput2[16 * 13] = TempBuffer[23] + TempBuffer[31];
	pOutput2[16 * 15] = TempBuffer[31];
}

/*! ************************************

	\brief MP3 Fast Fourier (Hartley) transform

	https://en.wikipedia.org/wiki/Hartley_transform
	
	Apply a Hartley transform optimized for either 256 or 1024
	entry arrays using the MP3 FHT algorithm

	\param pInput Pointer to an array of terms to apply the FHT
	\param uCount Either 256 or 1024 entries

***************************************/

#if defined(BURGER_AMD64) && !defined(DOXYGEN)

static const Burger::Vector_128Word32 g_NegXYZW = {0x80000000U,0x00000000U,0x00000000U,0x00000000U};
static const Burger::Vector_128Word32 g_XNegYZW = {0x00000000U,0x80000000U,0x00000000U,0x00000000U};
static const Burger::Vector_128Word32 g_XYNegZNegW = {0x00000000U,0x00000000U,0x80000000U,0x80000000U};

void BURGER_API Burger::MP3FHT(float *pInput,WordPtr uCount)
{
	const float *pCosines = &g_MP3FHTCosines[0]->f;
	const float *pInputEnd = pInput + uCount;

	// Initial stride of 16 entries, then 64, 256, 1024
	WordPtr uStride = 16>>2;
	do {
		// Create stride values
		WordPtr uEighthStride = uStride >> 1U;	
		WordPtr uQuarterStride = uStride;
		WordPtr uHalfStride = uStride << 1U;	
		WordPtr u3QuarterStride = uHalfStride + uQuarterStride;
		uStride = uStride << 2U;

		// Apply the 1st pass of the FFT
		float *pWork = pInput;
		float *pWorkAndEighth = pWork + uEighthStride;
		do {
			float fTemp1 = pWork[0] - pWork[uQuarterStride];
			float fTemp2 = pWork[0] + pWork[uQuarterStride];
			float fTemp3 = pWork[uHalfStride] - pWork[u3QuarterStride];
			float fTemp4 = pWork[uHalfStride] + pWork[u3QuarterStride];

			pWork[0] = fTemp2 + fTemp4;
			pWork[uQuarterStride] = fTemp1 + fTemp3;
			pWork[uHalfStride] = fTemp2 - fTemp4;
			pWork[u3QuarterStride] = fTemp1 - fTemp3;

			fTemp1 = pWorkAndEighth[0] - pWorkAndEighth[uQuarterStride];
			fTemp2 = pWorkAndEighth[0] + pWorkAndEighth[uQuarterStride];
			fTemp3 = pWorkAndEighth[u3QuarterStride] * g_fSqrt2;
			fTemp4 = pWorkAndEighth[uHalfStride] * g_fSqrt2;

			pWorkAndEighth[0] = fTemp2 + fTemp4;
			pWorkAndEighth[uQuarterStride] = fTemp1 + fTemp3;
			pWorkAndEighth[uHalfStride] = fTemp2 - fTemp4;
			pWorkAndEighth[u3QuarterStride] = fTemp1 - fTemp3;

			pWork += uStride;
			pWorkAndEighth += uStride;
		} while (pWork < pInputEnd);

		// Apply the 2nd pass of the FFT
		float fCosine1 = pCosines[0];
		float fSine1 = pCosines[1];
		WordPtr uIndex = 1;
		do {
			float fSine2 = fSine1+fSine1;
			float fCosine2 = 1.0f - (fSine2 * fSine1);
			fSine2 = fSine2 * fCosine1;

			pWork = pInput + uIndex;
			pWorkAndEighth = pInput + uQuarterStride - uIndex;

			// Splat the cosine and sines
			__m128 vCosine1 = _mm_set_ps1(fCosine1);
			__m128 vSine1 = _mm_set_ps1(fSine1);
			__m128 vCosine2 = _mm_set_ps1(fCosine2);

			// Adjust the terms for X,Y,Z,W negations
			vCosine1 = _mm_xor_ps(vCosine1,g_NegXYZW);		// -X,Y,Z,W
			vSine1 = _mm_xor_ps(vSine1,g_XNegYZW);			// X,-Y,Z,W
			vCosine2 = _mm_xor_ps(vCosine2,g_XYNegZNegW);	// X,Y,-Z,-W

			do {
				// 2/8, 6/8, 3/8, 7/8
				__m128 vTemp1 = _mm_set_ps(pWorkAndEighth[u3QuarterStride],pWorkAndEighth[uQuarterStride],pWork[u3QuarterStride],pWork[uQuarterStride]);

				// (2/8, 6/8, 3/8, 7/8) * fSine2
				__m128 vTemp2 = _mm_set_ps1(fSine2);
				vTemp2 = _mm_mul_ps(vTemp2,vTemp1);

				// (2/8, 6/8, -3/8, -7/8) * fCosine2
				vTemp1 = _mm_mul_ps(vTemp1,vCosine2);

				// (-3/8, -7/8, 2/8, 6/8) * fCosine2
				vTemp1 = _mm_shuffle_ps(vTemp1,vTemp1,_MM_SHUFFLE(1,0,3,2));

				// (2,3) (6,7) (3,2) (7,6) (fSine2,fCosine2) 
				vTemp2 = _mm_add_ps(vTemp2,vTemp1);

				// 1/8,5/8,0/8,4/8
				__m128 vTemp3 = _mm_set_ps(pWork[uHalfStride],pWork[0],pWorkAndEighth[uHalfStride],pWorkAndEighth[0]);

				// Perform the add and sub
				vTemp1 = _mm_sub_ps(vTemp3,vTemp2);
				vTemp3 = _mm_add_ps(vTemp3,vTemp2);

				// Swizzle
				vTemp2 = _mm_shuffle_ps(vTemp1,vTemp3,_MM_SHUFFLE(2,0,2,0));
				vTemp2 = _mm_shuffle_ps(vTemp2,vTemp2,_MM_SHUFFLE(3,1,2,0));
				vTemp1 = _mm_shuffle_ps(vTemp1,vTemp3,_MM_SHUFFLE(3,1,3,1));

				// Mul the cosine/sine
				vTemp3 = _mm_mul_ps(vTemp1,vCosine1);
				vTemp1 = _mm_mul_ps(vTemp1,vSine1);

				// Swizzle again and do another add
				vTemp1 = _mm_shuffle_ps(vTemp1,vTemp1,_MM_SHUFFLE(0,1,2,3));
				vTemp1 = _mm_add_ps(vTemp1,vTemp3);

				// Final values
				vTemp3 = _mm_sub_ps(vTemp2,vTemp1);
				vTemp2 = _mm_add_ps(vTemp2,vTemp1);

				// Store the values using swizzles to X
				_mm_store_ss(&pWorkAndEighth[u3QuarterStride],vTemp3);
				_mm_store_ss(&pWorkAndEighth[uQuarterStride],vTemp2);

				vTemp3 = _mm_shuffle_ps(vTemp3,vTemp3,_MM_SHUFFLE(3,3,2,1));
				vTemp2 = _mm_shuffle_ps(vTemp2,vTemp2,_MM_SHUFFLE(3,3,2,1));
				_mm_store_ss(&pWorkAndEighth[uHalfStride],vTemp3);
				_mm_store_ss(&pWorkAndEighth[ 0],vTemp2);

				vTemp3 = _mm_shuffle_ps(vTemp3,vTemp3,_MM_SHUFFLE(3,3,2,1));
				vTemp2 = _mm_shuffle_ps(vTemp2,vTemp2,_MM_SHUFFLE(3,3,2,1));
				_mm_store_ss(&pWork[u3QuarterStride],vTemp3);
				_mm_store_ss(&pWork[uQuarterStride],vTemp2);

				vTemp3 = _mm_shuffle_ps(vTemp3,vTemp3,_MM_SHUFFLE(3,3,2,1));
				vTemp2 = _mm_shuffle_ps(vTemp2,vTemp2,_MM_SHUFFLE(3,3,2,1));
				_mm_store_ss(&pWork[uHalfStride],vTemp3);
				_mm_store_ss(&pWork[ 0],vTemp2);

				pWorkAndEighth += uStride;
				pWork += uStride;
			} while (pWork < pInputEnd);

			// Step fCosine and fSine1
			float fTemp = fCosine1;
			fCosine1 = (fCosine1 * pCosines[0]) - (fSine1 * pCosines[1]);
			fSine1 = (fTemp * pCosines[1]) + (fSine1 * pCosines[0]);
		} while (++uIndex < uEighthStride);
		// Next entry in the table
		pCosines += 2;

		// All done? (2 passes or 4 passes for 256 or 1024 entry tables
	} while (uStride < uCount);
}

#else

void BURGER_API Burger::MP3FHT(float *pInput,WordPtr uCount)
{
	const float *pCosines = &g_MP3FHTCosines[0]->f;
	const float *pInputEnd = pInput + uCount;

	// Initial stride of 16 entries, then 64, 256, 1024
	WordPtr uStride = 16>>2;
	do {
		// Create stride values
		WordPtr uEighthStride = uStride >> 1U;	
		WordPtr uQuarterStride = uStride;
		WordPtr uHalfStride = uStride << 1U;	
		WordPtr u3QuarterStride = uHalfStride + uQuarterStride;
		uStride = uStride << 2U;

		// Apply the 1st pass of the FFT
		float *pWork = pInput;
		float *pWorkAndEighth = pWork + uEighthStride;
		do {
			float fTemp1 = pWork[0] - pWork[uQuarterStride];
			float fTemp2 = pWork[0] + pWork[uQuarterStride];
			float fTemp3 = pWork[uHalfStride] - pWork[u3QuarterStride];
			float fTemp4 = pWork[uHalfStride] + pWork[u3QuarterStride];

			pWork[0] = fTemp2 + fTemp4;
			pWork[uQuarterStride] = fTemp1 + fTemp3;
			pWork[uHalfStride] = fTemp2 - fTemp4;
			pWork[u3QuarterStride] = fTemp1 - fTemp3;

			fTemp1 = pWorkAndEighth[0] - pWorkAndEighth[uQuarterStride];
			fTemp2 = pWorkAndEighth[0] + pWorkAndEighth[uQuarterStride];
			fTemp3 = pWorkAndEighth[u3QuarterStride] * g_fSqrt2;
			fTemp4 = pWorkAndEighth[uHalfStride] * g_fSqrt2;

			pWorkAndEighth[0] = fTemp2 + fTemp4;
			pWorkAndEighth[uQuarterStride] = fTemp1 + fTemp3;
			pWorkAndEighth[uHalfStride] = fTemp2 - fTemp4;
			pWorkAndEighth[u3QuarterStride] = fTemp1 - fTemp3;

			pWork += uStride;
			pWorkAndEighth += uStride;
		} while (pWork < pInputEnd);

		// Apply the 2nd pass of the FFT
		float fCosine1 = pCosines[0];
		float fSine1 = pCosines[1];
		WordPtr uIndex = 1;
		do {
			float fSine2 = fSine1+fSine1;
			float fCosine2 = 1.0f - (fSine2 * fSine1);
			fSine2 = fSine2 * fCosine1;

			pWork = pInput + uIndex;
			pWorkAndEighth = pInput + uQuarterStride - uIndex;
			do {
				float fTerm1 = (fCosine2 * pWork[uQuarterStride]) + (fSine2 * pWorkAndEighth[uQuarterStride]);
				float fTerm2 = (fSine2 * pWork[uQuarterStride]) - (fCosine2 * pWorkAndEighth[uQuarterStride]);
				float fTemp1 = pWork[0] - fTerm1;
				float fTemp2 = pWork[0] + fTerm1;
				float fTemp3 = pWorkAndEighth[0] - fTerm2;
				float fTemp4 = pWorkAndEighth[0] + fTerm2;

				fTerm1 = (fCosine2 * pWork[u3QuarterStride]) + (fSine2 * pWorkAndEighth[u3QuarterStride]);
				fTerm2 = (fSine2 * pWork[u3QuarterStride]) - (fCosine2 * pWorkAndEighth[u3QuarterStride]);
				float fTemp5 = pWork[uHalfStride] - fTerm1;
				float fTemp6 = pWork[uHalfStride] + fTerm1;
				float fTemp7 = pWorkAndEighth[uHalfStride] - fTerm2;
				float fTemp8 = pWorkAndEighth[uHalfStride] + fTerm2;

				fTerm1 = (fSine1 * fTemp6) - (fCosine1 * fTemp7);
				fTerm2 = (fCosine1 * fTemp6) + (fSine1 * fTemp7);
				pWork[0] = fTemp2 + fTerm2;
				pWorkAndEighth[uQuarterStride] = fTemp3 + fTerm1;
				pWork[uHalfStride] = fTemp2 - fTerm2;
				pWorkAndEighth[u3QuarterStride] = fTemp3 - fTerm1;
				
				fTerm1 = (fCosine1 * fTemp8) - (fSine1 * fTemp5);
				fTerm2 = (fSine1 * fTemp8) + (fCosine1 * fTemp5);
				pWorkAndEighth[0] = fTemp4 + fTerm2;
				pWork[uQuarterStride] = fTemp1 + fTerm1;
				pWorkAndEighth[uHalfStride] = fTemp4 - fTerm2;
				pWork[u3QuarterStride] = fTemp1 - fTerm1;

				pWorkAndEighth += uStride;
				pWork += uStride;
			} while (pWork < pInputEnd);

			// Step fCosine and fSine1
			float fTemp = fCosine1;
			fCosine1 = (fCosine1 * pCosines[0]) - (fSine1 * pCosines[1]);
			fSine1 = (fTemp * pCosines[1]) + (fSine1 * pCosines[0]);
		} while (++uIndex < uEighthStride);
		// Next entry in the table
		pCosines += 2;

		// All done? (2 passes or 4 passes for 256 or 1024 entry tables
	} while (uStride < uCount);
}

#endif

/*! ************************************

	\brief 36 entry DCT for MP3

	Perform a 36 entry DCT for MP3 decoding

	"On Computing the Discrete Fourier Transform", 
	Mathematics of Computation, Volume 32, Number 141, January 1978,
	Pages 175-199
	http://www.ams.org/journals/mcom/1978-32-141/S0025-5718-1978-0468306-4/S0025-5718-1978-0468306-4.pdf
	
	\param pSideSamples Pointer a buffer of cSideSampleLimit (18) entries for cascade (Input/Output)
	\param pHybridOutput Pointer a buffer of cSubBandLimit*cSideSampleLimit (32*18) entries for transform (Input/Output)
	\param pOutput Pointer a buffer of cSideSampleLimit (18) entries for output
	\param pPrevious Pointer to the previous output buffer of cSideSampleLimit (18) entries
	\param pBlockType Pointer a 36 entry block type table
	
***************************************/

void BURGER_API Burger::MP3DCT36(float *pSideSamples,float *pHybridOutput,float *pOutput,const float *pPrevious,const float *pBlockType)
{
	// Step one, add each entry with the next one and cascade down
	pSideSamples[17]+=pSideSamples[16];
	pSideSamples[16]+=pSideSamples[15];
	pSideSamples[15]+=pSideSamples[14];
	pSideSamples[14]+=pSideSamples[13];
	pSideSamples[13]+=pSideSamples[12];
	pSideSamples[12]+=pSideSamples[11];
	pSideSamples[11]+=pSideSamples[10];
	pSideSamples[10]+=pSideSamples[9];
	pSideSamples[9] +=pSideSamples[8];
	pSideSamples[8] +=pSideSamples[7];
	pSideSamples[7] +=pSideSamples[6];
	pSideSamples[6] +=pSideSamples[5];
	pSideSamples[5] +=pSideSamples[4];
	pSideSamples[4] +=pSideSamples[3];
	pSideSamples[3] +=pSideSamples[2];
	pSideSamples[2] +=pSideSamples[1];
	pSideSamples[1] +=pSideSamples[0];

	// Now do it again, but with every other entry
	pSideSamples[17]+=pSideSamples[15];
	pSideSamples[15]+=pSideSamples[13];
	pSideSamples[13]+=pSideSamples[11];
	pSideSamples[11]+=pSideSamples[9];
	pSideSamples[9] +=pSideSamples[7];
	pSideSamples[7] +=pSideSamples[5];
	pSideSamples[5] +=pSideSamples[3];
	pSideSamples[3] +=pSideSamples[1];

	// Here's where the fun begins

	// Use a pointer to the constants for RISC chips to reduce generating pointers
	// to globals
	const float *pDCT36Constants = &g_MP3DCT36Constants->f;

	// Obtain some constants at 33% and 66%
	float fTemp33a = pSideSamples[(2*3)+0] * pDCT36Constants[3];
	float fTemp33b = pSideSamples[(2*3)+1] * pDCT36Constants[3];
	float fTemp66a = pSideSamples[(2*6)+0] * pDCT36Constants[6];
	float fTemp66b = pSideSamples[(2*6)+1] * pDCT36Constants[6];

	// Calculate the factors (Read the DCT paper mentioned in the header for details)
	float fFactor1a = (pSideSamples[2*1+0] * pDCT36Constants[1]) + (pSideSamples[2*5+0] * pDCT36Constants[5]) + (pSideSamples[(2*7)+0] * pDCT36Constants[7]) + fTemp33a;
	float fFactor1b = (pSideSamples[2*1+1] * pDCT36Constants[1]) + (pSideSamples[2*5+1] * pDCT36Constants[5]) + (pSideSamples[(2*7)+1] * pDCT36Constants[7]) + fTemp33b;
	float fFactor2a = (pSideSamples[2*2+0] * pDCT36Constants[2]) + (pSideSamples[2*4+0] * pDCT36Constants[4]) + (pSideSamples[2*8+0] * pDCT36Constants[8]) + pSideSamples[2*0+0] + fTemp66a;
	float fFactor2b = (pSideSamples[2*2+1] * pDCT36Constants[2]) + (pSideSamples[2*4+1] * pDCT36Constants[4]) + (pSideSamples[2*8+1] * pDCT36Constants[8]) + pSideSamples[2*0+1] + fTemp66b;

	// Store values into a column in the pHybridOutput buffer
	float fTotal1 = fFactor1a + fFactor2a;
	float fTotal2 = (fFactor1b + fFactor2b) * pDCT36Constants[9+0];
	float fTemp = fTotal1 + fTotal2;
	pOutput[9] = fTemp * pBlockType[27];
	pOutput[8] = fTemp * pBlockType[26];
	fTotal1 -= fTotal2;
	pHybridOutput[DecompressMP3::cSubBandLimit*8] = (fTotal1 * pBlockType[8])+pPrevious[8];
	pHybridOutput[DecompressMP3::cSubBandLimit*9] = (fTotal1 * pBlockType[9])+pPrevious[9];

	fTotal1 = fFactor2a - fFactor1a;
	fTotal2 = (fFactor2b - fFactor1b) * pDCT36Constants[9+8];
	fTemp = fTotal1 + fTotal2;
	pOutput[17] = fTemp * pBlockType[35];
	pOutput[0] = fTemp * pBlockType[18];
	fTotal1 -= fTotal2;
	pHybridOutput[DecompressMP3::cSubBandLimit*0] = (fTotal1 * pBlockType[0])+pPrevious[0];
	pHybridOutput[DecompressMP3::cSubBandLimit*17] = (fTotal1 * pBlockType[17])+pPrevious[17];

	// Next factors
	fFactor1a = ((pSideSamples[2*1+0] - pSideSamples[2*5+0]) - pSideSamples[2*7+0]) * pDCT36Constants[3];
	fFactor1b = ((pSideSamples[2*1+1] - pSideSamples[2*5+1]) - pSideSamples[2*7+1]) * pDCT36Constants[3];
	fFactor2a = ((((pSideSamples[2*2+0] - pSideSamples[2*4+0]) - pSideSamples[2*8+0]) * pDCT36Constants[6]) - pSideSamples[2*6+0]) + pSideSamples[2*0+0];
	fFactor2b = ((((pSideSamples[2*2+1] - pSideSamples[2*4+1]) - pSideSamples[2*8+1]) * pDCT36Constants[6]) - pSideSamples[2*6+1]) + pSideSamples[2*0+1];

	fTotal1 = fFactor1a + fFactor2a;
	fTotal2 = (fFactor1b + fFactor2b) * pDCT36Constants[9+1];
	fTemp = fTotal1 + fTotal2;
	pOutput[10] = fTemp * pBlockType[28];
	pOutput[7] = fTemp * pBlockType[25];
	fTotal1 -= fTotal2;
	pHybridOutput[DecompressMP3::cSubBandLimit*7] = (fTotal1 * pBlockType[7])+pPrevious[7];
	pHybridOutput[DecompressMP3::cSubBandLimit*10] = (fTotal1 * pBlockType[10])+pPrevious[10];
	
	fTotal1 = fFactor2a - fFactor1a;
	fTotal2 = (fFactor2b - fFactor1b) * pDCT36Constants[9+7];
	fTemp = fTotal1 + fTotal2;
	pOutput[16] = fTemp * pBlockType[34];
	pOutput[1] = fTemp * pBlockType[19];
	fTotal1 -= fTotal2;
	pHybridOutput[DecompressMP3::cSubBandLimit*1] = (fTotal1 * pBlockType[1])+pPrevious[1];
	pHybridOutput[DecompressMP3::cSubBandLimit*16] = (fTotal1 * pBlockType[16])+pPrevious[16];

	// Next factors
	fFactor1a = (pSideSamples[2*1+0] * pDCT36Constants[5]) - fTemp33a - (pSideSamples[2*5+0] * pDCT36Constants[7]) + (pSideSamples[2*7+0] * pDCT36Constants[1]);
	fFactor1b = (pSideSamples[2*1+1] * pDCT36Constants[5]) - fTemp33b - (pSideSamples[2*5+1] * pDCT36Constants[7]) + (pSideSamples[2*7+1] * pDCT36Constants[1]);
	fFactor2a = pSideSamples[2*0+0] - (pSideSamples[2*2+0] * pDCT36Constants[8]) - (pSideSamples[2*4+0] * pDCT36Constants[2]) + (pSideSamples[2*8+0] * pDCT36Constants[4]) + fTemp66a;
	fFactor2b = pSideSamples[2*0+1] - (pSideSamples[2*2+1] * pDCT36Constants[8]) - (pSideSamples[2*4+1] * pDCT36Constants[2]) + (pSideSamples[2*8+1] * pDCT36Constants[4]) + fTemp66b;

	fTotal1 = fFactor1a + fFactor2a;
	fTotal2 = (fFactor1b + fFactor2b) * pDCT36Constants[9+2];
	fTemp = fTotal1 + fTotal2;
	pOutput[11] = fTemp * pBlockType[29];
	pOutput[6] = fTemp * pBlockType[24];
	fTotal1 -= fTotal2;
	pHybridOutput[DecompressMP3::cSubBandLimit*6] = (fTotal1 * pBlockType[6])+pPrevious[6];
	pHybridOutput[DecompressMP3::cSubBandLimit*11] = (fTotal1 * pBlockType[11])+pPrevious[11];
	
	fTotal1 = fFactor2a - fFactor1a;
	fTotal2 = (fFactor2b - fFactor1b) * pDCT36Constants[9+6];
	fTemp = fTotal1 + fTotal2;
	pOutput[15] = fTemp * pBlockType[33];
	pOutput[2] = fTemp * pBlockType[20];
	fTotal1 -= fTotal2;
	pHybridOutput[DecompressMP3::cSubBandLimit*2] = (fTotal1 * pBlockType[2])+pPrevious[2];
	pHybridOutput[DecompressMP3::cSubBandLimit*15] = (fTotal1 * pBlockType[15])+pPrevious[15];

	// Next set of factors (Almost done!)
	fFactor1a = (pSideSamples[2*1+0] * pDCT36Constants[7]) - fTemp33a + (pSideSamples[2*5+0] * pDCT36Constants[1]) - (pSideSamples[2*7+0] * pDCT36Constants[5]);
	fFactor1b = (pSideSamples[2*1+1] * pDCT36Constants[7]) - fTemp33b + (pSideSamples[2*5+1] * pDCT36Constants[1]) - (pSideSamples[2*7+1] * pDCT36Constants[5]);
	fFactor2a = pSideSamples[2*0+0] - (pSideSamples[2*2+0] * pDCT36Constants[4]) + (pSideSamples[2*4+0] * pDCT36Constants[8]) - (pSideSamples[2*8+0] * pDCT36Constants[2]) + fTemp66a;
	fFactor2b = pSideSamples[2*0+1] - (pSideSamples[2*2+1] * pDCT36Constants[4]) + (pSideSamples[2*4+1] * pDCT36Constants[8]) - (pSideSamples[2*8+1] * pDCT36Constants[2]) + fTemp66b;

	fTotal1 = fFactor1a + fFactor2a;
	fTotal2 = (fFactor1b + fFactor2b) * pDCT36Constants[9+3];
	fTemp = fTotal1 + fTotal2;
	pOutput[12] = fTemp * pBlockType[30];
	pOutput[5] = fTemp * pBlockType[23];
	fTotal1 -= fTotal2;
	pHybridOutput[DecompressMP3::cSubBandLimit*5] = (fTotal1 * pBlockType[5])+pPrevious[5];
	pHybridOutput[DecompressMP3::cSubBandLimit*12] = (fTotal1 * pBlockType[12])+pPrevious[12];
	
	fTotal1 = fFactor2a - fFactor1a;
	fTotal2 = (fFactor2b - fFactor1b) * pDCT36Constants[9+5];
	fTemp = fTotal1 + fTotal2;
	pOutput[14] = fTemp * pBlockType[32];
	pOutput[3] = fTemp * pBlockType[21];
	fTotal1 -= fTotal2;
	pHybridOutput[DecompressMP3::cSubBandLimit*3] = (fTotal1 * pBlockType[3])+pPrevious[3];
	pHybridOutput[DecompressMP3::cSubBandLimit*14] = (fTotal1 * pBlockType[14])+pPrevious[14];

	// Last set of factors
	fTotal1 = (((pSideSamples[2*0+0] - pSideSamples[2*2+0]) + pSideSamples[2*4+0]) - pSideSamples[2*6+0]) + pSideSamples[2*8+0];
	fTotal2 = ((((pSideSamples[2*0+1] - pSideSamples[2*2+1]) + pSideSamples[2*4+1]) - pSideSamples[2*6+1]) + pSideSamples[2*8+1]) * pDCT36Constants[9+4];
	fTemp = fTotal1 + fTotal2;
	pOutput[13] = fTemp * pBlockType[31];
	pOutput[4] = fTemp * pBlockType[22];
	fTotal1 -= fTotal2;
	pHybridOutput[DecompressMP3::cSubBandLimit*4] = (fTotal1 * pBlockType[4])+pPrevious[4];
	pHybridOutput[DecompressMP3::cSubBandLimit*13] = (fTotal1 * pBlockType[13])+pPrevious[13];
}


/*! ************************************

	\brief 12 entry DCT for MP3

	Perform a 12 entry DCT for MP3 decoding. This is a simpler
	form of DCT36 that only processes 1/3 of the entries

	"On Computing the Discrete Fourier Transform", 
	Mathematics of Computation, Volume 32, Number 141, January 1978,
	Pages 175-199
	http://www.ams.org/journals/mcom/1978-32-141/S0025-5718-1978-0468306-4/S0025-5718-1978-0468306-4.pdf
	
	\param pSideSamples Pointer a buffer of cSideSampleLimit (18) entries for cascade (Input/Output)
	\param pHybridOutput Pointer a buffer of cSubBandLimit*cSideSampleLimit (32*18) entries for transform (Input/Output)
	\param pOutput Pointer a buffer of cSideSampleLimit (18) entries for output
	\param pPrevious Pointer to the previous output buffer of cSideSampleLimit (18) entries
	\param pBlockType Pointer a 36 entry block type table
	
***************************************/

void BURGER_API Burger::MP3DCT12(float *pSideSamples,float *pHybridOutput,float *pOutput,const float *pPrevious,const float *pBlockType)
{
	// Copy the first 6 entries into the column
	pHybridOutput[DecompressMP3::cSubBandLimit*0] = pPrevious[0];
	pHybridOutput[DecompressMP3::cSubBandLimit*1] = pPrevious[1];
	pHybridOutput[DecompressMP3::cSubBandLimit*2] = pPrevious[2];
	pHybridOutput[DecompressMP3::cSubBandLimit*3] = pPrevious[3];
	pHybridOutput[DecompressMP3::cSubBandLimit*4] = pPrevious[4];
	pHybridOutput[DecompressMP3::cSubBandLimit*5] = pPrevious[5];

	const float *pMP3DCT12Constants = &g_MP3DCT12Constants->f;
	// Load the terms in floating point registers
	float fTerm5 = pSideSamples[5*3];
	float fTerm4 = pSideSamples[4*3];
	float fTerm3 = pSideSamples[3*3];
	float fTerm2 = pSideSamples[2*3];
	float fTerm1 = pSideSamples[1*3];
	float fTerm0 = pSideSamples[0*3];

	// Perform the cascade using simple terms for best code
	fTerm5 += fTerm4;
	fTerm4 += fTerm3;
	fTerm3 += fTerm2;
	fTerm2 += fTerm1;
	fTerm1 += fTerm0;
	fTerm5 += fTerm3;
	fTerm3 += fTerm1;
	fTerm2 *= g_fCosPiDiv6;
	fTerm3 *= g_fCosPiDiv6;

	float fTemp2 = (fTerm0 - fTerm4);
	float fTemp3 = (fTerm1 - fTerm5) * pMP3DCT12Constants[1];
	float fTemp1 = fTemp2 + fTemp3;
	fTemp2 -= fTemp3;

	pHybridOutput[DecompressMP3::cSubBandLimit*7] = (fTemp2 * pBlockType[1])+pPrevious[7];
	pHybridOutput[DecompressMP3::cSubBandLimit*10] = (fTemp2 * pBlockType[4])+pPrevious[10];
	pHybridOutput[DecompressMP3::cSubBandLimit*13] = (fTemp1 * pBlockType[7])+pPrevious[13];
	pHybridOutput[DecompressMP3::cSubBandLimit*16] = (fTemp1 * pBlockType[10])+pPrevious[16];

	fTerm0 += fTerm4 * 0.5f;
	fTerm4 = fTerm0 + fTerm2;
	fTerm0 -= fTerm2;
	fTerm1 += fTerm5 * 0.5f;
	fTerm5 = (fTerm1 + fTerm3) * pMP3DCT12Constants[0];
	fTerm1 = (fTerm1 - fTerm3) * pMP3DCT12Constants[2];
	fTerm3 = fTerm4 + fTerm5;
	fTerm4 -= fTerm5;
	fTerm2 = fTerm0 + fTerm1;
	fTerm0 -= fTerm1;

	pHybridOutput[DecompressMP3::cSubBandLimit*6] = (fTerm0 * pBlockType[0])+pPrevious[6];
	pHybridOutput[DecompressMP3::cSubBandLimit*8] = (fTerm4 * pBlockType[2])+pPrevious[8];
	pHybridOutput[DecompressMP3::cSubBandLimit*9] = (fTerm4 * pBlockType[3])+pPrevious[9];
	pHybridOutput[DecompressMP3::cSubBandLimit*11] = (fTerm0 * pBlockType[5])+pPrevious[11];
	pHybridOutput[DecompressMP3::cSubBandLimit*12] = (fTerm2 * pBlockType[6])+pPrevious[12];
	pHybridOutput[DecompressMP3::cSubBandLimit*14] = (fTerm3 * pBlockType[8])+pPrevious[14];
	pHybridOutput[DecompressMP3::cSubBandLimit*15] = (fTerm3 * pBlockType[9])+pPrevious[15];
	pHybridOutput[DecompressMP3::cSubBandLimit*17] = (fTerm2 * pBlockType[11])+pPrevious[17];

	// Next round
	// Load the terms in floating point registers
	fTerm5 = pSideSamples[5*3+1];
	fTerm4 = pSideSamples[4*3+1];
	fTerm3 = pSideSamples[3*3+1];
	fTerm2 = pSideSamples[2*3+1];
	fTerm1 = pSideSamples[1*3+1];
	fTerm0 = pSideSamples[0*3+1];

	// Perform the cascade using simple terms for best code
	fTerm5 += fTerm4;
	fTerm4 += fTerm3;
	fTerm3 += fTerm2;
	fTerm2 += fTerm1;
	fTerm1 += fTerm0;
	fTerm5 += fTerm3;
	fTerm3 += fTerm1;
	fTerm2 *= g_fCosPiDiv6;
	fTerm3 *= g_fCosPiDiv6;

	fTemp2 = (fTerm0-fTerm4);
	fTemp3 = (fTerm1-fTerm5) * pMP3DCT12Constants[1];
	fTemp1 = fTemp2+fTemp3;
	fTemp2 -= fTemp3;
	pOutput[1] = fTemp1 * pBlockType[7];
	pOutput[4] = fTemp1 * pBlockType[10];
	pHybridOutput[DecompressMP3::cSubBandLimit*13] += fTemp2 * pBlockType[1];
	pHybridOutput[DecompressMP3::cSubBandLimit*16] += fTemp2 * pBlockType[4];

	fTerm0 += fTerm4 * 0.5f;
	fTerm4 = fTerm0 + fTerm2;
	fTerm0 -= fTerm2;
	fTerm1 += fTerm5 * 0.5f;
	fTerm5 = (fTerm1 + fTerm3) * pMP3DCT12Constants[0];
	fTerm1 = (fTerm1 - fTerm3) * pMP3DCT12Constants[2];
	fTerm3 = fTerm4 + fTerm5;
	fTerm4 -= fTerm5;
	fTerm2 = fTerm0 + fTerm1;
	fTerm0 -= fTerm1;

	pOutput[0] = fTerm2 * pBlockType[6];
	pOutput[2] = fTerm3 * pBlockType[8];
	pOutput[3] = fTerm3 * pBlockType[9];
	pOutput[5] = fTerm2 * pBlockType[11];

	pHybridOutput[DecompressMP3::cSubBandLimit*12] += fTerm0 * pBlockType[0];
	pHybridOutput[DecompressMP3::cSubBandLimit*14] += fTerm4 * pBlockType[2];
	pHybridOutput[DecompressMP3::cSubBandLimit*15] += fTerm4 * pBlockType[3];
	pHybridOutput[DecompressMP3::cSubBandLimit*17] += fTerm0 * pBlockType[5];

	// Final round
	// Load the terms in floating point registers
	fTerm5 = pSideSamples[5*3+2];
	fTerm4 = pSideSamples[4*3+2];
	fTerm3 = pSideSamples[3*3+2];
	fTerm2 = pSideSamples[2*3+2];
	fTerm1 = pSideSamples[1*3+2];
	fTerm0 = pSideSamples[0*3+2];

	// Perform the cascade using simple terms for best code
	fTerm5 += fTerm4;
	fTerm4 += fTerm3;
	fTerm3 += fTerm2;
	fTerm2 += fTerm1;
	fTerm1 += fTerm0;
	fTerm5 += fTerm3;
	fTerm3 += fTerm1;
	fTerm2 *= g_fCosPiDiv6;
	fTerm3 *= g_fCosPiDiv6;

	fTemp2 = (fTerm0 - fTerm4);
	fTemp3 = (fTerm1 - fTerm5) * pMP3DCT12Constants[1];
	fTemp1 = fTemp2 + fTemp3;
	fTemp2 -= fTemp3;
	pOutput[1] += fTemp2 * pBlockType[1];
	pOutput[4] += fTemp2 * pBlockType[4];
	pOutput[7] = fTemp1 * pBlockType[7];
	pOutput[10] = fTemp1 * pBlockType[10];

	fTerm0 += fTerm4 * 0.5f;
	fTerm4 = fTerm0 + fTerm2;
	fTerm0 -= fTerm2;
	fTerm1 += fTerm5 * 0.5f;
	fTerm5 = (fTerm1 + fTerm3) * pMP3DCT12Constants[0];
	fTerm1 = (fTerm1 - fTerm3) * pMP3DCT12Constants[2];
	fTerm3 = fTerm4 + fTerm5;
	fTerm4 -= fTerm5;
	fTerm2 = fTerm0 + fTerm1;
	fTerm0 -= fTerm1;

	pOutput[0] += fTerm0 * pBlockType[0];
	pOutput[2] += fTerm4 * pBlockType[2];
	pOutput[3] += fTerm4 * pBlockType[3];
	pOutput[5] += fTerm0 * pBlockType[5];

	pOutput[6] = fTerm2 * pBlockType[6];
	pOutput[8] = fTerm3 * pBlockType[8];
	pOutput[9] = fTerm3 * pBlockType[9];
	pOutput[11] = fTerm2 * pBlockType[11];

	// Clear out the rest
	pOutput[12]=0.0f;
	pOutput[13]=0.0f;
	pOutput[14]=0.0f;
	pOutput[15]=0.0f;
	pOutput[16]=0.0f;
	pOutput[17]=0.0f;
}
