/***************************************

	Functions to create the generated source code for Burgerlib

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "createtables.h"
#include "common.h"

#if defined(ALLOWCOMMANDLINE)
#include "brendian.h"
#include "brfilemanager.h"
#include "brconsolemanager.h"
#include "brfloatingpoint.h"
#include "brnumberstringhex.h"
#include "brmatrix3d.h"
#include <math.h>

//
// Create the Sin/Cosine constants so they can be converted
// to binary for the Sin() and Cos() function constants
//

static void BURGER_API CreateSinConstants(void)
{
#if defined(BURGER_X86)
	Burger::Set8087Precision(Burger::PRECISION64);
#endif
	double dValueTable[20];
	float fValueTable[20];
	double dInverseTable[20];
	float fInverseTable[20];

	// Process sine constants
	Word i = 0;
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
	Output.Append("Burger::Word32ToFloat g_fSineFactors[11] = {\n");
	i = 0;
	do {
		if ((i%5)==0) {
			if (i) {
				Output.Append('\n');
			}
			Output.Append('\t');
		}
		Output.Append("{{0x");
		Burger::NumberStringHex Hexit(reinterpret_cast<const Word32 *>(fValueTable)[i]^(((i^1U)&1U)<<31U));
		Output.Append(Hexit.GetPtr());
		Output.Append("}}");
		if (i!=10) {
			Output.Append(',');
		}
	} while (++i<11);
	Output.Append("\n};\n");

	Output.Append("// 1/3!, 1/5!, to 1/23!\n");
	Output.Append("#if !defined(BURGER_X86) && !defined(BURGER_MACOSX)\n");
	Output.Append("\tstatic const\n");
	Output.Append("#endif\n");
	Output.Append("Burger::Word32ToFloat g_fInverseSineFactors[11] = {\n");
	i = 0;
	do {
		if ((i%5)==0) {
			if (i) {
				Output.Append('\n');
			}
			Output.Append('\t');
		}
		Output.Append("{{0x");
		Burger::NumberStringHex Hexit(reinterpret_cast<const Word32 *>(fInverseTable)[i]^(((i^1U)&1U)<<31U));
		Output.Append(Hexit.GetPtr());
		Output.Append("}}");
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
	Output.Append("Burger::Word64ToDouble g_dSineFactors[11] = {\n");
	i = 0;
	do {
		if ((i%5)==0) {
			if (i) {
				Output.Append('\n');
			}
			Output.Append('\t');
		}
		Output.Append("{{0x");
		Burger::NumberStringHex Hexit(reinterpret_cast<const Word64 *>(dValueTable)[i]^(((static_cast<Word64>(i)^1U)&1U)<<63U));
		Output.Append(Hexit.GetPtr());
		Output.Append("ULL}}");
		if (i!=10) {
			Output.Append(',');
		}
	} while (++i<11);
	Output.Append("\n};\n");

	Output.Append("// 1/3!, 1/5!, to 1/23!\n");
	Output.Append("#if !defined(BURGER_X86) && !defined(BURGER_MACOSX)\n");
	Output.Append("\tstatic const\n");
	Output.Append("#endif\n");
	Output.Append("Burger::Word64ToDouble g_dInverseSineFactors[11] = {\n");
	i = 0;
	do {
		if ((i%5)==0) {
			if (i) {
				Output.Append('\n');
			}
			Output.Append('\t');
		}
		Output.Append("{{0x");
		Burger::NumberStringHex Hexit(reinterpret_cast<const Word64 *>(dInverseTable)[i]^(((static_cast<Word64>(i)^1U)&1U)<<63U));
		Output.Append(Hexit.GetPtr());
		Output.Append("ULL}}");
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

static void BURGER_API CreateCosConstants(void)
{
#if defined(BURGER_X86)
	Burger::Set8087Precision(Burger::PRECISION64);
#endif
	double dValueTable[20];
	float fValueTable[20];
	double dInverseTable[20];
	float fInverseTable[20];

	// Process cosine constants
	Word i = 0;
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
	Output.Append("Burger::Word32ToFloat g_fCosineFactors[11] = {\n");
	i = 0;
	do {
		if ((i%5)==0) {
			if (i) {
				Output.Append('\n');
			}
			Output.Append('\t');
		}
		Output.Append("{{0x");
		Burger::NumberStringHex Hexit(reinterpret_cast<const Word32 *>(fValueTable)[i]^(((i^1U)&1U)<<31U));
		Output.Append(Hexit.GetPtr());
		Output.Append("}}");
		if (i!=10) {
			Output.Append(',');
		}
	} while (++i<11);
	Output.Append("\n};\n");

	Output.Append("// 1/2!, 1/4!, to 1/22!\n");
	Output.Append("#if !defined(BURGER_X86) && !defined(BURGER_MACOSX)\n");
	Output.Append("\tstatic const\n");
	Output.Append("#endif\n");
	Output.Append("Burger::Word32ToFloat g_fInverseCosineFactors[11] = {\n");
	i = 0;
	do {
		if ((i%5)==0) {
			if (i) {
				Output.Append('\n');
			}
			Output.Append('\t');
		}
		Output.Append("{{0x");
		Burger::NumberStringHex Hexit(reinterpret_cast<const Word32 *>(fInverseTable)[i]^(((i^1U)&1U)<<31U));
		Output.Append(Hexit.GetPtr());
		Output.Append("}}");
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
	Output.Append("Burger::Word64ToDouble g_dCosineFactors[11] = {\n");
	i = 0;
	do {
		if ((i%5)==0) {
			if (i) {
				Output.Append('\n');
			}
			Output.Append('\t');
		}
		Output.Append("{{0x");
		Burger::NumberStringHex Hexit(reinterpret_cast<const Word64 *>(dValueTable)[i]^(((static_cast<Word64>(i)^1U)&1U)<<63U));
		Output.Append(Hexit.GetPtr());
		Output.Append("ULL}}");
		if (i!=10) {
			Output.Append(',');
		}
	} while (++i<11);
	Output.Append("\n};\n");

	Output.Append("// 1/2!, 1/4!, to 1/22!\n");
	Output.Append("#if !defined(BURGER_X86) && !defined(BURGER_MACOSX)\n");
	Output.Append("\tstatic const\n");
	Output.Append("#endif\n");
	Output.Append("Burger::Word64ToDouble g_dInverseCosineFactors[11] = {\n");
	i = 0;
	do {
		if ((i%5)==0) {
			if (i) {
				Output.Append('\n');
			}
			Output.Append('\t');
		}
		Output.Append("{{0x");
		Burger::NumberStringHex Hexit(reinterpret_cast<const Word64 *>(dInverseTable)[i]^(((static_cast<Word64>(i)^1U)&1U)<<63U));
		Output.Append(Hexit.GetPtr());
		Output.Append("ULL}}");
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

static Word BURGER_API SimpleFactor(Burger::OutputMemoryStream *pOutput,const Factor_t *pFactor,float fInput)
{
	Word bSuccess = 0;
	// Factor the primes!
	Word i = 0;
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

static Word BURGER_API Factor(Burger::OutputMemoryStream *pOutput,const char *pName,const Factor_t *pFactor,float fInput,float fPrevious)
{
	Word bSuccess=0;
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

		Word x = 0;
		do {
			if (pFactor[x].m_pValue[0]==fPrevious) {
				break;
			}
		} while (++x<9);

		if (x<9) {
			Word j=0;
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
			Word j=0;
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

static void BURGER_API CreateEulerRotations(void)
{
	Burger::Matrix3D_t Result;
	Burger::Matrix3D_t TempMatrix;

	Word i = 0;
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

static void BURGER_API CreateSqrtGuesses(void)
{
	Message("static const Burger::Word32ToFloat g_PPCSqrtGuess[2][256][2] = {{");

	Burger::OutputMemoryStream Output;

	// Generate the even roots
	Burger::Word32ToFloat Converter;
	Burger::NumberStringHex Hexit;
	Burger::String TempString;

	Word uCount = 256;
	double dStep = 0.5;
	Output.Append('\t');
	do {
		// Get the actual square root
		double dSqrt = sqrt(dStep);

		// Get the square root of the next step to get the rounding
		double dNextSqrt = sqrt(dStep+(1.0/512.0));

		// Generate the square root estimate by averaging between
		// the steps
		
		Converter.f = static_cast<float>((dSqrt+dNextSqrt)*0.5);

		// Print the value in hex
		Hexit = Converter.w;
		Output.Append("{{0x");
		Output.Append(Hexit.GetPtr());
		Output.Append("},{0x");

		// Get the reciprocals
		double dReciprocal2Guess = 1.0/(2.0*dSqrt);
		double dNextReciprocal2Guess = 1.0/(2.0*dNextSqrt);

		Converter.f = static_cast<float>((dReciprocal2Guess+dNextReciprocal2Guess)*0.5);
		Hexit = Converter.w;
		Output.Append(Hexit.GetPtr());
		Output.Append("}}");

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

		Converter.f = static_cast<float>((dSqrt+dNextSqrt)*0.5);

		// Print the value in hex
		Hexit = Converter.w;
		Output.Append("{{0x");
		Output.Append(Hexit.GetPtr());
		Output.Append("},{0x");

		// Get the reciprocals
		double dReciprocal2Guess = 1.0/(2.0*dSqrt);
		double dNextReciprocal2Guess = 1.0/(2.0*dNextSqrt);

		Converter.f = static_cast<float>((dReciprocal2Guess+dNextReciprocal2Guess)*0.5);
		Hexit = Converter.w;
		Output.Append(Hexit.GetPtr());
		Output.Append("}}");

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

//
// Output the data tables for constants
//

void BURGER_API WriteDataTables(void)
{
	CreateSinConstants();
	CreateCosConstants();
	CreateEulerRotations();
	CreateSqrtGuesses();
}

#endif

//
// Unit test for the FileXML class
//

#if 0

#include "brfilebmp.h"
#include "brfiletga.h"
#include "brfilepcx.h"
#include "brfilegif.h"
#include "brfilelbm.h"
#include "brfiledds.h"
#include "brfilepng.h"
#include "brfilemanager.h"
#include "brfileini.h"
#include "brkeyboard.h"
#include "brfloatingpoint.h"

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

#include "brglobals.h"
#include "brglobalmemorymanager.h"
#include "brconsolemanager.h"
#include "brperforce.h"
#include "brcriticalsection.h"
#include "brguid.h"
#include "brtick.h"
#include "brstdouthelpers.h"
#include "brfloatingpoint.h"
#include <stdio.h>

using namespace Burger;

static WordPtr BURGER_API Code(void *pInput)
{
	++static_cast<Word *>(pInput)[0];
	return 12345;
}
void BURGER_API CreateTables(void)
{
	ConsoleApp Ack(0,0);
	FloatTimer MyFloatTimer;
	printf("Test Tick::ReadMilliseconds() %u\n",static_cast<Word>(Tick::ReadMilliseconds()));
	printf("Test Tick::ReadMilliseconds() %u\n",static_cast<Word>(Tick::ReadMilliseconds()));
	printf("Test Tick::ReadMilliseconds() %u\n",static_cast<Word>(Tick::ReadMilliseconds()));
	printf("Test Tick::ReadMilliseconds() %u\n",static_cast<Word>(Tick::ReadMilliseconds()));
	printf("Test Tick::ReadMicroseconds() %u\n",static_cast<Word>(Tick::ReadMicroseconds()));
	printf("Test Tick::ReadMicroseconds() %u\n",static_cast<Word>(Tick::ReadMicroseconds()));
	printf("Test Tick::ReadMicroseconds() %u\n",static_cast<Word>(Tick::ReadMicroseconds()));
	printf("Test Tick::ReadMicroseconds() %u\n",static_cast<Word>(Tick::ReadMicroseconds()));
	printf("Test FloatTimer.GetTime() %g\n",MyFloatTimer.GetTime());

	CriticalSection foo;
	Word uResult=666;
	Thread bar(Code,&uResult);
	bar.Wait();
	printf("Result = %u, %u\n",static_cast<Word>(bar.GetResult()),uResult);
	Thread bar2;
	bar2.Start(Code,&uResult);
	bar.Wait();
	printf("Result = %u, %u\n",static_cast<Word>(bar.GetResult()),uResult);

	GUID gfoo = {0x3BBA0080,0x2421,0x11CF,{0xA3,0x1A,0x00,0xAA,0x00,0xB9,0x33,0x56 } };
	char buffer[256];
	GUIDToString(buffer,&gfoo);
	GUID hfoo;
	GUIDFromString(&hfoo,buffer);
	GUIDInit(&hfoo);
	GUIDToString(buffer,&hfoo);
}
