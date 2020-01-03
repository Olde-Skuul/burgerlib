/***************************************

	Atomic memory

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#if (defined(BURGER_AMD64) || defined(BURGER_XBOX)) && !defined(DOXYGEN)
BURGER_INLINE Word IsCPUIDPresent(void) { return TRUE; }

#elif defined(BURGER_X86) || defined(DOXYGEN)
extern Word BURGER_API IsCPUIDPresent(void);

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
		CPU_KVM,					///< CPU is Kernel Virtual Machine
		CPU_MICROSOFT_VIRTUAL_PC,	///< CPU is Microsoft Windows Virtual PC
		CPU_PARALLELS,				///< CPU is Parallels Virtual PC
		CPU_VMWARE,					///< CPU is VMWare
		CPU_XEN,					///< CPU is Xen Hypervisor Virtual Machine
		CPU_UNKNOWN					///< CPU is emulated or not on this list
	};
	Word32 m_uHighestCPUID;				///< Highest allowed CPUID input (0 if CPUID is not present)
	Word32 m_uHighestCPUIDExtended;		///< Highest allowed extended CPUID (0x80000000 and up or zero)
	Word32 m_uHighestCPUIDHyperVisor;	///< Highest allowed hypervisor CPUID (0x40000000 and up or zero)
	Word32 m_uModel;					///< Model information CPUID(1) -> EAX
	Word32 m_uBrand;					///< Brand information CPUID(1) -> EBX
	Word32 m_uCPUID1ECX;				///< Extended feature information CPUID(1) -> ECX
	Word32 m_uCPUID1EDX;				///< Feature information CPUID(1) -> EDX
	Word32 m_uCPUID80000001ECX;			///< Extended feature bits CPUID(0x80000001) -> ECX
	Word32 m_uCPUID80000001EDX;			///< Extended processor info CPUID(0x80000001) -> EDX
	Word32 m_uCPUID7EBX;				///< Feature bits CPUID(7)	-> EBX
	Word32 m_uCPUID7ECX;				///< Feature bits CPUID(7) -> ECX
	Word32 m_uCPUID7EDX;				///< Feature bits CPUID(7) -> EDX
	eCPU m_uCPUType;					///< \ref TRUE if the CPU's name is AuthenticAMD
	char m_CPUName[16];					///< 12 character brand name of the CPU (Null terminated)
	char m_BrandName[52];				///< 48 character full name of the CPU (Null terminated)
	char m_HypervisorName[16];			///< 12 character full name of the hypervisor (Null terminated)
	char m_HypervisorSignature[8];		///< 4 character hypervisor signature (Null terminated)
	BURGER_INLINE Word HasRTSC(void) const BURGER_NOEXCEPT { return m_uCPUID1EDX & 0x00000010U; }
	BURGER_INLINE Word HasCMOV(void) const BURGER_NOEXCEPT { return m_uCPUID1EDX & 0x00008000U; }
	BURGER_INLINE Word HasMMX(void) const BURGER_NOEXCEPT { return m_uCPUID1EDX & 0x00800000U; }
	BURGER_INLINE Word HasSSE(void) const BURGER_NOEXCEPT { return m_uCPUID1EDX & 0x02000000U; }
	BURGER_INLINE Word HasSSE2(void) const BURGER_NOEXCEPT { return m_uCPUID1EDX & 0x04000000U; }
	BURGER_INLINE Word HasSSE3(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x00000001U; }
	BURGER_INLINE Word HasSSSE3(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x00000200U; }
	BURGER_INLINE Word HasSSE4a(void) const BURGER_NOEXCEPT { return m_uCPUID80000001ECX & 0x00000040U; }
	BURGER_INLINE Word HasSSE41(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x00080000U; }
	BURGER_INLINE Word HasSSE42(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x00100000U; }
	BURGER_INLINE Word HasMOVBE(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x00400000U; }
	BURGER_INLINE Word HasAES(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x02000000U; }
	BURGER_INLINE Word HasAVX(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x10000000U; }
	BURGER_INLINE Word HasCMPXCHG16B(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x00002000U; }
	BURGER_INLINE Word HasF16C(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x20000000U; }
	BURGER_INLINE Word HasFMA3(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x00001000U; }
	BURGER_INLINE Word HasFMA4(void) const BURGER_NOEXCEPT { return m_uCPUID80000001ECX&0x00010000U; }
	BURGER_INLINE Word HasLAHFSAHF(void) const BURGER_NOEXCEPT { return m_uCPUID80000001ECX&0x00000001U; }
	BURGER_INLINE Word HasPrefetchW(void) const BURGER_NOEXCEPT { return m_uCPUID80000001ECX&0x00000100U; }
	BURGER_INLINE Word Has3DNOW(void) const BURGER_NOEXCEPT { return (m_uCPUType==CPU_AMD) && (m_uCPUID80000001EDX& 0x80000000U); }
	BURGER_INLINE Word HasExtended3DNOW(void) const BURGER_NOEXCEPT { return (m_uCPUType==CPU_AMD)&&(m_uCPUID80000001EDX&0x40000000U); }
};

extern void BURGER_API CPUID(CPUID_t *pOutput);

#if defined(BURGER_PS3) || defined(BURGER_XBOX360)
BURGER_INLINE Word HasAltiVec(void) { return TRUE; }
BURGER_INLINE Word HasFSqrt(void) { return TRUE; }

#elif (defined(BURGER_PPC) && defined(BURGER_MACOS)) || defined(DOXYGEN)
extern Word BURGER_API HasAltiVec(void);
extern Word BURGER_API HasFSqrt(void);

#else
BURGER_INLINE Word HasAltiVec(void) { return FALSE; }
BURGER_INLINE Word HasFSqrt(void) { return FALSE; }
#endif

#if (defined(BURGER_MAC) && defined(BURGER_68K)) || defined(DOXYGEN)
extern Word BURGER_API HasFPU(void);
#else
BURGER_INLINE Word HasFPU(void) { return TRUE; }
#endif

// PS3 specific atomics
	
#if defined(BURGER_PS3)
	
BURGER_INLINE Word32 AtomicSwap(volatile Word32 *pOutput,Word32 uInput) 
{
	Word32 uTemp;
	do {
		uTemp = __lwarx(pOutput);
	} while(__stwcx(pOutput,uInput)==0); 
	return uTemp; 
}

BURGER_INLINE Word32 AtomicPreIncrement(volatile Word32 *pInput) 
{
	Word32 uTemp; 
	do {
		uTemp = __lwarx(pInput)+1;
	} while(__stwcx(pInput,uTemp)==0); 
	return uTemp;
}

BURGER_INLINE Word32 AtomicPostIncrement(volatile Word32 *pInput) 
{
	Word32 uTemp;
	do {
		uTemp = __lwarx(pInput); 
	} while(__stwcx(pInput,uTemp+1)==0);
	return uTemp; 
}

BURGER_INLINE Word32 AtomicPreDecrement(volatile Word32 *pInput)
{
	Word32 uTemp; 
	do {
		uTemp = __lwarx(pInput)-1;
	} while(__stwcx(pInput,uTemp)==0);
	return uTemp; 
}

BURGER_INLINE Word32 AtomicPostDecrement(volatile Word32 *pInput)
{ 
	Word32 uTemp; 
	do { 
		uTemp = __lwarx(pInput);
	} while(__stwcx(pInput,uTemp-1)==0);
	return uTemp;
}

BURGER_INLINE Word32 AtomicAdd(volatile Word32 *pInput,Word32 uValue) 
{
	Word32 uTemp; 
	do {
		uTemp = __lwarx(pInput);
	} while(__stwcx(pInput,uTemp+uValue)==0); 
	return uTemp; 
}

BURGER_INLINE Word32 AtomicSubtract(volatile Word32 *pInput,Word32 uValue) 
{
	Word32 uTemp; 
	do {
		uTemp = __lwarx(pInput);
	} while(__stwcx(pInput,uTemp-uValue)==0); 
	return uTemp; 
}

BURGER_INLINE Word AtomicSetIfMatch(volatile Word32 *pInput,Word32 uBefore,Word32 uAfter) 
{
	Word uResult;
	do { 
		Word32 uTemp = __lwarx(pInput);
		uResult = (uTemp == uBefore);
		if (!uResult) {
			break; 
		}
	} while(__stwcx(pInput,uAfter)==0); 
	return uResult;
}

BURGER_INLINE Word32 AtomicSwap(volatile Word64 *pOutput,Word64 uInput) 
{
	Word64 uTemp; 
	do { 
		uTemp = __ldarx(pOutput);
	} while(__stdcx(pOutput,uInput)==0);
	return uTemp; 
}

BURGER_INLINE Word32 AtomicPreIncrement(volatile Word64 *pInput)
{
	Word64 uTemp; 
	do {
		uTemp = __ldarx(pInput)+1;
	} while(__stdcx(pInput,uTemp)==0);
	return uTemp; 
}

BURGER_INLINE Word32 AtomicPostIncrement(volatile Word64 *pInput)
{
	Word64 uTemp;
	do { 
		uTemp = __ldarx(pInput);
	} while(__stdcx(pInput,uTemp+1)==0); 
	return uTemp; 
}

BURGER_INLINE Word32 AtomicPreDecrement(volatile Word64 *pInput) 
{
	Word64 uTemp; 
	do {
		uTemp = __ldarx(pInput)-1;
	} while(__stdcx(pInput,uTemp)==0); 
	return uTemp; 
}

BURGER_INLINE Word32 AtomicPostDecrement(volatile Word64 *pInput) 
{
	Word64 uTemp; 
	do {
		uTemp = __ldarx(pInput);
	} while(__stdcx(pInput,uTemp-1)==0);
	return uTemp;
}

BURGER_INLINE Word32 AtomicAdd(volatile Word64 *pInput,Word64 uValue)
{
	Word64 uTemp;
	do {
		uTemp = __ldarx(pInput);
	} while(__stdcx(pInput,uTemp+uValue)==0); 
	return uTemp;
}

BURGER_INLINE Word32 AtomicSubtract(volatile Word64 *pInput,Word64 uValue)
{
	Word64 uTemp; 
	do { 
		uTemp = __ldarx(pInput); 
	} while(__stdcx(pInput,uTemp-uValue)==0);
	return uTemp;
}

BURGER_INLINE Word AtomicSetIfMatch(volatile Word64 *pInput,Word64 uBefore,Word64 uAfter) 
{
	Word uResult; 
	do {
		Word64 uTemp = __ldarx(pInput);
		uResult = (uTemp == uBefore);
		if (!uResult) {
			break;
		}
	} while(__stdcx(pInput,uAfter)==0); 
	return uResult; 
}

#elif defined(BURGER_PPC) && defined(BURGER_METROWERKS)
	
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

// Nintendo WiiU
#elif defined(BURGER_WIIU)

BURGER_INLINE Word32 AtomicSwap(volatile Word32 *pOutput,Word32 uInput) 
{
	Word32 uTemp;
	do {
		uTemp = __LWARX(const_cast<Word32 *>(pOutput),0);
		__DCBST(const_cast<Word32 *>(pOutput),0);
	} while(__STWCX(const_cast<Word32 *>(pOutput),0,uInput)==0); 
	return uTemp; 
}

BURGER_INLINE Word32 AtomicPreIncrement(volatile Word32 *pInput) 
{
	Word32 uTemp; 
	do {
		uTemp = __LWARX(const_cast<Word32 *>(pInput),0)+1;
		__DCBST(const_cast<Word32 *>(pInput),0);
	} while(__STWCX(const_cast<Word32 *>(pInput),0,uTemp)==0); 
	return uTemp;
}

BURGER_INLINE Word32 AtomicPostIncrement(volatile Word32 *pInput)
{
	Word32 uTemp;
	do {
		uTemp = __LWARX(const_cast<Word32 *>(pInput),0);
		__DCBST(const_cast<Word32 *>(pInput),0);
	} while (__STWCX(const_cast<Word32 *>(pInput),0,uTemp + 1) == 0);
	return uTemp;
}

BURGER_INLINE Word32 AtomicPreDecrement(volatile Word32 *pInput)
{
	Word32 uTemp;
	do {
		uTemp = __LWARX(const_cast<Word32 *>(pInput),0)-1;
		__DCBST(const_cast<Word32 *>(pInput),0);
	} while (__STWCX(const_cast<Word32 *>(pInput),0,uTemp) == 0);
	return uTemp;
}

BURGER_INLINE Word32 AtomicPostDecrement(volatile Word32 *pInput)
{
	Word32 uTemp;
	do {
		uTemp = __LWARX(const_cast<Word32 *>(pInput),0);
		__DCBST(const_cast<Word32 *>(pInput),0);
	} while (__STWCX(const_cast<Word32 *>(pInput),0,uTemp-1) == 0);
	return uTemp;
}

BURGER_INLINE Word32 AtomicAdd(volatile Word32 *pInput, Word32 uValue)
{
	Word32 uTemp;
	do {
		uTemp = __LWARX(const_cast<Word32 *>(pInput),0);
		__DCBST(const_cast<Word32 *>(pInput),0);
	} while (__STWCX(const_cast<Word32 *>(pInput),0,uTemp + uValue) == 0);
	return uTemp;
}

BURGER_INLINE Word32 AtomicSubtract(volatile Word32 *pInput, Word32 uValue)
{
	Word32 uTemp;
	do {
		uTemp = __LWARX(const_cast<Word32 *>(pInput),0);
		__DCBST(const_cast<Word32 *>(pInput),0);
	} while (__STWCX(const_cast<Word32 *>(pInput),0,uTemp - uValue) == 0);
	return uTemp;
}

BURGER_INLINE Word AtomicSetIfMatch(volatile Word32 *pInput, Word32 uBefore, Word32 uAfter)
{
	Word uResult;
	do {
		Word32 uTemp = __LWARX(const_cast<Word32 *>(pInput),0);
		uResult = (uTemp == uBefore);
		if (!uResult) {
			break;
		}
		__DCBST(const_cast<Word32 *>(pInput),0);
	} while (__STWCX(const_cast<Word32 *>(pInput),0,uAfter) == 0);
	return uResult;
}

// Visual C compilers
#elif defined(BURGER_MSVC) || ((defined(BURGER_WATCOM) || defined(BURGER_METROWERKS)) && defined(BURGER_X86))

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
	
// If GnuC 4.0 or higher. use the intrinsics
#elif (BURGER_GNUC > 40100)

BURGER_INLINE Word32 AtomicSwap(volatile Word32 *pOutput,Word32 uInput) { Word32 uTemp; do { uTemp = pOutput[0]; } while(__sync_val_compare_and_swap(pOutput,uTemp,uInput)!=uTemp); return uTemp;}
BURGER_INLINE Word32 AtomicPreIncrement(volatile Word32 *pInput) { return __sync_add_and_fetch(pInput,1); }
BURGER_INLINE Word32 AtomicPostIncrement(volatile Word32 *pInput) { return __sync_fetch_and_add(pInput,1); }
#if defined(BURGER_PPC) && defined(BURGER_MACOSX)	// Hack to get around a compiler crash for Xcode 3.1.4 for PowerPC!!!
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
	
#elif (defined(BURGER_GNUC) && (BURGER_GNUC <= 40100) && defined(BURGER_MACOSX)) || defined(DOXYGEN)
	
Word32 BURGER_API AtomicSwap(volatile Word32 *pOutput,Word32 uInput);
Word32 BURGER_API AtomicPreIncrement(volatile Word32 *pInput);
Word32 BURGER_API AtomicPostIncrement(volatile Word32 *pInput);
Word32 BURGER_API AtomicPreDecrement(volatile Word32 *pInput);
Word32 BURGER_API AtomicPostDecrement(volatile Word32 *pInput);
Word32 BURGER_API AtomicAdd(volatile Word32 *pInput,Word32 uValue);
Word32 BURGER_API AtomicSubtract(volatile Word32 *pInput,Word32 uValue);
Word BURGER_API AtomicSetIfMatch(volatile Word32 *pInput,Word32 uBefore,Word32 uAfter);

#if defined(BURGER_64BITCPU) || defined(DOXYGEN)
Word64 BURGER_API AtomicSwap(volatile Word64 *pOutput,Word64 uInput);
Word64 BURGER_API AtomicPreIncrement(volatile Word64 *pInput);
Word64 BURGER_API AtomicPostIncrement(volatile Word64 *pInput);
Word64 BURGER_API AtomicPreDecrement(volatile Word64 *pInput);
Word64 BURGER_API AtomicPostDecrement(volatile Word64 *pInput);
Word64 BURGER_API AtomicAdd(volatile Word64 *pInput,Word64 uValue);
Word64 BURGER_API AtomicSubtract(volatile Word64 *pInput,Word64 uValue);
Word BURGER_API AtomicSetIfMatch(volatile Word64 *pInput,Word64 uBefore,Word64 uAfter);
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

#if defined(BURGER_64BITCPU)
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
