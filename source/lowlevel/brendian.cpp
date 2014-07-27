/***************************************

	Handle endian swapping

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "brendian.h"

/*! ************************************

	\class Burger::BigEndian
	\brief Loads a 16, 32 or 64 bit value with byte swapping if needed.
	
	This class will map to either to \ref Burger::NativeEndian or \ref Burger::SwapEndian
	depending on if this is a big endian machine or not. Use of this class will allow the programmer
	to write code that is endian neutral since the compiler will perform the proper
	mapping depending on the target's settings..
	
	Big endian is considered true if the \ref Word32 value 0x12345678 is stored in memory as
	0x12, 0x34, 0x56, 0x78.
	
	Examples of use:
	\code
	Word32 LoadedInt;

	// Load 4 bytes from a file
	fread(fp,1,4,&LoadedInt);

	// Fetch the big endian data
	Word32 foo = Burger::BigEndian::Load(&LoadedInt);
	\endcode
	
	\note The documentation will describe the behavior of \ref Burger::SwapEndian, be
	warned that it will map to \ref Burger::NativeEndian on a big endian machine.
	
	\sa Burger::NativeEndian, Burger::LittleEndian and Burger::SwapEndian

***************************************/


/*! ************************************

	\class Burger::LittleEndian
	\brief Loads a 16, 32 or 64 bit value with byte swapping if needed.
	
	This class will map to either to \ref Burger::NativeEndian or \ref Burger::SwapEndian
	depending on if this is a little endian machine or not. Use of this class will allow the programmer
	to write code that is endian neutral since the compiler will perform the proper
	mapping depending on the target's settings..
	
	Little endian is considered true if the \ref Word32 value 0x12345678 is stored in memory as
	0x78, 0x56, 0x34, 0x12.
	
	Examples of use:
	\code
	Word32 LoadedInt;

	// Load 4 bytes from a file
	fread(fp,1,4,&LoadedInt);

	// Fetch the little endian data
	Word32 foo = Burger::LittleEndian::Load(&LoadedInt);
	\endcode
	
	\note The documentation will describe the behavior of Burger::NativeEndian, be
	warned that it will map to Burger::SwapEndian on a big endian machine.
	
	\sa Burger::NativeEndian, Burger::BigEndian and Burger::SwapEndian

***************************************/


/*! ************************************

	\class Burger::NativeEndian
	\brief Loads a 16, 32 or 64 bit value with no byte swapping.
	
	The classes Burger::LittleEndian and Burger::BigEndian either map
	to \ref Burger::NativeEndian or Burger::SwapEndian. If the machine's
	endian matches the class, then it maps to this class.
	
	This class does nothing for most functions by design. It
	is meant to vanish when the program is asking for no endian swapping
	since the data being read is the same endian as the machine.
	
	The only functions that do not disappear are the LoadAny(??? *) group
	of calls since they have the ability to fetch a 16, 32 or 64 bit value irregardless
	of the alignment of the data pointer. These are useful in grabbing data
	from a byte stream and won't trigger an alignment access fault.
	
	Under most circumstances, you will not call this class directly.
	
	\sa Burger::SwapEndian, Burger::LittleEndian and Burger::BigEndian

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(Word16 uInput);
	\brief Fetch a 16 bit value.
	
	Pass a 16 bit value through with no change. This function is usually
	optimized into oblivion.
	
	\param uInput Unsigned 16 bit input.
	\return \a uInput untouched.
	
	\sa Burger::SwapEndian::Load(Word16)

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(Word32 uInput);
	\brief Fetch a 32 bit value.
	
	Pass a 32 bit value through with no change. This function is usually
	optimized into oblivion.
	
	\param uInput Unsigned 32 bit input.
	\return \a uInput untouched.
	
	\sa Burger::SwapEndian::Load(Word32)

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(Word64 uInput);
	\brief Fetch a 64 bit value.
	
	Pass a 64 bit value through with no change. This function is usually
	optimized into oblivion.
	
	\param uInput Unsigned 64 bit input.
	\return \a uInput untouched.
	
	\sa Burger::SwapEndian::Load(Word64)

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(float fInput);
	\brief Fetch a float value.
	
	Pass a float value through with no change. This function is usually
	optimized into oblivion.
	
	\param fInput A float input.
	\return \a fInput untouched.
	
	\sa Burger::SwapEndian::Load(float)

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(double dInput);
	\brief Fetch a double value.
	
	Pass a double value through with no change. This function is usually
	optimized into oblivion.
	
	\param dInput A double input.
	\return \a dInput untouched.
	
	\sa Burger::SwapEndian::Load(double)

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(const Word16 *pInput);
	\brief Fetch a 16 bit value from memory.
	
	This function is usually optimized into a simple load operation.
	It assumes that the data is 16 bit aligned.
	
	\param pInput Pointer to an unsigned 16 bit value.
	\return The 16 bit unsigned value.
	
	\sa Burger::SwapEndian::Load(const Word16*) or
		Burger::NativeEndian::LoadAny(const Word16 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(const Word32 *pInput);
	\brief Fetch a 32 bit value from memory.
	
	This function is usually optimized into a simple load operation.
	It assumes that the data is 32 bit aligned.
	
	\param pInput Pointer to an unsigned 32 bit value.
	\return The 32 bit unsigned value.
	
	\sa Burger::SwapEndian::Load(const Word32*) or
		Burger::NativeEndian::LoadAny(const Word32 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(const Word64 *pInput);
	\brief Fetch a 64 bit value from memory.
	
	This function is usually optimized into a simple load operation.
	It assumes that the data is 64 bit aligned.
	
	\param pInput Pointer to an unsigned 64 bit value.
	\return The 64 bit unsigned value.
	
	\sa Burger::SwapEndian::Load(const Word64*) or
		Burger::NativeEndian::LoadAny(const Word64 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(const float *pInput);
	\brief Fetch a float value from memory.
	
	This function is usually optimized into a simple load operation.
	It assumes that the data is 32 bit aligned.
	
	\param pInput Pointer to a float value.
	\return The float value.
	
	\sa Burger::SwapEndian::Load(const float*) or
		Burger::NativeEndian::LoadAny(const float *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(const double *pInput);
	\brief Fetch a double value from memory.
	
	This function is usually optimized into a simple load operation.
	It assumes that the data is 64 bit aligned.
	
	\param pInput Pointer to a double value.
	\return The double value.
	
	\sa Burger::SwapEndian::Load(const double*) or
		Burger::NativeEndian::LoadAny(const double *).

***************************************/

/*! ************************************

	\brief Fetch a 16 bit unsigned value from memory with byte alignment.
	
	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a 16 bit value in native endian.
		
	\param pInput Pointer to a 16 bit value.
	\return The 16 bit unsigned value.
	
	\sa Burger::SwapEndian::Load(const Word16*) or
		Burger::NativeEndian::Load(const Word16 *).

***************************************/

#if !defined(__WATCOMC__)		// Inlined

#if !defined(BURGER_X86) || (defined(BURGER_MACOSX) && defined(BURGER_X86)) || defined(DOXYGEN)
Word16 BURGER_API Burger::NativeEndian::LoadAny(const Word16 *pInput)
{
	Word Low = reinterpret_cast<const Word8*>(pInput)[0];		// Grab 16 bits
	Word High = reinterpret_cast<const Word8*>(pInput)[1];
#if defined(BURGER_BIGENDIAN)
	Word16 Result = static_cast<Word16>((Low<<8) | High);	// PPC likes |
#else
	Word16 Result = static_cast<Word16>(Low + (High<<8));	// Intel likes +
#endif
	return Result;
}
#else
BURGER_DECLSPECNAKED Word16 BURGER_API Burger::NativeEndian::LoadAny(const Word16 * /* pInput */)
{
	BURGER_ASM {
#if !defined(BURGER_FASTCALLENABLED)
		mov		ecx,[esp+4]			// Pointer to the input
#endif
		mov		al,byte ptr[ecx]	// Grab the short
		mov		ah,byte ptr[ecx+1]
		ret
	}
}
#endif
#endif

/*! ************************************

	\brief Fetch a 32 bit unsigned value from memory with byte alignment.
	
	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a 32 bit value in native endian.
		
	\param pInput Pointer to a 32 bit value.
	\return The 32 bit unsigned value.
	
	\sa Burger::SwapEndian::Load(const Word32*) or
		Burger::NativeEndian::Load(const Word32 *).

***************************************/

#if !defined(__WATCOMC__)		// Inlined

#if !defined(BURGER_X86) || (defined(BURGER_MACOSX) && defined(BURGER_X86)) || defined(DOXYGEN)
Word32 BURGER_API Burger::NativeEndian::LoadAny(const Word32 *pInput)
{
	Word Low1 = reinterpret_cast<const Word8*>(pInput)[0];		// Grab 32 bits
	Word Low2 = reinterpret_cast<const Word8*>(pInput)[1];
	Word Low3 = reinterpret_cast<const Word8*>(pInput)[2];
	Word Low4 = reinterpret_cast<const Word8*>(pInput)[3];
#if defined(BURGER_BIGENDIAN)
	Word32 Result = static_cast<Word32>((Low1<<24) | (Low2<<16) |
		(Low3<<8) | Low4);	// PPC likes |
#else
	Word32 Result = static_cast<Word32>(Low1 + (Low2<<8) +
		(Low3<<16) + (Low4<<24));	// Intel likes +
#endif
	return Result;
}
#else
BURGER_DECLSPECNAKED Word32 BURGER_API Burger::NativeEndian::LoadAny(const Word32 * /* pInput */)
{
	BURGER_ASM {
#if !defined(BURGER_FASTCALLENABLED)
		mov		ecx,[esp+4]			// Pointer to the input
#endif
		mov		al,byte ptr[ecx]	// Grab the long in a virtual memory friendly way
		mov		ah,byte ptr[ecx+1]
		bswap	eax
		mov		ah,byte ptr[ecx+2]
		mov 	al,byte ptr[ecx+3]
		bswap	eax
		ret
	}
}
#endif
#endif

/*! ************************************

	\brief Fetch a 64 bit unsigned value from memory with byte alignment.
	
	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a 64 bit value in native endian.
		
	\param pInput Pointer to a 64 bit value.
	\return The 64 bit unsigned value.
	
	\sa Burger::SwapEndian::Load(const Word64*) or
		Burger::NativeEndian::Load(const Word64 *).

***************************************/

#if defined(__WATCOMC__)
// WatcomC passes the parm in EAX

BURGER_DECLSPECNAKED Word64 BURGER_API Burger::NativeEndian::LoadAny(const Word64 * /* pInput */)
{
	// Intel returns 64 bit values in EAX:EDX
	__asm {
	push	ecx
	mov		cl,byte ptr[eax]	// Grab the long in a virtual memory friendly way
	mov		ch,byte ptr[eax+1]
	bswap	ecx
	mov		ch,byte ptr[eax+2]
	mov		cl,byte ptr[eax+3]
	bswap	ecx
	mov		dl,byte ptr[eax+4]	// Grab the long in a virtual memory friendly way
	mov		dh,byte ptr[eax+5]
	bswap	edx
	mov		dh,byte ptr[eax+6]
	mov		dl,byte ptr[eax+7]
	bswap	edx
	mov		eax,ecx
	pop		ecx
	ret
	}
}

#else
#if !defined(BURGER_X86) || (defined(BURGER_MACOSX) && defined(BURGER_X86)) || defined(DOXYGEN)
Word64 BURGER_API Burger::NativeEndian::LoadAny(const Word64 *pInput)
{
	Word64 Result;		// Make sure it's aligned
	
	reinterpret_cast<Word8*>(&Result)[0] = reinterpret_cast<const Word8*>(pInput)[0];
	reinterpret_cast<Word8*>(&Result)[1] = reinterpret_cast<const Word8*>(pInput)[1];
	reinterpret_cast<Word8*>(&Result)[2] = reinterpret_cast<const Word8*>(pInput)[2];
	reinterpret_cast<Word8*>(&Result)[3] = reinterpret_cast<const Word8*>(pInput)[3];
	reinterpret_cast<Word8*>(&Result)[4] = reinterpret_cast<const Word8*>(pInput)[4];
	reinterpret_cast<Word8*>(&Result)[5] = reinterpret_cast<const Word8*>(pInput)[5];
	reinterpret_cast<Word8*>(&Result)[6] = reinterpret_cast<const Word8*>(pInput)[6];
	reinterpret_cast<Word8*>(&Result)[7] = reinterpret_cast<const Word8*>(pInput)[7];
	return Result;			// Return the 64 bit value in a register
}
#else
BURGER_DECLSPECNAKED Word64 BURGER_API Burger::NativeEndian::LoadAny(const Word64 * /* pInput */)
{
	// Intel returns 64 bit values in EAX:EDX
	BURGER_ASM {
#if !defined(BURGER_FASTCALLENABLED)
		mov		ecx,[esp+4]			// Pointer to the input
#endif
		mov		al,byte ptr[ecx]	// Grab the long in a virtual memory friendly way
		mov		ah,byte ptr[ecx+1]
		bswap	eax
		mov		ah,byte ptr[ecx+2]
		mov		al,byte ptr[ecx+3]
		bswap	eax
		mov		dl,byte ptr[ecx+4]	// Grab the long in a virtual memory friendly way
		mov		dh,byte ptr[ecx+5]
		bswap	edx
		mov		dh,byte ptr[ecx+6]
		mov		dl,byte ptr[ecx+7]
		bswap	edx
		ret
	}
}
#endif
#endif

/*! ************************************

	\brief Fetch a float value from memory with byte alignment.
	
	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a float value in native endian.
		
	\param pInput Pointer to a float value.
	\return The float value.
	
	\sa Burger::SwapEndian::Load(const float*) or
		Burger::NativeEndian::Load(const float *).

***************************************/

#if defined(__WATCOMC__)
// WatcomC passes the parm in EAX

BURGER_DECLSPECNAKED float BURGER_API Burger::NativeEndian::LoadAny(const float * /* pInput */)
{
	__asm {
	mov		dl,byte ptr[eax]	// Grab the long in a virtual memory friendly way
	mov		dh,byte ptr[eax+1]
	bswap	edx
	mov		dh,byte ptr[eax+2]
	mov		dl,byte ptr[eax+3]
	bswap	edx
	mov		eax,[esp]			// Get the return address in the dead cycle
	mov		dword ptr[esp],edx	// Store in memory
	fld		dword ptr[esp]		// Get the float
	add		esp,4				// Restore the stack (And the return)
	jmp		eax					// Faster than RET
	}
}

#elif defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && !defined(DOXYGEN)
BURGER_DECLSPECNAKED float __fastcall Burger::NativeEndian::LoadAny(const float * /* pInput */)
{
	BURGER_ASM {
		mov		al,byte ptr[ecx]	// Grab the long in a virtual memory friendly way
		mov		ah,byte ptr[ecx+1]
		mov		dl,byte ptr[ecx+2]
		mov		dh,byte ptr[ecx+3]
		mov		ecx,[esp]			// Get the return address in the dead cycle
		mov		[esp],ax			// Store in memory
		mov		[esp+2],dx
		fld		dword ptr[esp]		// Get the float
		add		esp,4				// Restore the stack (And the return)
		jmp		ecx					// Faster than RET
	}
}

#else 
float BURGER_API Burger::NativeEndian::LoadAny(const float *pInput)
{
	float fResult;
	
	reinterpret_cast<Word8*>(&fResult)[0] = reinterpret_cast<const Word8*>(pInput)[0];
	reinterpret_cast<Word8*>(&fResult)[1] = reinterpret_cast<const Word8*>(pInput)[1];
	reinterpret_cast<Word8*>(&fResult)[2] = reinterpret_cast<const Word8*>(pInput)[2];
	reinterpret_cast<Word8*>(&fResult)[3] = reinterpret_cast<const Word8*>(pInput)[3];
	return fResult;	
}

#endif

/*! ************************************

	\brief Fetch a double value from memory with byte alignment.
	
	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a double value in native endian.
		
	\param pInput Pointer to a double value.
	\return The double value.
	
	\sa Burger::SwapEndian::Load(const double*) or
		Burger::NativeEndian::Load(const double *).

***************************************/

#if defined(__WATCOMC__)
// WatcomC passes the parm in EAX

BURGER_DECLSPECNAKED double BURGER_API Burger::NativeEndian::LoadAny(const double * /* pInput */)
{
	// Intel returns 64 bit values in EAX:EDX
	__asm {
	push	ecx
	push	ebp
	mov		ebp,esp
	sub		esp,12				// Make space on the stack (Intel is okay for 4 byte alignment)
	and		esp,0xfffffff8
	mov		cl,byte ptr[eax]	// Grab the long in a virtual memory friendly way
	mov		ch,byte ptr[eax+1]
	mov		dl,byte ptr[eax+2]
	mov		dh,byte ptr[eax+3]		
	mov		[esp],cx			// Store in memory
	mov		[esp+2],dx
	mov		cl,byte ptr[eax+4]	// Grab the long in a virtual memory friendly way
	mov		ch,byte ptr[eax+5]
	mov		dl,byte ptr[eax+6]
	mov		dh,byte ptr[eax+7]		
	mov		[esp+4],cx			// Store in memory
	mov		[esp+6],dx
	fld		qword ptr[esp]		// Get the double
	mov		esp,ebp				// Restore the stack (And the return)
	pop		ebp
	pop		ecx
	ret
	}
}

#elif !defined(BURGER_X86) || (defined(BURGER_MACOSX) && defined(BURGER_X86)) || defined(DOXYGEN)
double BURGER_API Burger::NativeEndian::LoadAny(const double *pInput)
{
	double dResult;		// Make sure it's aligned
	
	reinterpret_cast<Word8*>(&dResult)[0] = reinterpret_cast<const Word8*>(pInput)[0];
	reinterpret_cast<Word8*>(&dResult)[1] = reinterpret_cast<const Word8*>(pInput)[1];
	reinterpret_cast<Word8*>(&dResult)[2] = reinterpret_cast<const Word8*>(pInput)[2];
	reinterpret_cast<Word8*>(&dResult)[3] = reinterpret_cast<const Word8*>(pInput)[3];
	reinterpret_cast<Word8*>(&dResult)[4] = reinterpret_cast<const Word8*>(pInput)[4];
	reinterpret_cast<Word8*>(&dResult)[5] = reinterpret_cast<const Word8*>(pInput)[5];
	reinterpret_cast<Word8*>(&dResult)[6] = reinterpret_cast<const Word8*>(pInput)[6];
	reinterpret_cast<Word8*>(&dResult)[7] = reinterpret_cast<const Word8*>(pInput)[7];
	return dResult;			// Return the 64 bit value in a register
}
#else
BURGER_DECLSPECNAKED double BURGER_API Burger::NativeEndian::LoadAny(const double * /* pInput */)
{
	// Intel returns 64 bit values in EAX:EDX
	BURGER_ASM {
#if !defined(BURGER_FASTCALLENABLED)
	mov		ecx,[esp+4]			// Pointer to the input
#endif
	sub		esp,8				// Make space on the stack (Intel is okay for 4 byte alignment)
	mov		al,byte ptr[ecx]	// Grab the long in a virtual memory friendly way
	mov		ah,byte ptr[ecx+1]
	mov		dl,byte ptr[ecx+2]
	mov		dh,byte ptr[ecx+3]		
	mov		[esp],ax			// Store in memory
	mov		[esp+2],dx
	mov		al,byte ptr[ecx+4]	// Grab the long in a virtual memory friendly way
	mov		ah,byte ptr[ecx+5]
	mov		dl,byte ptr[ecx+6]
	mov		dh,byte ptr[ecx+7]		
	mov		[esp+4],ax			// Store in memory
	mov		[esp+6],dx
	mov		eax,[esp+8]			// Get the return address in the dead cycle
	fld		qword ptr[esp]		// Get the double
	add		esp,12				// Restore the stack (And the return)
	jmp		eax					// Faster than RET
	}
}
#endif

/*! ************************************

	\fn Burger::NativeEndian::Fixup(Word16 *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to a 16 bit value.
	
	\sa Burger::SwapEndian::Fixup(Word16*) or
		Burger::NativeEndian::FixupAny(Word16 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Fixup(Word32 *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to a 32 bit value.
	
	\sa Burger::SwapEndian::Fixup(Word32*) or
		Burger::NativeEndian::FixupAny(Word32 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Fixup(Word64 *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to a 64 bit value.
	
	\sa Burger::SwapEndian::Fixup(Word64*) or
		Burger::NativeEndian::FixupAny(Word64 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::FixupAny(Word16 *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to a 16 bit value.
	
	\sa Burger::SwapEndian::FixupAny(Word16*) or
		Burger::NativeEndian::Fixup(Word16 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::FixupAny(Word32 *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to a 32 bit value.
	
	\sa Burger::SwapEndian::FixupAny(Word32*) or
		Burger::NativeEndian::Fixup(Word32 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::FixupAny(Word64 *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to a 64 bit value.
	
	\sa Burger::SwapEndian::FixupAny(Word64*) or
		Burger::NativeEndian::Fixup(Word64 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(Int16 iInput);
	\brief Fetch a 16 bit value.
	
	Pass a 16 bit value through with no change. This function is usually
	optimized into oblivion.
	
	\param iInput Signed 16 bit input.
	\return \a iInput untouched.
	
	\sa Burger::SwapEndian::Load(Int16)

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(Int32 iInput);
	\brief Fetch a 32 bit value.
	
	Pass a 32 bit value through with no change. This function is usually
	optimized into oblivion.
	
	\param iInput Signed 32 bit input.
	\return \a iInput untouched.
	
	\sa Burger::SwapEndian::Load(Int32)

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(Int64 iInput);
	\brief Fetch a 64 bit value.
	
	Pass a 64 bit value through with no change. This function is usually
	optimized into oblivion.
	
	\param iInput Signed 64 bit input.
	\return \a iInput untouched.
	
	\sa Burger::SwapEndian::Load(Int64)

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(Word uInput);
	\brief Fetch an integer value.
	
	Pass an integer value through with no change. This function is usually
	optimized into oblivion.
	
	\param uInput Unsigned integer input.
	\return \a uInput untouched.
	
	\sa Burger::SwapEndian::Load(Word)

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(Int iInput);
	\brief Fetch a signed integer value.
	
	Pass a signed integer value through with no change. This function is usually
	optimized into oblivion.
	
	\param iInput Signed integer input.
	\return \a iInput untouched.
	
	\sa Burger::SwapEndian::Load(Int)

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(const Int16 *pInput);
	\brief Fetch a 16 bit value from memory.
	
	This function is usually optimized into a simple load operation.
	It assumes that the data is 16 bit aligned.
	
	\param pInput Pointer to an signed 16 bit value.
	\return The 16 bit signed value.
	
	\sa Burger::SwapEndian::Load(const Int16*) or
		Burger::NativeEndian::LoadAny(const Int16 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(const Int32 *pInput);
	\brief Fetch a 32 bit value from memory.
	
	This function is usually optimized into a simple load operation.
	It assumes that the data is 32 bit aligned.
	
	\param pInput Pointer to an signed 32 bit value.
	\return The 32 bit signed value.
	
	\sa Burger::SwapEndian::Load(const Int32*) or
		Burger::NativeEndian::LoadAny(const Int32 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(const Int64 *pInput);
	\brief Fetch a 64 bit value from memory.
	
	This function is usually optimized into a simple load operation.
	It assumes that the data is 64 bit aligned.
	
	\param pInput Pointer to an signed 64 bit value.
	\return The 64 bit signed value.
	
	\sa Burger::SwapEndian::Load(const Int64*) or
		Burger::NativeEndian::LoadAny(const Int64 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(const Word *pInput);
	\brief Fetch an unsigned integer value from memory.
	
	This function is usually optimized into a simple load operation.
	It assumes that the data is aligned to the size of a native integer (Usually 32 bits).
	
	\param pInput Pointer to an unsigned integer value.
	\return The unsigned integer value.
	
	\sa Burger::SwapEndian::Load(const Word*) or
		Burger::NativeEndian::LoadAny(const Word *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Load(const Int *pInput);
	\brief Fetch a signed integer value from memory.
	
	This function is usually optimized into a simple load operation.
	It assumes that the data is aligned to the size of a native integer (Usually 32 bits).
	
	\param pInput Pointer to a signed integer value.
	\return The signed integer value.
	
	\sa Burger::SwapEndian::Load(const Int*) or
		Burger::NativeEndian::LoadAny(const Int *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::LoadAny(const Int16 *pInput)
	\brief Fetch a 16 bit signed value from memory with byte alignment.
	
	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a 16 bit value in native endian.
		
	\param pInput Pointer to a 16 bit value.
	\return The 16 bit signed value.
	
	\sa Burger::SwapEndian::Load(const Int16*) or
		Burger::NativeEndian::Load(const Int16 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::LoadAny(const Int32 *pInput)
	\brief Fetch a 32 bit signed value from memory with byte alignment.
	
	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a 32 bit value in native endian.
		
	\param pInput Pointer to a 32 bit value.
	\return The 32 bit signed value.
	
	\sa Burger::SwapEndian::Load(const Int32*) or
		Burger::NativeEndian::Load(const Int32 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::LoadAny(const Int64 *pInput)
	\brief Fetch a 64 bit signed value from memory with byte alignment.
	
	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a 64 bit value in native endian.
		
	\param pInput Pointer to a 64 bit value.
	\return The 64 bit signed value.
	
	\sa Burger::SwapEndian::Load(const Int64*) or
		Burger::NativeEndian::Load(const Int64 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::LoadAny(const Word *pInput)
	\brief Fetch an unsigned integer value from memory with byte alignment.
	
	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into an unsigned integer value in native endian.
		
	\param pInput Pointer to an integer value.
	\return The unsigned integer value.
	
	\sa Burger::SwapEndian::Load(const Word*) or
		Burger::NativeEndian::Load(const Word *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::LoadAny(const Int *pInput)
	\brief Fetch an signed integer value from memory with byte alignment.
	
	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into an signed integer value in native endian.
		
	\param pInput Pointer to an integer value.
	\return The signed integer value.
	
	\sa Burger::SwapEndian::Load(const Int*) or
		Burger::NativeEndian::Load(const Int *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Fixup(Int16 *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to a 16 bit value.
	
	\sa Burger::SwapEndian::Fixup(Int16*) or
		Burger::NativeEndian::FixupAny( Int16 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Fixup(Int32 *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to a 32 bit value.
	
	\sa Burger::SwapEndian::Fixup(Int32*) or
		Burger::NativeEndian::FixupAny( Int32 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Fixup(Int64 *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to a 64 bit value.
	
	\sa Burger::SwapEndian::Fixup(Int64*) or
		Burger::NativeEndian::FixupAny( Int64 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Fixup(Word *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to an unsigned integer value.
	
	\sa Burger::SwapEndian::Fixup(Word*) or
		Burger::NativeEndian::FixupAny(Word *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Fixup(Int *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to an integer value.
	
	\sa Burger::SwapEndian::Fixup(Int*) or
		Burger::NativeEndian::FixupAny(Int *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Fixup(float *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to a float value.
	
	\sa Burger::SwapEndian::Fixup(float*) or
		Burger::NativeEndian::FixupAny(float *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::Fixup(double *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to a double value.
	
	\sa Burger::SwapEndian::Fixup(double*) or
		Burger::NativeEndian::FixupAny(double *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::FixupAny(Int16 *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to a 16 bit value.
	
	\sa Burger::SwapEndian::FixupAny(Int16*) or
		Burger::NativeEndian::Fixup(Int16 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::FixupAny(Int32 *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to a 32 bit value.
	
	\sa Burger::SwapEndian::FixupAny(Int32*) or
		Burger::NativeEndian::Fixup( Int32 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::FixupAny(Int64 *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to a 64 bit value.
	
	\sa Burger::SwapEndian::FixupAny(Int64*) or
		Burger::NativeEndian::Fixup( Int64 *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::FixupAny(Word *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to an unsigned integer value.
	
	\sa Burger::SwapEndian::FixupAny(Word*) or
		Burger::NativeEndian::Fixup(Word *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::FixupAny(Int *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to an integer value.
	
	\sa Burger::SwapEndian::FixupAny(Int*) or
		Burger::NativeEndian::Fixup(Int *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::FixupAny(float *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to a float value.
	
	\sa Burger::SwapEndian::FixupAny(float*) or
		Burger::NativeEndian::Fixup(float *).

***************************************/

/*! ************************************

	\fn Burger::NativeEndian::FixupAny(double *pInput)
	\brief Does nothing.
	
	The Burger::SwapEndian class would swap the endian of the
	variable, but this class performs no operation since the
	endian is already a match for what the machine expects.
			
	\param pInput Pointer to a double value.
	
	\sa Burger::SwapEndian::FixupAny(double*) or
		Burger::NativeEndian::Fixup(double *).

***************************************/






/*! ************************************

	\class Burger::SwapEndian
	\brief Swap the byte order of 16, 32 and 64 bit values.
	
	When reading data from machines that are using different
	microprocessors, the byte order could be reversed. These
	functions will swap the byte order.
	
	\note These functions will always swap the bytes, use Burger::LittleEndian
	or Burger::BigEndian to let the compiler determine whether byte
	swapping is needed.
	
	\sa Burger::NativeEndian, Burger::LittleEndian and Burger::BigEndian

***************************************/

/*! ************************************

	\brief Reverse the endian of a 16 bit integer

	Given a 16 bit value in an integer register, swap the bytes
	so that 0x1234 becomes 0x3412

	\param uInput Value to endian convert
	\return The input with the bytes swapped, no errors are possible.

	\sa Burger::NativeEndian::Load(Word16), Burger::SwapEndian::Load(Int16) or Burger::SwapEndian::Load(const Word16 *)

***************************************/

#if defined(__WATCOMC__) || (defined(__MWERKS__) && defined(BURGER_68K))
#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
#elif defined(BURGER_POWERPC) && defined(__MWERKS__)
#elif defined(BURGER_X86) && defined(__MWERKS__)
#elif defined(__ICL)
#elif (defined(BURGER_X86) || defined(BURGER_AMD64)) && (_MSC_VER)

#elif defined(BURGER_ARM) && defined(__MWERKS__)

asm Word16 BURGER_API Burger::SwapEndian::Load(Word16 /* uInput */)
{
	and	r1,r0,#0xFF			// Mask the low byte
	mov	r0,r0,lsr #8		// Shift upper to lower
	and r0,r0,#0xFF			// Mask to a byte
	orr	r0,r0,r1,lsl #8		// The masking keeps it 16 bit clean
	bx	lr
}
#else

Word16 BURGER_API Burger::SwapEndian::Load(Word16 uInput)
{
	Word uTemp;
	uTemp = (uInput>>8)&0xFFU;
	uTemp |= (uInput<<8)&0xFF00;
	return static_cast<Word16>(uTemp);
}

#endif

/*! ************************************

	\brief Reverse the endian of a 32 bit integer

	Given a 32 bit value in an integer register, swap the bytes
	so that 0x12345678 becomes 0x78563412

	\param uInput Value to endian convert
	\return The input with the bytes swapped, no errors are possible.

	\sa Burger::NativeEndian::Load(Word32), Burger::SwapEndian::Load(Int32) or Burger::SwapEndian::Load(const Word32 *)

***************************************/

#if defined(__WATCOMC__) || (defined(__MWERKS__) && defined(BURGER_68K))
#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
#elif defined(BURGER_POWERPC) && defined(__MWERKS__)
#elif defined(BURGER_X86) && defined(__MWERKS__)
#elif defined(__ICL)
#elif (defined(BURGER_X86) || defined(BURGER_AMD64)) && (_MSC_VER)
#elif defined(BURGER_ARM) && defined(__MWERKS__)

asm Word32 BURGER_API Burger::SwapEndian::Load(Word32 uInput)
{
	eor	r1,r0,r0,ror #16	// r1 = 12^56,34^78,12^56,34^78
	bic	r1,r1,#0x00ff0000	// r1 = 12^56,0,12^56,34^78
	mov	r0,r0,ror #8		// r0 = 78,12,34,56
	eor	r0,r0,r1,lsr #8		// r0 = 78^0,12^12^56,34^0,56^12^56 (78563412)
	bx	lr
}
#else

Word32 BURGER_API Burger::SwapEndian::Load(Word32 uInput)
{
	Word32 uTemp;
	uTemp = (uInput>>24)&0xFF;
	uTemp |= (uInput>>8)&0xFF00;
	uTemp |= (uInput<<8)&0xFF0000;
	uTemp |= uInput<<24;
	return uTemp;
}

#endif

/*! ************************************

	\brief Reverse the endian of a 16 bit integer

	Given a pointer to a 16 bit value, load it and swap the bytes
	so that 0x1234 becomes 0x3412

	\param pInput Pointer to a value to endian convert
	\return The value with the bytes swapped.
	
	\note \ref NULL pointers are illegal and will page fault.

	\sa Burger::NativeEndian::Load(Word16), Burger::SwapEndian::Load(Int16) or Burger::SwapEndian::Load(Word16)

***************************************/

#if defined(__WATCOMC__) || (defined(__MWERKS__) && defined(BURGER_68K))
#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
#elif defined(BURGER_POWERPC) && defined(__MWERKS__)
#elif defined(BURGER_X86) && defined(__MWERKS__)
#elif defined(__ICL)
#elif (defined(BURGER_X86) || defined(BURGER_AMD64)) && (_MSC_VER)
#else

Word16 BURGER_API Burger::SwapEndian::Load(const Word16 *pInput)
{
	Word uTemp;
	Word uInput;
	uInput = pInput[0];
	uTemp = (uInput>>8)&0xFF;
	uTemp |= (uInput<<8)&0xFF00;
	return static_cast<Word16>(uTemp);
}

#endif

/*! ************************************

	\brief Reverse the endian of a 32 bit integer

	Given a pointer to a 32 bit value, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412

	\param pInput Pointer to value to endian convert
	\return The value with the bytes swapped.

	\note \ref NULL pointers are illegal and will page fault.

	\sa Burger::NativeEndian::Load(Word32), Burger::SwapEndian::Load(Int32) or Burger::SwapEndian::Load(Word32)

***************************************/

#if defined(__WATCOMC__) || (defined(__MWERKS__) && defined(BURGER_68K))
#elif defined(BURGER_XBOX360) && !defined(DOXYGEN)
#elif defined(BURGER_POWERPC) && defined(__MWERKS__)
#elif defined(BURGER_X86) && defined(__MWERKS__)
#elif defined(__ICL)
#elif (defined(BURGER_X86) || defined(BURGER_AMD64)) && (_MSC_VER)
#else

Word32 BURGER_API Burger::SwapEndian::Load(const Word32 *pInput)
{
	Word32 uInput;		
	Word32 uTemp;
	uInput = pInput[0];
	uTemp = (uInput>>24)&0xFF;
	uTemp |= (uInput>>8)&0xFF00;
	uTemp |= (uInput<<8)&0xFF0000;
	uTemp |= uInput<<24;
	return uTemp;
}

#endif

/*! ************************************

	\brief Reverse the endian of a 64 bit integer

	Given a 64 bit value, swap the bytes
	so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412

	\param uInput Value to endian convert
	\return The value with the bytes swapped, no errors are possible.

	\sa Burger::NativeEndian::Load(Word64), Burger::SwapEndian::Load(Int64) or Burger::SwapEndian::Load(const Word64 *)

***************************************/

#if (defined(BURGER_XBOX360) || ((defined(BURGER_X86) || defined(BURGER_AMD64)) && defined(_MSC_VER))) && !defined(DOXYGEN)
#elif defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED Word64 __fastcall Burger::SwapEndian::Load(Word64 /* uInput */)
{
	BURGER_ASM {
	mov edx,dword ptr [esp+4]		// Fetch the 64 bit integer
	mov eax,dword ptr [esp+8]
	mov ecx,dword ptr [esp]			// Get the return address
	add esp,12						// Fix the stack
	bswap eax						// Swap the lower 32 bits
	bswap edx						// Swap the upper 32 bits
	jmp ecx							// Exit
	}
}

#elif defined(BURGER_POWERPC) && defined(__MWERKS__)

asm Word64 BURGER_API Burger::SwapEndian::Load(Word64 /* uInput */)
{
	stw		r3,24(SP)	// Store the 64 bit value on the stack
	addi	r5,sp,24	// Index to the stack addresses
	stw		r4,28(SP)
	addi	r6,sp,28
	lwbrx	r4,r0,r5	// Load the values endian reversed
	lwbrx	r3,r0,r6
	blr
}
#else
Word64 BURGER_API Burger::SwapEndian::Load(Word64 uInput)
{
	Word8 bTemp1;
	Word8 bTemp2;
	Word64 uTemp;	// This MUST be cast as a Word64 to be Word64 aligned!

	bTemp1 = reinterpret_cast<const Word8 *>(&uInput)[0];
	bTemp2 = reinterpret_cast<const Word8 *>(&uInput)[1];
	reinterpret_cast<Word8 *>(&uTemp)[7] = bTemp1;	/* Perform the swap */
	reinterpret_cast<Word8 *>(&uTemp)[6] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(&uInput)[2];
	bTemp2 = reinterpret_cast<const Word8 *>(&uInput)[3];
	reinterpret_cast<Word8 *>(&uTemp)[5] = bTemp1;
	reinterpret_cast<Word8 *>(&uTemp)[4] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(&uInput)[4];
	bTemp2 = reinterpret_cast<const Word8 *>(&uInput)[5];
	reinterpret_cast<Word8 *>(&uTemp)[3] = bTemp1;
	reinterpret_cast<Word8 *>(&uTemp)[2] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(&uInput)[6];
	bTemp2 = reinterpret_cast<const Word8 *>(&uInput)[7];
	reinterpret_cast<Word8 *>(&uTemp)[1] = bTemp1;
	reinterpret_cast<Word8 *>(&uTemp)[0] = bTemp2;
	return uTemp;					// Return the Word64
}
#endif

/*! ************************************

	\brief Reverse the endian of a 32-bit float

	Given a 32-bit floating point value, swap the bytes
	so that 0x12345678 becomes 0x78563412

	\param fInput Value to endian convert
	\return The value with the bytes swapped, no errors are possible.
	\note Use of this form is discouraged since it involves moving a floating point value
	into an integer register and back again. Use the const float * version instead.

	\sa Burger::NativeEndian::Load(float) or Burger::SwapEndian::Load(const float *)

***************************************/

#if defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED float __fastcall Burger::SwapEndian::Load(float /* fInput */)
{
	BURGER_ASM {
	mov edx,dword ptr [esp+4]	// Get the float
	mov eax,dword ptr [esp]		// Get the return address
	bswap edx					// Swap the float
	mov dword ptr [esp+4],edx	// Store the float
	fld dword ptr [esp+4]		// Load into the x87 FPU
	add esp,8					// Fix the stack
	jmp eax						// Exit
	}
}

#elif defined(BURGER_POWERPC) && defined(__MWERKS__)

asm float BURGER_API Burger::SwapEndian::Load(float /* fInput */)
{
	stfs	fp1,24(SP)	// Store the 32 bit float on the stack
	addi	r4,sp,24	// Index to the stack addresses
	lwbrx	r3,r0,r4	// Load the values endian reversed
	stw		r3,24(SP)	// Store the swapped data
	lfs		fp1,24(SP)	// Load the endian reversed float
	blr
}

#else
float BURGER_API Burger::SwapEndian::Load(float fInput)
{
	Word8 bTemp1;
	Word8 bTemp2;

	bTemp1 = reinterpret_cast<const Word8 *>(&fInput)[0];
	bTemp2 = reinterpret_cast<const Word8 *>(&fInput)[3];
	reinterpret_cast<Word8 *>(&fInput)[3] = bTemp1;	// Perform the swap
	reinterpret_cast<Word8 *>(&fInput)[0] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(&fInput)[1];
	bTemp2 = reinterpret_cast<const Word8 *>(&fInput)[2];
	reinterpret_cast<Word8 *>(&fInput)[2] = bTemp1;
	reinterpret_cast<Word8 *>(&fInput)[1] = bTemp2;
	return fInput;					// Return the float
}
#endif

/*! ************************************

	\brief Reverse the endian of a 64-bit float

	Given a 64-bit floating point value, swap the bytes
	so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412

	\param dInput Value to endian convert
	\return The value with the bytes swapped, no errors are possible.
	\note Use of this form is discouraged since it involves moving a floating point value
	into an integer register and back again. Use the const double * version instead.

	\sa Burger::NativeEndian::Load(double) or Burger::SwapEndian::Load(const double *)

***************************************/

#if defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED double __fastcall Burger::SwapEndian::Load(double /* dInput */)
{
	BURGER_ASM {
	mov eax,dword ptr [esp+4]		// Fetch the double
	mov edx,dword ptr [esp+8]
	bswap eax						// Swap the lower 32 bits
	mov ecx,dword ptr [esp]			// Get the return address
	bswap edx						// Swap the upper 32 bits
	mov dword ptr [esp+8],eax		// Store double, swapped
	mov dword ptr [esp+4],edx
	fld qword ptr [esp+4]			// Load the double into the FPU
	add esp,12						// Fix the stack
	jmp ecx							// Exit
	}
}

#elif defined(BURGER_POWERPC) && defined(__MWERKS__)

asm double BURGER_API Burger::SwapEndian::Load(double /* dInput */)
{
	stfd	fp1,24(SP)	// Store the 64 bit float on the stack
	addi	r5,sp,24	// Index to the stack addresses
	addi	r6,sp,28
	lwbrx	r3,r0,r5	// Load the values endian reversed
	lwbrx	r4,r0,r6
	stw		r4,24(SP)	// Store the swapped data
	stw		r3,28(SP)
	lfd		fp1,24(SP)	// Load the endian reversed float
	blr
}

#else
double BURGER_API Burger::SwapEndian::Load(double dInput)
{
	Word8 bTemp1;
	Word8 bTemp2;

	bTemp1 = reinterpret_cast<const Word8 *>(&dInput)[0];
	bTemp2 = reinterpret_cast<const Word8 *>(&dInput)[7];
	reinterpret_cast<Word8 *>(&dInput)[7] = bTemp1;	// Perform the swap
	reinterpret_cast<Word8 *>(&dInput)[0] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(&dInput)[1];
	bTemp2 = reinterpret_cast<const Word8 *>(&dInput)[6];
	reinterpret_cast<Word8 *>(&dInput)[6] = bTemp1;
	reinterpret_cast<Word8 *>(&dInput)[1] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(&dInput)[2];
	bTemp2 = reinterpret_cast<const Word8 *>(&dInput)[5];
	reinterpret_cast<Word8 *>(&dInput)[5] = bTemp1;
	reinterpret_cast<Word8 *>(&dInput)[2] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(&dInput)[3];
	bTemp2 = reinterpret_cast<const Word8 *>(&dInput)[4];
	reinterpret_cast<Word8 *>(&dInput)[4] = bTemp1;
	reinterpret_cast<Word8 *>(&dInput)[3] = bTemp2;
	return dInput;					// Return the double
}

#endif

/*! ************************************

	\brief Reverse the endian of a 64-bit integer

	Given a pointer to a 64-bit value, load it and swap the bytes
	so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412

	\param pInput Pointer to the value to endian convert
	\return The value with the bytes swapped.

	\note \ref NULL pointers are illegal and will page fault.

	\sa Burger::SwapEndian::Load(Word64) or Burger::NativeEndian::Load(const Word64 *)

***************************************/

#if defined(BURGER_XBOX360) && !defined(DOXYGEN)
#elif defined(BURGER_AMD64) && defined(_MSC_VER)
#elif defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED Word64 __fastcall Burger::SwapEndian::Load(const Word64 * /* pInput */)
{
	BURGER_ASM {
	mov edx,dword ptr [ecx]		// Fetch the 64 bit integer
	mov eax,dword ptr [ecx+4]
	bswap eax					// Swap the lower 32 bits
	bswap edx					// Swap the upper 32 bits
	ret							// Exit
	}
}

#elif defined(BURGER_POWERPC) && defined(__MWERKS__)

asm Word64 BURGER_API Burger::SwapEndian::Load(const Word64 * /* pInput */)
{
	li		r5,4		// Index to the high word
	lwbrx	r4,r0,r3	// Load the values endian reversed
	lwbrx	r3,r5,r3
	blr
}

#else 
Word64 BURGER_API Burger::SwapEndian::Load(const Word64 *pInput)
{
	Word8 bTemp1;
	Word8 bTemp2;
	Word64 uTemp;		// This MUST be cast as a Word64 to be Word64 aligned!

	bTemp1 = reinterpret_cast<const Word8 *>(pInput)[0];
	bTemp2 = reinterpret_cast<const Word8 *>(pInput)[1];
	reinterpret_cast<Word8 *>(&uTemp)[7] = bTemp1;	// Perform the swap
	reinterpret_cast<Word8 *>(&uTemp)[6] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(pInput)[2];
	bTemp2 = reinterpret_cast<const Word8 *>(pInput)[3];
	reinterpret_cast<Word8 *>(&uTemp)[5] = bTemp1;
	reinterpret_cast<Word8 *>(&uTemp)[4] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(pInput)[4];
	bTemp2 = reinterpret_cast<const Word8 *>(pInput)[5];
	reinterpret_cast<Word8 *>(&uTemp)[3] = bTemp1;
	reinterpret_cast<Word8 *>(&uTemp)[2] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(pInput)[6];
	bTemp2 = reinterpret_cast<const Word8 *>(pInput)[7];
	reinterpret_cast<Word8 *>(&uTemp)[1] = bTemp1;
	reinterpret_cast<Word8 *>(&uTemp)[0] = bTemp2;
	return uTemp;					// Return the 64-bit integer
}

#endif

/*! ************************************

	\brief Reverse the endian of a 32-bit float

	Given a pointer to a 32-bit floating point value, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412

	\param pInput Pointer to the value to endian convert
	\return The value with the bytes swapped.

	\note \ref NULL pointers are illegal and will page fault.

	\sa Burger::SwapEndian::Load(float) or Burger::NativeEndian::Load(const float *)

***************************************/

#if defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED float __fastcall Burger::SwapEndian::Load(const float * /* pInput */)
{
	BURGER_ASM {
	mov edx,dword ptr [ecx]		// Get the float
	mov eax,dword ptr [esp]		// Get the return address
	bswap edx					// Swap the float
	mov dword ptr [esp],edx		// Store the float
	fld dword ptr [esp]			// Load into the x87 FPU
	add esp,4					// Fix the stack
	jmp eax						// Exit
	}
}

#elif defined(BURGER_POWERPC) && defined(__MWERKS__)

asm float BURGER_API Burger::SwapEndian::Load(const float * /* pInput */)
{
	lwbrx	r0,r0,r3		// Fetch it endian swap
	stw		r0,24(SP)		// Save it on the stack
	lfs		fp1,24(SP)		// Load it as a float
	blr
}

#else
float BURGER_API Burger::SwapEndian::Load(const float *pInput)
{
	Word8 bTemp1;
	Word8 bTemp2;
	float fInput;		// This MUST be cast as a float to be float aligned!

	bTemp1 = reinterpret_cast<const Word8 *>(pInput)[0];
	bTemp2 = reinterpret_cast<const Word8 *>(pInput)[1];
	reinterpret_cast<Word8 *>(&fInput)[3] = bTemp1;	// Perform the swap
	reinterpret_cast<Word8 *>(&fInput)[2] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(pInput)[2];
	bTemp2 = reinterpret_cast<const Word8 *>(pInput)[3];
	reinterpret_cast<Word8 *>(&fInput)[1] = bTemp1;
	reinterpret_cast<Word8 *>(&fInput)[0] = bTemp2;
	return fInput;					// Return the float
}

#endif

/*! ************************************

	\brief Reverse the endian of a 64-bit float

	Given a pointer to a 64-bit floating point value, load it and swap the bytes
	so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412

	\param pInput Pointer to the value to endian convert
	\return The value with the bytes swapped.

	\note \ref NULL pointers are illegal and will page fault.

	\sa Burger::SwapEndian::Load(double) or Burger::NativeEndian::Load(const double *)

***************************************/

#if defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED double __fastcall Burger::SwapEndian::Load(const double * /* dInput */)
{
	BURGER_ASM {
	mov edx,dword ptr [ecx]			// Fetch the double
	mov eax,dword ptr [ecx+4]
	mov ecx,dword ptr [esp]			// Get the return address
	bswap edx						// Swap the upper 32 bits
	bswap eax						// Swap the lower 32 bits
	mov dword ptr [esp],edx			// Store the upper 32 bits
	push eax						// Store the lower 32 bits by making room for it
	fld qword ptr [esp]				// Load the double into the FPU
	add esp,8						// Fix the stack
	jmp ecx							// Exit
	}
}

#elif defined(BURGER_POWERPC) && defined(__MWERKS__)

asm double BURGER_API Burger::SwapEndian::Load(const double * /* pInput */)
{
	li		r5,4
	lwbrx	r0,r0,r3		// Fetch it endian swap
	lwbrx	r4,r5,r3		
	stw		r4,24(SP)		// Save it on the stack
	stw		r0,28(SP)
	lfd		fp1,24(SP)		// Load it as a float
	blr
}

#else
double BURGER_API Burger::SwapEndian::Load(const double *pInput)
{
	Word8 bTemp1;
	Word8 bTemp2;
	double dInput;		// This MUST be cast as a double to be double aligned!

	bTemp1 = reinterpret_cast<const Word8 *>(pInput)[0];
	bTemp2 = reinterpret_cast<const Word8 *>(pInput)[1];
	reinterpret_cast<Word8 *>(&dInput)[7] = bTemp1;	// Perform the swap
	reinterpret_cast<Word8 *>(&dInput)[6] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(pInput)[2];
	bTemp2 = reinterpret_cast<const Word8 *>(pInput)[3];
	reinterpret_cast<Word8 *>(&dInput)[5] = bTemp1;
	reinterpret_cast<Word8 *>(&dInput)[4] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(pInput)[4];
	bTemp2 = reinterpret_cast<const Word8 *>(pInput)[5];
	reinterpret_cast<Word8 *>(&dInput)[3] = bTemp1;
	reinterpret_cast<Word8 *>(&dInput)[2] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(pInput)[6];
	bTemp2 = reinterpret_cast<const Word8 *>(pInput)[7];
	reinterpret_cast<Word8 *>(&dInput)[1] = bTemp1;
	reinterpret_cast<Word8 *>(&dInput)[0] = bTemp2;
	return dInput;					// Return the double
}

#endif

/*! ************************************

	\brief Fetch a 16 bit unsigned reverse endian integer from memory with byte alignment.

	Given a pointer to a 16 bit value, load it and swap the bytes
	so that 0x1234 becomes 0x3412. The pointer does not have
	to be 16-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to a value to endian convert
	\return The value with the bytes swapped.
	
	\note \ref NULL pointers are illegal and will page fault.

	\sa Burger::SwapEndian::Load(const Word16 *) or Burger::NativeEndian::LoadAny(const Word16 *)

***************************************/

#if defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && !defined(DOXYGEN)
BURGER_DECLSPECNAKED Word16 __fastcall Burger::SwapEndian::LoadAny(const Word16 * /* pInput */)
{
	BURGER_ASM {
		mov		ah,byte ptr[ecx]	// Grab the short
		mov		al,byte ptr[ecx+1]
		ret
	}
}
#else

Word16 BURGER_API Burger::SwapEndian::LoadAny(const Word16 *pInput)
{
	Word Low = reinterpret_cast<const Word8*>(pInput)[0];	// Grab 16 bits
	Word High = reinterpret_cast<const Word8*>(pInput)[1];
#if defined(BURGER_LITTLEENDIAN)
	Word16 Result = static_cast<Word16>((Low<<8) + High);	// Intel likes +
#else
	Word16 Result = static_cast<Word16>(Low | (High<<8));	// PPC likes |
#endif
	return Result;
}
#endif

/*! ************************************

	\brief Fetch a 32 bit unsigned reverse endian integer from memory with byte alignment.

	Given a pointer to a 32 bit value, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412. The pointer does not have
	to be 32-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to value to endian convert
	\return The value with the bytes swapped.

	\note \ref NULL pointers are illegal and will page fault.

	\sa Burger::SwapEndian::Load(const Word32 *) or Burger::NativeEndian::LoadAny(const Word32 *)

***************************************/

#if defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && !defined(DOXYGEN)
BURGER_DECLSPECNAKED Word32 __fastcall Burger::SwapEndian::LoadAny(const Word32 * /* pInput */)
{
	BURGER_ASM {
	mov		al,byte ptr[ecx]	// Grab the long in a virtual memory friendly way
	mov		ah,byte ptr[ecx+1]
	bswap	eax
	mov		ah,byte ptr[ecx+2]
	mov 	al,byte ptr[ecx+3]
	ret
	}
}
#else
Word32 BURGER_API Burger::SwapEndian::LoadAny(const Word32 *pInput)
{
	Word Low1 = reinterpret_cast<const Word8*>(pInput)[0];		// Grab 32 bits
	Word Low2 = reinterpret_cast<const Word8*>(pInput)[1];
	Word Low3 = reinterpret_cast<const Word8*>(pInput)[2];
	Word Low4 = reinterpret_cast<const Word8*>(pInput)[3];
#if defined(BURGER_LITTLEENDIAN)
	Word32 Result = static_cast<Word32>((Low1<<24) + (Low2<<16) +
		(Low3<<8) + Low4);			// Intel likes +
#else
	Word32 Result = static_cast<Word32>(Low1 | (Low2<<8) |
		(Low3<<16) | (Low4<<24));	// PPC likes |
#endif
	return Result;
}
#endif

/*! ************************************

	\brief Fetch a 64 bit unsigned reverse endian integer from memory with byte alignment.
	
	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a 64 bit value in reverse endian. The pointer does not have
	to be 64-bit aligned. \ref Word8 alignment is acceptable.
		
	\param pInput Pointer to a 64 bit value.
	\return The value with the bytes swapped.
	
	\note \ref NULL pointers are illegal and will page fault.

	\sa Burger::SwapEndian::Load(const Word64 *) or Burger::NativeEndian::LoadAny(const Word64 *)

***************************************/

#if defined(__WATCOMC__)
// WatcomC passes the parm in EAX

BURGER_DECLSPECNAKED Word64 BURGER_API Burger::SwapEndian::LoadAny(const Word64 * /* pInput */)
{
	// Intel returns 64 bit values in EAX:EDX
	__asm {
	push	ecx
	mov		dl,byte ptr[eax]	// Grab the long in a virtual memory friendly way
	mov		dh,byte ptr[eax+1]
	bswap	edx
	mov		dh,byte ptr[eax+2]
	mov		dl,byte ptr[eax+3]
	mov		cl,byte ptr[eax+4]	// Grab the long in a virtual memory friendly way
	mov		ch,byte ptr[eax+5]
	bswap	ecx
	mov		ch,byte ptr[eax+6]
	mov		cl,byte ptr[eax+7]
	mov		eax,ecx
	pop		ecx
	ret
	}
}

#elif defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED Word64 __fastcall Burger::SwapEndian::LoadAny(const Word64 * /* pInput */)
{
	// Intel returns 64 bit values in EAX:EDX
	BURGER_ASM {
	mov		dl,byte ptr[ecx]	// Grab the long in a virtual memory friendly way
	mov		dh,byte ptr[ecx+1]
	bswap	edx
	mov		dh,byte ptr[ecx+2]
	mov		dl,byte ptr[ecx+3]
	mov		al,byte ptr[ecx+4]	// Grab the long in a virtual memory friendly way
	mov		ah,byte ptr[ecx+5]
	bswap	eax
	mov		ah,byte ptr[ecx+6]
	mov		al,byte ptr[ecx+7]
	ret
	}
}

#else
Word64 BURGER_API Burger::SwapEndian::LoadAny(const Word64 *pInput)
{
	Word64 Result;		// Make sure it's aligned
	
	reinterpret_cast<Word8*>(&Result)[7] = reinterpret_cast<const Word8*>(pInput)[0];
	reinterpret_cast<Word8*>(&Result)[6] = reinterpret_cast<const Word8*>(pInput)[1];
	reinterpret_cast<Word8*>(&Result)[5] = reinterpret_cast<const Word8*>(pInput)[2];
	reinterpret_cast<Word8*>(&Result)[4] = reinterpret_cast<const Word8*>(pInput)[3];
	reinterpret_cast<Word8*>(&Result)[3] = reinterpret_cast<const Word8*>(pInput)[4];
	reinterpret_cast<Word8*>(&Result)[2] = reinterpret_cast<const Word8*>(pInput)[5];
	reinterpret_cast<Word8*>(&Result)[1] = reinterpret_cast<const Word8*>(pInput)[6];
	reinterpret_cast<Word8*>(&Result)[0] = reinterpret_cast<const Word8*>(pInput)[7];
	return Result;			// Return the 64 bit value in a register
}

#endif

/*! ************************************

	\brief Fetch a 32 bit reverse endian float from memory with byte alignment.

	Given a pointer to a 32-bit floating point value, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412. The pointer does not have
	to be 32-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to the value to endian convert
	\return The value with the bytes swapped.

	\note \ref NULL pointers are illegal and will page fault.

	\sa Burger::SwapEndian::Load(float) or Burger::NativeEndian::Load(const float *)

***************************************/

#if defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED float __fastcall Burger::SwapEndian::LoadAny(const float * /* pInput */)
{
	BURGER_ASM {
	mov		ah,byte ptr[ecx]	// Grab the long in a virtual memory friendly way
	mov		al,byte ptr[ecx+1]
	mov		dh,byte ptr[ecx+2]
	mov		dl,byte ptr[ecx+3]
	mov		ecx,[esp]			// Get the return address in the dead cycle
	mov		[esp],dx			// Store in memory
	mov		[esp+2],ax
	fld		dword ptr[esp]		// Get the float
	add		esp,4				// Restore the stack (And the return)
	jmp		ecx					// Faster than RET
	}
}

#else
float BURGER_API Burger::SwapEndian::LoadAny(const float *pInput)
{
	Word8 bTemp1;
	Word8 bTemp2;
	float fInput;		// This MUST be cast as a float to be float aligned!

	bTemp1 = reinterpret_cast<const Word8 *>(pInput)[0];
	bTemp2 = reinterpret_cast<const Word8 *>(pInput)[1];
	reinterpret_cast<Word8 *>(&fInput)[3] = bTemp1;	// Perform the swap
	reinterpret_cast<Word8 *>(&fInput)[2] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(pInput)[2];
	bTemp2 = reinterpret_cast<const Word8 *>(pInput)[3];
	reinterpret_cast<Word8 *>(&fInput)[1] = bTemp1;
	reinterpret_cast<Word8 *>(&fInput)[0] = bTemp2;
	return fInput;					// Return the float
}
#endif

/*! ************************************

	\brief Fetch a 64 bit reverse endian float from memory with byte alignment.

	Given a pointer to a 64-bit floating point value, load it and swap the bytes
	so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412. The pointer does not have
	to be 64-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to the value to endian convert
	\return The value with the bytes swapped.

	\note \ref NULL pointers are illegal and will page fault.

	\sa Burger::SwapEndian::Load(double) or Burger::NativeEndian::Load(const double *)

***************************************/

#if defined(BURGER_X86) && defined(BURGER_FASTCALLENABLED) && !defined(DOXYGEN)

BURGER_DECLSPECNAKED double BURGER_API Burger::SwapEndian::LoadAny(const double * /* pInput */)
{
	// Intel returns 64 bit values in EAX:EDX
	BURGER_ASM {
	sub		esp,8				// Make space on the stack (Intel is okay for 4 byte alignment)
	mov		ah,byte ptr[ecx]	// Grab the long in a virtual memory friendly way
	mov		al,byte ptr[ecx+1]
	mov		dh,byte ptr[ecx+2]
	mov		dl,byte ptr[ecx+3]		
	mov		[esp+6],ax			// Store in memory
	mov		[esp+4],dx
	mov		ah,byte ptr[ecx+4]	// Grab the long in a virtual memory friendly way
	mov		al,byte ptr[ecx+5]
	mov		dh,byte ptr[ecx+6]
	mov		dl,byte ptr[ecx+7]		
	mov		[esp+2],ax			// Store in memory
	mov		[esp],dx
	mov		eax,[esp+8]			// Get the return address in the dead cycle
	fld		qword ptr[esp]		// Get the double
	add		esp,12				// Restore the stack (And the return)
	jmp		eax					// Faster than RET
	}
}

#else 

double BURGER_API Burger::SwapEndian::LoadAny(const double *pInput)
{
	Word8 bTemp1;
	Word8 bTemp2;
	double dTemp;		// This MUST be cast as a double to be double aligned!

	bTemp1 = reinterpret_cast<const Word8 *>(pInput)[0];
	bTemp2 = reinterpret_cast<const Word8 *>(pInput)[1];
	reinterpret_cast<Word8 *>(&dTemp)[7] = bTemp1;	// Perform the swap
	reinterpret_cast<Word8 *>(&dTemp)[6] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(pInput)[2];
	bTemp2 = reinterpret_cast<const Word8 *>(pInput)[3];
	reinterpret_cast<Word8 *>(&dTemp)[5] = bTemp1;
	reinterpret_cast<Word8 *>(&dTemp)[4] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(pInput)[4];
	bTemp2 = reinterpret_cast<const Word8 *>(pInput)[5];
	reinterpret_cast<Word8 *>(&dTemp)[3] = bTemp1;
	reinterpret_cast<Word8 *>(&dTemp)[2] = bTemp2;
	bTemp1 = reinterpret_cast<const Word8 *>(pInput)[6];
	bTemp2 = reinterpret_cast<const Word8 *>(pInput)[7];
	reinterpret_cast<Word8 *>(&dTemp)[1] = bTemp1;
	reinterpret_cast<Word8 *>(&dTemp)[0] = bTemp2;
	return dTemp;					// Return the double
}

#endif

/*! ************************************

	\fn Burger::SwapEndian::Fixup(Word16 *pInput);
	\brief Reverse the endian of a 16-bit value

	Given a pointer to a 16-bit value in memory, load it and swap the bytes
	so that 0x1234 becomes 0x3412.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.

	\sa Burger::SwapEndian::FixupAny(Word16 *) or Burger::NativeEndian::Fixup(Word16 *)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Fixup(Word32 *pInput);
	\brief Reverse the endian of a 32-bit value

	Given a pointer to a 32-bit value in memory, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.

	\sa Burger::SwapEndian::FixupAny(Word32 *) or Burger::NativeEndian::Fixup(Word32 *)

***************************************/

/*! ************************************

	\brief Reverse the endian of a 64-bit value

	Given a pointer to a 64-bit value in memory, load it and swap the bytes
	so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.

	\sa Burger::SwapEndian::FixupAny(Word64 *) or Burger::NativeEndian::Fixup(Word64 *)

***************************************/

#if (defined(BURGER_XBOX360) || defined(BURGER_AMD64)) && !defined(DOXYGEN)
#elif defined(BURGER_POWERPC) && defined(__MWERKS__)
#else
void BURGER_API Burger::SwapEndian::Fixup(Word64 *pInput)
{
	Word32 uLow = Burger::SwapEndian::Load(&reinterpret_cast<Word32 *>(pInput)[0]);
	Word32 uHigh = Burger::SwapEndian::Load(&reinterpret_cast<Word32 *>(pInput)[1]);
	reinterpret_cast<Word32 *>(pInput)[0] = uHigh;
	reinterpret_cast<Word32 *>(pInput)[1] = uLow;
}
#endif

/*! ************************************

	\brief Reverse the endian of a 16-bit value

	Given a pointer to a 16-bit value in memory, load it and swap the bytes
	so that 0x1234 becomes 0x3412. The pointer does not have
	to be 16-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.

	\sa Burger::SwapEndian::Fixup(Word16 *) or Burger::NativeEndian::FixupAny(Word16 *)

***************************************/

void BURGER_API Burger::SwapEndian::FixupAny(Word16 *pInput)
{
	Word8 bTemp1;
	Word8 bTemp2;
	bTemp1 = reinterpret_cast<Word8*>(pInput)[0];
	bTemp2 = reinterpret_cast<Word8*>(pInput)[1];
	reinterpret_cast<Word8*>(pInput)[1] = bTemp1;
	reinterpret_cast<Word8*>(pInput)[0] = bTemp2;
}

/*! ************************************

	\brief Reverse the endian of a 32-bit value

	Given a pointer to a 32-bit value in memory, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412. The pointer does not have
	to be 32-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.

	\sa Burger::SwapEndian::Fixup(Word32 *) or Burger::NativeEndian::FixupAny(Word32 *)

***************************************/

void BURGER_API Burger::SwapEndian::FixupAny(Word32 *pInput)
{
	Word8 bTemp1;
	Word8 bTemp2;
	bTemp1 = reinterpret_cast<Word8*>(pInput)[0];
	bTemp2 = reinterpret_cast<Word8*>(pInput)[3];
	reinterpret_cast<Word8*>(pInput)[3] = bTemp1;
	reinterpret_cast<Word8*>(pInput)[0] = bTemp2;
	bTemp1 = reinterpret_cast<Word8*>(pInput)[1];
	bTemp2 = reinterpret_cast<Word8*>(pInput)[2];
	reinterpret_cast<Word8*>(pInput)[2] = bTemp1;
	reinterpret_cast<Word8*>(pInput)[1] = bTemp2;
}

/*! ************************************

	\brief Reverse the endian of a 64-bit value

	Given a pointer to a 64-bit value in memory, load it and swap the bytes
	so that 0x123456789aBCDEF0 becomes 0xF0DEBC9A78563412. The pointer does not have
	to be 64-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.

	\sa Burger::SwapEndian::Fixup(Word64 *) or Burger::NativeEndian::FixupAny(Word64 *)

***************************************/

void BURGER_API Burger::SwapEndian::FixupAny(Word64 *pInput)
{
	Word8 bTemp1;
	Word8 bTemp2;
	bTemp1 = reinterpret_cast<Word8*>(pInput)[0];
	bTemp2 = reinterpret_cast<Word8*>(pInput)[7];
	reinterpret_cast<Word8*>(pInput)[7] = bTemp1;
	reinterpret_cast<Word8*>(pInput)[0] = bTemp2;
	bTemp1 = reinterpret_cast<Word8*>(pInput)[1];
	bTemp2 = reinterpret_cast<Word8*>(pInput)[6];
	reinterpret_cast<Word8*>(pInput)[6] = bTemp1;
	reinterpret_cast<Word8*>(pInput)[1] = bTemp2;
	bTemp1 = reinterpret_cast<Word8*>(pInput)[2];
	bTemp2 = reinterpret_cast<Word8*>(pInput)[5];
	reinterpret_cast<Word8*>(pInput)[5] = bTemp1;
	reinterpret_cast<Word8*>(pInput)[2] = bTemp2;
	bTemp1 = reinterpret_cast<Word8*>(pInput)[3];
	bTemp2 = reinterpret_cast<Word8*>(pInput)[4];
	reinterpret_cast<Word8*>(pInput)[4] = bTemp1;
	reinterpret_cast<Word8*>(pInput)[3] = bTemp2;
}

/*! ************************************

	\fn Burger::SwapEndian::Load(Int16 iInput);
	\brief Reverse the endian of a 16 bit signed integer.
	
	Given a 16 bit value in an integer register, swap the bytes
	so that 0x1234 becomes 0x3412.

	\param iInput = Value to endian convert

	\return The input with the bytes swapped, no errors are possible.

	\note This function is inlined to actually use Burger::SwapEndian::Load(Word16).
	
	\sa Burger::SwapEndian::Load(Word16)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Load(Int32 iInput);
	\brief Reverse the endian of a 32 bit signed integer.
	
	Given a 32 bit value in an integer register, swap the bytes
	so that 0x12345678 becomes 0x78563412.

	\param iInput = Value to endian convert

	\return The input with the bytes swapped, no errors are possible.

	\note This function is inlined to actually use Burger::SwapEndian::Load(Word32).
	
	\sa Burger::SwapEndian::Load(Word32)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Load(Int64 iInput);
	\brief Reverse the endian of a 64 bit signed integer.
	
	Given a 32 bit value in an integer register, swap the bytes
	so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412.

	\param iInput = Value to endian convert

	\return The input with the bytes swapped, no errors are possible.

	\note This function is inlined to actually use Burger::SwapEndian::Load(Word64).
	
	\sa Burger::SwapEndian::Load(Word64)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Load(Word uInput);
	\brief Reverse the endian of a 32 bit unsigned integer.
	
	Given a 32 bit value in an integer register, swap the bytes
	so that 0x12345678 becomes 0x78563412.

	\param uInput = Value to endian convert

	\return The input with the bytes swapped, no errors are possible.

	\note This function is inlined to actually use Burger::SwapEndian::Load(Word32).
	
	\sa Burger::SwapEndian::Load(Word32)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Load(Int iInput);
	\brief Reverse the endian of a 32 bit signed integer.
	
	Given a 32 bit value in an integer register, swap the bytes
	so that 0x12345678 becomes 0x78563412.

	\param iInput = Value to endian convert

	\return The input with the bytes swapped, no errors are possible.

	\note This function is inlined to actually use Burger::SwapEndian::Load(Word32).
	
	\sa Burger::SwapEndian::Load(Word32)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Load(const Int16 *pInput);
	\brief Reverse the endian of a 16 bit signed integer

	Given a pointer to a 16 bit value, load it and swap the bytes
	so that 0x1234 becomes 0x3412

	\param pInput Pointer to a value to endian convert
	\return The value with the bytes swapped.
	
	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::Load(const Word16 *).

	\sa Burger::SwapEndian::Load(const Word16*)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Load(const Int32 *pInput);
	\brief Reverse the endian of a 32 bit signed integer

	Given a pointer to a 32 bit value, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412

	\param pInput Pointer to a value to endian convert
	\return The value with the bytes swapped.
	
	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::Load(const Word32 *).

	\sa Burger::SwapEndian::Load(const Word32*)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Load(const Int64 *pInput);
	\brief Reverse the endian of a 64 bit signed integer

	Given a pointer to a 64 bit value, load it and swap the bytes
	so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412

	\param pInput Pointer to a value to endian convert
	\return The value with the bytes swapped.
	
	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::Load(const Word64 *).

	\sa Burger::SwapEndian::Load(const Word64*)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Load(const Word *pInput);
	\brief Reverse the endian of a 32 bit unsigned integer

	Given a pointer to a 32 bit value, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412

	\param pInput Pointer to a value to endian convert
	\return The value with the bytes swapped.
	
	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::Load(const Word32 *).

	\sa Burger::SwapEndian::Load(const Word32*)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Load(const Int *pInput);
	\brief Reverse the endian of a 32 bit signed integer

	Given a pointer to a 32 bit value, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412

	\param pInput Pointer to a value to endian convert
	\return The value with the bytes swapped.
	
	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::Load(const Word32 *).

	\sa Burger::SwapEndian::Load(const Word32*)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::LoadAny(const Int16 *pInput);
	\brief Reverse the endian of a 16 bit signed integer with byte alignment.

	Given a pointer to a 16 bit value, load it and swap the bytes
	so that 0x1234 becomes 0x3412. The pointer does not have
	to be 16-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to a value to endian convert
	\return The value with the bytes swapped.
	
	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::LoadAny(const Word16 *).

	\sa Burger::SwapEndian::LoadAny(const Word16*)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::LoadAny(const Int32 *pInput);
	\brief Reverse the endian of a 32 bit signed integer with byte alignment.

	Given a pointer to a 32 bit value, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412. The pointer does not have
	to be 32-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to a value to endian convert
	\return The value with the bytes swapped.
	
	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::LoadAny(const Word32 *).

	\sa Burger::SwapEndian::LoadAny(const Word32*)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::LoadAny(const Int64 *pInput);
	\brief Reverse the endian of a 64 bit signed integer with byte alignment.

	Given a pointer to a 64 bit value, load it and swap the bytes
	so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412. The pointer does not have
	to be 64-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to a value to endian convert
	\return The value with the bytes swapped.
	
	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::LoadAny(const Word64 *).

	\sa Burger::SwapEndian::LoadAny(const Word64*)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::LoadAny(const Word *pInput);
	\brief Reverse the endian of a 32 bit unsigned integer with byte alignment.

	Given a pointer to a 32 bit value, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412. The pointer does not have
	to be 32-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to a value to endian convert
	\return The value with the bytes swapped.
	
	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::LoadAny(const Word32 *).

	\sa Burger::SwapEndian::LoadAny(const Word32*)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::LoadAny(const Int *pInput);
	\brief Reverse the endian of a 32 bit signed integer with byte alignment.

	Given a pointer to a 32 bit value, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412. The pointer does not have
	to be 32-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to a value to endian convert
	\return The value with the bytes swapped.
	
	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::LoadAny(const Word32 *).

	\sa Burger::SwapEndian::LoadAny(const Word32*)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Fixup(Int16 *pInput);
	\brief Reverse the endian of a 16-bit value

	Given a pointer to a 16-bit value in memory, load it and swap the bytes
	so that 0x1234 becomes 0x3412.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::Fixup(Word16 *).

	\sa Burger::SwapEndian::Fixup(Word16 *)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Fixup(Int32 *pInput);
	\brief Reverse the endian of a 32-bit value

	Given a pointer to a 32-bit value in memory, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::Fixup(Word32 *).

	\sa Burger::SwapEndian::Fixup(Word32 *)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Fixup(Int64 *pInput);
	\brief Reverse the endian of a 64-bit value

	Given a pointer to a 64-bit value in memory, load it and swap the bytes
	so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::Fixup(Word64 *).

	\sa Burger::SwapEndian::Fixup(Word64 *)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Fixup(Word *pInput);
	\brief Reverse the endian of a 32-bit value

	Given a pointer to a 32-bit value in memory, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::Fixup(Word32 *).

	\sa Burger::SwapEndian::Fixup(Word32 *)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Fixup(Int *pInput);
	\brief Reverse the endian of a 32-bit value

	Given a pointer to a 32-bit value in memory, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::Fixup(Word32 *).

	\sa Burger::SwapEndian::Fixup(Word32 *)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Fixup(float *pInput);
	\brief Reverse the endian of a 32-bit float value

	Given a pointer to a 32-bit float value in memory, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::Fixup(Word32 *).

	\sa Burger::SwapEndian::Fixup(Word32 *)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::Fixup(double *pInput);
	\brief Reverse the endian of a 64-bit float value

	Given a pointer to a 64-bit value in memory, load it and swap the bytes
	so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::Fixup(Word64 *).

	\sa Burger::SwapEndian::Fixup(Word64 *)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::FixupAny(Int16 *pInput);
	\brief Reverse the endian of a 16-bit value with byte alignment.

	Given a pointer to a 16-bit value in memory, load it and swap the bytes
	so that 0x1234 becomes 0x3412. The pointer does not have
	to be 16-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::FixupAny(Word16 *).

	\sa Burger::SwapEndian::FixupAny(Word16 *)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::FixupAny(Int32 *pInput);
	\brief Reverse the endian of a 32-bit value with byte alignment.

	Given a pointer to a 32-bit value in memory, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412. The pointer does not have
	to be 16-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::FixupAny(Word32 *).

	\sa Burger::SwapEndian::FixupAny(Word32 *)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::FixupAny(Int64 *pInput);
	\brief Reverse the endian of a 64-bit value with byte alignment.

	Given a pointer to a 64-bit value in memory, load it and swap the bytes
	so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412. The pointer does not have
	to be 16-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::FixupAny(Word64 *).

	\sa Burger::SwapEndian::FixupAny(Word64 *)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::FixupAny(Word *pInput);
	\brief Reverse the endian of a 32-bit value with byte alignment.

	Given a pointer to a 32-bit value in memory, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412. The pointer does not have
	to be 16-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::FixupAny(Word32 *).

	\sa Burger::SwapEndian::FixupAny(Word32 *)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::FixupAny(Int *pInput);
	\brief Reverse the endian of a 32-bit value with byte alignment.

	Given a pointer to a 32-bit value in memory, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412. The pointer does not have
	to be 16-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::FixupAny(Word32 *).

	\sa Burger::SwapEndian::FixupAny(Word32 *)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::FixupAny(float *pInput);
	\brief Reverse the endian of a 32-bit float value with byte alignment.

	Given a pointer to a 32-bit float value in memory, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412. The pointer does not have
	to be 16-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::FixupAny(Word32 *).

	\sa Burger::SwapEndian::FixupAny(Word32 *)

***************************************/

/*! ************************************

	\fn Burger::SwapEndian::FixupAny(double *pInput);
	\brief Reverse the endian of a 64-bit float value with byte alignment.

	Given a pointer to a 64-bit value in memory, load it and swap the bytes
	so that 0x123456789ABCDEF0 becomes 0xF0DEBC9A78563412. The pointer does not have
	to be 16-bit aligned. \ref Word8 alignment is acceptable.

	\param pInput Pointer to the value to endian convert

	\note \ref NULL pointers are illegal and will page fault.
	\note This function is inlined to actually use Burger::SwapEndian::FixupAny(Word64 *).

	\sa Burger::SwapEndian::FixupAny(Word64 *)

***************************************/
