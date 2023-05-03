/***************************************

	Atomic memory

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRWATCOM_H__
#include "brwatcom.h"
#endif

#ifndef __BRVISUALSTUDIO_H__
#include "brvisualstudio.h"
#endif

#ifndef __BRMETROWERKS_H__
#include "brmetrowerks.h"
#endif

/* BEGIN */
namespace Burger {

#if defined(BURGER_INTEL) || defined(DOXYGEN)

struct CPUID_t {
	enum eCPU {
		/** CPU is from Intel */
		kCPUIntel,
		/** CPU is from AMD */
		kCPUAMD,
		/** CPU is a Centaur */
		kCPUCentaur,
		/** CPU is from Cyrix */
		kCPUCyrix,
		/** CPU is from Transmeta */
		kCPUTransmeta,
		/** CPU is from National Semiconductor (Geode) */
		kCPUNationalSemiconductor,
		/** CPU is from NexGen */
		kCPUNexgen,
		/** CPU is from Rise Technology (Now SiS) */
		kCPURise,
		/** CPU is from Silicon Integrated Systems */
		kCPUSIS,
		/** CPU is from United Microelectronics Corporation */
		kCPUUMC,
		/** CPU is from VIA Technology */
		kCPUVIA,
		/** CPU is from Vortex Technology */
		kCPUVortex,
		/** CPU is from Zhaoxin */
		kCPUShanghai,
		/** CPU is from Hygon*/
		kCPUHygon,
		/** CPU is from RDC Semiconductor */
		kCPURDC,
		/** CPU is Russian Elbrus 2000 */
		kCPUElbrus2000,
		/** CPU is MiSTer 486 */
		kCPUMiSTer486,
		/** CPU is Kernel Virtual Machine */
		kCPUKVM,
		/** CPU is Microsoft Windows Virtual PC */
		kCPUMicrosoftVirtualPC,
		/** CPU is Parallels Virtual PC */
		kCPUParallels,
		/** CPU is VMWare */
		kCPUVMWare,
		/** CPU is Xen Hypervisor Virtual Machine */
		kCPUXen,
		/** CPU is Apple Rosetta 2*/
		kCPURosetta2,
		/** CPU is emulated or not on this list */
		kCPUUnknown
	};

	/** Result from XGETBV(0) */
	uint64_t m_uXGETBV;
	
	/** Highest allowed CPUID input (0 if CPUID is not present) */
	uint32_t m_uHighestCPUID;
	/** Highest allowed extended CPUID (0x80000000 and up or zero) */
	uint32_t m_uHighestCPUIDExtended;
	/** Highest allowed hypervisor CPUID (0x40000000 and up or zero) */
	uint32_t m_uHighestCPUIDHyperVisor;

	/** Model information CPUID(1) -> EAX */
	uint32_t m_uModel;
	/** Brand information CPUID(1) -> EBX */
	uint32_t m_uBrand;
	/** Extended feature information CPUID(1) -> ECX */
	uint32_t m_uCPUID1ECX;
	/** Feature information CPUID(1) -> EDX */
	uint32_t m_uCPUID1EDX;

	/** Extended feature bits CPUID(0x80000001) -> ECX */
	uint32_t m_uCPUID80000001ECX;
	/** Extended processor info CPUID(0x80000001) -> EDX */
	uint32_t m_uCPUID80000001EDX;

	/** Feature bits CPUID(7) -> EBX */
	uint32_t m_uCPUID7EBX;
	/** Feature bits CPUID(7) -> ECX */
	uint32_t m_uCPUID7ECX;
	/** Feature bits CPUID(7) -> EDX */
	uint32_t m_uCPUID7EDX;

	/** Enumeration of the manufacturer of the CPU */
	eCPU m_uCPUType;

	/** 12 character brand name of the CPU (Null terminated) */
	char m_CPUName[16];

	/** 48 character full name of the CPU (Null terminated) */
	char m_BrandName[52];

	/** 12 character full name of the hypervisor (Null terminated) */
	char m_HypervisorName[16];

	/** 4 character hypervisor signature (Null terminated) */
	char m_HypervisorSignature[8];

	BURGER_INLINE uint_t has_RTSC(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID1EDX & 0x00000010U) >> 4U);
	}

	BURGER_INLINE uint_t has_CMOV(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID1EDX & 0x00008000U) >> 15U);
	}

	BURGER_INLINE uint_t has_MMX(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID1EDX & 0x00800000U) >> 23U);
	}

	BURGER_INLINE uint_t has_SSE(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID1EDX & 0x02000000U) >> 25U);
	}

	BURGER_INLINE uint_t has_SSE2(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID1EDX & 0x04000000U) >> 26U);
	}

	BURGER_INLINE uint_t has_SSE3(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(m_uCPUID1ECX & 0x00000001U);
	}

	BURGER_INLINE uint_t has_SSSE3(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID1ECX & 0x00000200U) >> 9U);
	}

	BURGER_INLINE uint_t has_SSE4a(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID80000001ECX & 0x00000040U) >> 6U);
	}

	BURGER_INLINE uint_t has_SSE41(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID1ECX & 0x00080000U) >> 19U);
	}

	BURGER_INLINE uint_t has_SSE42(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID1ECX & 0x00100000U) >> 20U);
	}

	BURGER_INLINE uint_t has_MOVBE(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID1ECX & 0x00400000U) >> 22U);
	}

	BURGER_INLINE uint_t has_AES(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID1ECX & 0x02000000U) >> 25U);
	}

	BURGER_INLINE uint_t has_AVX(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID1ECX & 0x10000000U) >> 28U);
	}

	BURGER_INLINE uint_t has_AVX2(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID7EBX & 0x20U) >> 5U);
	}

	BURGER_INLINE uint_t has_CMPXCHG16B(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID1ECX & 0x00002000U) >> 13U);
	}

	BURGER_INLINE uint_t has_F16C(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID1ECX & 0x20000000U) >> 29U);
	}

	BURGER_INLINE uint_t has_FMA3(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID1ECX & 0x00001000U) >> 12U);
	}

	BURGER_INLINE uint_t has_FMA4(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID80000001ECX & 0x00010000U) >> 16U);
	}

	BURGER_INLINE uint_t has_LAHFSAHF(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>(m_uCPUID80000001ECX & 0x00000001U);
	}

	BURGER_INLINE uint_t has_PrefetchW(void) const BURGER_NOEXCEPT
	{
		return static_cast<uint_t>((m_uCPUID80000001ECX & 0x00000100U) >> 8U);
	}

	BURGER_INLINE uint_t has_3DNOW(void) const BURGER_NOEXCEPT
	{
		return (m_uCPUType == kCPUAMD) && (m_uCPUID80000001EDX & 0x80000000U);
	}

	BURGER_INLINE uint_t has_extended3DNOW(void) const BURGER_NOEXCEPT
	{
		return (m_uCPUType == kCPUAMD) && (m_uCPUID80000001EDX & 0x40000000U);
	}
};

#if (defined(BURGER_AMD64) || defined(BURGER_XBOX)) && !defined(DOXYGEN)
BURGER_INLINE uint_t has_CPUID(void) BURGER_NOEXCEPT
{
	return TRUE;
}
#else
extern uint_t BURGER_API has_CPUID(void) BURGER_NOEXCEPT;
#endif

extern void BURGER_API CPUID(CPUID_t* pOutput) BURGER_NOEXCEPT;
#endif

#if defined(BURGER_PS3) || defined(BURGER_XBOX360)
BURGER_INLINE uint_t has_AltiVec(void) BURGER_NOEXCEPT
{
	return TRUE;
}
BURGER_INLINE uint_t has_PPC_fsqrt(void) BURGER_NOEXCEPT
{
	return TRUE;
}

#elif (defined(BURGER_PPC) && defined(BURGER_MACOS)) || defined(DOXYGEN)
extern uint_t BURGER_API has_AltiVec(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_PPC_fsqrt(void) BURGER_NOEXCEPT;

#else
BURGER_INLINE uint_t has_AltiVec(void) BURGER_NOEXCEPT
{
	return FALSE;
}
BURGER_INLINE uint_t has_PPC_fsqrt(void) BURGER_NOEXCEPT
{
	return FALSE;
}
#endif

#if (defined(BURGER_MAC) && defined(BURGER_68K)) || defined(DOXYGEN)
extern uint_t BURGER_API has_68kFPU(void) BURGER_NOEXCEPT;
#else
BURGER_INLINE uint_t has_68kFPU(void) BURGER_NOEXCEPT
{
	return TRUE;
}
#endif

// Atomic operations

extern uint_t BURGER_API atomic_compare_and_set(
	volatile uint32_t* pOutput, uint32_t uOld, uint32_t uNew) BURGER_NOEXCEPT;
extern uint32_t BURGER_API atomic_set(
	volatile uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT;
extern uint32_t BURGER_API atomic_get(
	volatile uint32_t* pInput) BURGER_NOEXCEPT;
extern uint32_t BURGER_API atomic_add(
	volatile uint32_t* pOutput, uint32_t uInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API atomic_trylock(
	volatile uint32_t* pInput) BURGER_NOEXCEPT;
extern void BURGER_API atomic_lock(volatile uint32_t* pInput) BURGER_NOEXCEPT;
extern void BURGER_API atomic_unlock(volatile uint32_t* pInput) BURGER_NOEXCEPT;

#if defined(BURGER_64BITCPU)
extern uint_t BURGER_API atomic_compare_and_set(
	volatile uint64_t* pOutput, uint64_t uOld, uint64_t uNew) BURGER_NOEXCEPT;
extern uint64_t BURGER_API atomic_set(
	volatile uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT;
extern uint64_t BURGER_API atomic_get(
	volatile uint64_t* pInput) BURGER_NOEXCEPT;
extern uint64_t BURGER_API atomic_add(
	volatile uint64_t* pOutput, uint64_t uInput) BURGER_NOEXCEPT;
#endif
}
/* END */

#endif
