/***************************************

    Flash player ActionScript disassembler

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brflashdisasmactionscript.h"
#include "brassert.h"
#include "brendian.h"
#include "brdebug.h"
#include "brstringfunctions.h"

#if !defined(DOXYGEN)

//
// Argument types for Action script 2.0
//

enum eArgumentFormat {
	ARGUMENT_NONE,			// No arguments
	ARGUMENT_STRING,		// String argument
	ARGUMENT_STRING2,		// Two String argument
	ARGUMENT_DUMP,			// Memory dump
	ARGUMENT_UI8,			// uint8_t
	ARGUMENT_UI16,			// uint16_t
	ARGUMENT_SI16,			// int16_t
	ARGUMENT_WAITFORFRAME,	// ActionWaitForFrame
	ARGUMENT_ACTIONPUSH,	// ActionPush
	ARGUMENT_CONSTANT_POOL,	// ActionConstantPool
	ARGUMENT_DEFINEFUNCTION2	// ActionDefineFunction2
};

struct Instruction_t {
	const char *m_pName;				// Name of the function
	eArgumentFormat	m_eArgumentFormat;	// Data type
};

static const Instruction_t g_Instructions[0xA0] = {
	{"EndOfScript",ARGUMENT_NONE},				// 0x00
	{NULL,ARGUMENT_NONE},						// 0x01
	{NULL,ARGUMENT_NONE},						// 0x02
	{NULL,ARGUMENT_NONE},						// 0x03
	{"ActionNextFrame",ARGUMENT_NONE},			// 0x04
	{"ActionPrevFrame",ARGUMENT_NONE},			// 0x05
	{"ActionPlay",ARGUMENT_NONE},				// 0x06
	{"ActionStop",ARGUMENT_NONE},				// 0x07
	{"ActionToggleQualty",ARGUMENT_NONE},		// 0x08
	{"ActionStopSounds",ARGUMENT_NONE},			// 0x09
	{"ActionAdd",ARGUMENT_NONE},				// 0x0A
	{"ActionSubtract",ARGUMENT_NONE},			// 0x0B
	{"ActionMultiply",ARGUMENT_NONE},			// 0x0C
	{"ActionDivide",ARGUMENT_NONE},				// 0x0D
	{"ActionEquals",ARGUMENT_NONE},				// 0x0E
	{"ActionLess",ARGUMENT_NONE},				// 0x0F
	{"ActionAnd",ARGUMENT_NONE},				// 0x10
	{"ActionOr",ARGUMENT_NONE},					// 0x11
	{"ActionNot",ARGUMENT_NONE},				// 0x12
	{"ActionStringEquals",ARGUMENT_NONE},		// 0x13
	{"ActionStringLength",ARGUMENT_NONE},		// 0x14
	{"ActionStringExtract",ARGUMENT_NONE},		// 0x15
	{NULL,ARGUMENT_NONE},						// 0x16
	{"ActionPop",ARGUMENT_NONE},				// 0x17
	{"ActionToInteger",ARGUMENT_NONE},			// 0x18
	{NULL,ARGUMENT_NONE},						// 0x19
	{NULL,ARGUMENT_NONE},						// 0x1A
	{NULL,ARGUMENT_NONE},						// 0x1B
	{"ActionGetVariable",ARGUMENT_NONE},		// 0x1C
	{"ActionSetVariable",ARGUMENT_NONE},		// 0x1D
	{NULL,ARGUMENT_NONE},						// 0x1E
	{NULL,ARGUMENT_NONE},						// 0x1F
	{"ActionSetTarget2",ARGUMENT_NONE},			// 0x20
	{"ActionStringAdd",ARGUMENT_NONE},			// 0x21
	{"ActionGetProperty",ARGUMENT_NONE},		// 0x22
	{"ActionSetProperty",ARGUMENT_NONE},		// 0x23
	{"ActionCloneSprite",ARGUMENT_NONE},		// 0x24
	{"ActionRemoveSprite",ARGUMENT_NONE},		// 0x25
	{"ActionTrace",ARGUMENT_NONE},				// 0x26
	{"ActionStartDrag",ARGUMENT_NONE},			// 0x27
	{"ActionEndDrag",ARGUMENT_NONE},			// 0x28
	{"ActionStringLess",ARGUMENT_NONE},			// 0x29
	{NULL,ARGUMENT_NONE},						// 0x2A
	{"ActionCastOp",ARGUMENT_NONE},				// 0x2B
	{"ActionImplementsOp",ARGUMENT_NONE},		// 0x2C
	{NULL,ARGUMENT_NONE},						// 0x2D
	{NULL,ARGUMENT_NONE},						// 0x2E
	{NULL,ARGUMENT_NONE},						// 0x2F
	{"ActionRandomNumber",ARGUMENT_NONE},		// 0x30
	{"ActionMBStringLength",ARGUMENT_NONE},		// 0x31
	{"ActionCharToAscii",ARGUMENT_NONE},		// 0x32
	{"ActionAsciiToChar",ARGUMENT_NONE},		// 0x33
	{"ActionGetTime",ARGUMENT_NONE},			// 0x34
	{"ActionMBStringExtract",ARGUMENT_NONE},	// 0x35
	{"ActionMBCharToAscii",ARGUMENT_NONE},		// 0x36
	{"ActionMBAsciiToChar",ARGUMENT_NONE},		// 0x37
	{NULL,ARGUMENT_NONE},						// 0x38
	{NULL,ARGUMENT_NONE},						// 0x39
	{"ActionDelete",ARGUMENT_NONE},				// 0x3A
	{"ActionDelete2",ARGUMENT_NONE},			// 0x3B
	{"ActionDefineLocal",ARGUMENT_NONE},		// 0x3C
	{"ActionCallFunction",ARGUMENT_NONE},		// 0x3D
	{"ActionReturn",ARGUMENT_NONE},				// 0x3E
	{"ActionModulo",ARGUMENT_NONE},				// 0x3F
	{"ActionNewObject",ARGUMENT_NONE},			// 0x40
	{"ActionDefineLocal2",ARGUMENT_NONE},		// 0x41
	{"ActionInitArray",ARGUMENT_NONE},			// 0x42
	{"ActionInitObject",ARGUMENT_NONE},			// 0x43
	{"ActionTypeOf",ARGUMENT_NONE},				// 0x44
	{"ActionTargetPath",ARGUMENT_NONE},			// 0x45
	{"ActionEnumerate",ARGUMENT_NONE},			// 0x46
	{"ActionAdd2",ARGUMENT_NONE},				// 0x47
	{"ActionLess2",ARGUMENT_NONE},				// 0x48
	{"ActionEquals2",ARGUMENT_NONE},			// 0x49
	{"ActionToNumber",ARGUMENT_NONE},			// 0x4A
	{"ActionToString",ARGUMENT_NONE},			// 0x4B
	{"ActionPushDuplicate",ARGUMENT_NONE},		// 0x4C
	{"ActionStackSwap",ARGUMENT_NONE},			// 0x4D
	{"ActionGetMember",ARGUMENT_NONE},			// 0x4E
	{"ActionSetMember",ARGUMENT_NONE},			// 0x4F
	{"ActionIncrement",ARGUMENT_NONE},			// 0x50
	{"ActionDecrement",ARGUMENT_NONE},			// 0x51
	{"ActionCallMethod",ARGUMENT_NONE},			// 0x52
	{"ActionNewMethod",ARGUMENT_NONE},			// 0x53
	{"ActionInstanceOf",ARGUMENT_NONE},			// 0x54
	{"ActionEnumerate2",ARGUMENT_NONE},			// 0x55
	{NULL,ARGUMENT_NONE},						// 0x56
	{NULL,ARGUMENT_NONE},						// 0x57
	{NULL,ARGUMENT_NONE},						// 0x58
	{NULL,ARGUMENT_NONE},						// 0x59
	{NULL,ARGUMENT_NONE},						// 0x5A
	{NULL,ARGUMENT_NONE},						// 0x5B
	{NULL,ARGUMENT_NONE},						// 0x5C
	{NULL,ARGUMENT_NONE},						// 0x5D
	{NULL,ARGUMENT_NONE},						// 0x5E
	{NULL,ARGUMENT_NONE},						// 0x5F
	{"ActionBitAnd",ARGUMENT_NONE},				// 0x60
	{"ActionBitOr",ARGUMENT_NONE},				// 0x61
	{"ActionBitXor",ARGUMENT_NONE},				// 0x62
	{"ActionBitLShift",ARGUMENT_NONE},			// 0x63
	{"ActionBitRShift",ARGUMENT_NONE},			// 0x64
	{"ActionBitURShift",ARGUMENT_NONE},			// 0x65
	{"ActionStrictEquals",ARGUMENT_NONE},		// 0x66
	{"ActionGreater",ARGUMENT_NONE},			// 0x67
	{"ActionStringGreater",ARGUMENT_NONE},		// 0x68
	{"ActionExtends",ARGUMENT_NONE},			// 0x69
	{NULL,ARGUMENT_NONE},						// 0x6A
	{NULL,ARGUMENT_NONE},						// 0x6B
	{NULL,ARGUMENT_NONE},						// 0x6C
	{NULL,ARGUMENT_NONE},						// 0x6E
	{NULL,ARGUMENT_NONE},						// 0x6E
	{NULL,ARGUMENT_NONE},						// 0x6F
	{NULL,ARGUMENT_NONE},						// 0x70
	{NULL,ARGUMENT_NONE},						// 0x71
	{NULL,ARGUMENT_NONE},						// 0x72
	{NULL,ARGUMENT_NONE},						// 0x73
	{NULL,ARGUMENT_NONE},						// 0x74
	{NULL,ARGUMENT_NONE},						// 0x75
	{NULL,ARGUMENT_NONE},						// 0x76
	{NULL,ARGUMENT_NONE},						// 0x77
	{NULL,ARGUMENT_NONE},						// 0x78
	{NULL,ARGUMENT_NONE},						// 0x79
	{NULL,ARGUMENT_NONE},						// 0x7A
	{NULL,ARGUMENT_NONE},						// 0x7B
	{NULL,ARGUMENT_NONE},						// 0x7C
	{NULL,ARGUMENT_NONE},						// 0x7D
	{NULL,ARGUMENT_NONE},						// 0x7E
	{NULL,ARGUMENT_NONE},						// 0x7F
	{NULL,ARGUMENT_NONE},						// 0x80
	{"ActionGotoFrame",ARGUMENT_UI16},				// 0x81
	{NULL,ARGUMENT_NONE},						// 0x82
	{"ActionGetURL",ARGUMENT_STRING2},			// 0x83
	{NULL,ARGUMENT_NONE},						// 0x84
	{NULL,ARGUMENT_NONE},						// 0x85
	{NULL,ARGUMENT_NONE},						// 0x86
	{"ActionStoreRegister",ARGUMENT_UI8},			// 0x87
	{"ActionConstantPool",ARGUMENT_CONSTANT_POOL},	// 0x88
	{NULL,ARGUMENT_NONE},						// 0x89
	{"ActionWaitForFrame",ARGUMENT_WAITFORFRAME},		// 0x8A
	{"ActionSetTarget",ARGUMENT_STRING},		// 0x8B
	{"ActionGoToLabel",ARGUMENT_STRING},		// 0x8C
	{"ActionWaitForFrame2",ARGUMENT_UI8},		// 0x8D
	{"ActionDefineFunction2",ARGUMENT_DEFINEFUNCTION2},// 0x8E
	{NULL,ARGUMENT_NONE},						// 0x8F
	{NULL,ARGUMENT_NONE},						// 0x90
	{NULL,ARGUMENT_NONE},						// 0x91
	{NULL,ARGUMENT_NONE},						// 0x92
	{NULL,ARGUMENT_NONE},						// 0x93
	{"ActionWith",ARGUMENT_UI16},					// 0x94
	{NULL,ARGUMENT_NONE},						// 0x95
	{"ActionPush",ARGUMENT_ACTIONPUSH},			// 0x96
	{NULL,ARGUMENT_NONE},						// 0x97
	{NULL,ARGUMENT_NONE},						// 0x98
	{"ActionJump",ARGUMENT_SI16},					// 0x99
	{"ActionGetURL2",ARGUMENT_DUMP},			// 0x9A
	{"ActionDefineFunction",ARGUMENT_DUMP},		// 0x9B
	{NULL,ARGUMENT_NONE},						// 0x9C
	{"ActionIf",ARGUMENT_SI16},						// 0x9D
	{"ActionCall",ARGUMENT_DUMP},				// 0x9E
	{"ActionGotoFrame2",ARGUMENT_DUMP}			// 0x9F
};
#endif

/*! ************************************

	\brief Disassemble a line of ActionScript

	This debugging function will parse the data at
	the pointer as a single instruction of ActionScript
	and output it using the Debug::String(const char *)
	function.

	Documentation is found at http://www.adobe.com/content/dam/Adobe/en/devnet/swf/pdf/swf-file-format-spec.pdf

	\param pInput Pointer to ActionScript byte code
	\return Number of bytes processed (To help skip the pointer to the next instruction)

***************************************/

uintptr_t BURGER_API Burger::Flash::DisassembleActionScript(const uint8_t* pInput)
{
	// Get the action script opcode
	uint_t uOpcode = pInput[0];
	const Instruction_t *pInstruction;
	// Off the table?
	if (uOpcode>=BURGER_ARRAYSIZE(g_Instructions)) {
		pInstruction = &g_Instructions[1];		// Force a bogus entry
	} else {
		// Look it up
		pInstruction = &g_Instructions[uOpcode];
	}
	uintptr_t uResult = 1;
	// Unknown??
	if (!pInstruction->m_pName) {
		// Print this and exit
		Debug::Message("Unsupported opcode 0x%02X\n",uOpcode);
	} else {
		// Print the instruction
		Debug::PrintString(pInstruction->m_pName);

		// Print the arguments
		eArgumentFormat uFormatCode = pInstruction->m_eArgumentFormat;
		switch (uFormatCode) {
		case ARGUMENT_NONE:
			Debug::PrintString("\n");
			break;

		// A single unsigned 8 bit value
		case ARGUMENT_UI8:
			Debug::Message(" Value uint8_t:%u\n",static_cast<uint_t>(pInput[3]));
			uResult = 4;
			break;

		// A single unsigned 16 bit value
		case ARGUMENT_UI16:
			Debug::Message(" Value uint16_t:%u\n",static_cast<uint_t>(LittleEndian::LoadAny(reinterpret_cast<const uint16_t *>(pInput+3))));
			uResult = 5;
			break;

		// A single signed 16 bit value
		case ARGUMENT_SI16:
			Debug::Message(" Value int16_t:%d\n",static_cast<int_t>(LittleEndian::LoadAny(reinterpret_cast<const int16_t *>(pInput+3))));
			uResult = 5;
			break;

		// An ascii string
		case ARGUMENT_STRING:
			{
				uResult = LittleEndian::LoadAny(reinterpret_cast<const uint16_t *>(pInput+1))+3U;
				uint8_t uTemp = pInput[uResult];
				const_cast<uint8_t *>(pInput)[uResult] = 0;
				Debug::PrintString(" \"");
				Debug::PrintString(reinterpret_cast<const char *>(pInput+3));
				Debug::PrintString("\"\n");
				const_cast<uint8_t *>(pInput)[uResult] = uTemp;
			}
			break;

			// Two ascii string
		case ARGUMENT_STRING2:
			{
				uResult = LittleEndian::LoadAny(reinterpret_cast<const uint16_t *>(pInput+1))+3U;
				uint8_t uTemp = pInput[uResult];
				const_cast<uint8_t *>(pInput)[uResult] = 0;
				Debug::PrintString(" \"");
				Debug::PrintString(reinterpret_cast<const char *>(pInput+3));
				Debug::PrintString("\", \"");
				Debug::PrintString(reinterpret_cast<const char *>(pInput+4)+StringLength(reinterpret_cast<const char *>(pInput+3)));
				Debug::PrintString("\"\n");
				const_cast<uint8_t *>(pInput)[uResult] = uTemp;
			}
			break;

			// ActionWaitForFrame 16 bit, 8 bit
		case ARGUMENT_WAITFORFRAME:
			Debug::Message(" Frame:%u, Skip:%u\n",static_cast<uint_t>(LittleEndian::LoadAny(reinterpret_cast<const uint16_t *>(pInput+3))),static_cast<uint_t>(pInput[5]));
			break;

		case ARGUMENT_DUMP:
			{
				uintptr_t uCount = LittleEndian::LoadAny(reinterpret_cast<const uint16_t *>(pInput+1));
				// Pass back the final size
				uResult = uCount+3;
				if (uCount) {
					pInput+=3;
					do {
						Debug::Message(" 0x%02X",pInput[0]);
						++pInput;
					} while (--uCount);
				}
				Debug::PrintString("\n");
			}
			break;

		case ARGUMENT_ACTIONPUSH:
			{
				uintptr_t uLength = LittleEndian::LoadAny(reinterpret_cast<const uint16_t *>(pInput+1));
				uResult = uLength+3;
				pInput+=3;
				const uint8_t *pEnd = pInput+uLength;
				while (pInput < pEnd) {
					uint_t uPushType = pInput[0];
					++pInput;
					switch (uPushType) {
					case 0:
						// string
						Debug::PrintString(" STRING:\"");
						Debug::PrintString(reinterpret_cast<const char *>(pInput));
						Debug::PrintString("\"");
						pInput+=StringLength(reinterpret_cast<const char *>(pInput))+1;
						break;
					case 1:
						// 32 bit float (Little endian)
						Debug::Message(" FLOAT:%f",LittleEndian::LoadAny(reinterpret_cast<const float *>(pInput)));
						pInput+=4;
						break;
					case 2:
						Debug::Message(" NULL");
						break;
					case 3:
					default:
						Debug::Message(" undefined");
						break;
					case 4:
						// Register value
						Debug::Message(" Register:0x%02X",pInput[0]);
						++pInput;
						break;
					case 5:
						Debug::Message(" UI8:%u",static_cast<uint_t>(pInput[0]));
						++pInput;
						break;
					case 6:
						Debug::Message(" DOUBLE:%f",LittleEndian::LoadAny(reinterpret_cast<const double *>(pInput)));
						pInput+=8;
						break;
					case 7:
						Debug::Message(" UI32:%u",LittleEndian::LoadAny(reinterpret_cast<const uint32_t *>(pInput)));
						pInput+=4;
						break;
					case 8:
						Debug::Message(" Constant8:0x%02X",pInput[0]);
						++pInput;
						break;
					case 9:
						Debug::Message(" Constant16:0x%04X",LittleEndian::LoadAny(reinterpret_cast<const uint16_t *>(pInput)));
						pInput+=2;
						break;
					}
				}
				Debug::PrintString("\n");
			}
			break;
		case ARGUMENT_CONSTANT_POOL:
			{
				uintptr_t uLength = LittleEndian::LoadAny(reinterpret_cast<const uint16_t *>(pInput+1));
				uResult = uLength+3;
				if (uLength>=2) {
					uintptr_t uCount = LittleEndian::LoadAny(reinterpret_cast<const uint16_t *>(pInput+3));
					if (uCount) {
						Debug::Message(" Constants:%u",static_cast<uint_t>(uCount));
						uint8_t *pTemp = &const_cast<uint8_t *>(pInput)[uResult];
						uint8_t uTemp = pTemp[0];
						pTemp[0] = 0;
						pInput+=5;
						do {
							Debug::PrintString(" \"");
							Debug::PrintString(reinterpret_cast<const char *>(pInput));
							Debug::PrintString("\"");
							pInput+=StringLength(reinterpret_cast<const char *>(pInput))+1;
						} while (--uCount);
						pTemp[0] = uTemp;
					}
				}
				Debug::PrintString("\n");
			}
			break;
		case ARGUMENT_DEFINEFUNCTION2:
			{
				uintptr_t uLength = LittleEndian::LoadAny(reinterpret_cast<const uint16_t *>(pInput+1));
				uResult = uLength+3;
				const char *pFunctionName = reinterpret_cast<const char *>(pInput+3);
				pInput += StringLength(pFunctionName)+4;
				uint_t uNumParams = LittleEndian::LoadAny(reinterpret_cast<const uint16_t *>(pInput));
				uint_t uRegisterCount = pInput[2];
				pInput+=3;
				Debug::Message(" Function name:\"%s\" Argc:%u, RegCount:%u",pFunctionName,static_cast<uint_t>(uNumParams),static_cast<uint_t>(uRegisterCount));

				uint_t uFlag = pInput[0];
				if (uFlag&0x80) {
					Debug::PrintString(" PreloadParentFlag");
				}
				if (uFlag&0x40) {
					Debug::PrintString(" PreloadRootFlag");
				}
				if (uFlag&0x20) {
					Debug::PrintString(" SuppressSuperFlag");
				}
				if (uFlag&0x10) {
					Debug::PrintString(" PreloadSuperFlag");
				}
				if (uFlag&0x8) {
					Debug::PrintString(" SuppressArgumentsFlag");
				}
				if (uFlag&0x4) {
					Debug::PrintString(" PreloadArgumentsFlag");
				}
				if (uFlag&0x2) {
					Debug::PrintString(" SuppressThisFlag");
				}
				if (uFlag&0x1) {
					Debug::PrintString(" PreloadThisFlag");
				}
				uFlag = pInput[1];
				if (uFlag&0x1) {
					Debug::PrintString(" PreloadGlobalFlag");
				}
				pInput+=2;
				if (uNumParams) {
					do {
						Debug::Message(" Register 0x%02X:\"%s\"",static_cast<uint_t>(pInput[0]),reinterpret_cast<const char *>(pInput+1));
						pInput += StringLength(reinterpret_cast<const char *>(pInput+1))+2;
					} while (--uNumParams);
				}

				uNumParams = LittleEndian::LoadAny(reinterpret_cast<const uint16_t *>(pInput));
				Debug::Message(" Code size %u\n",static_cast<uint_t>(uNumParams));
			}
		}
	}
	return uResult;
}
