/***************************************

	Compression manager

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brcompress.h"

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::Compress,Burger::Base);
#endif

/*! ************************************

	\class Burger::Compress
	\brief Base class for data compression

	For functions that allow compression, this class will allow any
	compression algorithm to be used as long as they derive from
	this base class.

	The only data present is an OutputMemoryStream that will
	contain the compressed data stream and will be valid
	once the Compress::Finalize() function is called.

	\sa Burger::Decompress

***************************************/

/*! ************************************

	\brief Default constructor

	Initializes the output buffer.

***************************************/

Burger::Compress::Compress(void) :
	m_Output() 
{
}

/*! ************************************

	\brief Default destructor

	Releases the output buffer if needed

***************************************/

Burger::Compress::~Compress()
{
}

/*! ************************************

	\fn Burger::Compress::eError Burger::Compress::Init(void)
	\brief Initialize the compression algorithm

	This function will reset the compression algorithm (Which may or may
	not require memory allocations) and returns an error code
	if there was a failure.

	\note This is must be implemented by derived class and it
	also acts as a "reset" function to recycle this class
	to perform compression on new data.

	\return Zero if no failure, non-zero is an error code

***************************************/

/*! ************************************

	\fn Burger::Compress::eError Burger::Compress::Process(const void *pInput,WordPtr uInputLength)
	\brief Compress data

	Pass data into the compressor and store the output into the data stream

	\note This is must be implemented by derived class.

	\param pInput Pointer to data to compress
	\param uInputLength Number of bytes in the data
	\return Zero if no failure, non-zero is an error code

***************************************/

/*! ************************************

	\fn Burger::Compress::eError Burger::Compress::Finalize(void)
	\brief Finish the compression

	Perform the final data compaction and clean up. After this call is performed,
	the output is valid and can be accessed with calls to GetOutput()
	and GetOutputSize()

	\note This is must be implemented by derived class.

	\return Zero if no failure, non-zero is an error code

	\sa GetOutput() and GetOutputSize()

***************************************/

/*! ************************************

	\fn OutputMemoryStream *Burger::Compress::GetOutput(void)
	\brief Get the output data

	After Finalize() has been called, the output data can be
	obtained with this call

	\return Pointer to the OutputMemoryStream structure containing the output

	\sa Finalize() and GetOutputSize()

***************************************/

/*! ************************************

	\fn WordPtr Burger::Compress::GetOutputSize(void) const
	\brief Get the output data size in bytes

	After Finalize() has been called, the output data size can be
	obtained with this call

	\return Number of bytes contained in the output stream

	\sa Finalize() and GetOutput()

***************************************/

/*! ************************************

	\fn Word32 Burger::Compress::GetSignature(void) const
	\brief Return the signature for this compressor

	Each compressor has a unique signature to identify the algorithm used
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

/*! ************************************

	\var const Burger::StaticRTTI Burger::Compress::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/
