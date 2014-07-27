/***************************************

	Decompression manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdecompress.h"

/*! ************************************

	\class Burger::Decompress
	\brief Base class for data decompression

	For functions that allow decompression, this class will allow any
	decompression algorithm to be used as long as they derive from
	this base class.

	\sa Burger::Compress

***************************************/

/*! ************************************

	\enum Burger::Decompress::eError
	\brief Decompression error code
	
***************************************/

/*! ************************************

	\brief Default constructor

	Initializes the defaults

***************************************/

Burger::Decompress::Decompress(void) :
	m_uTotalInput(0),
	m_uTotalOutput(0),
	m_uInputLength(0),
	m_uOutputLength(0)
{
}

/*! ************************************

	\fn eError Burger::Decompress::Reset(void)
	\brief Reset the decompression algorithm

	This function will reset the decompression algorithm (Which may or may
	not require memory allocations) and returns an error code
	if there was a failure.

	\note This is must be implemented by derived class and it
	also acts as a "reset" function to recycle this class
	to perform decompression on new data.

	\return Decompress::eError enum (Zero is a success code)

***************************************/

/*! ************************************

	\fn eError Burger::Decompress::Process(void *pOutput,WordPtr uOutputChunkSize,const void *pInput,WordPtr uInputChunkLength)
	\brief Decompress data into the output buffer

	Pass data into the decompressor and store the output into the output buffer.
	Bounds checking is performed to ensure there are no buffer overruns or
	underruns on output. If the input decompressed into the output buffer without
	any excess, the code Decompress::DECOMPRESS_OKAY (0) is returned. If there wasn't enough
	input data to satisfy the output then Decompress::DECOMPRESS_INPUTUNDERRUN is returned
	and if there was input data unprocessed then Decompress::DECOMPRESS_OUTPUTOVERRUN is returned.

	If Decompress::DECOMPRESS_BADINPUT is returned, there was an unrecoverable error.

	\note This is must be implemented by derived class.

	\param pOutput Pointer to the buffer to accept the decompressed data
	\param uOutputChunkSize Number of bytes in the output buffer
	\param pInput Pointer to data to compress
	\param uInputChunkLength Number of bytes in the data to decompress

	\return Decompress::eError code with zero if no failure, non-zero is an error code

***************************************/

/*! ************************************

	\fn WordPtr Burger::Decompress::GetTotalInputSize(void) const
	\brief Get the total processed input data in bytes

	Returns the number of input processed since Init() was called.

	\return Number of input bytes processed since Init() was called.

	\sa GetTotalOutputSize(), GetProcessedInputSize() and GetProcessedOutputSize()

***************************************/

/*! ************************************

	\fn WordPtr Burger::Decompress::GetTotalOutputSize(void) const
	\brief Get the total processed output data in bytes

	Returns the number of output processed since Init() was called.

	\return Number of output bytes processed since Init() was called.

	\sa GetTotalInputSize(), GetProcessedInputSize() and GetProcessedOutputSize()

***************************************/

/*! ************************************

	\fn WordPtr Burger::Decompress::GetProcessedInputSize(void) const
	\brief Get the total processed output data in bytes from the last process pass.

	Returns the number of input processed from the last call to Process().

	\return Number of input bytes processed from the last call to Process().

	\sa GetTotalInputSize(), GetTotalOutputSize() and GetProcessedOutputSize()

***************************************/

/*! ************************************

	\fn WordPtr Burger::Decompress::GetProcessedOutputSize(void) const
	\brief Get the total processed output data in bytes from the last process pass.

	Returns the number of output processed from the last call to Process().

	\return Number of output bytes processed from the last call to Process().

	\sa GetTotalInputSize(), GetTotalOutputSize() and GetProcessedInputSize()

***************************************/


/*! ************************************

	\fn Word32 Burger::Decompress::GetSignature(void) const
	\brief Return the signature for this decompressor

	Each decompressor has a unique signature to identify the algorithm used
	for compression. It's in the form of a 4 character code.

	Popular codes are...
	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>Algorithm</th><th>Code</th></tr>
	<tr><td>LZSS</td>			<td>LZSS</td></tr>
	<tr><td>ILBM RLE</td>		<td>RLE </td></tr>
	<tr><td>Inflate (ZLib)</td>	<td>ZLIB</td></tr>
	<tr><td>LZARI</td>			<td>LZAR</td></tr>
	<tr><td>RefPack</td>		<td>REFP</td></tr>
	<tr><td>Huffman</td>		<td>HUFF</td></tr>
	</table>

	\return A 32 bit number form of the 4 byte character code. It's an
		endian neutral text string

***************************************/