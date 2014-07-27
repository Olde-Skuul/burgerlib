/***************************************

	Atomic memory

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "bratomic.h"

/*! ************************************

	\fn Word32 Burger::AtomicSwap(volatile Word32 *pOutput,Word32 uInput)
	\brief Atomically swap a 32 bit value for one in memory

	For multi-CPU machines, this function will atomically swap a value
	with one stored in memory using appropriate locking for all CPUs

	\param pOutput Pointer to a 32 bit aligned memory location swap with
	\param uInput 32 bit value to exchange with the memory location
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word32 Burger::AtomicPreIncrement(volatile Word32 *pInput)
	\brief Atomically pre-increment a 32 bit variable in memory

	For multi-CPU machines, this function will atomically increment a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value after the increment was applied.

	\param pInput Pointer to a 32 bit aligned memory location to increment
	\return Value that was stored in the variable previously plus one

***************************************/

/*! ************************************

	\fn Word32 Burger::AtomicPostIncrement(volatile Word32 *pInput)
	\brief Atomically post-increment a 32 bit variable in memory

	For multi-CPU machines, this function will atomically increment a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value before the increment was applied.

	\param pInput Pointer to a 32 bit aligned memory location to increment
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word32 Burger::AtomicPreDecrement(volatile Word32 *pInput)
	\brief Atomically pre-decrement a 32 bit variable in memory

	For multi-CPU machines, this function will atomically decrement a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value after the decrement was applied.

	\param pInput Pointer to a 32 bit aligned memory location to decrement
	\return Value that was stored in the variable previously minus one

***************************************/

/*! ************************************

	\fn Word32 Burger::AtomicPostDecrement(volatile Word32 *pInput)
	\brief Atomically post-decrement a 32 bit variable in memory

	For multi-CPU machines, this function will atomically decrement a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value before the decrement was applied.

	\param pInput Pointer to a 32 bit aligned memory location to decrement
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word32 Burger::AtomicAdd(volatile Word32 *pInput,Word32 uValue)
	\brief Atomically add a 32 bit value to a variable in memory

	For multi-CPU machines, this function will atomically add a value
	to one stored in memory using appropriate locking for all CPUs. It will return
	the value before the addition was applied.

	\param pInput Pointer to a 32 bit aligned memory location to add a value to
	\param uValue 32 bit value to add
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word32 Burger::AtomicSubtract(volatile Word32 *pInput,Word32 uValue)
	\brief Atomically subtract a 32 bit value from a variable in memory

	For multi-CPU machines, this function will atomically subtract a value
	from one stored in memory using appropriate locking for all CPUs. It will return
	the value before the subtraction was applied.

	\param pInput Pointer to a 32 bit aligned memory location to subtract a value from
	\param uValue 32 bit value to subtract
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word32 Burger::AtomicSetIfMatch(volatile Word32 *pInput,Word32 uBefore,Word32 uAfter)
	\brief Atomically set a 32 bit value from a variable in memory if it matches a requested value

	For multi-CPU machines, this function will atomically test a value
	with one stored in memory using appropriate locking for all CPUs. If the
	value is a match, a new value will be written to the memory value.
	It will return \ref TRUE if the swap was successful.

	\param pInput Pointer to a 32 bit aligned memory location to test and swap from
	\param uBefore 32 bit value to test the memory variable with
	\param uAfter 32 bit value to write into the memory variable if the test succeeded
	\return \ref TRUE if the exchange occured, \ref FALSE if not

***************************************/

/*! ************************************

	\fn Word64 Burger::AtomicSwap(volatile Word64 *pOutput,Word64 uInput)
	\brief Atomically swap a 64 bit value for one in memory

	For multi-CPU machines, this function will atomically swap a value
	with one stored in memory using appropriate locking for all CPUs

	\note This function is only available on systems with a 64 bit CPU
	\param pOutput Pointer to a 64 bit aligned memory location swap with
	\param uInput 64 bit value to exchange with the memory location
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word64 Burger::AtomicPreIncrement(volatile Word64 *pInput)
	\brief Atomically pre-increment a 64 bit variable in memory

	For multi-CPU machines, this function will atomically increment a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value after the increment was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to increment
	\return Value that was stored in the variable previously plus one

***************************************/

/*! ************************************

	\fn Word64 Burger::AtomicPostIncrement(volatile Word64 *pInput)
	\brief Atomically post-increment a 64 bit variable in memory

	For multi-CPU machines, this function will atomically increment a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value before the increment was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to increment
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word64 Burger::AtomicPreDecrement(volatile Word64 *pInput)
	\brief Atomically pre-decrement a 64 bit variable in memory

	For multi-CPU machines, this function will atomically decrement a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value after the decrement was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to decrement
	\return Value that was stored in the variable previously minus one

***************************************/

/*! ************************************

	\fn Word64 Burger::AtomicPostDecrement(volatile Word64 *pInput)
	\brief Atomically post-decrement a 64 bit variable in memory

	For multi-CPU machines, this function will atomically decrement a value
	stored in memory using appropriate locking for all CPUs. It will return
	the value before the decrement was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to decrement
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word64 Burger::AtomicAdd(volatile Word64 *pInput,Word64 uValue) 
	\brief Atomically add a 64 bit value to a variable in memory

	For multi-CPU machines, this function will atomically add a value
	to one stored in memory using appropriate locking for all CPUs. It will return
	the value before the addition was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to add a value to
	\param uValue 64 bit value to add
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word64 Burger::AtomicSubtract(volatile Word64 *pInput,Word64 uValue)
	\brief Atomically subtract a 64 bit value from a variable in memory

	For multi-CPU machines, this function will atomically subtract a value
	from one stored in memory using appropriate locking for all CPUs. It will return
	the value before the subtraction was applied.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to subtract a value from
	\param uValue 64 bit value to subtract
	\return Value that was stored in the variable previously

***************************************/

/*! ************************************

	\fn Word64 Burger::AtomicSetIfMatch(volatile Word64 *pInput,Word64 uBefore,Word64 uAfter)
	\brief Atomically set a 64 bit value from a variable in memory if it matches a requested value

	For multi-CPU machines, this function will atomically test a value
	with one stored in memory using appropriate locking for all CPUs. If the
	value is a match, a new value will be written to the memory value.
	It will return \ref TRUE if the swap was successful.

	\note This function is only available on systems with a 64 bit CPU
	\param pInput Pointer to a 64 bit aligned memory location to test and swap from
	\param uBefore 64 bit value to test the memory variable with
	\param uAfter 64 bit value to write into the memory variable if the test succeeded
	\return \ref TRUE if the exchange occured, \ref FALSE if not

***************************************/

