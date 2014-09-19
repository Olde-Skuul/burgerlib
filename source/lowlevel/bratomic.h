/***************************************

	Atomic memory

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

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

/* BEGIN */
namespace Burger {
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
	BURGER_INLINE Word AtomicSetIfMatch(volatile Word32 *pInput,Word32 uBefore,Word32 uAfter) { Word uTemp = pInput[0]==uBefore; if (uTemp) { pInput[0] = uAfter; } return uTemp; }
#if defined(BURGER_64BITCPU) || defined(DOXYGEN)
	BURGER_INLINE Word64 AtomicSwap(volatile Word64 *pOutput,Word64 uInput) { Word64 uTemp = pOutput[0]; pOutput[0] = uInput; return uTemp; }
	BURGER_INLINE Word64 AtomicPreIncrement(volatile Word64 *pInput) { Word64 uTemp = pInput[0]+1; pInput[0] = uTemp; return uTemp; }
	BURGER_INLINE Word64 AtomicPostIncrement(volatile Word64 *pInput) { Word64 uTemp = pInput[0]; pInput[0] = uTemp+1; return uTemp; }
	BURGER_INLINE Word64 AtomicPreDecrement(volatile Word64 *pInput) { Word64 uTemp = pInput[0]-1; pInput[0] = uTemp; return uTemp; }
	BURGER_INLINE Word64 AtomicPostDecrement(volatile Word64 *pInput) { Word64 uTemp = pInput[0]; pInput[0] = uTemp-1; return uTemp; }
	BURGER_INLINE Word64 AtomicAdd(volatile Word64 *pInput,Word64 uValue) { Word64 uTemp = pInput[0]; pInput[0] = uTemp+uValue; return uTemp; }
	BURGER_INLINE Word64 AtomicSubtract(volatile Word64 *pInput,Word64 uValue) { Word64 uTemp = pInput[0]; pInput[0] = uTemp-uValue; return uTemp; }
	BURGER_INLINE Word AtomicSetIfMatch(volatile Word64 *pInput,Word64 uBefore,Word64 uAfter) { Word uTemp = pInput[0]==uBefore; if (uTemp) { pInput[0] = uAfter; } return uTemp; }
#endif
#endif
}
/* END */

#endif
