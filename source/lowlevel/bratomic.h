/***************************************

    Atomic memory

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
BURGER_INLINE uint_t IsCPUIDPresent(void) { return TRUE; }

#elif defined(BURGER_X86) || defined(DOXYGEN)
extern uint_t BURGER_API IsCPUIDPresent(void);

#else
BURGER_INLINE uint_t IsCPUIDPresent(void) { return FALSE; }
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
	uint32_t m_uHighestCPUID;				///< Highest allowed CPUID input (0 if CPUID is not present)
	uint32_t m_uHighestCPUIDExtended;		///< Highest allowed extended CPUID (0x80000000 and up or zero)
	uint32_t m_uHighestCPUIDHyperVisor;	///< Highest allowed hypervisor CPUID (0x40000000 and up or zero)
	uint32_t m_uModel;					///< Model information CPUID(1) -> EAX
	uint32_t m_uBrand;					///< Brand information CPUID(1) -> EBX
	uint32_t m_uCPUID1ECX;				///< Extended feature information CPUID(1) -> ECX
	uint32_t m_uCPUID1EDX;				///< Feature information CPUID(1) -> EDX
	uint32_t m_uCPUID80000001ECX;			///< Extended feature bits CPUID(0x80000001) -> ECX
	uint32_t m_uCPUID80000001EDX;			///< Extended processor info CPUID(0x80000001) -> EDX
	uint32_t m_uCPUID7EBX;				///< Feature bits CPUID(7)	-> EBX
	uint32_t m_uCPUID7ECX;				///< Feature bits CPUID(7) -> ECX
	uint32_t m_uCPUID7EDX;				///< Feature bits CPUID(7) -> EDX
	eCPU m_uCPUType;					///< \ref TRUE if the CPU's name is AuthenticAMD
	char m_CPUName[16];					///< 12 character brand name of the CPU (Null terminated)
	char m_BrandName[52];				///< 48 character full name of the CPU (Null terminated)
	char m_HypervisorName[16];			///< 12 character full name of the hypervisor (Null terminated)
	char m_HypervisorSignature[8];		///< 4 character hypervisor signature (Null terminated)
	BURGER_INLINE uint_t HasRTSC(void) const BURGER_NOEXCEPT { return m_uCPUID1EDX & 0x00000010U; }
	BURGER_INLINE uint_t HasCMOV(void) const BURGER_NOEXCEPT { return m_uCPUID1EDX & 0x00008000U; }
	BURGER_INLINE uint_t HasMMX(void) const BURGER_NOEXCEPT { return m_uCPUID1EDX & 0x00800000U; }
	BURGER_INLINE uint_t HasSSE(void) const BURGER_NOEXCEPT { return m_uCPUID1EDX & 0x02000000U; }
	BURGER_INLINE uint_t HasSSE2(void) const BURGER_NOEXCEPT { return m_uCPUID1EDX & 0x04000000U; }
	BURGER_INLINE uint_t HasSSE3(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x00000001U; }
	BURGER_INLINE uint_t HasSSSE3(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x00000200U; }
	BURGER_INLINE uint_t HasSSE4a(void) const BURGER_NOEXCEPT { return m_uCPUID80000001ECX & 0x00000040U; }
	BURGER_INLINE uint_t HasSSE41(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x00080000U; }
	BURGER_INLINE uint_t HasSSE42(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x00100000U; }
	BURGER_INLINE uint_t HasMOVBE(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x00400000U; }
	BURGER_INLINE uint_t HasAES(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x02000000U; }
	BURGER_INLINE uint_t HasAVX(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x10000000U; }
	BURGER_INLINE uint_t HasCMPXCHG16B(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x00002000U; }
	BURGER_INLINE uint_t HasF16C(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x20000000U; }
	BURGER_INLINE uint_t HasFMA3(void) const BURGER_NOEXCEPT { return m_uCPUID1ECX & 0x00001000U; }
	BURGER_INLINE uint_t HasFMA4(void) const BURGER_NOEXCEPT { return m_uCPUID80000001ECX&0x00010000U; }
	BURGER_INLINE uint_t HasLAHFSAHF(void) const BURGER_NOEXCEPT { return m_uCPUID80000001ECX&0x00000001U; }
	BURGER_INLINE uint_t HasPrefetchW(void) const BURGER_NOEXCEPT { return m_uCPUID80000001ECX&0x00000100U; }
	BURGER_INLINE uint_t Has3DNOW(void) const BURGER_NOEXCEPT { return (m_uCPUType==CPU_AMD) && (m_uCPUID80000001EDX& 0x80000000U); }
	BURGER_INLINE uint_t HasExtended3DNOW(void) const BURGER_NOEXCEPT { return (m_uCPUType==CPU_AMD)&&(m_uCPUID80000001EDX&0x40000000U); }
};

extern void BURGER_API CPUID(CPUID_t *pOutput);

#if defined(BURGER_PS3) || defined(BURGER_XBOX360)
BURGER_INLINE uint_t HasAltiVec(void) { return TRUE; }
BURGER_INLINE uint_t HasFSqrt(void) { return TRUE; }

#elif (defined(BURGER_PPC) && defined(BURGER_MACOS)) || defined(DOXYGEN)
extern uint_t BURGER_API HasAltiVec(void);
extern uint_t BURGER_API HasFSqrt(void);

#else
BURGER_INLINE uint_t HasAltiVec(void) { return FALSE; }
BURGER_INLINE uint_t HasFSqrt(void) { return FALSE; }
#endif

#if (defined(BURGER_MAC) && defined(BURGER_68K)) || defined(DOXYGEN)
extern uint_t BURGER_API HasFPU(void);
#else
BURGER_INLINE uint_t HasFPU(void) { return TRUE; }
#endif

// PS3 specific atomics
	
#if defined(BURGER_PS3)
	
BURGER_INLINE uint32_t AtomicSwap(volatile uint32_t *pOutput,uint32_t uInput) 
{
	uint32_t uTemp;
	do {
		uTemp = __lwarx(pOutput);
	} while(__stwcx(pOutput,uInput)==0); 
	return uTemp; 
}

BURGER_INLINE uint32_t AtomicPreIncrement(volatile uint32_t *pInput) BURGER_NOEXCEPT
{
	uint32_t uTemp; 
	do {
		uTemp = __lwarx(pInput)+1;
	} while(__stwcx(pInput,uTemp)==0); 
	return uTemp;
}

BURGER_INLINE uint32_t AtomicPostIncrement(volatile uint32_t *pInput) 
{
	uint32_t uTemp;
	do {
		uTemp = __lwarx(pInput); 
	} while(__stwcx(pInput,uTemp+1)==0);
	return uTemp; 
}

BURGER_INLINE uint32_t AtomicPreDecrement(volatile uint32_t *pInput) BURGER_NOEXCEPT
{
	uint32_t uTemp; 
	do {
		uTemp = __lwarx(pInput)-1;
	} while(__stwcx(pInput,uTemp)==0);
	return uTemp; 
}

BURGER_INLINE uint32_t AtomicPostDecrement(volatile uint32_t *pInput)
{ 
	uint32_t uTemp; 
	do { 
		uTemp = __lwarx(pInput);
	} while(__stwcx(pInput,uTemp-1)==0);
	return uTemp;
}

BURGER_INLINE uint32_t AtomicAdd(volatile uint32_t *pInput,uint32_t uValue) 
{
	uint32_t uTemp; 
	do {
		uTemp = __lwarx(pInput);
	} while(__stwcx(pInput,uTemp+uValue)==0); 
	return uTemp; 
}

BURGER_INLINE uint32_t AtomicSubtract(volatile uint32_t *pInput,uint32_t uValue) 
{
	uint32_t uTemp; 
	do {
		uTemp = __lwarx(pInput);
	} while(__stwcx(pInput,uTemp-uValue)==0); 
	return uTemp; 
}

BURGER_INLINE uint_t AtomicSetIfMatch(volatile uint32_t *pInput,uint32_t uBefore,uint32_t uAfter) 
{
	uint_t uResult;
	do { 
		uint32_t uTemp = __lwarx(pInput);
		uResult = (uTemp == uBefore);
		if (!uResult) {
			break; 
		}
	} while(__stwcx(pInput,uAfter)==0); 
	return uResult;
}

BURGER_INLINE uint32_t AtomicSwap(volatile uint64_t *pOutput,uint64_t uInput) 
{
	uint64_t uTemp; 
	do { 
		uTemp = __ldarx(pOutput);
	} while(__stdcx(pOutput,uInput)==0);
	return uTemp; 
}

BURGER_INLINE uint32_t AtomicPreIncrement(volatile uint64_t *pInput) BURGER_NOEXCEPT
{
	uint64_t uTemp; 
	do {
		uTemp = __ldarx(pInput)+1;
	} while(__stdcx(pInput,uTemp)==0);
	return uTemp; 
}

BURGER_INLINE uint32_t AtomicPostIncrement(volatile uint64_t *pInput)
{
	uint64_t uTemp;
	do { 
		uTemp = __ldarx(pInput);
	} while(__stdcx(pInput,uTemp+1)==0); 
	return uTemp; 
}

BURGER_INLINE uint32_t AtomicPreDecrement(volatile uint64_t *pInput) BURGER_NOEXCEPT
{
	uint64_t uTemp; 
	do {
		uTemp = __ldarx(pInput)-1;
	} while(__stdcx(pInput,uTemp)==0); 
	return uTemp; 
}

BURGER_INLINE uint32_t AtomicPostDecrement(volatile uint64_t *pInput) 
{
	uint64_t uTemp; 
	do {
		uTemp = __ldarx(pInput);
	} while(__stdcx(pInput,uTemp-1)==0);
	return uTemp;
}

BURGER_INLINE uint32_t AtomicAdd(volatile uint64_t *pInput,uint64_t uValue)
{
	uint64_t uTemp;
	do {
		uTemp = __ldarx(pInput);
	} while(__stdcx(pInput,uTemp+uValue)==0); 
	return uTemp;
}

BURGER_INLINE uint32_t AtomicSubtract(volatile uint64_t *pInput,uint64_t uValue)
{
	uint64_t uTemp; 
	do { 
		uTemp = __ldarx(pInput); 
	} while(__stdcx(pInput,uTemp-uValue)==0);
	return uTemp;
}

BURGER_INLINE uint_t AtomicSetIfMatch(volatile uint64_t *pInput,uint64_t uBefore,uint64_t uAfter) 
{
	uint_t uResult; 
	do {
		uint64_t uTemp = __ldarx(pInput);
		uResult = (uTemp == uBefore);
		if (!uResult) {
			break;
		}
	} while(__stdcx(pInput,uAfter)==0); 
	return uResult; 
}

#elif defined(BURGER_PPC) && defined(BURGER_METROWERKS)
	
BURGER_INLINE uint32_t AtomicSwap(register volatile uint32_t *pOutput,register uint32_t uInput) 
{
	register uint32_t uResult;
	asm {
loop:	lwarx	uResult,0,pOutput
		stwcx.	uInput,0,pOutput
		bne-	loop
	}
	return uResult; 
}
	
BURGER_INLINE uint32_t AtomicPreIncrement(register volatile uint32_t *pInput) BURGER_NOEXCEPT
{
	register uint32_t uResult;
	asm {
loop:	lwarx	uResult,0,pInput
		addi	uResult,uResult,1
		stwcx.	uResult,0,pInput
		bne-	loop
	}
	return uResult; 
}
	
BURGER_INLINE uint32_t AtomicPostIncrement(register volatile uint32_t *pInput) 
{
	register uint32_t uTemp;
	register uint32_t uResult;
	asm {
loop:	lwarx	uResult,0,pInput
		addi	uTemp,uResult,1
		stwcx.	uTemp,0,pInput
		bne-	loop
	}
	return uResult;
}
	
BURGER_INLINE uint32_t AtomicPreDecrement(register volatile uint32_t *pInput) BURGER_NOEXCEPT
{
	register uint32_t uResult;
	asm {
loop:	lwarx	uResult,0,pInput
		subi	uResult,uResult,1
		stwcx.	uResult,0,pInput
		bne-	loop
	}
	return uResult;
}
	
BURGER_INLINE uint32_t AtomicPostDecrement(register volatile uint32_t *pInput) 
{
	register uint32_t uTemp;
	register uint32_t uResult;
	asm {
loop:	lwarx	uResult,0,pInput
		subi	uTemp,uResult,1
		stwcx.	uTemp,0,pInput
		bne-	loop
	}
	return uResult;
}
	
BURGER_INLINE uint32_t AtomicAdd(register volatile uint32_t *pInput,register uint32_t uValue)
{
	register uint32_t uResult;
	asm {
loop:	lwarx	uResult,0,pInput
		add		uResult,uResult,uValue
		stwcx.	uResult,0,pInput
		bne-	loop
	}
	return uResult;
}
	
BURGER_INLINE uint32_t AtomicSubtract(register volatile uint32_t *pInput,register uint32_t uValue)
{
	register uint32_t uResult;
	asm {
loop:	lwarx	uResult,0,pInput
		sub		uResult,uResult,uValue
		stwcx.	uResult,0,pInput
		bne-	loop
	}
	return uResult;
}
	
BURGER_INLINE uint_t AtomicSetIfMatch(register volatile uint32_t *pInput,register uint32_t uBefore,register uint32_t uAfter)
{
	register uint32_t uTemp;
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

BURGER_INLINE uint32_t AtomicSwap(volatile uint32_t *pOutput,uint32_t uInput) 
{
	uint32_t uTemp;
	do {
		uTemp = __LWARX(const_cast<uint32_t *>(pOutput),0);
		__DCBST(const_cast<uint32_t *>(pOutput),0);
	} while(__STWCX(const_cast<uint32_t *>(pOutput),0,uInput)==0); 
	return uTemp; 
}

BURGER_INLINE uint32_t AtomicPreIncrement(volatile uint32_t *pInput) BURGER_NOEXCEPT
{
	uint32_t uTemp; 
	do {
		uTemp = __LWARX(const_cast<uint32_t *>(pInput),0)+1;
		__DCBST(const_cast<uint32_t *>(pInput),0);
	} while(__STWCX(const_cast<uint32_t *>(pInput),0,uTemp)==0); 
	return uTemp;
}

BURGER_INLINE uint32_t AtomicPostIncrement(volatile uint32_t *pInput)
{
	uint32_t uTemp;
	do {
		uTemp = __LWARX(const_cast<uint32_t *>(pInput),0);
		__DCBST(const_cast<uint32_t *>(pInput),0);
	} while (__STWCX(const_cast<uint32_t *>(pInput),0,uTemp + 1) == 0);
	return uTemp;
}

BURGER_INLINE uint32_t AtomicPreDecrement(volatile uint32_t *pInput) BURGER_NOEXCEPT
{
	uint32_t uTemp;
	do {
		uTemp = __LWARX(const_cast<uint32_t *>(pInput),0)-1;
		__DCBST(const_cast<uint32_t *>(pInput),0);
	} while (__STWCX(const_cast<uint32_t *>(pInput),0,uTemp) == 0);
	return uTemp;
}

BURGER_INLINE uint32_t AtomicPostDecrement(volatile uint32_t *pInput)
{
	uint32_t uTemp;
	do {
		uTemp = __LWARX(const_cast<uint32_t *>(pInput),0);
		__DCBST(const_cast<uint32_t *>(pInput),0);
	} while (__STWCX(const_cast<uint32_t *>(pInput),0,uTemp-1) == 0);
	return uTemp;
}

BURGER_INLINE uint32_t AtomicAdd(volatile uint32_t *pInput, uint32_t uValue)
{
	uint32_t uTemp;
	do {
		uTemp = __LWARX(const_cast<uint32_t *>(pInput),0);
		__DCBST(const_cast<uint32_t *>(pInput),0);
	} while (__STWCX(const_cast<uint32_t *>(pInput),0,uTemp + uValue) == 0);
	return uTemp;
}

BURGER_INLINE uint32_t AtomicSubtract(volatile uint32_t *pInput, uint32_t uValue)
{
	uint32_t uTemp;
	do {
		uTemp = __LWARX(const_cast<uint32_t *>(pInput),0);
		__DCBST(const_cast<uint32_t *>(pInput),0);
	} while (__STWCX(const_cast<uint32_t *>(pInput),0,uTemp - uValue) == 0);
	return uTemp;
}

BURGER_INLINE uint_t AtomicSetIfMatch(volatile uint32_t *pInput, uint32_t uBefore, uint32_t uAfter)
{
	uint_t uResult;
	do {
		uint32_t uTemp = __LWARX(const_cast<uint32_t *>(pInput),0);
		uResult = (uTemp == uBefore);
		if (!uResult) {
			break;
		}
		__DCBST(const_cast<uint32_t *>(pInput),0);
	} while (__STWCX(const_cast<uint32_t *>(pInput),0,uAfter) == 0);
	return uResult;
}

// Visual C compilers
#elif defined(BURGER_MSVC) || ((defined(BURGER_WATCOM) || defined(BURGER_METROWERKS)) && defined(BURGER_X86))

BURGER_INLINE uint32_t AtomicSwap(volatile uint32_t *pOutput,uint32_t uInput) { return static_cast<uint32_t>(_InterlockedExchange(reinterpret_cast<volatile long *>(pOutput),static_cast<long>(uInput)));}
BURGER_INLINE uint32_t AtomicPreIncrement(volatile uint32_t *pInput) BURGER_NOEXCEPT { return static_cast<uint32_t>(_InterlockedIncrement(reinterpret_cast<volatile long *>(pInput))); }
BURGER_INLINE uint32_t AtomicPostIncrement(volatile uint32_t *pInput) { return static_cast<uint32_t>(_InterlockedIncrement(reinterpret_cast<volatile long *>(pInput))-1); }
BURGER_INLINE uint32_t AtomicPreDecrement(volatile uint32_t *pInput) BURGER_NOEXCEPT { return static_cast<uint32_t>(_InterlockedDecrement(reinterpret_cast<volatile long *>(pInput))); }
BURGER_INLINE uint32_t AtomicPostDecrement(volatile uint32_t *pInput) { return static_cast<uint32_t>(_InterlockedDecrement(reinterpret_cast<volatile long *>(pInput))+1); }
BURGER_INLINE uint32_t AtomicAdd(volatile uint32_t *pInput,uint32_t uValue) { return static_cast<uint32_t>(_InterlockedExchangeAdd(reinterpret_cast<volatile long *>(pInput),static_cast<long>(uValue))); }
BURGER_INLINE uint32_t AtomicSubtract(volatile uint32_t *pInput,uint32_t uValue) { return static_cast<uint32_t>(_InterlockedExchangeAdd(reinterpret_cast<volatile long *>(pInput),-static_cast<long>(uValue))); }
BURGER_INLINE uint_t AtomicSetIfMatch(volatile uint32_t *pInput,uint32_t uBefore,uint32_t uAfter) { return _InterlockedCompareExchange(reinterpret_cast<volatile long *>(pInput),static_cast<long>(uAfter),static_cast<long>(uBefore))==static_cast<long>(uBefore); }
	
#if defined(BURGER_64BITCPU)
BURGER_INLINE uint64_t AtomicSwap(volatile uint64_t *pOutput,uint64_t uInput) { return _InterlockedExchange64(reinterpret_cast<volatile __int64 *>(pOutput),uInput); }
BURGER_INLINE uint64_t AtomicPreIncrement(volatile uint64_t *pInput) BURGER_NOEXCEPT { return _InterlockedIncrement64(reinterpret_cast<volatile __int64 *>(pInput)); }
BURGER_INLINE uint64_t AtomicPostIncrement(volatile uint64_t *pInput) { return _InterlockedIncrement64(reinterpret_cast<volatile __int64 *>(pInput))-1; }
BURGER_INLINE uint64_t AtomicPreDecrement(volatile uint64_t *pInput) BURGER_NOEXCEPT { return _InterlockedDecrement64(reinterpret_cast<volatile __int64 *>(pInput)); }
BURGER_INLINE uint64_t AtomicPostDecrement(volatile uint64_t *pInput) { return _InterlockedDecrement64(reinterpret_cast<volatile __int64 *>(pInput))+1; }
BURGER_INLINE uint64_t AtomicAdd(volatile uint32_t *pInput,uint64_t uValue) { return _InterlockedExchangeAdd64(reinterpret_cast<volatile __int64 *>(pInput),uValue); }
BURGER_INLINE uint64_t AtomicSubtract(volatile uint64_t *pInput,uint64_t uValue) { return _InterlockedExchangeAdd64(reinterpret_cast<volatile __int64 *>(pInput),0-uValue); }
BURGER_INLINE uint_t AtomicSetIfMatch(volatile uint64_t *pInput,uint64_t uBefore,uint64_t uAfter) { return _InterlockedCompareExchange64(reinterpret_cast<volatile __int64 *>(pInput),uAfter,uBefore)==static_cast<__int64>(uBefore); }
#endif
	
// If GnuC 4.0 or higher. use the intrinsics
#elif (BURGER_GNUC > 40100)

BURGER_INLINE uint32_t AtomicSwap(volatile uint32_t *pOutput,uint32_t uInput) { uint32_t uTemp; do { uTemp = pOutput[0]; } while(__sync_val_compare_and_swap(pOutput,uTemp,uInput)!=uTemp); return uTemp;}
BURGER_INLINE uint32_t AtomicPreIncrement(volatile uint32_t *pInput) BURGER_NOEXCEPT { return __sync_add_and_fetch(pInput,1); }
BURGER_INLINE uint32_t AtomicPostIncrement(volatile uint32_t *pInput) { return __sync_fetch_and_add(pInput,1); }
#if defined(BURGER_PPC) && defined(BURGER_MACOSX)	// Hack to get around a compiler crash for Xcode 3.1.4 for PowerPC!!!
BURGER_INLINE uint32_t AtomicPreDecrement(volatile uint32_t *pInput) BURGER_NOEXCEPT { return __sync_add_and_fetch(pInput,0xFFFFFFFFU); }
#else
BURGER_INLINE uint32_t AtomicPreDecrement(volatile uint32_t *pInput) BURGER_NOEXCEPT { return __sync_sub_and_fetch(pInput,1); }
#endif
BURGER_INLINE uint32_t AtomicPostDecrement(volatile uint32_t *pInput) { return __sync_fetch_and_sub(pInput,1); }
BURGER_INLINE uint32_t AtomicAdd(volatile uint32_t *pInput,uint32_t uValue) { return __sync_fetch_and_add(pInput,uValue); }
BURGER_INLINE uint32_t AtomicSubtract(volatile uint32_t *pInput,uint32_t uValue) { return __sync_fetch_and_sub(pInput,uValue); }
BURGER_INLINE uint_t AtomicSetIfMatch(volatile uint32_t *pInput,uint32_t uBefore,uint32_t uAfter) { return __sync_bool_compare_and_swap(pInput,uAfter,uBefore); }

BURGER_INLINE uint64_t AtomicSwap(volatile uint64_t *pOutput,uint64_t uInput) { uint64_t uTemp; do { uTemp = pOutput[0]; } while(__sync_val_compare_and_swap(pOutput,uTemp,uInput)!=uTemp); return uTemp;}
BURGER_INLINE uint64_t AtomicPreIncrement(volatile uint64_t *pInput) BURGER_NOEXCEPT { return __sync_add_and_fetch(pInput,1); }
BURGER_INLINE uint64_t AtomicPostIncrement(volatile uint64_t *pInput) { return __sync_fetch_and_add(pInput,1); }
BURGER_INLINE uint64_t AtomicPreDecrement(volatile uint64_t *pInput) BURGER_NOEXCEPT { return __sync_sub_and_fetch(pInput,1); }
BURGER_INLINE uint64_t AtomicPostDecrement(volatile uint64_t *pInput) { return __sync_fetch_and_sub(pInput,1); }
BURGER_INLINE uint64_t AtomicAdd(volatile uint64_t *pInput,uint64_t uValue) { return __sync_fetch_and_add(pInput,uValue); }
BURGER_INLINE uint64_t AtomicSubtract(volatile uint64_t *pInput,uint64_t uValue) { return __sync_fetch_and_sub(pInput,uValue); }
BURGER_INLINE uint_t AtomicSetIfMatch(volatile uint64_t *pInput,uint64_t uBefore,uint64_t uAfter) { return __sync_bool_compare_and_swap(pInput,uAfter,uBefore); }
	
#elif (defined(BURGER_GNUC) && (BURGER_GNUC <= 40100) && defined(BURGER_MACOSX)) || defined(DOXYGEN)
	
uint32_t BURGER_API AtomicSwap(volatile uint32_t *pOutput,uint32_t uInput);
uint32_t BURGER_API AtomicPreIncrement(volatile uint32_t *pInput)BURGER_NOEXCEPT;
uint32_t BURGER_API AtomicPostIncrement(volatile uint32_t *pInput);
uint32_t BURGER_API AtomicPreDecrement(volatile uint32_t *pInput) BURGER_NOEXCEPT;
uint32_t BURGER_API AtomicPostDecrement(volatile uint32_t *pInput);
uint32_t BURGER_API AtomicAdd(volatile uint32_t *pInput,uint32_t uValue);
uint32_t BURGER_API AtomicSubtract(volatile uint32_t *pInput,uint32_t uValue);
uint_t BURGER_API AtomicSetIfMatch(volatile uint32_t *pInput,uint32_t uBefore,uint32_t uAfter);

#if defined(BURGER_64BITCPU) || defined(DOXYGEN)
uint64_t BURGER_API AtomicSwap(volatile uint64_t *pOutput,uint64_t uInput);
uint64_t BURGER_API AtomicPreIncrement(volatile uint64_t *pInput)BURGER_NOEXCEPT;
uint64_t BURGER_API AtomicPostIncrement(volatile uint64_t *pInput);
uint64_t BURGER_API AtomicPreDecrement(volatile uint64_t *pInput) BURGER_NOEXCEPT;
uint64_t BURGER_API AtomicPostDecrement(volatile uint64_t *pInput);
uint64_t BURGER_API AtomicAdd(volatile uint64_t *pInput,uint64_t uValue);
uint64_t BURGER_API AtomicSubtract(volatile uint64_t *pInput,uint64_t uValue);
uint_t BURGER_API AtomicSetIfMatch(volatile uint64_t *pInput,uint64_t uBefore,uint64_t uAfter);
#endif
	
#else
	
BURGER_INLINE uint32_t AtomicSwap(volatile uint32_t *pOutput,uint32_t uInput) { uint32_t uTemp = pOutput[0]; pOutput[0] = uInput; return uTemp; }
BURGER_INLINE uint32_t AtomicPreIncrement(volatile uint32_t *pInput) BURGER_NOEXCEPT { uint32_t uTemp = pInput[0]+1; pInput[0] = uTemp; return uTemp; }
BURGER_INLINE uint32_t AtomicPostIncrement(volatile uint32_t *pInput) { uint32_t uTemp = pInput[0]; pInput[0] = uTemp+1; return uTemp; }
BURGER_INLINE uint32_t AtomicPreDecrement(volatile uint32_t *pInput) BURGER_NOEXCEPT { uint32_t uTemp = pInput[0]-1; pInput[0] = uTemp; return uTemp; }
BURGER_INLINE uint32_t AtomicPostDecrement(volatile uint32_t *pInput) { uint32_t uTemp = pInput[0]; pInput[0] = uTemp-1; return uTemp; }
BURGER_INLINE uint32_t AtomicAdd(volatile uint32_t *pInput,uint32_t uValue) { uint32_t uTemp = pInput[0]; pInput[0] = uTemp+uValue; return uTemp; }
BURGER_INLINE uint32_t AtomicSubtract(volatile uint32_t *pInput,uint32_t uValue) { uint32_t uTemp = pInput[0]; pInput[0] = uTemp-uValue; return uTemp; }
BURGER_INLINE uint_t AtomicSetIfMatch(volatile uint32_t *pInput,uint32_t uBefore,uint32_t uAfter) { uint_t uTemp = (pInput[0]==uBefore); if (uTemp) { pInput[0] = uAfter; } return uTemp; }

#if defined(BURGER_64BITCPU)
BURGER_INLINE uint64_t AtomicSwap(volatile uint64_t *pOutput,uint64_t uInput) { uint64_t uTemp = pOutput[0]; pOutput[0] = uInput; return uTemp; }
BURGER_INLINE uint64_t AtomicPreIncrement(volatile uint64_t *pInput) BURGER_NOEXCEPT { uint64_t uTemp = pInput[0]+1; pInput[0] = uTemp; return uTemp; }
BURGER_INLINE uint64_t AtomicPostIncrement(volatile uint64_t *pInput) { uint64_t uTemp = pInput[0]; pInput[0] = uTemp+1; return uTemp; }
BURGER_INLINE uint64_t AtomicPreDecrement(volatile uint64_t *pInput) BURGER_NOEXCEPT { uint64_t uTemp = pInput[0]-1; pInput[0] = uTemp; return uTemp; }
BURGER_INLINE uint64_t AtomicPostDecrement(volatile uint64_t *pInput) { uint64_t uTemp = pInput[0]; pInput[0] = uTemp-1; return uTemp; }
BURGER_INLINE uint64_t AtomicAdd(volatile uint64_t *pInput,uint64_t uValue) { uint64_t uTemp = pInput[0]; pInput[0] = uTemp+uValue; return uTemp; }
BURGER_INLINE uint64_t AtomicSubtract(volatile uint64_t *pInput,uint64_t uValue) { uint64_t uTemp = pInput[0]; pInput[0] = uTemp-uValue; return uTemp; }
BURGER_INLINE uint_t AtomicSetIfMatch(volatile uint64_t *pInput,uint64_t uBefore,uint64_t uAfter) { uint_t uTemp = (pInput[0]==uBefore); if (uTemp) { pInput[0] = uAfter; } return uTemp; }
#endif

#endif
}
/* END */

#endif
