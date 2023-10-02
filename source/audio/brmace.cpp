/***************************************

    MACE decompresser

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brmace.h"
#include "brmemoryfunctions.h"
#include "brfixedpoint.h"

#if !defined(DOXYGEN)

#if defined(BURGER_WATCOM)
// Disable 'sizeof' operand contains compiler generated information
#pragma disable_message(549)
#endif

BURGER_CREATE_STATICRTTI_PARENT(Burger::DecompressMace3,Burger::DecompressAudio);
BURGER_CREATE_STATICRTTI_PARENT(Burger::DecompressMace6,Burger::DecompressAudio);
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::DecompressMace3::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/

/*! ************************************

	\var const Burger::StaticRTTI Burger::DecompressMace6::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/

/*! ************************************

	\struct Burger::MACEState_t
	\brief State of MACE compression/decompression

	To initialize this structure, fill all members with zero

***************************************/

/***************************************

	Slope table for 2 bit lookups

***************************************/

const int16_t Burger::MACEState_t::g_Table4Small[4] = {
-   18,   140,   140,-   18
};

const int16_t Burger::MACEState_t::g_Table4Big[512] = {
    64,   216,-  217,-   65,
    67,   226,-  227,-   68,
    70,   236,-  237,-   71,
    74,   246,-  247,-   75,
    77,   257,-  258,-   78,
    80,   268,-  269,-   81,
    84,   280,-  281,-   85,
    88,   294,-  295,-   89,
    92,   307,-  308,-   93,
    96,   321,-  322,-   97,
   100,   334,-  335,-  101,
   104,   350,-  351,-  105,
   109,   365,-  366,-  110,
   114,   382,-  383,-  115,
   119,   399,-  400,-  120,
   124,   416,-  417,-  125,
   130,   434,-  435,-  131,
   136,   454,-  455,-  137,
   142,   475,-  476,-  143,
   148,   495,-  496,-  149,
   155,   519,-  520,-  156,
   162,   541,-  542,-  163,
   169,   564,-  565,-  170,
   176,   590,-  591,-  177,
   185,   617,-  618,-  186,
   193,   644,-  645,-  194,
   201,   673,-  674,-  202,
   210,   703,-  704,-  211,
   220,   735,-  736,-  221,
   230,   767,-  768,-  231,
   240,   801,-  802,-  241,
   251,   838,-  839,-  252,
   262,   876,-  877,-  263,
   274,   914,-  915,-  275,
   286,   955,-  956,-  287,
   299,   997,-  998,-  300,
   312,  1041,- 1042,-  313,
   326,  1089,- 1090,-  327,
   341,  1138,- 1139,-  342,
   356,  1188,- 1189,-  357,
   372,  1241,- 1242,-  373,
   388,  1297,- 1298,-  389,
   406,  1354,- 1355,-  407,
   424,  1415,- 1416,-  425,
   443,  1478,- 1479,-  444,
   462,  1544,- 1545,-  463,
   483,  1613,- 1614,-  484,
   505,  1684,- 1685,-  506,
   527,  1760,- 1761,-  528,
   551,  1838,- 1839,-  552,
   576,  1921,- 1922,-  577,
   601,  2007,- 2008,-  602,
   628,  2097,- 2098,-  629,
   656,  2190,- 2191,-  657,
   686,  2288,- 2289,-  687,
   716,  2389,- 2390,-  717,
   748,  2496,- 2497,-  749,
   781,  2607,- 2608,-  782,
   816,  2724,- 2725,-  817,
   853,  2846,- 2847,-  854,
   891,  2973,- 2974,-  892,
   930,  3104,- 3105,-  931,
   972,  3243,- 3244,-  973,
  1016,  3389,- 3390,- 1017,
  1061,  3539,- 3540,- 1062,
  1108,  3698,- 3699,- 1109,
  1158,  3862,- 3863,- 1159,
  1209,  4035,- 4036,- 1210,
  1264,  4216,- 4217,- 1265,
  1320,  4403,- 4404,- 1321,
  1379,  4599,- 4600,- 1380,
  1441,  4806,- 4807,- 1442,
  1505,  5019,- 5020,- 1506,
  1572,  5244,- 5245,- 1573,
  1642,  5477,- 5478,- 1643,
  1715,  5722,- 5723,- 1716,
  1792,  5978,- 5979,- 1793,
  1872,  6245,- 6246,- 1873,
  1955,  6522,- 6523,- 1956,
  2043,  6813,- 6814,- 2044,
  2134,  7118,- 7119,- 2135,
  2229,  7436,- 7437,- 2230,
  2329,  7767,- 7768,- 2330,
  2432,  8114,- 8115,- 2433,
  2541,  8477,- 8478,- 2542,
  2655,  8854,- 8855,- 2656,
  2773,  9250,- 9251,- 2774,
  2897,  9663,- 9664,- 2898,
  3026, 10094,-10095,- 3027,
  3162, 10546,-10547,- 3163,
  3303, 11016,-11017,- 3304,
  3450, 11508,-11509,- 3451,
  3604, 12020,-12021,- 3605,
  3765, 12556,-12557,- 3766,
  3933, 13118,-13119,- 3934,
  4108, 13703,-13704,- 4109,
  4292, 14315,-14316,- 4293,
  4483, 14953,-14954,- 4484,
  4683, 15621,-15622,- 4684,
  4892, 16318,-16319,- 4893,
  5111, 17046,-17047,- 5112,
  5339, 17807,-17808,- 5340,
  5577, 18602,-18603,- 5578,
  5826, 19433,-19434,- 5827,
  6086, 20300,-20301,- 6087,
  6358, 21205,-21206,- 6359,
  6642, 22152,-22153,- 6643,
  6938, 23141,-23142,- 6939,
  7248, 24173,-24174,- 7249,
  7571, 25252,-25253,- 7572,
  7909, 26380,-26381,- 7910,
  8262, 27557,-27558,- 8263,
  8631, 28786,-28787,- 8632,
  9016, 30072,-30073,- 9017,
  9419, 31413,-31414,- 9420,
  9839, 32767,-32768,- 9840,
 10278, 32767,-32768,-10279,
 10737, 32767,-32768,-10738,
 11216, 32767,-32768,-11217,
 11717, 32767,-32768,-11718,
 12240, 32767,-32768,-12241,
 12786, 32767,-32768,-12787,
 13356, 32767,-32768,-13357,
 13953, 32767,-32768,-13954,
 14576, 32767,-32768,-14577,
 15226, 32767,-32768,-15227,
 15906, 32767,-32768,-15907,
 16615, 32767,-32768,-16616
};

/***************************************

	Slope table for 3 bit lookups

***************************************/

const int16_t Burger::MACEState_t::g_Table8Small[8] = {
-   13,     8,    76,   222,   222,    76,     8,-   13
};

const int16_t Burger::MACEState_t::g_Table8Big[1024] = {
    37,   116,   206,   330,-  331,-  207,-  117,-   38,
    39,   121,   216,   346,-  347,-  217,-  122,-   40,
    41,   127,   225,   361,-  362,-  226,-  128,-   42,
    42,   132,   235,   377,-  378,-  236,-  133,-   43,
    44,   137,   245,   392,-  393,-  246,-  138,-   45,
    46,   144,   256,   410,-  411,-  257,-  145,-   47,
    48,   150,   267,   428,-  429,-  268,-  151,-   49,
    51,   157,   280,   449,-  450,-  281,-  158,-   52,
    53,   165,   293,   470,-  471,-  294,-  166,-   54,
    55,   172,   306,   490,-  491,-  307,-  173,-   56,
    58,   179,   319,   511,-  512,-  320,-  180,-   59,
    60,   187,   333,   534,-  535,-  334,-  188,-   61,
    63,   195,   348,   557,-  558,-  349,-  196,-   64,
    66,   205,   364,   583,-  584,-  365,-  206,-   67,
    69,   214,   380,   609,-  610,-  381,-  215,-   70,
    72,   223,   396,   635,-  636,-  397,-  224,-   73,
    75,   233,   414,   663,-  664,-  415,-  234,-   76,
    79,   244,   433,   694,-  695,-  434,-  245,-   80,
    82,   254,   453,   725,-  726,-  454,-  255,-   83,
    86,   265,   472,   756,-  757,-  473,-  266,-   87,
    90,   278,   495,   792,-  793,-  496,-  279,-   91,
    94,   290,   516,   826,-  827,-  517,-  291,-   95,
    98,   303,   538,   862,-  863,-  539,-  304,-   99,
   102,   316,   562,   901,-  902,-  563,-  317,-  103,
   107,   331,   588,   942,-  943,-  589,-  332,-  108,
   112,   345,   614,   983,-  984,-  615,-  346,-  113,
   117,   361,   641,  1027,- 1028,-  642,-  362,-  118,
   122,   377,   670,  1074,- 1075,-  671,-  378,-  123,
   127,   394,   701,  1123,- 1124,-  702,-  395,-  128,
   133,   411,   732,  1172,- 1173,-  733,-  412,-  134,
   139,   430,   764,  1224,- 1225,-  765,-  431,-  140,
   145,   449,   799,  1280,- 1281,-  800,-  450,-  146,
   152,   469,   835,  1337,- 1338,-  836,-  470,-  153,
   159,   490,   872,  1397,- 1398,-  873,-  491,-  160,
   166,   512,   911,  1459,- 1460,-  912,-  513,-  167,
   173,   535,   951,  1523,- 1524,-  952,-  536,-  174,
   181,   558,   993,  1590,- 1591,-  994,-  559,-  182,
   189,   584,  1038,  1663,- 1664,- 1039,-  585,-  190,
   197,   610,  1085,  1738,- 1739,- 1086,-  611,-  198,
   206,   637,  1133,  1815,- 1816,- 1134,-  638,-  207,
   215,   665,  1183,  1895,- 1896,- 1184,-  666,-  216,
   225,   695,  1237,  1980,- 1981,- 1238,-  696,-  226,
   235,   726,  1291,  2068,- 2069,- 1292,-  727,-  236,
   246,   759,  1349,  2161,- 2162,- 1350,-  760,-  247,
   257,   792,  1409,  2257,- 2258,- 1410,-  793,-  258,
   268,   828,  1472,  2357,- 2358,- 1473,-  829,-  269,
   280,   865,  1538,  2463,- 2464,- 1539,-  866,-  281,
   293,   903,  1606,  2572,- 2573,- 1607,-  904,-  294,
   306,   944,  1678,  2688,- 2689,- 1679,-  945,-  307,
   319,   986,  1753,  2807,- 2808,- 1754,-  987,-  320,
   334,  1030,  1832,  2933,- 2934,- 1833,- 1031,-  335,
   349,  1076,  1914,  3065,- 3066,- 1915,- 1077,-  350,
   364,  1124,  1999,  3202,- 3203,- 2000,- 1125,-  365,
   380,  1174,  2088,  3344,- 3345,- 2089,- 1175,-  381,
   398,  1227,  2182,  3494,- 3495,- 2183,- 1228,-  399,
   415,  1281,  2278,  3649,- 3650,- 2279,- 1282,-  416,
   434,  1339,  2380,  3811,- 3812,- 2381,- 1340,-  435,
   453,  1398,  2486,  3982,- 3983,- 2487,- 1399,-  454,
   473,  1461,  2598,  4160,- 4161,- 2599,- 1462,-  474,
   495,  1526,  2714,  4346,- 4347,- 2715,- 1527,-  496,
   517,  1594,  2835,  4540,- 4541,- 2836,- 1595,-  518,
   540,  1665,  2961,  4741,- 4742,- 2962,- 1666,-  541,
   564,  1740,  3093,  4953,- 4954,- 3094,- 1741,-  565,
   589,  1818,  3232,  5175,- 5176,- 3233,- 1819,-  590,
   615,  1898,  3375,  5405,- 5406,- 3376,- 1899,-  616,
   643,  1984,  3527,  5647,- 5648,- 3528,- 1985,-  644,
   671,  2072,  3683,  5898,- 5899,- 3684,- 2073,-  672,
   701,  2164,  3848,  6161,- 6162,- 3849,- 2165,-  702,
   733,  2261,  4020,  6438,- 6439,- 4021,- 2262,-  734,
   766,  2362,  4199,  6724,- 6725,- 4200,- 2363,-  767,
   800,  2467,  4386,  7024,- 7025,- 4387,- 2468,-  801,
   836,  2578,  4583,  7339,- 7340,- 4584,- 2579,-  837,
   873,  2692,  4786,  7664,- 7665,- 4787,- 2693,-  874,
   912,  2813,  5001,  8008,- 8009,- 5002,- 2814,-  913,
   952,  2938,  5223,  8364,- 8365,- 5224,- 2939,-  953,
   995,  3070,  5457,  8739,- 8740,- 5458,- 3071,-  996,
  1039,  3207,  5701,  9129,- 9130,- 5702,- 3208,- 1040,
  1086,  3350,  5956,  9537,- 9538,- 5957,- 3351,- 1087,
  1134,  3499,  6220,  9960,- 9961,- 6221,- 3500,- 1135,
  1185,  3655,  6497, 10404,-10405,- 6498,- 3656,- 1186,
  1238,  3818,  6788, 10869,-10870,- 6789,- 3819,- 1239,
  1293,  3989,  7091, 11355,-11356,- 7092,- 3990,- 1294,
  1351,  4166,  7407, 11861,-11862,- 7408,- 4167,- 1352,
  1411,  4352,  7738, 12390,-12391,- 7739,- 4353,- 1412,
  1474,  4547,  8084, 12946,-12947,- 8085,- 4548,- 1475,
  1540,  4750,  8444, 13522,-13523,- 8445,- 4751,- 1541,
  1609,  4962,  8821, 14126,-14127,- 8822,- 4963,- 1610,
  1680,  5183,  9215, 14756,-14757,- 9216,- 5184,- 1681,
  1756,  5415,  9626, 15415,-15416,- 9627,- 5416,- 1757,
  1834,  5657, 10057, 16104,-16105,-10058,- 5658,- 1835,
  1916,  5909, 10505, 16822,-16823,-10506,- 5910,- 1917,
  2001,  6173, 10975, 17574,-17575,-10976,- 6174,- 2002,
  2091,  6448, 11463, 18356,-18357,-11464,- 6449,- 2092,
  2184,  6736, 11974, 19175,-19176,-11975,- 6737,- 2185,
  2282,  7037, 12510, 20032,-20033,-12511,- 7038,- 2283,
  2383,  7351, 13068, 20926,-20927,-13069,- 7352,- 2384,
  2490,  7679, 13652, 21861,-21862,-13653,- 7680,- 2491,
  2601,  8021, 14260, 22834,-22835,-14261,- 8022,- 2602,
  2717,  8380, 14897, 23854,-23855,-14898,- 8381,- 2718,
  2838,  8753, 15561, 24918,-24919,-15562,- 8754,- 2839,
  2965,  9144, 16256, 26031,-26032,-16257,- 9145,- 2966,
  3097,  9553, 16982, 27193,-27194,-16983,- 9554,- 3098,
  3236,  9979, 17740, 28407,-28408,-17741,- 9980,- 3237,
  3380, 10424, 18532, 29675,-29676,-18533,-10425,- 3381,
  3531, 10890, 19359, 31000,-31001,-19360,-10891,- 3532,
  3688, 11375, 20222, 32382,-32383,-20223,-11376,- 3689,
  3853, 11883, 21125, 32767,-32768,-21126,-11884,- 3854,
  4025, 12414, 22069, 32767,-32768,-22070,-12415,- 4026,
  4205, 12967, 23053, 32767,-32768,-23054,-12968,- 4206,
  4392, 13546, 24082, 32767,-32768,-24083,-13547,- 4393,
  4589, 14151, 25157, 32767,-32768,-25158,-14152,- 4590,
  4793, 14783, 26280, 32767,-32768,-26281,-14784,- 4794,
  5007, 15442, 27452, 32767,-32768,-27453,-15443,- 5008,
  5231, 16132, 28678, 32767,-32768,-28679,-16133,- 5232,
  5464, 16851, 29957, 32767,-32768,-29958,-16852,- 5465,
  5708, 17603, 31294, 32767,-32768,-31295,-17604,- 5709,
  5963, 18389, 32691, 32767,-32768,-32692,-18390,- 5964,
  6229, 19210, 32767, 32767,-32768,-32768,-19211,- 6230,
  6507, 20067, 32767, 32767,-32768,-32768,-20068,- 6508,
  6797, 20963, 32767, 32767,-32768,-32768,-20964,- 6798,
  7101, 21899, 32767, 32767,-32768,-32768,-21900,- 7102,
  7418, 22876, 32767, 32767,-32768,-32768,-22877,- 7419,
  7749, 23897, 32767, 32767,-32768,-32768,-23898,- 7750,
  8095, 24964, 32767, 32767,-32768,-32768,-24965,- 8096,
  8456, 26078, 32767, 32767,-32768,-32768,-26079,- 8457,
  8833, 27242, 32767, 32767,-32768,-32768,-27243,- 8834,
  9228, 28457, 32767, 32767,-32768,-32768,-28458,- 9229,
  9639, 29727, 32767, 32767,-32768,-32768,-29728,- 9640
};

/*! ************************************

	\brief Reset the state

	Clear out the MACEState_t structure

***************************************/

void BURGER_API Burger::MACEState_t::Clear(void)
{
	MemoryClear(this,sizeof(*this));
}

/*! ************************************

	\brief Perform a decompression step

	Given a 2 or 3 bit number from the MACE compressed stream,
	look up in the coefficient tables that currently active slope
	and determine the value of the next audio sample.

	\note This routine uses some magic numbers.
	It took a while to glean the
	the magic numbers. Braver men have tried
	to understand this routine and failed.

	\param uTableOffset Coefficient index into the slope tables
	\param bUse3Bits \ref TRUE if the offset is 3 bits or \ref FALSE if it's 2 bits wide

***************************************/

int32_t BURGER_API Burger::MACEState_t::CoefficientStep(uint_t uTableOffset,uint_t bUse3Bits)
{
	int32_t iTemp1;

	int32_t iTemp2 = m_iTableIndex;									// Get the previous state
	if (bUse3Bits) {
		iTemp1 = g_Table8Big[((iTemp2>>1)&0x3F8)+uTableOffset];		// Save the index to the table
		iTemp2 = (g_Table8Small[uTableOffset]-(iTemp2>>5))+iTemp2;	// Step to the next table
	} else {
		iTemp1 = g_Table4Big[((iTemp2>>2)&0x1FC)+uTableOffset];		// Save the index to the table
		iTemp2 = (g_Table4Small[uTableOffset]-(iTemp2>>5))+iTemp2;	// Step to the next table
	}
	if (iTemp2<0) {						// Make SURE I don't wrap
		iTemp2 = 0;
	}
	m_iTableIndex = iTemp2;				// Store the new index for next time

	iTemp2 = iTemp1^m_iLastStep;		// Did the direction change?
	iTemp1 = iTemp1+m_iLastAmplitude;	// Move from previous amplitude
	if (iTemp1>0x7FFF) {				// Bounds check!
		iTemp1 = 0x7FFF;
	} else if (iTemp1<-0x7FFF) {
		iTemp1 = -0x7FFF;
	}

	if (!(iTemp2&0x8000)) {			// Same direction?
		iTemp2 = m_iLastSlope+506;	// Magic number (DO NOT CHANGE!)
		if (iTemp2>0x7FFF) {		// Bounds check
			iTemp2 = 0x7FFF;
		}
	} else {
		iTemp2 = m_iLastSlope-314;	// Magic number (DO NOT CHANGE!)
		if (iTemp2<-0x7FFF) {		// Bounds check
			iTemp2 = -0x7FFF;
		}
	}
	m_iLastSlope = iTemp2;			// Store the amplitude
	m_iLastStep = iTemp1;			// Store the current sample
	m_iLastAmplitude = (iTemp1*iTemp1)>>15;	// Get the difference
	return iTemp1;							// Return the sample
}

/*! ************************************

	\brief Decompress audio compressed with MACE 6:1

	This routine will decompress data by
	trying to "predict" where the next sample
	will be. This is a lossy compression targeted
	to unsigned 8 bit samples

	Each call will only decompress one channel of data. To decompress stereo
	input, call this function twice like this

	Each packet of compressed data occupies 1 byte and yields 6 bytes

	\code
		// Decompress mono
		uint8_t SoundBufferMono[1024*6];
		uint8_t CompressedMono[1024];
		Burger::MACEExp1to6(SoundBufferMono,CompressedMono,1024,NULL,NULL,1,1);

		// Decompress stereo
		uint8_t SoundBufferLeft[1024*6];
		uint8_t SoundBufferRight[1024*6];
		uint8_t Compressed[1024*2];
		Burger::MACEExp1to6(SoundBufferLeft,Compressed,1024,NULL,NULL,2,1);
		Burger::MACEExp1to6(SoundBufferRight,Compressed,1024,NULL,NULL,2,2);
	\endcode

	\note The output buffer must be 6*uPacketCount bytes in size!

	\param pInput Pointer to the compressed data
	\param pOutput Pointer to the uint8_t buffer to receive the decompressed data
	\param uPacketCount Number of compressed packets to decompress
	\param pInputState Pointer to a running \ref MACEState_t for partial decompression
	\param pOutputState Pointer to a \ref MACEState_t for the state of the decompression after this call completes
	\param uNumChannels 1 for mono, 2 for stereo
	\param uWhichChannel 1 for left channel, 2 for right channel
	\sa MACEExp1to3()

***************************************/

void BURGER_API Burger::MACEExp1to6(const uint8_t *pInput,uint8_t *pOutput,uintptr_t uPacketCount,MACEState_t *pInputState,MACEState_t *pOutputState,uint_t uNumChannels,uint_t uWhichChannel)
{
	MACEState_t TempState;	// Temp buffer

	// First, is there an input buffer?

	if (!pInputState) {
		MemoryClear(&TempState,sizeof(TempState));		// Zero out my own buffer
		pInputState = &TempState;						// Save the pointer
	}

	// Do I want the output?

	if (!pOutputState) {
		pOutputState = pInputState;		// Use the input as the output
	} else {
		MemoryCopy(pOutputState,pInputState,sizeof(MACEState_t));	// Copy to the next buffer
	}

	// Anything to decompress?

	if (uPacketCount) {
		pInput = pInput+(uWhichChannel-1);				// Index to the first byte from channel #
		int32_t iSample1 = pOutputState->m_iSample1;		// Init the running samples
		int32_t iSample2 = pOutputState->m_iSample2;
		do {
			uint_t uInputByte = pInput[0];				// Get a compressed byte

			int32_t iTempSample = pOutputState->CoefficientStep(uInputByte>>5,TRUE);
			int32_t iHalfSample = iSample1>>1;			// Half adjust
			int32_t iTemp = (iTempSample>>3)+((iSample2*3)>>3)+iHalfSample;
			if (iTemp>0x7FFF) {
				iTemp = 0x7FFF;
			} else if (iTemp<-0x7FFF) {
				iTemp = -0x7FFF;
			}
			pOutput[0] = static_cast<uint8_t>((iTemp>>8)^0x80U);

			iTemp = (iSample2>>3)+((iTempSample*3)>>3)+iHalfSample;
			if (iTemp>0x7FFF) {
				iTemp = 0x7FFF;
			} else if (iTemp<-0x7FFF) {
				iTemp = -0x7fff;
			}
			pOutput[1] = static_cast<uint8_t>((iTemp>>8)^0x80U);

			iSample2 = iSample1;		// Propagate the samples
			iSample1 = iTempSample;

			iTempSample = pOutputState->CoefficientStep((uInputByte>>3)&3,FALSE);
			iHalfSample = iSample1>>1;
			iTemp = (iTempSample>>3)+((iSample2*3)>>3)+iHalfSample;
			if (iTemp>0x7FFF) {
				iTemp = 0x7FFF;
			} else if (iTemp<-0x7FFF) {
				iTemp = -0x7FFF;
			}
			pOutput[2] = static_cast<uint8_t>((iTemp>>8)^0x80U);

			iTemp = (iSample2>>3)+((iTempSample*3)>>3)+iHalfSample;
			if (iTemp>0x7FFF) {
				iTemp = 0x7FFF;
			} else if (iTemp<-0x7fff) {
				iTemp = -0x7FFF;
			}
			pOutput[3] = static_cast<uint8_t>((iTemp>>8)^0x80U);

			iSample2 = iSample1;		// Propagate the samples
			iSample1 = iTempSample;

			iTempSample = pOutputState->CoefficientStep(uInputByte&7U,TRUE);
			iHalfSample = iSample1>>1;
			iTemp = (iTempSample>>3)+((iSample2*3)>>3)+iHalfSample;
			if (iTemp>0x7FFF) {
				iTemp = 0x7FFF;
			} else if (iTemp<-0x7FFF) {
				iTemp = -0x7FFF;
			}
			pOutput[4] = static_cast<uint8_t>((iTemp>>8)^0x80U);

			iTemp = (iSample2>>3)+((iTempSample*3)>>3)+iHalfSample;
			if (iTemp>0x7FFF) {
				iTemp = 0x7FFF;
			} else if (iTemp<-0x7FFF) {
				iTemp = -0x7FFF;
			}
			pOutput[5] = static_cast<uint8_t>((iTemp>>8)^0x80U);
			pOutput+=6;

			// Step the samples
			iSample2 = iSample1;
			iSample1 = iTempSample;

			pInput = pInput+uNumChannels;		// Next channel
		} while (--uPacketCount);				// Still more?
		pOutputState->m_iSample1 = iSample1;	// Save the temp samples
		pOutputState->m_iSample2 = iSample2;
	}
}

/*! ************************************

	\brief Decompress audio compressed with MACE 3:1

	This routine will decompress data by
	trying to "predict" where the next sample
	will be. This is a lossy compression targeted
	to unsigned 8 bit samples

	Each call will only decompress one channel of data. To decompress stereo
	input, call this function twice like this

	Each packet of compressed data occupies 2 bytes and yields 6 bytes

	\code
		// Decompress mono
		uint8_t SoundBufferMono[1024*6];
		uint8_t CompressedMono[1024*2];
		Burger::MACEExp1to3(SoundBufferMono,CompressedMono,1024,NULL,NULL,1,1);

		// Decompress stereo
		uint8_t SoundBufferLeft[1024*6];
		uint8_t SoundBufferRight[1024*6];
		uint8_t Compressed[(1024*2)*2];
		Burger::MACEExp1to3(SoundBufferLeft,Compressed,1024,NULL,NULL,2,1);
		Burger::MACEExp1to3(SoundBufferRight,Compressed,1024,NULL,NULL,2,2);
	\endcode

	\note The output buffer must be 6*uPacketCount bytes in size!

	\param pInput Pointer to the compressed data
	\param pOutput Pointer to the uint8_t buffer to receive the decompressed data
	\param uPacketCount Number of compressed packets to decompress
	\param pInputState Pointer to a running \ref MACEState_t for partial decompression
	\param pOutputState Pointer to a \ref MACEState_t for the state of the decompression after this call completes
	\param uNumChannels 1 for mono, 2 for stereo
	\param uWhichChannel 1 for left channel, 2 for right channel
	\sa MACEExp1to6()

***************************************/

void BURGER_API Burger::MACEExp1to3(const uint8_t *pInput,uint8_t *pOutput,uintptr_t uPacketCount,MACEState_t *pInputState,MACEState_t *pOutputState,uint_t uNumChannels,uint_t uWhichChannel)
{
	MACEState_t TempState;		// Temp buffer

	// First, is there an input buffer?

	if (!pInputState) {
		MemoryClear(&TempState,sizeof(TempState));		// Zero out my own buffer
		pInputState = &TempState;						// Save the pointer
	}

	// Do I want the output?

	if (!pOutputState) {
		pOutputState = pInputState;									// Use the input as the output
	} else {
		MemoryCopy(pOutputState,pInputState,sizeof(MACEState_t));	// Copy to the next buffer
	}

	// Anything to decompress?

	if (uPacketCount) {
		pInput = pInput+((uWhichChannel-1)*2);
		uNumChannels = uNumChannels*2;			// 2 bytes per packet
		int32_t iSample1 = pOutputState->m_iSample1;
		int32_t iSample2 = pOutputState->m_iSample2;
		do {
			uint_t uInputByte = pInput[0];
			uint_t uIndex = uInputByte&7U;
			int32_t Temp = static_cast<int32_t>(((iSample1>>1)&0x3F8)+uIndex);
			iSample1 = iSample1+MACEState_t::g_Table8Small[uIndex]-(iSample1>>5);
			if (iSample1<0) {
				iSample1 = 0;
			}
			Temp = MACEState_t::g_Table8Big[Temp]+iSample2;
			if (Temp<-0x7FFF) {
				Temp = -0x7FFF;
			} else if (Temp>0x7FFF) {
				Temp = 0x7FFF;
			}
			pOutput[0] = static_cast<uint8_t>((Temp>>8)^0x80U);
			iSample2 = Temp-(Temp>>3);

			uIndex = (uInputByte>>3)&3U;
			Temp = static_cast<int32_t>(((iSample1>>2)&0x1FCU)+uIndex);
			iSample1 = iSample1+MACEState_t::g_Table4Small[uIndex]-(iSample1>>5);
			if (iSample1<0) {
				iSample1 = 0;
			}
			Temp = MACEState_t::g_Table4Big[Temp]+iSample2;
			if (Temp<-0x7FFF) {
				Temp = -0x7FFF;
			} else if (Temp>0x7FFF) {
				Temp = 0x7FFF;
			}
			pOutput[1] = static_cast<uint8_t>((Temp>>8)^0x80U);
			iSample2 = Temp-(Temp>>3);

			uInputByte = uInputByte>>5;
			Temp = static_cast<int32_t>(((iSample1>>1)&0x3F8)+uInputByte);
			iSample1 = iSample1+MACEState_t::g_Table8Small[uInputByte]-(iSample1>>5);
			if (iSample1<0) {
				iSample1 = 0;
			}
			Temp = MACEState_t::g_Table8Big[Temp]+iSample2;
			if (Temp<-0x7fff) {
				Temp = -0x7fff;
			} else if (Temp>0x7fff) {
				Temp = 0x7fff;
			}
			pOutput[2] = static_cast<uint8_t>((Temp>>8)^0x80U);
			iSample2 = Temp-(Temp>>3);

			uInputByte = pInput[1];
			uIndex = uInputByte&7U;
			Temp = static_cast<int32_t>(((iSample1>>1)&0x3F8)+uIndex);
			iSample1 = iSample1+MACEState_t::g_Table8Small[uIndex]-(iSample1>>5);
			if (iSample1<0) {
				iSample1 = 0;
			}
			Temp = MACEState_t::g_Table8Big[Temp]+iSample2;
			if (Temp<-0x7FFF) {
				Temp = -0x7FFF;
			} else if (Temp>0x7FFF) {
				Temp = 0x7FFF;
			}
			pOutput[3] = static_cast<uint8_t>((Temp>>8)^0x80U);
			iSample2 = Temp-(Temp>>3);

			uIndex = (uInputByte>>3)&3U;
			Temp = static_cast<int32_t>(((iSample1>>2)&0x1FC)+uIndex);
			iSample1 = iSample1+MACEState_t::g_Table4Small[uIndex]-(iSample1>>5);
			if (iSample1<0) {
				iSample1 = 0;
			}
			Temp = MACEState_t::g_Table4Big[Temp]+iSample2;
			if (Temp<-0x7FFF) {
				Temp = -0x7fff;
			} else if (Temp>0x7FFF) {
				Temp = 0x7fff;
			}
			pOutput[4] = static_cast<uint8_t>((Temp>>8)^0x80U);
			iSample2 = Temp-(Temp>>3);

			uInputByte = uInputByte>>5;
			Temp = static_cast<int32_t>(((iSample1>>1)&0x3F8)+uInputByte);
			iSample1 = iSample1+MACEState_t::g_Table8Small[uInputByte]-(iSample1>>5);
			if (iSample1<0) {
				iSample1 = 0;
			}
			Temp = MACEState_t::g_Table8Big[Temp]+iSample2;
			if (Temp<-0x7fff) {
				Temp = -0x7FFF;
			} else if (Temp>0x7fff) {
				Temp = 0x7fff;
			}
			pOutput[5] = static_cast<uint8_t>((Temp>>8)^0x80U);
			iSample2 = Temp-(Temp>>3);

			pOutput+=6;						// Next output byte
			pInput = pInput+uNumChannels;	// Next channel to decode
		} while (--uPacketCount);

		// Save the state
		pOutputState->m_iSample1 = iSample1;
		pOutputState->m_iSample2 = iSample2;
	}
}



/*! ************************************

	\class Burger::DecompressMace3
	\brief Decompress MACE 3 format
	
	Decompress audio data in MACE 3 format

	\sa \ref Decompress and \ref DecompressMace6

***************************************/

/*! ************************************

	\brief Default constructor

	Initializes the defaults

***************************************/

Burger::DecompressMace3::DecompressMace3() :
	DecompressAudio(SoundManager::TYPEBYTE)
{
	m_uSignature = Signature;
	// Clear out the state
	DecompressMace3::Reset();
}

/*! ************************************

	\brief Resets the MAC3 decompresser to defaults

	\return \ref kErrorNone if successful

***************************************/

Burger::eError Burger::DecompressMace3::Reset(void)
{
	m_uTotalInput = 0;
	m_uTotalOutput = 0;
	MaceStateLeft.Clear();
	MaceStateRight.Clear();
	m_eState = STATE_INIT;
	m_uCacheSize = 0;
	// No worries!
	return kErrorNone;
}

/*! ************************************

	\brief Decompress audio data using MAC3 compression

	Using the MAC3 compression algorithm, decompress the audio data data

	\param pOutput Pointer to the buffer to accept the decompressed data
	\param uOutputChunkLength Number of bytes in the output buffer
	\param pInput Pointer to data to compress
	\param uInputChunkLength Number of bytes in the data to decompress

	\return Decompress::eError code with zero if no failure, non-zero is an error code
	\sa DecompressMace6

***************************************/

Burger::eError Burger::DecompressMace3::Process(void *pOutput, uintptr_t uOutputChunkLength,const void *pInput, uintptr_t uInputChunkLength)
{
	//
	// Handle data "decompression"
	//

	// Save the pointers to determine consumption
	void *pOldOutput = pOutput;
	const void *pOldInput = pInput;

	//
	// Process based on state
	//

	eState uState = m_eState;
	uint_t bAbort = FALSE;

	// Process as many packets as possible
	// 2 bytes per input packet.
	// 6 bytes per output packet
	uintptr_t uInputGranularity = 2;
	uintptr_t uOutputGranularity = 6;
	if (m_bStereo) {
		// Packets are twice as large
		uInputGranularity<<=1;
		uOutputGranularity<<=1;
	}

	do {
		switch (uState) {

		//
		// Cache has not been used, just copy
		//

		case STATE_INIT:
			{

				// Determine the number of packets to process
				uintptr_t uPacketCount = minimum(uInputChunkLength/uInputGranularity,uOutputChunkLength/uOutputGranularity);

				// Any intact packets?
				if (uPacketCount) {
					
					// Remove from the data streams now
					uInputChunkLength -= uPacketCount*uInputGranularity;
					uOutputChunkLength -= uPacketCount*uOutputGranularity;
					
					if (!m_bStereo) {
						// Mono data is easy, decompress directly into output buffer
						MACEExp1to3(static_cast<const uint8_t *>(pInput),static_cast<uint8_t *>(pOutput),uPacketCount,&MaceStateLeft,&MaceStateLeft,1,1);
						pInput = static_cast<const uint16_t *>(pInput)+(uPacketCount*uInputGranularity);
						pOutput = static_cast<uint16_t *>(pOutput)+(uPacketCount*uOutputGranularity);
					} else {

						// MACE doesn't decompress into interleaved stereo buffers.
						// Instead, it decompresses into two mono buffers that
						// need to be recombined into the 
						uint8_t LeftBuffer[512*6];
						uint8_t RightBuffer[512*6];
						do {
							uintptr_t uChunkCount = maximum(uPacketCount,static_cast<uintptr_t>(512));
							MACEExp1to3(static_cast<const uint8_t *>(pInput),LeftBuffer,uChunkCount,&MaceStateLeft,&MaceStateLeft,2,1);
							MACEExp1to3(static_cast<const uint8_t *>(pInput),RightBuffer,uChunkCount,&MaceStateRight,&MaceStateRight,2,2);
							CopyStereoInterleaved(pOutput,LeftBuffer,RightBuffer,(uChunkCount*uOutputGranularity)>>1U);
							pInput = static_cast<const uint16_t *>(pInput)+(uChunkCount*uInputGranularity);
							pOutput = static_cast<uint16_t *>(pOutput)+(uChunkCount*uOutputGranularity);
							uPacketCount-=uChunkCount;
						} while (uPacketCount);
					}					
				}

				//
				// Is there any input data remaining?
				//

				if (uInputChunkLength) {
					// Init the cache
					m_uCacheSize = 0;
					uState = STATE_FILLINGCACHE;
				} else {
					// No more data coming in? Exit
					bAbort = TRUE;
				}
			}
			break;

		//
		// Cache is full, output the data
		//

		case STATE_CACHEFULL:
			if (uOutputChunkLength) {

				// Output 1 or 12 bytes

				uintptr_t uCacheSize = m_uCacheSize;
				uintptr_t uSteps = minimum(uOutputChunkLength,static_cast<uintptr_t>(uCacheSize));

				// Mark the byte(s) as consumed
				uOutputChunkLength -= uSteps;

				// Start copying where it left off
				const uint8_t *pSrc = &m_Cache[uOutputGranularity-uCacheSize];

				// Update the cache size
				uCacheSize = static_cast<uint_t>(uCacheSize-uSteps);
				if (uCacheSize) {
					// Number of bytes remaining in cache
					m_uCacheSize = static_cast<uint_t>(uCacheSize);
				} else {
					// Cache will be empty, so switch to normal mode
					uState = STATE_INIT;
				}

				//
				// Copy out the cache data
				//
				do {
					static_cast<uint8_t *>(pOutput)[0] = pSrc[0];
					++pSrc;
					pOutput = static_cast<uint8_t *>(pOutput)+1;
				} while (--uSteps);
			} else {
				bAbort = TRUE;
			}
			break;

		//
		// Fill up the cache
		//
		case STATE_FILLINGCACHE:
			if (uInputChunkLength) {

				// Get the number of bytes already obtained
				uintptr_t uCacheSize = m_uCacheSize;

				// How many is needed to fill
				uintptr_t uRemaining = uInputGranularity-uCacheSize;

				// Number of bytes to process
				uintptr_t uChunk = minimum(uRemaining,uInputChunkLength);

				// Fill in the cache
				MemoryCopy(&m_Cache[uCacheSize],pInput,uChunk);

				// Consume the input bytes

				pInput = static_cast<const uint8_t *>(pInput)+uChunk;
				uInputChunkLength-=uChunk;

				// Did the cache fill up?
				uCacheSize += uChunk;
				m_uCacheSize = static_cast<uint_t>(uCacheSize);
				if (uCacheSize!=uInputGranularity) {
					bAbort = TRUE;
				} else {
					uState = STATE_PROCESSCACHE;
				}
			} else {
				bAbort = TRUE;
			}
			break;
		//
		// Input cache is full, convert to decompressed data
		//
		case STATE_PROCESSCACHE:
			{
				// Copy the compressed data into the intermediate buffer
				uint8_t Buffer[6*2];
				MemoryCopy(Buffer,m_Cache,uInputGranularity);

				if (!m_bStereo) {
					// Mono is easy!
					MACEExp1to3(Buffer,m_Cache,1,&MaceStateLeft,&MaceStateLeft,1,1);
				} else {
					// Fun, let's generate the two mono buffers and then merge them
					uint8_t TempLeft[6];
					uint8_t TempRight[6];
					MACEExp1to3(TempLeft,m_Cache,1,&MaceStateLeft,&MaceStateLeft,2,1);
					MACEExp1to3(TempRight,m_Cache,1,&MaceStateRight,&MaceStateRight,2,2);
					CopyStereoInterleaved(m_Cache,TempLeft,TempRight,uOutputGranularity>>1U);
				}
			}
			// Cache is full
			m_uCacheSize = static_cast<uint_t>(uOutputGranularity);
			uState = STATE_CACHEFULL;
			break;
		}
	} while (!bAbort);

	// Return the number of bytes actually consumed
	uintptr_t uInputConsumed = static_cast<uintptr_t>(static_cast<const uint8_t *>(pInput)-static_cast<const uint8_t *>(pOldInput));
	uintptr_t uOutputConsumed = static_cast<uintptr_t>(static_cast<const uint8_t *>(pOutput)-static_cast<const uint8_t *>(pOldOutput));

	// Store the amount of data that was processed

	m_uInputLength = uInputConsumed;
	m_uOutputLength = uOutputConsumed;

	// Add the decompressed data to the totals
	m_uTotalInput += uInputConsumed;
	m_uTotalOutput += uOutputConsumed;

	// Output buffer not big enough?
	if (uOutputChunkLength!=uOutputConsumed) {
		return kErrorDataStarvation;
	}

	// Input data remaining?
	if (uInputChunkLength!=uInputConsumed) {
		return kErrorBufferTooSmall;
	}
	// Decompression is complete
	return kErrorNone;
}


/*! ************************************

	\brief Allocate and initialize a DecompressMace3

	\return A pointer to a default DecompressMace3 class or \ref NULL if out of memory
	\sa Delete(const T *)

***************************************/

Burger::DecompressMace3 * BURGER_API Burger::DecompressMace3::New(void)
{
	// Allocate the memory
	return new (Alloc(sizeof(DecompressMace3))) DecompressMace3();
}







/*! ************************************

	\class Burger::DecompressMace6
	\brief Decompress MACE 6 format
	
	Decompress audio data in MACE 6 format

	\sa \ref Decompress and \ref DecompressMace3

***************************************/

/*! ************************************

	\brief Default constructor

	Initializes the defaults

***************************************/

Burger::DecompressMace6::DecompressMace6() :
	DecompressAudio(SoundManager::TYPEBYTE)
{
	m_uSignature = Signature;
	// Clear out the state
	DecompressMace6::Reset();
}

/*! ************************************

	\brief Resets the MAC6 decompresser to defaults

	\return \ref kErrorNone if successful

***************************************/

Burger::eError Burger::DecompressMace6::Reset(void)
{
	m_uTotalInput = 0;
	m_uTotalOutput = 0;
	MaceStateLeft.Clear();
	MaceStateRight.Clear();
	m_eState = STATE_INIT;
	m_uCacheSize = 0;
	// No worries!
	return kErrorNone;
}

/*! ************************************

	\brief Decompress audio data using MAC6 compression

	Using the MAC6 compression algorithm, decompress the audio data data

	\param pOutput Pointer to the buffer to accept the decompressed data
	\param uOutputChunkLength Number of bytes in the output buffer
	\param pInput Pointer to data to compress
	\param uInputChunkLength Number of bytes in the data to decompress

	\return Decompress::eError code with zero if no failure, non-zero is an error code
	\sa DecompressMace6

***************************************/

Burger::eError Burger::DecompressMace6::Process(void *pOutput, uintptr_t uOutputChunkLength,const void *pInput, uintptr_t uInputChunkLength)
{
	//
	// Handle data "decompression"
	//

	// Save the pointers to determine consumption
	void *pOldOutput = pOutput;
	const void *pOldInput = pInput;

	//
	// Process based on state
	//

	eState uState = m_eState;
	uint_t bAbort = FALSE;

	// Process as many packets as possible
	// 1 bytes per input packet.
	// 6 bytes per output packet
	uintptr_t uInputGranularity = 1;
	uintptr_t uOutputGranularity = 6;
	if (m_bStereo) {
		// Packets are twice as large
		uInputGranularity<<=1;
		uOutputGranularity<<=1;
	}

	do {
		switch (uState) {

		//
		// Cache has not been used, just copy
		//

		case STATE_INIT:
			{

				// Determine the number of packets to process
				uintptr_t uPacketCount = minimum(uInputChunkLength/uInputGranularity,uOutputChunkLength/uOutputGranularity);

				// Any intact packets?
				if (uPacketCount) {
					
					// Remove from the data streams now
					uInputChunkLength -= uPacketCount*uInputGranularity;
					uOutputChunkLength -= uPacketCount*uOutputGranularity;
					
					if (!m_bStereo) {
						// Mono data is easy, decompress directly into output buffer
						MACEExp1to6(static_cast<const uint8_t *>(pInput),static_cast<uint8_t *>(pOutput),uPacketCount,&MaceStateLeft,&MaceStateLeft,1,1);
						pInput = static_cast<const uint16_t *>(pInput)+(uPacketCount*uInputGranularity);
						pOutput = static_cast<uint16_t *>(pOutput)+(uPacketCount*uOutputGranularity);
					} else {

						// MACE doesn't decompress into interleaved stereo buffers.
						// Instead, it decompresses into two mono buffers that
						// need to be recombined into the 
						uint8_t LeftBuffer[512*6];
						uint8_t RightBuffer[512*6];
						do {
							uintptr_t uChunkCount = maximum(uPacketCount,static_cast<uintptr_t>(512));
							MACEExp1to6(static_cast<const uint8_t *>(pInput),LeftBuffer,uChunkCount,&MaceStateLeft,&MaceStateLeft,2,1);
							MACEExp1to6(static_cast<const uint8_t *>(pInput),RightBuffer,uChunkCount,&MaceStateRight,&MaceStateRight,2,2);
							CopyStereoInterleaved(pOutput,LeftBuffer,RightBuffer,(uChunkCount*uOutputGranularity)>>1U);
							pInput = static_cast<const uint16_t *>(pInput)+(uChunkCount*uInputGranularity);
							pOutput = static_cast<uint16_t *>(pOutput)+(uChunkCount*uOutputGranularity);
							uPacketCount-=uChunkCount;
						} while (uPacketCount);
					}					
				}

				//
				// Is there any input data remaining?
				//

				if (uInputChunkLength) {
					// Init the cache
					m_uCacheSize = 0;
					uState = STATE_FILLINGCACHE;
				} else {
					// No more data coming in? Exit
					bAbort = TRUE;
				}
			}
			break;

		//
		// Cache is full, output the data
		//

		case STATE_CACHEFULL:
			if (uOutputChunkLength) {

				// Output 1 or 12 bytes

				uintptr_t uCacheSize = m_uCacheSize;
				uintptr_t uSteps = minimum(uOutputChunkLength,static_cast<uintptr_t>(uCacheSize));

				// Mark the byte(s) as consumed
				uOutputChunkLength -= uSteps;

				// Start copying where it left off
				const uint8_t *pSrc = &m_Cache[uOutputGranularity-uCacheSize];

				// Update the cache size
				uCacheSize = static_cast<uint_t>(uCacheSize-uSteps);
				if (uCacheSize) {
					// Number of bytes remaining in cache
					m_uCacheSize = static_cast<uint_t>(uCacheSize);
				} else {
					// Cache will be empty, so switch to normal mode
					uState = STATE_INIT;
				}

				//
				// Copy out the cache data
				//
				do {
					static_cast<uint8_t *>(pOutput)[0] = pSrc[0];
					++pSrc;
					pOutput = static_cast<uint8_t *>(pOutput)+1;
				} while (--uSteps);
			} else {
				bAbort = TRUE;
			}
			break;

		//
		// Fill up the cache
		//
		case STATE_FILLINGCACHE:
			if (uInputChunkLength) {

				// Get the number of bytes already obtained
				uintptr_t uCacheSize = m_uCacheSize;

				// How many is needed to fill
				uintptr_t uRemaining = uInputGranularity-uCacheSize;

				// Number of bytes to process
				uintptr_t uChunk = minimum(uRemaining,uInputChunkLength);

				// Fill in the cache
				MemoryCopy(&m_Cache[uCacheSize],pInput,uChunk);

				// Consume the input bytes

				pInput = static_cast<const uint8_t *>(pInput)+uChunk;
				uInputChunkLength-=uChunk;

				// Did the cache fill up?
				uCacheSize += uChunk;
				m_uCacheSize = static_cast<uint_t>(uCacheSize);
				if (uCacheSize!=uInputGranularity) {
					bAbort = TRUE;
				} else {
					uState = STATE_PROCESSCACHE;
				}
			} else {
				bAbort = TRUE;
			}
			break;
		//
		// Input cache is full, convert to decompressed data
		//
		case STATE_PROCESSCACHE:
			{
				// Copy the compressed data into the intermediate buffer
				uint8_t Buffer[6*2];
				MemoryCopy(Buffer,m_Cache,uInputGranularity);

				if (!m_bStereo) {
					// Mono is easy!
					MACEExp1to6(Buffer,m_Cache,1,&MaceStateLeft,&MaceStateLeft,1,1);
				} else {
					// Fun, let's generate the two mono buffers and then merge them
					uint8_t TempLeft[6];
					uint8_t TempRight[6];
					MACEExp1to6(TempLeft,m_Cache,1,&MaceStateLeft,&MaceStateLeft,2,1);
					MACEExp1to6(TempRight,m_Cache,1,&MaceStateRight,&MaceStateRight,2,2);
					CopyStereoInterleaved(m_Cache,TempLeft,TempRight,uOutputGranularity>>1U);
				}
			}
			// Cache is full
			m_uCacheSize = static_cast<uint_t>(uOutputGranularity);
			uState = STATE_CACHEFULL;
			break;
		}
	} while (!bAbort);

	// Return the number of bytes actually consumed
	uintptr_t uInputConsumed = static_cast<uintptr_t>(static_cast<const uint8_t *>(pInput)-static_cast<const uint8_t *>(pOldInput));
	uintptr_t uOutputConsumed = static_cast<uintptr_t>(static_cast<const uint8_t *>(pOutput)-static_cast<const uint8_t *>(pOldOutput));

	// Store the amount of data that was processed

	m_uInputLength = uInputConsumed;
	m_uOutputLength = uOutputConsumed;

	// Add the decompressed data to the totals
	m_uTotalInput += uInputConsumed;
	m_uTotalOutput += uOutputConsumed;

	// Output buffer not big enough?
	if (uOutputChunkLength!=uOutputConsumed) {
		return kErrorDataStarvation;
	}

	// Input data remaining?
	if (uInputChunkLength!=uInputConsumed) {
		return kErrorBufferTooSmall;
	}
	// Decompression is complete
	return kErrorNone;
}


/*! ************************************

	\brief Allocate and initialize a DecompressMace6

	\return A pointer to a default DecompressMace6 class or \ref NULL if out of memory
	\sa Delete(const T *)

***************************************/

Burger::DecompressMace6 * BURGER_API Burger::DecompressMace6::New(void)
{
	// Allocate the memory
	return new (Alloc(sizeof(DecompressMace6))) DecompressMace6();
}
