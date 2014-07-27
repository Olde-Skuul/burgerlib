/***************************************

	String handlers for UTF8 support
	
	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRUTF8_H__
#define __BRUTF8_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
class UTF8 {
public:
	enum {BAD=-1};
	static const Word8 BURGER_ALIGN(TokenSizeTable[256],16);
	static const Word8 ByteOrderMark[3];
	static Word BURGER_API IsValidSingle(const char *pInput);
	static Word BURGER_API IsValid(const char *pInput);
	static Word BURGER_API IsValid(const char *pInput,WordPtr uInputSize);
	static WordPtr BURGER_API GetTokenSize(const char *pInput);
	static const char * BURGER_API NextToken(const char *pInput);
	static WordPtr BURGER_API FromMacRomanUS(char *pOutput,Word uInput);
	static WordPtr BURGER_API FromMacRomanUS(char *pOutput,WordPtr uOutputSize,const char *pInput);
	static WordPtr BURGER_API FromMacRomanUS(char *pOutput,WordPtr uOutputSize,const char *pInput,WordPtr uInputSize);
	static WordPtr BURGER_API FromWin1252(char *pOutput,Word uInput);
	static WordPtr BURGER_API FromWin1252(char *pOutput,WordPtr uOutputSize,const char *pInput);
	static WordPtr BURGER_API FromWin1252(char *pOutput,WordPtr uOutputSize,const char *pInput,WordPtr uInputSize);
	static WordPtr BURGER_API FromISOLatin1(char *pOutput,Word uInput);
	static WordPtr BURGER_API FromISOLatin1(char *pOutput,WordPtr uOutputSize,const char *pInput);
	static WordPtr BURGER_API FromISOLatin1(char *pOutput,WordPtr uOutputSize,const char *pInput,WordPtr uInputSize);
	static WordPtr BURGER_API FromGeneric(char *pOutput,const Word8 pTranslateTable[128][4],Word uInput);
	static WordPtr BURGER_API FromGeneric(char *pOutput,WordPtr uOutputSize,const Word8 pTranslateTable[128][4],const char *pInput);
	static WordPtr BURGER_API FromGeneric(char *pOutput,WordPtr uOutputSize,const Word8 pTranslateTable[128][4],const char *pInput,WordPtr uInputSize);
	static WordPtr BURGER_API FromUTF16(char *pOutput,Word16 uInput);
	static WordPtr BURGER_API FromUTF16(char *pOutput,WordPtr uOutputSize,const Word16 *pInput);
	static WordPtr BURGER_API FromUTF16(char *pOutput,WordPtr uOutputSize,const Word16 *pInput,WordPtr uInputSize);
	static char * BURGER_API FromUTF16(const Word16 *pInput);
	static WordPtr BURGER_API FromUTF32(char *pOutput,Word32 uInput);
	static WordPtr BURGER_API FromUTF32(char *pOutput,WordPtr uOutputSize,const Word32 *pInput);
	static WordPtr BURGER_API FromUTF32(char *pOutput,WordPtr uOutputSize,const Word32 *pInput,WordPtr uInputSize);
	static Word BURGER_API ToGeneric(const char *pInput,const Word8 pTranslateTable[128][4]);
	static WordPtr BURGER_API ToGeneric(char *pOutput,WordPtr uOutputSize,const Word8 pTranslateTable[128][4],const char *pInput);
	static WordPtr BURGER_API ToGeneric(char *pOutput,WordPtr uOutputSize,const Word8 pTranslateTable[128][4],const char *pInput,WordPtr uInputSize);
};
}
/* END */

#endif

