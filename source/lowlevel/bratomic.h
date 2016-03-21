/***************************************

	Atomic memory

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRATOMIC_H__
#define __BRATOMIC_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

#if defined(BURGER_MSDOS) && !defined(__BRMSDOSTYPES_H__)
#include "brmsdostypes.h"
#endif

#ifndef __BRVISUALSTUDIO_H__
#include "brvisualstudio.h"
#endif

/* BEGIN */
namespace Burger {

#if defined(BURGER_X86) || defined(DOXYGEN)
extern Word BURGER_API IsCPUIDPresent(void);
#elif defined(BURGER_AMD64)
BURGER_INLINE Word IsCPUIDPresent(void) { return TRUE; }
#else
BURGER_INLINE Word IsCPUIDPresent(void) { return FALSE; }
#endif

struct CPUID_t {
	enum eCPU {
		CPU_INTEL,					///< CPU is from Intel
		CPU_AMD,					///< CPU is from AMD
		CPU_CENTAUR,				///< CPU is a Centaur
		CPU_CYRIX,					///< CPU is from Cyrix
		CPU_TRANSMETA,				///< CPU is from Transmeta
		CPU_NATIONALSEMICONDUCTOR,	///< CPU is from National Semiconductor (Geode)
		CPU_NEXGEN,					///< CPU is from NexGen
		CPU_RISE,					///< CPU is from Rise Technology (Now SiS)
		CPU_SIS,					///< CPU is from Silicon Integrated Systems
		CPU_UMC,					///< CPU is from United Microelectronics Corporation
		CPU_VIA,					///< CPU is from VIA Technology
		CPU_VORTEX,					///< CPU is from Vortex Technology
		CPU_UNKNOWN					///< CPU is emulated or not on this list
	};
	Word32 m_uHighestCPUID;				///< Highest allowed CPUID input (0 if CPUID is not present)
	Word32 m_uHighestCPUIDExtended;		///< Highest allowed extended CPUID (0x80000000 and up or zero)
	Word32 m_uHighestCPUIDHyperVisor;	///< Highest allowed hypervisor CPUID (0x40000000 and up or zero)
	Word32 m_uModel;					///< Model information CPUID(1) -> EAX
	Word32 m_uBrand;					///< Brand information CPUID(1) -> EBX
	Word32 m_uFeatureInformation;		///< Feature information CPUID(1) -> EDX
	Word32 m_uExtendedFeatureInformation;	///< Extended feature information CPUID(1) -> ECX
	Word32 m_uExtendedProcessorInformation;	///< Extended processor info CPUID(0x80000001) -> EDX
	Word32 m_uExtendedFeatureBits;		///< Extended feature bits CPUID(0x80000001) -> ECX'
	eCPU m_uCPUType;					///< \ref TRUE if the CPU's name is AuthenticAMD
	char m_CPUName[16];					///< 12 character brand name of the CPU (Null terminated)
	char m_BrandName[52];				///< 48 character full name of the CPU (Null terminated)
	char m_HypervisorName[16];			///< 12 character full name of the hypervisor (Null terminated)
	char m_HypervisorSignature[8];		///< 4 character hypervisor signature (Null terminated)
	BURGER_INLINE Word HasRTSC(void) const { return m_uFeatureInformation & 0x00000010U; }
	BURGER_INLINE Word HasCMOV(void) const { return m_uFeatureInformation & 0x00008000U; }
	BURGER_INLINE Word HasMMX(void) const { return m_uFeatureInformation & 0x00800000U; }
	BURGER_INLINE Word HasSSE(void) const { return m_uFeatureInformation & 0x02000000U; }
	BURGER_INLINE Word HasSSE2(void) const { return m_uFeatureInformation & 0x04000000U; }
	BURGER_INLINE Word HasSSE3(void) const { return m_uExtendedFeatureInformation & 0x00000001U; }
	BURGER_INLINE Word HasSSSE3(void) const { return m_uExtendedFeatureInformation & 0x00000200U; }
	BURGER_INLINE Word HasSSE41(void) const { return m_uExtendedFeatureInformation & 0x00080000U; }
	BURGER_INLINE Word HasSSE42(void) const { return m_uExtendedFeatureInformation & 0x00100000U; }
	BURGER_INLINE Word HasAVX(void) const { return m_uExtendedFeatureBits & 0x10000000U; }
	BURGER_INLINE Word Has3DNOW(void) const { return (m_uCPUType==CPU_AMD) && (m_uExtendedProcessorInformation & 0x80000000U); }
};
extern void BURGER_API CPUID(CPUID_t *pOutput);

#if defined(BURGER_PS3) || defined(BURGER_XBOX360)
BURGER_INLINE Word HasAltiVec(void) { return TRUE; }
#elif (defined(BURGER_POWERPC) && defined(BURGER_MACOS)) || defined(DOXYGEN)
extern Word BURGER_API HasAltiVec(void);
#else
BURGER_INLINE Word HasAltiVec(void) { return FALSE; }
#endif

#if defined(BURGER_PS3)
BURGER_INLINE Word32 AtomicSwap(volatile Word32 *pOutput,Word32 uInput) { Word32 uTemp; do { uTemp = __builtin_lwarx(pOutput,0); } while(__builtin_stwcx(uInput,pOutput,0)==0); return uTemp; }
BURGER_INLINE Word32 AtomicPreIncrement(volatile Word32 *pInput) { Word32 uTemp; do { uTemp = __builtin_lwarx(pInput,0)+1; } while(__builtin_stwcx(uTemp,pInput,0)==0); return uTemp; }
BURGER_INLINE Word32 AtomicPostIncrement(volatile Word32 *pInput) { Word32 uTemp; do { uTemp = __builtin_lwarx(pInput,0); } while(__builtin_stwcx(uTemp+1,pInput,0)==0); return uTemp; }
BURGER_INLINE Word32 AtomicPreDecrement(volatile Word32 *pInput) { Word32 uTemp; do { uTemp = __builtin_lwarx(pInput,0)-1; } while(__builtin_stwcx(uTemp,pInput,0)==0); return uTemp; }
BURGER_INLINE Word32 AtomicPostDecrement(volatile Word32 *pInput) { Word32 uTemp; do { uTemp = __builtin_lwarx(pInput,0); } while(__builtin_stwcx(uTemp-1,pInput,0)==0); return uTemp; }
BURGER_INLINE Word32 AtomicAdd(volatile Word32 *pInput,Word32 uValue) { Word32 uTemp; do { uTemp = __builtin_lwarx(pInput,0); } while(__builtin_stwcx(uTemp+uValue,pInput,0)==0); return uTemp; }
BURGER_INLINE Word32 AtomicSubtract(volatile Word32 *pInput,Word32 uValue) { Word32 uTemp; do { uTemp = __builtin_lwarx(pInput,0); } while(__builtin_stwcx(uTemp-uValue,pInput,0)==0); return uTemp; }
BURGER_INLINE Word AtomicSetIfMatch(volatile Word32 *pInput,Word32 uBefore,Word32 uAfter) { Word uResult; do { Word32 uTemp = __builtin_lwarx(pInput,0); uResult = (uTemp == uBefore); if (!uResult) break; } while(__builtin_stwcx(uAfter,pInput,0)==0); return uResult; }
BURGER_INLINE Word32 AtomicSwap(volatile Word64 *pOutput,Word64 uInput) { Word64 uTemp; do { uTemp = __builtin_ldarx(pOutput,0); } while(__builtin_stdcx(uInput,pOutput,0)==0); return uTemp; }
BURGER_INLINE Word32 AtomicPreIncrement(volatile Word64 *pInput) { Word64 uTemp; do { uTemp = __builtin_ldarx(pInput,0)+1; } while(__builtin_stdcx(uTemp,pInput,0)==0); return uTemp; }
BURGER_INLINE Word32 AtomicPostIncrement(volatile Word64 *pInput) { Word64 uTemp; do { uTemp = __builtin_ldarx(pInput,0); } while(__builtin_stdcx(uTemp+1,pInput,0)==0); return uTemp; }
BURGER_INLINE Word32 AtomicPreDecrement(volatile Word64 *pInput) { Word64 uTemp; do { uTemp = __builtin_ldarx(pInput,0)-1; } while(__builtin_stdcx(uTemp,pInput,0)==0); return uTemp; }
BURGER_INLINE Word32 AtomicPostDecrement(volatile Word64 *pInput) { Word64 uTemp; do { uTemp = __builtin_ldarx(pInput,0); } while(__builtin_stdcx(uTemp-1,pInput,0)==0); return uTemp; }
BURGER_INLINE Word32 AtomicAdd(volatile Word64 *pInput,Word64 uValue) { Word64 uTemp; do { uTemp = __builtin_ldarx(pInput,0); } while(__builtin_stdcx(uTemp+uValue,pInput,0)==0); return uTemp; }
BURGER_INLINE Word32 AtomicSubtract(volatile Word64 *pInput,Word64 uValue) { Word64 uTemp; do { uTemp = __builtin_ldarx(pInput,0); } while(__builtin_stdcx(uTemp-uValue,pInput,0)==0); return uTemp; }
BURGER_INLINE Word AtomicSetIfMatch(volatile Word64 *pInput,Word64 uBefore,Word64 uAfter) { Word uResult; do { Word64 uTemp = __builtin_ldarx(pInput,0); uResult = (uTemp == uBefore); if (!uResult) break; } while(__builtin_stdcx(uAfter,pInput,0)==0); return uResult; }

#elif defined(BURGER_POWERPC) && defined(BURGER_METROWERKS)
BURGER_INLINE Word32 AtomicSwap(register volatile Word32 *pOutput,register Word32 uInput) 
{
	register Word32 uResult;
	asm {
loop:	lwarx	uResult,0,pOutput
		stwcx.	uInput,0,pOutput
		bne-	loop
	}
	return uResult; 
}
BURGER_INLINE Word32 AtomicPreIncrement(register volatile Word32 *pInput) 
{
	register Word32 uResult;
	asm {
loop:	lwarx	uResult,0,pInput
		addi	uResult,uResult,1
		stwcx.	uResult,0,pInput
		bne-	loop
	}
	return uResult; 
}
BURGER_INLINE Word32 AtomicPostIncrement(register volatile Word32 *pInput) 
{
	register Word32 uTemp;
	register Word32 uResult;
	asm {
loop:	lwarx	uResult,0,pInput
		addi	uTemp,uResult,1
		stwcx.	uTemp,0,pInput
		bne-	loop
	}
	return uResult;
}
BURGER_INLINE Word32 AtomicPreDecrement(register volatile Word32 *pInput)
{
	register Word32 uResult;
	asm {
loop:	lwarx	uResult,0,pInput
		subi	uResult,uResult,1
		stwcx.	uResult,0,pInput
		bne-	loop
	}
	return uResult;
}
BURGER_INLINE Word32 AtomicPostDecrement(register volatile Word32 *pInput) 
{
	register Word32 uTemp;
	register Word32 uResult;
	asm {
loop:	lwarx	uResult,0,pInput
		subi	uTemp,uResult,1
		stwcx.	uTemp,0,pInput
		bne-	loop
	}
	return uResult;
}
BURGER_INLINE Word32 AtomicAdd(register volatile Word32 *pInput,register Word32 uValue)
{
	register Word32 uResult;
	asm {
loop:	lwarx	uResult,0,pInput
		add		uResult,uResult,uValue
		stwcx.	uResult,0,pInput
		bne-	loop
	}
	return uResult;
}
BURGER_INLINE Word32 AtomicSubtract(register volatile Word32 *pInput,register Word32 uValue)
{
	register Word32 uResult;
	asm {
loop:	lwarx	uResult,0,pInput
		sub		uResult,uResult,uValue
		stwcx.	uResult,0,pInput
		bne-	loop
	}
	return uResult;
}
BURGER_INLINE Word AtomicSetIfMatch(register volatile Word32 *pInput,register Word32 uBefore,register Word32 uAfter)
{
	register Word32 uTemp;
	asm {
loop:	lwarx	uTemp,0,pInput
		cmplw	uTemp,uBefore
		bne-	abort
		stwcx.	uAfter,0,pInput
		bne-	loop
abort:
	}
	return (uTemp==uBefore);
}

#elif defined(BURGER_ANDROID) || defined(BURGER_PS4) || defined(BURGER_PSP2) || defined(BURGER_MACOSX) || defined(BURGER_IOS)
BURGER_INLINE Word32 AtomicSwap(volatile Word32 *pOutput,Word32 uInput) { Word32 uTemp; do { uTemp = pOutput[0]; } while(__sync_val_compare_and_swap(pOutput,uTemp,uInput)!=uTemp); return uTemp;}
BURGER_INLINE Word32 AtomicPreIncrement(volatile Word32 *pInput) { return __sync_add_and_fetch(pInput,1); }
BURGER_INLINE Word32 AtomicPostIncrement(volatile Word32 *pInput) { return __sync_fetch_and_add(pInput,1); }
#if defined(BURGER_POWERPC) && defined(BURGER_MACOSX)	// Hack to get around a compiler crash for Xcode 3.1.4 for PowerPC!!!
BURGER_INLINE Word32 AtomicPreDecrement(volatile Word32 *pInput) { return __sync_add_and_fetch(pInput,0xFFFFFFFFU); }
#else
BURGER_INLINE Word32 AtomicPreDecrement(volatile Word32 *pInput) { return __sync_sub_and_fetch(pInput,1); }
#endif
BURGER_INLINE Word32 AtomicPostDecrement(volatile Word32 *pInput) { return __sync_fetch_and_sub(pInput,1); }
BURGER_INLINE Word32 AtomicAdd(volatile Word32 *pInput,Word32 uValue) { return __sync_fetch_and_add(pInput,uValue); }
BURGER_INLINE Word32 AtomicSubtract(volatile Word32 *pInput,Word32 uValue) { return __sync_fetch_and_sub(pInput,uValue); }
BURGER_INLINE Word AtomicSetIfMatch(volatile Word32 *pInput,Word32 uBefore,Word32 uAfter) { return __sync_bool_compare_and_swap(pInput,uAfter,uBefore); }
BURGER_INLINE Word64 AtomicSwap(volatile Word64 *pOutput,Word64 uInput) { Word64 uTemp; do { uTemp = pOutput[0]; } while(__sync_val_compare_and_swap(pOutput,uTemp,uInput)!=uTemp); return uTemp;}
BURGER_INLINE Word64 AtomicPreIncrement(volatile Word64 *pInput) { return __sync_add_and_fetch(pInput,1); }
BURGER_INLINE Word64 AtomicPostIncrement(volatile Word64 *pInput) { return __sync_fetch_and_add(pInput,1); }
BURGER_INLINE Word64 AtomicPreDecrement(volatile Word64 *pInput) { return __sync_sub_and_fetch(pInput,1); }
BURGER_INLINE Word64 AtomicPostDecrement(volatile Word64 *pInput) { return __sync_fetch_and_sub(pInput,1); }
BURGER_INLINE Word64 AtomicAdd(volatile Word64 *pInput,Word64 uValue) { return __sync_fetch_and_add(pInput,uValue); }
BURGER_INLINE Word64 AtomicSubtract(volatile Word64 *pInput,Word64 uValue) { return __sync_fetch_and_sub(pInput,uValue); }
BURGER_INLINE Word AtomicSetIfMatch(volatile Word64 *pInput,Word64 uBefore,Word64 uAfter) { return __sync_bool_compare_and_swap(pInput,uAfter,uBefore); }

#elif defined(BURGER_XBOX360) || defined(BURGER_WINDOWS) || defined(BURGER_MSDOS)
BURGER_INLINE Word32 AtomicSwap(volatile Word32 *pOutput,Word32 uInput) { return static_cast<Word32>(_InterlockedExchange(reinterpret_cast<volatile long *>(pOutput),static_cast<long>(uInput)));}
BURGER_INLINE Word32 AtomicPreIncrement(volatile Word32 *pInput) { return static_cast<Word32>(_InterlockedIncrement(reinterpret_cast<volatile long *>(pInput))); }
BURGER_INLINE Word32 AtomicPostIncrement(volatile Word32 *pInput) { return static_cast<Word32>(_InterlockedIncrement(reinterpret_cast<volatile long *>(pInput))-1); }
BURGER_INLINE Word32 AtomicPreDecrement(volatile Word32 *pInput) { return static_cast<Word32>(_InterlockedDecrement(reinterpret_cast<volatile long *>(pInput))); }
BURGER_INLINE Word32 AtomicPostDecrement(volatile Word32 *pInput) { return static_cast<Word32>(_InterlockedDecrement(reinterpret_cast<volatile long *>(pInput))+1); }
BURGER_INLINE Word32 AtomicAdd(volatile Word32 *pInput,Word32 uValue) { return static_cast<Word32>(_InterlockedExchangeAdd(reinterpret_cast<volatile long *>(pInput),static_cast<long>(uValue))); }
BURGER_INLINE Word32 AtomicSubtract(volatile Word32 *pInput,Word32 uValue) { return static_cast<Word32>(_InterlockedExchangeAdd(reinterpret_cast<volatile long *>(pInput),-static_cast<long>(uValue))); }
BURGER_INLINE Word AtomicSetIfMatch(volatile Word32 *pInput,Word32 uBefore,Word32 uAfter) { return _InterlockedCompareExchange(reinterpret_cast<volatile long *>(pInput),static_cast<long>(uAfter),static_cast<long>(uBefore))==static_cast<long>(uBefore); }
#if defined(BURGER_64BITCPU)
BURGER_INLINE Word64 AtomicSwap(volatile Word64 *pOutput,Word64 uInput) { return _InterlockedExchange64(reinterpret_cast<volatile __int64 *>(pOutput),uInput); }
BURGER_INLINE Word64 AtomicPreIncrement(volatile Word64 *pInput) { return _InterlockedIncrement64(reinterpret_cast<volatile __int64 *>(pInput)); }
BURGER_INLINE Word64 AtomicPostIncrement(volatile Word64 *pInput) { return _InterlockedIncrement64(reinterpret_cast<volatile __int64 *>(pInput))-1; }
BURGER_INLINE Word64 AtomicPreDecrement(volatile Word64 *pInput) { return _InterlockedDecrement64(reinterpret_cast<volatile __int64 *>(pInput)); }
BURGER_INLINE Word64 AtomicPostDecrement(volatile Word64 *pInput) { return _InterlockedDecrement64(reinterpret_cast<volatile __int64 *>(pInput))+1; }
BURGER_INLINE Word64 AtomicAdd(volatile Word32 *pInput,Word64 uValue) { return _InterlockedExchangeAdd64(reinterpret_cast<volatile __int64 *>(pInput),uValue); }
BURGER_INLINE Word64 AtomicSubtract(volatile Word64 *pInput,Word64 uValue) { return _InterlockedExchangeAdd64(reinterpret_cast<volatile __int64 *>(pInput),0-uValue); }
BURGER_INLINE Word AtomicSetIfMatch(volatile Word64 *pInput,Word64 uBefore,Word64 uAfter) { return _InterlockedCompareExchange64(reinterpret_cast<volatile __int64 *>(pInput),uAfter,uBefore)==static_cast<__int64>(uBefore); }
#endif

#else
BURGER_INLINE Word32 AtomicSwap(volatile Word32 *pOutput,Word32 uInput) { Word32 uTemp = pOutput[0]; pOutput[0] = uInput; return uTemp; }
BURGER_INLINE Word32 AtomicPreIncrement(volatile Word32 *pInput) { Word32 uTemp = pInput[0]+1; pInput[0] = uTemp; return uTemp; }
BURGER_INLINE Word32 AtomicPostIncrement(volatile Word32 *pInput) { Word32 uTemp = pInput[0]; pInput[0] = uTemp+1; return uTemp; }
BURGER_INLINE Word32 AtomicPreDecrement(volatile Word32 *pInput) { Word32 uTemp = pInput[0]-1; pInput[0] = uTemp; return uTemp; }
BURGER_INLINE Word32 AtomicPostDecrement(volatile Word32 *pInput) { Word32 uTemp = pInput[0]; pInput[0] = uTemp-1; return uTemp; }
BURGER_INLINE Word32 AtomicAdd(volatile Word32 *pInput,Word32 uValue) { Word32 uTemp = pInput[0]; pInput[0] = uTemp+uValue; return uTemp; }
BURGER_INLINE Word32 AtomicSubtract(volatile Word32 *pInput,Word32 uValue) { Word32 uTemp = pInput[0]; pInput[0] = uTemp-uValue; return uTemp; }
BURGER_INLINE Word AtomicSetIfMatch(volatile Word32 *pInput,Word32 uBefore,Word32 uAfter) { Word uTemp = (pInput[0]==uBefore); if (uTemp) { pInput[0] = uAfter; } return uTemp; }
#if defined(BURGER_64BITCPU) || defined(DOXYGEN)
BURGER_INLINE Word64 AtomicSwap(volatile Word64 *pOutput,Word64 uInput) { Word64 uTemp = pOutput[0]; pOutput[0] = uInput; return uTemp; }
BURGER_INLINE Word64 AtomicPreIncrement(volatile Word64 *pInput) { Word64 uTemp = pInput[0]+1; pInput[0] = uTemp; return uTemp; }
BURGER_INLINE Word64 AtomicPostIncrement(volatile Word64 *pInput) { Word64 uTemp = pInput[0]; pInput[0] = uTemp+1; return uTemp; }
BURGER_INLINE Word64 AtomicPreDecrement(volatile Word64 *pInput) { Word64 uTemp = pInput[0]-1; pInput[0] = uTemp; return uTemp; }
BURGER_INLINE Word64 AtomicPostDecrement(volatile Word64 *pInput) { Word64 uTemp = pInput[0]; pInput[0] = uTemp-1; return uTemp; }
BURGER_INLINE Word64 AtomicAdd(volatile Word64 *pInput,Word64 uValue) { Word64 uTemp = pInput[0]; pInput[0] = uTemp+uValue; return uTemp; }
BURGER_INLINE Word64 AtomicSubtract(volatile Word64 *pInput,Word64 uValue) { Word64 uTemp = pInput[0]; pInput[0] = uTemp-uValue; return uTemp; }
BURGER_INLINE Word AtomicSetIfMatch(volatile Word64 *pInput,Word64 uBefore,Word64 uAfter) { Word uTemp = (pInput[0]==uBefore); if (uTemp) { pInput[0] = uAfter; } return uTemp; }
#endif

#endif
}
/* END */

#endif
