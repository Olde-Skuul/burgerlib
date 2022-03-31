/***************************************

	Functions to create the generated source code for Burgerlib

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "createtables.h"
#include "common.h"

#if defined(ALLOWCOMMANDLINE)
#include "brconsolemanager.h"
#include "brendian.h"
#include "brfilemanager.h"
#include "brfloatingpoint.h"
#include "brmatrix3d.h"
#include "brmemoryfunctions.h"
#include "brmp3.h"
#include "brnumberstring.h"
#include "brnumberstringhex.h"
#include <math.h>

//
// Convert a float into an entry for uint32_float_t
//

static void BURGER_API OutputAsHex(
	Burger::OutputMemoryStream* pOutput, float fInput) BURGER_NOEXCEPT
{
	Burger::uint32_float_t Converter;
	Converter.f = fInput;
	// Print the value in hex
	const Burger::NumberStringHex Hexit(Converter.w);
	pOutput->Append("{0x");
	pOutput->Append(Hexit.c_str());
	pOutput->Append("}");
}

//
// Convert an array of floats into a string output
//

static void BURGER_API OutputArrayAsHex(
	const char* pName, const float* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	Message("const Burger::uint32_float_t Burger::%s[%u] = {", pName,
		static_cast<uint_t>(uLength));

	Burger::OutputMemoryStream Output;
	Burger::String TempString;

	Output.Append('\t');
	uint_t uCounter = 0;
	do {
		// Print as hex
		OutputAsHex(&Output, pInput[0]);
		if ((uCounter & 7) == 7 || uCounter == (uLength - 1)) {
			if (uCounter != (uLength - 1)) {
				Output.Append(',');
			}
			Output.Save(&TempString);
			Message(TempString.c_str());
			Output.Clear();
			if (uCounter != (uLength - 1)) {
				Output.Append('\t');
			}
		} else {
			Output.Append(',');
		}
		++pInput;
	} while (++uCounter < uLength);
	Message("};");
}

//
// Convert a 2 dimensional array of floats into a string output
//

static void BURGER_API Output2DArrayAsHex(const char* pName,
	const float* pInput, uintptr_t uLength1, uintptr_t uLength2) BURGER_NOEXCEPT
{
	Message("const Burger::uint32_float_t Burger::%s[%u][%u] = {", pName,
		static_cast<uint_t>(uLength1), static_cast<uint_t>(uLength2));
	Message("{");
	Burger::OutputMemoryStream Output;
	Burger::String TempString;

	uint_t uIndex = 0;
	do {
		uint_t uCounter = 0;
		Output.Clear();
		Output.Append('\t');
		do {
			OutputAsHex(&Output, pInput[0]);
			if ((uCounter & 7) == 7 || (uCounter == (uLength2 - 1))) {
				if (uCounter != (uLength2 - 1)) {
					Output.Append(',');
				}
				Output.Save(&TempString);
				Message(TempString.c_str());
				Output.Clear();
				if (uCounter != (uLength2 - 1)) {
					Output.Append('\t');
				}
			} else {
				Output.Append(',');
			}
			++pInput;
		} while (++uCounter < uLength2);
		if (uIndex != (uLength1 - 1)) {
			Message("},{");
		}
	} while (++uIndex < uLength1);
	Message("}};");
}

//
// Convert an array of unsigned integers into a string output
//

static void BURGER_API OutputArrayAsUnsigned(const char* pName,
	const uint32_t* pInput, uintptr_t uLength,
	uint_t bHex = FALSE) BURGER_NOEXCEPT
{
	Message(
		"const uint_t Burger::%s[%u] = {", pName, static_cast<uint_t>(uLength));

	Burger::OutputMemoryStream Output;
	Burger::String TempString;

	Output.Append('\t');
	uint_t uCounter = 0;
	const uint_t uMask =
		bHex ? 7U : 15U; // Hex numbers group in 8, decimal as 16
	do {
		// Print as unsigned
		if (!bHex) {
			const Burger::NumberString Num(pInput[0]);
			Output.Append(Num.c_str());
		} else {
			const Burger::NumberStringHex Num(pInput[0]);
			Output.Append("0x");
			Output.Append(Num.c_str());
		}
		if ((uCounter & uMask) == uMask || uCounter == (uLength - 1)) {
			if (uCounter != (uLength - 1)) {
				Output.Append(',');
			}
			Output.Save(&TempString);
			Message(TempString.c_str());
			Output.Clear();
			if (uCounter != (uLength - 1)) {
				Output.Append('\t');
			}
		} else {
			Output.Append(',');
		}
		++pInput;
	} while (++uCounter < uLength);
	Message("};");
}

//
// Convert a 2 dimensional array of floats into a string output
//

static void BURGER_API Output2DArrayAsUnsigned(const char* pName,
	const uint32_t* pInput, uintptr_t uLength1,
	uintptr_t uLength2) BURGER_NOEXCEPT
{
	Message("const uint_t Burger::%s[%u][%u] = {", pName,
		static_cast<uint_t>(uLength1), static_cast<uint_t>(uLength2));
	Message("{");
	Burger::OutputMemoryStream Output;
	Burger::String TempString;

	uint_t uIndex = 0;
	do {
		uint_t uCounter = 0;
		Output.Clear();
		Output.Append('\t');
		do {
			const Burger::NumberString Num(pInput[0]);
			Output.Append(Num.c_str());
			if ((uCounter & 15) == 15 || (uCounter == (uLength2 - 1))) {
				if (uCounter != (uLength2 - 1)) {
					Output.Append(',');
				}
				Output.Save(&TempString);
				Message(TempString.c_str());
				Output.Clear();
				if (uCounter != (uLength2 - 1)) {
					Output.Append('\t');
				}
			} else {
				Output.Append(',');
			}
			++pInput;
		} while (++uCounter < uLength2);
		if (uIndex != (uLength1 - 1)) {
			Message("},{");
		}
	} while (++uIndex < uLength1);
	Message("}};");
}

//
// Negate every other entry in an array of floats
//

static void BURGER_API PhaseTable(
	float* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	++pInput;
	uLength >>= 1U;
	do {
		pInput[0] = -pInput[0];
		pInput += 2;
	} while (--uLength);
}

#if 0
//
// Create the Sin/Cosine constants so they can be converted
// to binary for the Sin() and Cos() function constants
//

static void BURGER_API CreateSinConstants(void) BURGER_NOEXCEPT
{
#if defined(BURGER_X86)
	Burger::Set8087Precision(Burger::PRECISION64);
#endif
	double dValueTable[20];
	float fValueTable[20];
	double dInverseTable[20];
	float fInverseTable[20];

	// Process sine constants
	uint_t i = 0;
	double dFactor = 1.0*2.0*3.0;
	double dStep = 4.0;
	do {
		dValueTable[i] = dFactor;
		fValueTable[i] = static_cast<float>(dFactor);
		dInverseTable[i] = 1.0/dFactor;
		fInverseTable[i] = static_cast<float>(1.0/dFactor);
		dFactor = dFactor*dStep;
		dStep = dStep+1.0;
		dFactor = dFactor*dStep;
		dStep = dStep+1.0;
	} while (++i<11);

	Burger::OutputMemoryStream Output;

	//
	// Print the floating point constants for cosine
	//

	Output.Append("// 3!, 5!, to 23!\n");
	Output.Append("#if !defined(BURGER_X86) && !defined(BURGER_MACOSX)\n");
	Output.Append("\tstatic const\n");
	Output.Append("#endif\n");
	Output.Append("Burger::uint32_float_t g_fSineFactors[11] = {\n");
	i = 0;
	do {
		if ((i%5)==0) {
			if (i) {
				Output.Append('\n');
			}
			Output.Append('\t');
		}
		Output.Append("{0x");
		Burger::NumberStringHex Hexit(reinterpret_cast<const uint32_t *>(fValueTable)[i]^(((i^1U)&1U)<<31U));
		Output.Append(Hexit.GetPtr());
		Output.Append("}");
		if (i!=10) {
			Output.Append(',');
		}
	} while (++i<11);
	Output.Append("\n};\n");

	Output.Append("// 1/3!, 1/5!, to 1/23!\n");
	Output.Append("#if !defined(BURGER_X86) && !defined(BURGER_MACOSX)\n");
	Output.Append("\tstatic const\n");
	Output.Append("#endif\n");
	Output.Append("Burger::uint32_float_t g_fInverseSineFactors[11] = {\n");
	i = 0;
	do {
		if ((i%5)==0) {
			if (i) {
				Output.Append('\n');
			}
			Output.Append('\t');
		}
		Output.Append("{0x");
		Burger::NumberStringHex Hexit(reinterpret_cast<const uint32_t *>(fInverseTable)[i]^(((i^1U)&1U)<<31U));
		Output.Append(Hexit.GetPtr());
		Output.Append("}");
		if (i!=10) {
			Output.Append(',');
		}
	} while (++i<11);
	Output.Append("\n};\n");

	//
	// Print the double precision point constants for cosine
	//

	Output.Append("// 3!, 5!, to 23!\n");
	Output.Append("#if !defined(BURGER_X86) && !defined(BURGER_MACOSX)\n");
	Output.Append("\tstatic const\n");
	Output.Append("#endif\n");
	Output.Append("Burger::uint64_double_t g_dSineFactors[11] = {\n");
	i = 0;
	do {
		if ((i%5)==0) {
			if (i) {
				Output.Append('\n');
			}
			Output.Append('\t');
		}
		Output.Append("{0x");
		Burger::NumberStringHex Hexit(reinterpret_cast<const uint64_t *>(dValueTable)[i]^(((static_cast<uint64_t>(i)^1U)&1U)<<63U));
		Output.Append(Hexit.GetPtr());
		Output.Append("ULL}");
		if (i!=10) {
			Output.Append(',');
		}
	} while (++i<11);
	Output.Append("\n};\n");

	Output.Append("// 1/3!, 1/5!, to 1/23!\n");
	Output.Append("#if !defined(BURGER_X86) && !defined(BURGER_MACOSX)\n");
	Output.Append("\tstatic const\n");
	Output.Append("#endif\n");
	Output.Append("Burger::uint64_double_t g_dInverseSineFactors[11] = {\n");
	i = 0;
	do {
		if ((i%5)==0) {
			if (i) {
				Output.Append('\n');
			}
			Output.Append('\t');
		}
		Output.Append("{0x");
		Burger::NumberStringHex Hexit(reinterpret_cast<const uint64_t *>(dInverseTable)[i]^(((static_cast<uint64_t>(i)^1U)&1U)<<63U));
		Output.Append(Hexit.GetPtr());
		Output.Append("ULL}");
		if (i!=10) {
			Output.Append(',');
		}
	} while (++i<11);
	Output.Append("\n};\n");

	//
	// Dump the output
	//

	Burger::String TheString;
	Output.Save(&TheString);
	Message(TheString.GetPtr());
}

static void BURGER_API CreateCosConstants(void) BURGER_NOEXCEPT
{
#if defined(BURGER_X86)
	Burger::Set8087Precision(Burger::PRECISION64);
#endif
	double dValueTable[20];
	float fValueTable[20];
	double dInverseTable[20];
	float fInverseTable[20];

	// Process cosine constants
	uint_t i = 0;
	double dFactor = 1.0*2.0;
	double dStep = 3.0;
	do {
		dValueTable[i] = dFactor;
		fValueTable[i] = static_cast<float>(dFactor);
		dInverseTable[i] = 1.0/dFactor;
		fInverseTable[i] = static_cast<float>(1.0/dFactor);
		dFactor = dFactor*dStep;
		dStep = dStep+1.0;
		dFactor = dFactor*dStep;
		dStep = dStep+1.0;
	} while (++i<11);

	Burger::OutputMemoryStream Output;

	//
	// Print the floating point constants for cosine
	//

	Output.Append("// 2!, 4!, to 22!\n");
	Output.Append("#if !defined(BURGER_X86) && !defined(BURGER_MACOSX)\n");
	Output.Append("\tstatic const\n");
	Output.Append("#endif\n");
	Output.Append("Burger::uint32_float_t g_fCosineFactors[11] = {\n");
	i = 0;
	do {
		if ((i%5)==0) {
			if (i) {
				Output.Append('\n');
			}
			Output.Append('\t');
		}
		Output.Append("{0x");
		Burger::NumberStringHex Hexit(reinterpret_cast<const uint32_t *>(fValueTable)[i]^(((i^1U)&1U)<<31U));
		Output.Append(Hexit.GetPtr());
		Output.Append("}");
		if (i!=10) {
			Output.Append(',');
		}
	} while (++i<11);
	Output.Append("\n};\n");

	Output.Append("// 1/2!, 1/4!, to 1/22!\n");
	Output.Append("#if !defined(BURGER_X86) && !defined(BURGER_MACOSX)\n");
	Output.Append("\tstatic const\n");
	Output.Append("#endif\n");
	Output.Append("Burger::uint32_float_t g_fInverseCosineFactors[11] = {\n");
	i = 0;
	do {
		if ((i%5)==0) {
			if (i) {
				Output.Append('\n');
			}
			Output.Append('\t');
		}
		Output.Append("{0x");
		Burger::NumberStringHex Hexit(reinterpret_cast<const uint32_t *>(fInverseTable)[i]^(((i^1U)&1U)<<31U));
		Output.Append(Hexit.GetPtr());
		Output.Append("}");
		if (i!=10) {
			Output.Append(',');
		}
	} while (++i<11);
	Output.Append("\n};\n");

	//
	// Print the double precision point constants for cosine
	//

	Output.Append("// 2!, 4!, to 22!\n");
	Output.Append("#if !defined(BURGER_X86) && !defined(BURGER_MACOSX)\n");
	Output.Append("\tstatic const\n");
	Output.Append("#endif\n");
	Output.Append("Burger::uint64_double_t g_dCosineFactors[11] = {\n");
	i = 0;
	do {
		if ((i%5)==0) {
			if (i) {
				Output.Append('\n');
			}
			Output.Append('\t');
		}
		Output.Append("{0x");
		Burger::NumberStringHex Hexit(reinterpret_cast<const uint64_t *>(dValueTable)[i]^(((static_cast<uint64_t>(i)^1U)&1U)<<63U));
		Output.Append(Hexit.GetPtr());
		Output.Append("ULL}");
		if (i!=10) {
			Output.Append(',');
		}
	} while (++i<11);
	Output.Append("\n};\n");

	Output.Append("// 1/2!, 1/4!, to 1/22!\n");
	Output.Append("#if !defined(BURGER_X86) && !defined(BURGER_MACOSX)\n");
	Output.Append("\tstatic const\n");
	Output.Append("#endif\n");
	Output.Append("Burger::uint64_double_t g_dInverseCosineFactors[11] = {\n");
	i = 0;
	do {
		if ((i%5)==0) {
			if (i) {
				Output.Append('\n');
			}
			Output.Append('\t');
		}
		Output.Append("{0x");
		Burger::NumberStringHex Hexit(reinterpret_cast<const uint64_t *>(dInverseTable)[i]^(((static_cast<uint64_t>(i)^1U)&1U)<<63U));
		Output.Append(Hexit.GetPtr());
		Output.Append("ULL}");
		if (i!=10) {
			Output.Append(',');
		}
	} while (++i<11);
	Output.Append("\n};\n");

	//
	// Dump the output
	//

	Burger::String TheString;
	Output.Save(&TheString);
	Message(TheString.GetPtr());
}

//
// Create the constants needed for the Euler calculations
//

//
// Using primes to reverse the multiplications,
// create matrices for Yaw, Pitch and Roll
//

static const float g_SineY = 3.0f;
static const float g_NegSineY = 5.0f;
static const float g_CosineY = 7.0f;
Burger::Matrix3D_t g_Yaw3D = {
	{g_CosineY,0,g_NegSineY},
	{0,1,0},
	{g_SineY,0,g_CosineY}
};

static const float g_SineX = 11.0f;
static const float g_NegSineX = 13.0f;
static const float g_CosineX = 17.0f;
Burger::Matrix3D_t g_Pitch3D = {
	{1,0,0},
	{0,g_CosineX,g_NegSineX},
	{0,g_SineX,g_CosineX}
};

static const float g_SineZ = 19.0f;
static const float g_NegSineZ = 23.0f;
static const float g_CosineZ = 29.0f;
Burger::Matrix3D_t g_Roll3D = {
	{g_CosineZ,g_NegSineZ,0},
	{g_SineZ,g_CosineZ,0},
	{0,0,1}
};

struct Factor_t {
	const float *m_pValue;
	const char *m_pName;
};

static const Factor_t FactorTableDocs[9] = {
	{&g_SineY,"sy"},
	{&g_NegSineY,"-sy"},
	{&g_CosineY,"cy"},
	{&g_SineX,"sx"},
	{&g_NegSineX,"-sx"},
	{&g_CosineX,"cx"},
	{&g_SineZ,"sz"},
	{&g_NegSineZ,"-sz"},
	{&g_CosineZ,"cz"}
};

static const Factor_t FactorTableCode[9] = {
	{&g_SineY,"fSY"},
	{&g_NegSineY,"-fSY"},
	{&g_CosineY,"fCY"},
	{&g_SineX,"fSX"},
	{&g_NegSineX,"-fSX"},
	{&g_CosineX,"fCX"},
	{&g_SineZ,"fSZ"},
	{&g_NegSineZ,"-fSZ"},
	{&g_CosineZ,"fCZ"}
};
//
// Factor a number to its core parts
// Return TRUE if factoring was successful
//

static uint_t BURGER_API SimpleFactor(Burger::OutputMemoryStream *pOutput,const Factor_t *pFactor,float fInput) BURGER_NOEXCEPT
{
	uint_t bSuccess = 0;
	// Factor the primes!
	uint_t i = 0;
	do {
		// Divide by a test prime
		float fTest = fInput/pFactor->m_pValue[0];
		// Did it yield a whole number?
		if (fTest==Burger::Round(fTest)) {
			// Accept it!
			fInput = fTest;
			if (pOutput) {
				if (bSuccess) {
					pOutput->Append('*');
				}
				pOutput->Append(pFactor->m_pName);
			}
			++bSuccess;
			// Factored to the last value?
			if (fInput==1.0f) {
				break;
			}
		}
		++pFactor;
	} while (++i<9);
	if (fInput!=1.0f) {
		bSuccess = 0;
	}
	return bSuccess;
}

//
// Factor a complex number with also a possible
// addition in the middle
//

static uint_t BURGER_API Factor(Burger::OutputMemoryStream *pOutput,const char *pName,const Factor_t *pFactor,float fInput,float fPrevious) BURGER_NOEXCEPT
{
	uint_t bSuccess=0;
	pOutput->Append(pName);

	// Already factored?
	if (fInput==0.0f) {
		pOutput->Append('0');
		bSuccess = 1;
	// 1 is already factored
	} else if (fInput==1.0f) {
		pOutput->Append('1');
		bSuccess = 1;
	// If odd, then it's pure primes
	} else if ((fInput*0.5f)!=Burger::Round(fInput*0.5f)) {
		bSuccess = SimpleFactor(pOutput,pFactor,fInput);
	} else {
		// It's even, it means two groups of primes,
		// with one being a pair and the other
		// a possible triplet

		// Is the previous number a single prime, or two primes multiplied together?

		uint_t x = 0;
		do {
			if (pFactor[x].m_pValue[0]==fPrevious) {
				break;
			}
		} while (++x<9);

		if (x<9) {
			uint_t j=0;
			do {
				if (j!=x) {
					// Create the possible pair
					float fBisect = fPrevious*pFactor[j].m_pValue[0];
					float fRemainder = fInput-fBisect;
					if (fRemainder>=3.0f) {
						float fFixer1 = fRemainder/fPrevious;
						float fFixer2 = fRemainder/pFactor[j].m_pValue[0];
						if ((fFixer1!=Burger::Round(fFixer1)) && (fFixer2!=Burger::Round(fFixer2))) {
							// Is the remainder a set of primes?
							if (SimpleFactor(NULL,pFactor,fInput-fBisect)==3) {
								// Looks like it was found, output the initial pair
								pOutput->Append('(');
								pOutput->Append(pFactor[x].m_pName);
								pOutput->Append('*');
								pOutput->Append(pFactor[j].m_pName);
								pOutput->Append(")+(");
								// Output the successful factored pair or triplet
								bSuccess = SimpleFactor(pOutput,pFactor,fInput-fBisect)+2;
								pOutput->Append(')');
								// Force exit
								break;
							}
						}
					}
				}
			} while (++j<9);
		} else {
			uint_t j=0;
			do {
				// Create the possible triplet
				float fBisect = fPrevious*pFactor[j].m_pValue[0];
				float fRemainder = fInput-fBisect;
				if (fRemainder>=3.0f) {
					float fFixer1 = fRemainder/fPrevious;
					float fFixer2 = fRemainder/pFactor[j].m_pValue[0];
					if ((fFixer1!=Burger::Round(fFixer1)) && (fFixer2!=Burger::Round(fFixer2))) {
						// Is the remainder a set of primes?
						if (SimpleFactor(NULL,pFactor,fInput-fBisect)==2) {
							// Looks like it was found, output the initial pair
							pOutput->Append('(');
							SimpleFactor(pOutput,pFactor,fPrevious);
							pOutput->Append('*');
							pOutput->Append(pFactor[j].m_pName);
							pOutput->Append(")+(");
							// Output the successful factored pair or triplet
							bSuccess = SimpleFactor(pOutput,pFactor,fInput-fBisect)+3;
							pOutput->Append(')');
							// Force exit
							break;
						}
					}
				}
			} while (++j<9);
		}
	}
	return bSuccess;
}

//
// There are six combinations for Yaw/Pitch/Roll
// To prevent errors, create the formulas for the rotations and
// output them to the message stream
//

struct RotationTypes_t {
	const char *m_pName;
	const Burger::Matrix3D_t *m_pMatrices[3];
};

static const RotationTypes_t Rotations[6] = {
	{"YXZ",{&g_Yaw3D,&g_Pitch3D,&g_Roll3D}},
	{"YZX",{&g_Yaw3D,&g_Roll3D,&g_Pitch3D}},
	{"XYZ",{&g_Pitch3D,&g_Yaw3D,&g_Roll3D}},
	{"XZY",{&g_Pitch3D,&g_Roll3D,&g_Yaw3D}},
	{"ZYX",{&g_Roll3D,&g_Yaw3D,&g_Pitch3D}},
	{"ZXY",{&g_Roll3D,&g_Pitch3D,&g_Yaw3D}}
};

static void BURGER_API CreateEulerRotations(void) BURGER_NOEXCEPT
{
	Burger::Matrix3D_t Result;
	Burger::Matrix3D_t TempMatrix;

	uint_t i = 0;
	const RotationTypes_t *pRotations = Rotations;
	do {
		Burger::OutputMemoryStream Output;
		TempMatrix.Multiply(pRotations->m_pMatrices[0],pRotations->m_pMatrices[1]);
		Result.Multiply(&TempMatrix,pRotations->m_pMatrices[2]);
		Output.Append(pRotations->m_pName);
		Output.Append('\n');
		Factor(&Output,"\t<table border=\"1\" style=\"margin-right:auto;margin-left:auto;text-align:center;width:80%%\">\n\t<tr><th/><th>x</th><th>y</th><th>z</th></tr>\n\t<tr><th>x</th><td>",FactorTableDocs,Result.x.x,TempMatrix.x.x);
		Factor(&Output,"</td><td>",FactorTableDocs,Result.x.y,TempMatrix.x.y);
		Factor(&Output,"</td><td>",FactorTableDocs,Result.x.z,TempMatrix.x.z);
		Factor(&Output,"</td></tr>\n\t<tr><th>y</th><td>",FactorTableDocs,Result.y.x,TempMatrix.y.x);
		Factor(&Output,"</td><td>",FactorTableDocs,Result.y.y,TempMatrix.y.y);
		Factor(&Output,"</td><td>",FactorTableDocs,Result.y.z,TempMatrix.y.z);
		Factor(&Output,"</td></tr>\n\t<tr><th>z</th><td>",FactorTableDocs,Result.z.x,TempMatrix.z.x);
		Factor(&Output,"</td><td>",FactorTableDocs,Result.z.y,TempMatrix.z.y);
		Factor(&Output,"</td><td>",FactorTableDocs,Result.z.z,TempMatrix.z.z);
		Output.Append("</td></tr>\n\t</table>\n");

		Output.Append("// Code for ");
		Output.Append(pRotations->m_pName);
		Output.Append('\n');
		Factor(&Output,"\tx.x = ",FactorTableCode,Result.x.x,TempMatrix.x.x);
		Factor(&Output,";\n\tx.y = ",FactorTableCode,Result.x.y,TempMatrix.x.y);
		Factor(&Output,";\n\tx.z = ",FactorTableCode,Result.x.z,TempMatrix.x.z);
		Factor(&Output,";\n\n\ty.x = ",FactorTableCode,Result.y.x,TempMatrix.y.x);
		Factor(&Output,";\n\ty.y = ",FactorTableCode,Result.y.y,TempMatrix.y.y);
		Factor(&Output,";\n\ty.z = ",FactorTableCode,Result.y.z,TempMatrix.y.z);
		Factor(&Output,";\n\n\tz.x = ",FactorTableCode,Result.z.x,TempMatrix.z.x);
		Factor(&Output,";\n\tz.y = ",FactorTableCode,Result.z.y,TempMatrix.z.y);
		Factor(&Output,";\n\tz.z = ",FactorTableCode,Result.z.z,TempMatrix.z.z);
		Output.Append(";\n\n");

		Burger::String Temp;
		Output.Save(&Temp);
		Message(Temp.GetPtr());
		++pRotations;
	} while (++i<6);

	i = 0;
	pRotations = Rotations;
	do {
		Burger::OutputMemoryStream Output;
		Burger::Matrix3D_t Trans;
		Result.Transpose(pRotations->m_pMatrices[0]);
		Trans.Transpose(pRotations->m_pMatrices[1]);
		TempMatrix.Multiply(&Result,&Trans);
		Trans.Transpose(pRotations->m_pMatrices[2]);
		Result.Multiply(&TempMatrix,&Trans);
		Output.Append("Transpose");
		Output.Append(pRotations->m_pName);
		Output.Append('\n');
		Factor(&Output,"\t<table border=\"1\" style=\"margin-right:auto;margin-left:auto;text-align:center;width:80%%\">\n\t<tr><th/><th>x</th><th>y</th><th>z</th></tr>\n\t<tr><th>x</th><td>",FactorTableDocs,Result.x.x,TempMatrix.x.x);
		Factor(&Output,"</td><td>",FactorTableDocs,Result.x.y,TempMatrix.x.y);
		Factor(&Output,"</td><td>",FactorTableDocs,Result.x.z,TempMatrix.x.z);
		Factor(&Output,"</td></tr>\n\t<tr><th>y</th><td>",FactorTableDocs,Result.y.x,TempMatrix.y.x);
		Factor(&Output,"</td><td>",FactorTableDocs,Result.y.y,TempMatrix.y.y);
		Factor(&Output,"</td><td>",FactorTableDocs,Result.y.z,TempMatrix.y.z);
		Factor(&Output,"</td></tr>\n\t<tr><th>z</th><td>",FactorTableDocs,Result.z.x,TempMatrix.z.x);
		Factor(&Output,"</td><td>",FactorTableDocs,Result.z.y,TempMatrix.z.y);
		Factor(&Output,"</td><td>",FactorTableDocs,Result.z.z,TempMatrix.z.z);
		Output.Append("</td></tr>\n\t</table>\n");

		Output.Append("// Code for ");
		Output.Append(pRotations->m_pName);
		Output.Append('\n');
		Factor(&Output,"\tx.x = ",FactorTableCode,Result.x.x,TempMatrix.x.x);
		Factor(&Output,";\n\tx.y = ",FactorTableCode,Result.x.y,TempMatrix.x.y);
		Factor(&Output,";\n\tx.z = ",FactorTableCode,Result.x.z,TempMatrix.x.z);
		Factor(&Output,";\n\n\ty.x = ",FactorTableCode,Result.y.x,TempMatrix.y.x);
		Factor(&Output,";\n\ty.y = ",FactorTableCode,Result.y.y,TempMatrix.y.y);
		Factor(&Output,";\n\ty.z = ",FactorTableCode,Result.y.z,TempMatrix.y.z);
		Factor(&Output,";\n\n\tz.x = ",FactorTableCode,Result.z.x,TempMatrix.z.x);
		Factor(&Output,";\n\tz.y = ",FactorTableCode,Result.z.y,TempMatrix.z.y);
		Factor(&Output,";\n\tz.z = ",FactorTableCode,Result.z.z,TempMatrix.z.z);
		Output.Append(";\n\n");

		Burger::String Temp;
		Output.Save(&Temp);
		Message(Temp.GetPtr());
		++pRotations;
	} while (++i<6);
}

/***************************************

	Generate sqrt tables for the PowerPC
	version of sqrt and sqrtf

***************************************/

static void BURGER_API CreateSqrtGuesses(void) BURGER_NOEXCEPT
{
	Message("static const Burger::uint32_float_t g_PPCSqrtGuess[2][256][2] = {{");

	Burger::OutputMemoryStream Output;

	// Generate the even roots
	Burger::String TempString;

	uint_t uCount = 256;
	double dStep = 0.5;
	Output.Append('\t');
	do {
		// Get the actual square root
		double dSqrt = sqrt(dStep);

		// Get the square root of the next step to get the rounding
		double dNextSqrt = sqrt(dStep+(1.0/512.0));

		// Generate the square root estimate by averaging between
		// the steps

		// Print the value in hex
		OutputAsHex(&Output,static_cast<float>((dSqrt+dNextSqrt)*0.5));
		Output.Append(',');

		// Get the reciprocals
		double dReciprocal2Guess = 1.0/(2.0*dSqrt);
		double dNextReciprocal2Guess = 1.0/(2.0*dNextSqrt);

		OutputAsHex(&Output,static_cast<float>((dReciprocal2Guess+dNextReciprocal2Guess)*0.5));

		if ((uCount&3)==1) {
			if (uCount!=1) {
				Output.Append(',');
			}
			Output.Save(&TempString);
			Message(TempString.GetPtr());
			Output.Clear();
			if (uCount!=1) {
				Output.Append('\t');
			}
		} else {
			Output.Append(',');
		}

		// Next step
		dStep = dStep + (1.0/512.0);
	} while (--uCount);

	Message("},{");

	// Generate the odd roots

	uCount = 256;
	dStep = 0.5;
	do {
		// Get the actual square root * 2
		double dSqrt = sqrt(dStep*2.0);

		// Get the square root of the next step to get the rounding
		double dNextSqrt = sqrt((dStep+1/512.0)*2.0);

		// Generate the square root estimate by averaging between
		// the steps

		// Print the value in hex
		OutputAsHex(&Output,static_cast<float>((dSqrt+dNextSqrt)*0.5));
		Output.Append(',');

		// Get the reciprocals
		double dReciprocal2Guess = 1.0/(2.0*dSqrt);
		double dNextReciprocal2Guess = 1.0/(2.0*dNextSqrt);

		OutputAsHex(&Output,static_cast<float>((dReciprocal2Guess+dNextReciprocal2Guess)*0.5));

		if ((uCount&3)==1) {
			if (uCount!=1) {
				Output.Append(',');
			}
			Output.Save(&TempString);
			Message(TempString.GetPtr());
			Output.Clear();
			if (uCount!=1) {
				Output.Append('\t');
			}
		} else {
			Output.Append(',');
		}

		// Next step
		dStep = dStep + (1.0/512.0);
	} while (--uCount);

	Message("}};");
}

#endif

/***************************************

	Create the decode tables for MP3
	Table obtained from the MP3 docs scattered over the internet

***************************************/

//
// Synth window base constants
//

static const double g_dSynthWindow[257] = {0.000000000, -0.000015259,
	-0.000015259, -0.000015259, -0.000015259, -0.000015259, -0.000015259,
	-0.000030518, -0.000030518, -0.000030518, -0.000030518, -0.000045776,
	-0.000045776, -0.000061035, -0.000061035, -0.000076294, -0.000076294,
	-0.000091553, -0.000106812, -0.000106812, -0.000122070, -0.000137329,
	-0.000152588, -0.000167847, -0.000198364, -0.000213623, -0.000244141,
	-0.000259399, -0.000289917, -0.000320435, -0.000366211, -0.000396729,
	-0.000442505, -0.000473022, -0.000534058, -0.000579834, -0.000625610,
	-0.000686646, -0.000747681, -0.000808716, -0.000885010, -0.000961304,
	-0.001037598, -0.001113892, -0.001205444, -0.001296997, -0.001388550,
	-0.001480103, -0.001586914, -0.001693726, -0.001785278, -0.001907349,
	-0.002014160, -0.002120972, -0.002243042, -0.002349854, -0.002456665,
	-0.002578735, -0.002685547, -0.002792358, -0.002899170, -0.002990723,
	-0.003082275, -0.003173828, -0.003250122, -0.003326416, -0.003387451,
	-0.003433228, -0.003463745, -0.003479004, -0.003479004, -0.003463745,
	-0.003417969, -0.003372192, -0.003280640, -0.003173828, -0.003051758,
	-0.002883911, -0.002700806, -0.002487183, -0.002227783, -0.001937866,
	-0.001617432, -0.001266479, -0.000869751, -0.000442505, 0.000030518,
	0.000549316, 0.001098633, 0.001693726, 0.002334595, 0.003005981,
	0.003723145, 0.004486084, 0.005294800, 0.006118774, 0.007003784,
	0.007919312, 0.008865356, 0.009841919, 0.010848999, 0.011886597,
	0.012939453, 0.014022827, 0.015121460, 0.016235352, 0.017349243,
	0.018463135, 0.019577026, 0.020690918, 0.021789551, 0.022857666,
	0.023910522, 0.024932861, 0.025909424, 0.026840210, 0.027725220,
	0.028533936, 0.029281616, 0.029937744, 0.030532837, 0.031005859,
	0.031387329, 0.031661987, 0.031814575, 0.031845093, 0.031738281,
	0.031478882, 0.031082153, 0.030517578, 0.029785156, 0.028884888,
	0.027801514, 0.026535034, 0.025085449, 0.023422241, 0.021575928,
	0.019531250, 0.017257690, 0.014801025, 0.012115479, 0.009231567,
	0.006134033, 0.002822876, -0.000686646, -0.004394531, -0.008316040,
	-0.012420654, -0.016708374, -0.021179199, -0.025817871, -0.030609131,
	-0.035552979, -0.040634155, -0.045837402, -0.051132202, -0.056533813,
	-0.061996460, -0.067520142, -0.073059082, -0.078628540, -0.084182739,
	-0.089706421, -0.095169067, -0.100540161, -0.105819702, -0.110946655,
	-0.115921021, -0.120697021, -0.125259399, -0.129562378, -0.133590698,
	-0.137298584, -0.140670776, -0.143676758, -0.146255493, -0.148422241,
	-0.150115967, -0.151306152, -0.151962280, -0.152069092, -0.151596069,
	-0.150497437, -0.148773193, -0.146362305, -0.143264771, -0.139450073,
	-0.134887695, -0.129577637, -0.123474121, -0.116577148, -0.108856201,
	-0.100311279, -0.090927124, -0.080688477, -0.069595337, -0.057617187,
	-0.044784546, -0.031082153, -0.016510010, -0.001068115, 0.015228271,
	0.032379150, 0.050354004, 0.069168091, 0.088775635, 0.109161377,
	0.130310059, 0.152206421, 0.174789429, 0.198059082, 0.221984863,
	0.246505737, 0.271591187, 0.297210693, 0.323318481, 0.349868774,
	0.376800537, 0.404083252, 0.431655884, 0.459472656, 0.487472534,
	0.515609741, 0.543823242, 0.572036743, 0.600219727, 0.628295898,
	0.656219482, 0.683914185, 0.711318970, 0.738372803, 0.765029907,
	0.791213989, 0.816864014, 0.841949463, 0.866363525, 0.890090942,
	0.913055420, 0.935195923, 0.956481934, 0.976852417, 0.996246338,
	1.014617920, 1.031936646, 1.048156738, 1.063217163, 1.077117920,
	1.089782715, 1.101211548, 1.111373901, 1.120223999, 1.127746582,
	1.133926392, 1.138763428, 1.142211914, 1.144287109, 1.144989014};

static void BURGER_API CreateMP3Tables(void) BURGER_NOEXCEPT
{
	Burger::OutputMemoryStream Output;
	Burger::String TempString;
	float TempFloats[10240];
	float* pWork;

	//
	// Create the five cosine tables
	//

	uint_t uCounter = 0;
	do {
		uint_t uEntries = 16U >> uCounter;
		const uint_t uDivisor = 64U >> uCounter;

		const double dDivisor =
			1.0 / static_cast<double>(static_cast<int>(uDivisor));
		double dStep = 1.0;
		pWork = TempFloats;
		do {
			// Create the constants
			pWork[0] = static_cast<float>(
				1.0 / (cos(Burger::g_dPi * dStep * dDivisor) * 2.0));
			++pWork;
			// Next step
			dStep = dStep + 2.0;
		} while (--uEntries);

		TempString.Printf("DecompressMP3::g_MP3Cosine%u", uDivisor);
		OutputArrayAsHex(TempString.c_str(), TempFloats, 16U >> uCounter);
	} while (++uCounter < 5);

	// Buffer to scatter fill

	Burger::MemoryClear(TempFloats, sizeof(TempFloats));
	pWork = TempFloats;
	// Scale to 16 bit int
	double dScaleval = -32767.0;
	uCounter = 0;
	do {
		if (pWork < &TempFloats[512 + 16]) {
			pWork[16] = pWork[0] =
				static_cast<float>(g_dSynthWindow[uCounter] * dScaleval);
		}
		if ((uCounter & 31U) == 31U) {
			pWork -= 1023;
		}
		if ((uCounter & 63U) == 63U) {
			dScaleval = -dScaleval;
		}
		pWork += 32;
	} while (++uCounter < 256U);

	// uCounter = 256
	do {
		if (pWork < &TempFloats[512 + 16]) {
			pWork[16] = pWork[0] =
				static_cast<float>(g_dSynthWindow[512 - uCounter] * dScaleval);
		}
		if ((uCounter & 31U) == 31U) {
			pWork -= 1023;
		}
		if ((uCounter & 63U) == 63U) {
			dScaleval = -dScaleval;
		}
		pWork += 32;
	} while (++uCounter < 512U);
	OutputArrayAsHex("DecompressMP3::g_fDecodeWindow", TempFloats, 512 + 32);

	//
	// Tables for Layer 2 decoding
	//

	// Generate the group tables
	static const uint8_t g_MP2TableValues[3][9] = {
		{1, 0, 2}, {17, 18, 0, 19, 20}, {21, 1, 22, 23, 0, 24, 25, 2, 26}};
	static const uint8_t g_MP2TableSizes[3] = {3, 5, 9};
	static const char* g_MP2TableNames[3] = {
		"g_GroupTable3", "g_GroupTable5", "g_GroupTable9"};
	uCounter = 0;
	do {
		uint_t uTableSize = g_MP2TableSizes[uCounter];
		Message("const uint8_t Burger::DecompressMP3::%s[%u][%u][%u][3] = {",
			g_MP2TableNames[uCounter], uTableSize, uTableSize, uTableSize);
		Message("{");

		uint_t j = 0;
		do {
			uint_t k = 0;
			do {
				Output.Append("\t{");
				uint_t l = 0;
				do {
					TempString.Printf("{%u,%u,%u}",
						g_MP2TableValues[uCounter][l],
						g_MP2TableValues[uCounter][k],
						g_MP2TableValues[uCounter][j]);
					Output.Append(TempString.c_str());
					if (l != (uTableSize - 1)) {
						Output.Append(',');
					}
				} while (++l < uTableSize);
				Output.Append('}');
				if (k != (uTableSize - 1)) {
					Output.Append(',');
				}
				Output.Save(&TempString);
				Message(TempString.c_str());
				Output.Clear();
			} while (++k < uTableSize);

			if (j != (uTableSize - 1)) {
				Message("},{");
			}

		} while (++j < uTableSize);
		Message("}};");
	} while (++uCounter < 3);

	// Layer 2 scaling table

	static const double g_MP3MulTable64Raw[27] = {0.0, -2.0 / 3.0, 2.0 / 3.0,
		2.0 / 7.0, 2.0 / 15.0, 2.0 / 31.0, 2.0 / 63.0, 2.0 / 127.0, 2.0 / 255.0,
		2.0 / 511.0, 2.0 / 1023.0, 2.0 / 2047.0, 2.0 / 4095.0, 2.0 / 8191.0,
		2.0 / 16383.0, 2.0 / 32767.0, 2.0 / 65535.0, -4.0 / 5.0, -2.0 / 5.0,
		2.0 / 5.0, 4.0 / 5.0, -8.0 / 9.0, -4.0 / 9.0, -2.0 / 9.0, 2.0 / 9.0,
		4.0 / 9.0, 8.0 / 9.0};

	pWork = TempFloats;
	uint_t uIndex = 0;
	do {
		double dScalar = g_MP3MulTable64Raw[uIndex];
		uCounter = 64;
		double dStep = 3.0;
		do {
			if (uCounter == 1) {
				pWork[0] = 0.0f;
			} else {
				pWork[0] = static_cast<float>(dScalar * pow(2.0, dStep / 3.0));
			}
			++pWork;
			dStep = dStep - 1.0;
		} while (--uCounter);
	} while (++uIndex < 27);

	Output2DArrayAsHex("DecompressMP3::g_MP3MulTable64", TempFloats, 27, 64);

	//
	//  Generate a table of powers of 4/3
	//

	uCounter = 0;
	double dStepPow = 0.0;
	pWork = TempFloats;
	do {
		// Print as hex
		pWork[0] = static_cast<float>(pow(dStepPow, 4.0 / 3.0));
		++pWork;
		dStepPow = dStepPow + 1.0;
	} while (++uCounter < 8207);
	OutputArrayAsHex("DecompressMP3::g_MP3Power3Div4", TempFloats, 8207);

	//
	// Cosine tables
	//

	uCounter = 9;
	pWork = TempFloats;
	dStepPow = 0.0;
	do {
		pWork[0] = static_cast<float>(cos((BURGER_PI / 18.0) * dStepPow));
		++pWork;
		dStepPow = dStepPow + 1.0;
	} while (--uCounter);
	OutputArrayAsHex("DecompressMP3::g_MP3CosPIDiv18s", TempFloats, 9);

	uCounter = 9;
	pWork = TempFloats;
	dStepPow = 1.0;
	do {
		pWork[0] =
			static_cast<float>(0.5 / cos((Burger::g_dPi * dStepPow) / 36.0));
		++pWork;
		dStepPow = dStepPow + 2.0;
	} while (--uCounter);
	OutputArrayAsHex("DecompressMP3::g_MP3CosDiv36s", TempFloats, 9);

	uCounter = 3;
	pWork = TempFloats;
	dStepPow = 1.0;
	do {
		pWork[0] =
			static_cast<float>(0.5 / cos((Burger::g_dPi * dStepPow) / 12.0));
		++pWork;
		dStepPow = dStepPow + 2.0;
	} while (--uCounter);
	OutputArrayAsHex("DecompressMP3::g_MP3CosDiv12s", TempFloats, 3);

	//
	// Gain table
	//

	uCounter = 378;
	pWork = TempFloats;
	dStepPow = 11.5;
	do {
		pWork[0] = static_cast<float>(pow(2.0, dStepPow));
		++pWork;
		dStepPow = dStepPow - 0.25; // 0.25 is a precise number in double, so
									// stepping won't cause problems
	} while (--uCounter);
	OutputArrayAsHex("DecompressMP3::g_MP3GainPow2", TempFloats, 378);

	//
	// Create the anti-aliasing constants
	//

	static const double g_AntiAliasingBasline[8] = {
		-0.6, -0.535, -0.33, -0.185, -0.095, -0.041, -0.0142, -0.0037};
	uCounter = 0;
	do {
		double dAntiAliasConstant = g_AntiAliasingBasline[uCounter];
		dStepPow = Burger::Sqrt(1.0 + dAntiAliasConstant * dAntiAliasConstant);
		TempFloats[uCounter] = static_cast<float>(1.0 / dStepPow);
		TempFloats[uCounter + 8] =
			static_cast<float>(dAntiAliasConstant / dStepPow);
	} while (++uCounter < 8);
	OutputArrayAsHex("DecompressMP3::g_MP3AntiAliasCSs", TempFloats, 8);
	OutputArrayAsHex("DecompressMP3::g_MP3AntiAliasCAs", TempFloats + 8, 8);

	//
	// 12 or 36 entry DCT block tables
	//

	pWork = TempFloats;
	uCounter = 36;
	dStepPow = 1.0;
	do {
		pWork[0] =
			static_cast<float>((0.5 * sin((BURGER_PI / 72.0) * dStepPow)) /
				cos((BURGER_PI * (dStepPow + 18.0)) / 72.0));
		++pWork;
		dStepPow = dStepPow + 2.0;
	} while (--uCounter);
	OutputArrayAsHex("DecompressMP3::g_MP3BlockType0", TempFloats, 36);
	PhaseTable(TempFloats, 36);
	OutputArrayAsHex("DecompressMP3::g_MP3PhasedBlockType0", TempFloats, 36);

	// Table 2

	pWork = TempFloats;
	uCounter = 18;
	dStepPow = 1.0;
	do {
		pWork[0] =
			static_cast<float>((0.5 * sin((BURGER_PI / 72.0) * dStepPow)) /
				cos((BURGER_PI * (dStepPow + 18.0)) / 72.0));
		++pWork;
		dStepPow = dStepPow + 2.0;
	} while (--uCounter);

	uCounter = 6;
	dStepPow = 36.0 + 19.0;
	do {
		pWork[0] = static_cast<float>(0.5 / cos((BURGER_PI * dStepPow) / 72.0));
		++pWork;
		dStepPow = dStepPow + 2.0;
	} while (--uCounter);

	uCounter = 6;
	dStepPow = 13.0;
	do {
		pWork[0] =
			static_cast<float>((0.5 * sin((BURGER_PI / 24.0) * dStepPow)) /
				cos((BURGER_PI * (dStepPow + 54.0)) / 72.0));
		++pWork;
		dStepPow = dStepPow + 2.0;
	} while (--uCounter);

	uCounter = 6;
	do {
		pWork[0] = 0.0f;
		++pWork;
	} while (--uCounter);
	OutputArrayAsHex("DecompressMP3::g_MP3BlockType1", TempFloats, 36);
	PhaseTable(TempFloats, 36);
	OutputArrayAsHex("DecompressMP3::g_MP3PhasedBlockType1", TempFloats, 36);

	// Table 3
	pWork = TempFloats;
	uCounter = 12;
	dStepPow = 1.0;
	do {
		pWork[0] =
			static_cast<float>((0.5 * sin((BURGER_PI / 24.0) * dStepPow)) /
				cos((BURGER_PI * (dStepPow + 6.0)) / 24.0));
		++pWork;
		dStepPow = dStepPow + 2.0;
	} while (--uCounter);
	OutputArrayAsHex("DecompressMP3::g_MP3BlockType2", TempFloats, 12);
	PhaseTable(TempFloats, 12);
	OutputArrayAsHex("DecompressMP3::g_MP3PhasedBlockType2", TempFloats, 12);

	// Table 4

	pWork = TempFloats;
	uCounter = 6;
	do {
		pWork[0] = 0.0f;
		++pWork;
	} while (--uCounter);

	uCounter = 6;
	dStepPow = 1.0;
	do {
		pWork[0] =
			static_cast<float>((0.5 * sin((BURGER_PI / 24.0) * dStepPow)) /
				cos((BURGER_PI * (dStepPow + 30.0)) / 72.0));
		++pWork;
		dStepPow = dStepPow + 2.0;
	} while (--uCounter);

	uCounter = 6;
	dStepPow = 43.0;
	do {
		pWork[0] = static_cast<float>(0.5 / cos((BURGER_PI * dStepPow) / 72.0));
		++pWork;
		dStepPow = dStepPow + 2.0;
	} while (--uCounter);

	uCounter = 18;
	dStepPow = 37.0;
	do {
		pWork[0] =
			static_cast<float>((0.5 * sin((BURGER_PI / 72.0) * dStepPow)) /
				cos((BURGER_PI * (dStepPow + 18)) / 72.0));
		++pWork;
		dStepPow = dStepPow + 2.0;
	} while (--uCounter);

	OutputArrayAsHex("DecompressMP3::g_MP3BlockType3", TempFloats, 36);
	PhaseTable(TempFloats, 36);
	OutputArrayAsHex("DecompressMP3::g_MP3PhasedBlockType3", TempFloats, 36);

	//
	// Tangent tables
	//

	pWork = TempFloats;
	uCounter = 16;
	dStepPow = 0.0;
	do {
		double dTangent = tan((dStepPow * BURGER_PI) / 12.0);
		double dTangentPlusOne = dTangent + 1.0;
		pWork[0] = static_cast<float>(dTangent / dTangentPlusOne);
		pWork[16] = static_cast<float>(1.0 / dTangentPlusOne);
		pWork[32] =
			static_cast<float>((BURGER_SQRT2 * dTangent) / dTangentPlusOne);
		pWork[48] = static_cast<float>(BURGER_SQRT2 / dTangentPlusOne);
		++pWork;
		dStepPow = dStepPow + 1.0;
	} while (--uCounter);
	OutputArrayAsHex("DecompressMP3::g_MP3TanDivTan", TempFloats, 16);
	OutputArrayAsHex("DecompressMP3::g_MP3OneDivTan", TempFloats + 16, 16);
	OutputArrayAsHex("DecompressMP3::g_MP3Sqrt2TanDivTan", TempFloats + 32, 16);
	OutputArrayAsHex("DecompressMP3::g_MP3Sqrt2DivTan", TempFloats + 48, 16);

	//
	// Pow tables
	//

	pWork = TempFloats;
	uCounter = 2;
	dStepPow = -0.25;
	do {
		double dPower = pow(2.0, dStepPow);
		uIndex = 0;
		double dStep = 0.0;
		do {
			double dFactor1;
			double dFactor2;
			if (!uIndex) {
				dFactor1 = 1.0;
				dFactor2 = 1.0;
			} else if (uIndex & 1) {
				dFactor1 = pow(dPower, (dStep + 1.0) * 0.5);
				dFactor2 = 1.0;
			} else {
				dFactor2 = pow(dPower, dStep * 0.5);
				dFactor1 = 1.0;
			}
			pWork[0] = static_cast<float>(dFactor1);
			pWork[32] = static_cast<float>(dFactor2);
			pWork[64] = static_cast<float>(dFactor1 * BURGER_SQRT2);
			pWork[96] = static_cast<float>(dFactor2 * BURGER_SQRT2);
			++pWork;
			dStep = dStep + 1.0;
		} while (++uIndex < 16);
		dStepPow =
			dStepPow - 0.25; // 0.25 is 100% accurate, so stepping is okay
	} while (--uCounter);

	Output2DArrayAsHex("DecompressMP3::g_MP3PowMono1", TempFloats, 2, 16);
	Output2DArrayAsHex("DecompressMP3::g_MP3PowMono2", TempFloats + 32, 2, 16);
	Output2DArrayAsHex(
		"DecompressMP3::g_MP3PowStereo1", TempFloats + 64, 2, 16);
	Output2DArrayAsHex(
		"DecompressMP3::g_MP3PowStereo2", TempFloats + 96, 2, 16);

	//
	// Create the quick look up tables for the banding information
	//

	uint32_t TempWords[512];
	uCounter = 0;
	uint32_t* pWord;
	do {
		const Burger::DecompressMP3::BandInfo_t* pBandInformation =
			&Burger::DecompressMP3::g_MP3BandInformation[uCounter];
		pWord = TempWords;

		// Hash both long and short differences
		const uint16_t* pBitCount = pBandInformation->m_uLongDifference;
		uIndex = 0;
		uint_t uBitCounter = 0;
		do {
			uint_t uTemp = pBitCount[0];
			pWord[0] = uTemp >> 1U;
			pWord[1] = uBitCounter;
			pWord[2] = 3;
			pWord[3] = uIndex;
			pWord += 4;
			uBitCounter += uTemp;
			++pBitCount;
		} while (++uIndex < 8);

		pBitCount = pBandInformation->m_uShortDifference + 3;
		uIndex = 3;
		do {
			uint_t uTemp = static_cast<uint_t>(pBitCount[0]) >> 1U;
			++pBitCount;
			uint_t uTemp2 = 0;
			do {
				pWord[0] = uTemp;
				pWord[1] = uBitCounter +
					uTemp2; // Add in the long offset (Since that's the base)
				pWord[2] = uTemp2;
				pWord[3] = uIndex;
				pWord += 4;
			} while (++uTemp2 < 3);
			uBitCounter += uTemp * 6;
		} while (++uIndex < 13);
		TempString.Printf("DecompressMP3::g_MP3BandInfoDiffHash%u", uCounter);
		OutputArrayAsUnsigned(TempString.c_str(), TempWords,
			static_cast<uintptr_t>(pWord - TempWords));

		// Hash only the short distances
		pWord = TempWords;
		pBitCount = pBandInformation->m_uShortDifference;
		uIndex = 0;
		uBitCounter = 0;
		do {
			uint_t uTemp = static_cast<uint_t>(pBitCount[0]) >> 1U;
			++pBitCount;
			uint_t uTemp2 = 0;
			do {
				pWord[0] = uTemp;
				pWord[1] = uBitCounter + uTemp2;
				pWord[2] = uTemp2;
				pWord[3] = uIndex;
				pWord += 4;
			} while (++uTemp2 < 3);
			uBitCounter += 6 * uTemp;
		} while (++uIndex < 13);

		TempString.Printf(
			"DecompressMP3::g_MP3BandInfoShortDiffHash%u", uCounter);
		OutputArrayAsUnsigned(TempString.c_str(), TempWords,
			static_cast<uintptr_t>(pWord - TempWords));

		// Hash only the long distances

		pWord = TempWords;
		pBitCount = pBandInformation->m_uLongDifference;
		uIndex = 0;
		do {
			pWord[0] = static_cast<uint_t>(pBitCount[0]) >> 1U;
			pWord[1] = uIndex;
			++pBitCount;
			pWord += 2;
		} while (++uIndex < 22U);
		TempString.Printf(
			"DecompressMP3::g_MP3BandInfoLongDiffHash%u", uCounter);
		OutputArrayAsUnsigned(TempString.c_str(), TempWords,
			static_cast<uintptr_t>(pWord - TempWords));

	} while (++uCounter < 9);

	//
	// Limit tables
	//

	pWord = TempWords;
	uCounter = 0;
	do {
		uIndex = 0;
		do {
			uint_t uTemp =
				((Burger::DecompressMP3::g_MP3BandInformation[uCounter]
						 .m_uLongIndex[uIndex] +
					 7U) /
					18U) +
				1U;
			if (uTemp > Burger::DecompressMP3::cSubBandLimit) {
				uTemp = Burger::DecompressMP3::cSubBandLimit;
			}
			pWord[0] = uTemp;
			++pWord;
		} while (++uIndex < 23);
	} while (++uCounter < 9);
	Output2DArrayAsUnsigned(
		"DecompressMP3::g_MP3BandLongLimits", TempWords, 9, 23);

	pWord = TempWords;
	uCounter = 0;
	do {
		uIndex = 0;
		do {
			uint_t uTemp = static_cast<uint_t>(
				((static_cast<int>(
					  Burger::DecompressMP3::g_MP3BandInformation[uCounter]
						  .m_uShortIndex[uIndex]) -
					 1) /
					18) +
				1);
			if (uTemp > Burger::DecompressMP3::cSubBandLimit) {
				uTemp = Burger::DecompressMP3::cSubBandLimit;
			}
			pWord[0] = uTemp;
			++pWord;
		} while (++uIndex < 14);
	} while (++uCounter < 9);
	Output2DArrayAsUnsigned(
		"DecompressMP3::g_MP3BandShortLimits", TempWords, 9, 14);

	//
	// Intensity and normal tables
	//

	pWord = TempWords;
	uCounter = 0;
	do {
		uIndex = 0;
		do {
			uint_t uTemp = 0;
			do {
				pWord[0] =
					uCounter + (uIndex << 3U) + (uTemp << 6U) + (3U << 12U);
				++pWord;
			} while (++uTemp < 6);
		} while (++uIndex < 6);
	} while (++uCounter < 5);

	uCounter = 0;
	do {
		uIndex = 0;
		do {
			uint_t uTemp = 0;
			do {
				pWord[0] =
					uCounter + (uIndex << 3U) + (uTemp << 6U) + (4U << 12U);
				++pWord;
			} while (++uTemp < 4);
		} while (++uIndex < 4);
	} while (++uCounter < 4);

	uCounter = 0;
	do {
		uIndex = 0;
		do {
			pWord[0] = uCounter + (uIndex << 3U) + (5 << 12);
			++pWord;
		} while (++uIndex < 3);
	} while (++uCounter < 4);
	OutputArrayAsUnsigned(
		"DecompressMP3::g_MP3IntensitySLen", TempWords, 256, TRUE);

	// Normal table

	pWord = TempWords;
	uCounter = 0;
	do {
		uIndex = 0;
		do {
			uint_t uTemp = 0;
			do {
				uint_t uTemp2 = 0;
				do {
					pWord[0] = uCounter + (uIndex << 3) + (uTemp << 6) +
						(uTemp2 << 9) + (0 << 12);
					++pWord;
				} while (++uTemp2 < 4);
			} while (++uTemp < 4);
		} while (++uIndex < 5);
	} while (++uCounter < 5);

	uCounter = 0;
	do {
		uIndex = 0;
		do {
			uint_t uTemp = 0;
			do {
				pWord[0] = uCounter + (uIndex << 3) + (uTemp << 6) + (1 << 12);
				++pWord;
			} while (++uTemp < 4);
		} while (++uIndex < 5);
	} while (++uCounter < 5);

	uCounter = 0;
	do {
		uIndex = 0;
		do {
			pWord[0] = uCounter + (uIndex << 3U) + ((2 << 12) + (1 << 15));
			++pWord;
		} while (++uIndex < 3);
	} while (++uCounter < 4);

	OutputArrayAsUnsigned(
		"DecompressMP3::g_MP3NormalSLen", TempWords, 512, TRUE);

	//
	// FFT Blackman window
	//

	uCounter = 1024;
	pWork = TempFloats;
	dStepPow = .5;
	do {
		pWork[0] = static_cast<float>(0.42 -
			(cos((BURGER_PI / 512.0) * dStepPow) * 0.5) +
			(cos((BURGER_PI / 256.0) * dStepPow) * 0.08));
		++pWork;
		dStepPow += 1.0;
	} while (--uCounter);
	OutputArrayAsHex("DecompressMP3::g_MP3FFTWindow", TempFloats, 1024);

	//
	// Short FFT windows
	//

	uCounter = 128;
	pWork = TempFloats;
	dStepPow = .5;
	do {
		pWork[0] = static_cast<float>(
			(1.0 - cos((BURGER_PI / 128.0) * dStepPow)) * 0.5);
		++pWork;
		dStepPow += 1.0;
	} while (--uCounter);
	OutputArrayAsHex("DecompressMP3::g_MP3FFTShortWindow", TempFloats, 128);
}

//
// Output the data tables for constants
//

void BURGER_API WriteDataTables(void) BURGER_NOEXCEPT
{
#if 0
	CreateSinConstants();
	CreateCosConstants();
	CreateEulerRotations();
	CreateSqrtGuesses();
#endif
	CreateMP3Tables();
}

#endif

//
// Unit test for the FileXML class
//

#if 0

#include "brfilebmp.h"
#include "brfiledds.h"
#include "brfilegif.h"
#include "brfileini.h"
#include "brfilelbm.h"
#include "brfilemanager.h"
#include "brfilepcx.h"
#include "brfilepng.h"
#include "brfiletga.h"
#include "brfloatingpoint.h"
#include "brkeyboard.h"


//	FileManagerSimple Foo;

#if 0
InputMemoryStream Input;
if (!Input.Open("9:..:..:data:test32bit.bmp")) {
	FileBMP LoadFile;
	FileBMP SaveFile;
	Image *pImage = LoadFile.Load(&Input);
	if (pImage) {
#if 0
		printf("MipMapCount = %u\n",pImage->GetMipMapCount());
		printf("Height = %u\n",pImage->GetHeight());
		printf("Height = %u\n",pImage->GetHeight(1));
		printf("Height = %u\n",pImage->GetHeight(2));
		printf("Width = %u\n",pImage->GetWidth());
		printf("Width = %u\n",pImage->GetWidth(1));
		printf("Width = %u\n",pImage->GetWidth(2));
		printf("GetImageSize = %u\n",pImage->GetImageSize(0));
		printf("GetImageSize = %u\n",pImage->GetImageSize(1));
		printf("GetImageSize = %u\n",pImage->GetImageSize(2));
		printf("GetImageSize = %u\n",pImage->GetImageSize(3));
		printf("GetImageSize = %u\n",pImage->GetImageSize(4));
		printf("GetImageSize = %u\n",pImage->GetImageSize(5));
		printf("GetImageSize = %u\n",pImage->GetImageSize(6));
		printf("GetImageSize = %u\n",pImage->GetImageSize(7));
		printf("GetImageSize = %u\n",pImage->GetImageSize(8));
		printf("GetImage = %p\n",pImage->GetImage(0));
		printf("GetImage = %p\n",pImage->GetImage(1));
		printf("GetImage = %p\n",pImage->GetImage(2));
		printf("GetImage = %p\n",pImage->GetImage(3));
		printf("GetImage = %p\n",pImage->GetImage(4));
		printf("GetImage = %p\n",pImage->GetImage(5));
		printf("GetImage = %p\n",pImage->GetImage(6));
		printf("GetImage = %p\n",pImage->GetImage(7));
		printf("GetImage = %p\n",pImage->GetImage(8));
#endif

		//			SaveFile.SetPalette(LoadFile.GetPalette());
		OutputMemoryStream Output;
		if (!SaveFile.Save(&Output,pImage)) {
			Output.SaveFile("9:..:..:data:foo.bmp");
		}
	}
	Delete(pImage);
}
Input.Clear();
#endif

#endif

//
// Perform one shot functions
//

#include "brconsolemanager.h"
#include "brcriticalsection.h"
#include "brfloatingpoint.h"
#include "brglobalmemorymanager.h"
#include "brglobals.h"
#include "brguid.h"
#include "brperforce.h"
#include "brstdouthelpers.h"
#include "brtick.h"
#include <stdio.h>

using namespace Burger;

static uintptr_t BURGER_API Code(void* pInput) BURGER_NOEXCEPT
{
	++static_cast<uint_t*>(pInput)[0];
	return 12345;
}
void BURGER_API CreateTables(void) BURGER_NOEXCEPT
{
	ConsoleApp Ack(0, 0);
	FloatTimer MyFloatTimer;
	printf("Test Tick::ReadMilliseconds() %u\n",
		static_cast<uint_t>(Tick::ReadMilliseconds()));
	printf("Test Tick::ReadMilliseconds() %u\n",
		static_cast<uint_t>(Tick::ReadMilliseconds()));
	printf("Test Tick::ReadMilliseconds() %u\n",
		static_cast<uint_t>(Tick::ReadMilliseconds()));
	printf("Test Tick::ReadMilliseconds() %u\n",
		static_cast<uint_t>(Tick::ReadMilliseconds()));
	printf("Test Tick::ReadMicroseconds() %u\n",
		static_cast<uint_t>(Tick::ReadMicroseconds()));
	printf("Test Tick::ReadMicroseconds() %u\n",
		static_cast<uint_t>(Tick::ReadMicroseconds()));
	printf("Test Tick::ReadMicroseconds() %u\n",
		static_cast<uint_t>(Tick::ReadMicroseconds()));
	printf("Test Tick::ReadMicroseconds() %u\n",
		static_cast<uint_t>(Tick::ReadMicroseconds()));
	printf("Test FloatTimer.GetTime() %g\n", MyFloatTimer.GetTime());

	CriticalSection foo;
	uint_t uResult = 666;
	Thread bar(Code, &uResult);
	bar.Wait();
	printf("Result = %u, %u\n", static_cast<uint_t>(bar.GetResult()), uResult);
	Thread bar2;
	bar2.Start(Code, &uResult);
	bar.Wait();
	printf("Result = %u, %u\n", static_cast<uint_t>(bar.GetResult()), uResult);

	GUID gfoo = {0x3BBA0080, 0x2421, 0x11CF,
		{0xA3, 0x1A, 0x00, 0xAA, 0x00, 0xB9, 0x33, 0x56}};
	char buffer[256];
	GUIDToString(buffer, &gfoo);
	GUID hfoo;
	GUIDFromString(&hfoo, buffer);
	GUIDInit(&hfoo);
	GUIDToString(buffer, &hfoo);
}
