/***************************************

	Flash player data stream reader
	
	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#include "brflashstream.h"
#include "brdebug.h"
#include "brflashmanager.h"

/*! ************************************

	\class Burger::Flash::Stream
	\brief Data stream manager for flash file parsing

	Flash uses bit streams in addition to byte data. This 
	class sits on top of a InputMemoryStream to handle
	parsing the special data types that are used exclusively
	by Flash.

	\sa Manager or InputMemoryStream

***************************************/

/*! ************************************

	\brief Default constructor

	Attaches an InputMemoryStream and a parent Flash Manager
	to a new input stream.

	\note This class does not take ownership of the InputMemoryStream. 
	It will not release the Manager nor the InputMemoryStream on shutdown

	\param pFlashPlayer Pointer to the parent flash player manager
	\param pInput Pointer to an InputMemoryStream to read flash player data from
	\sa Manager or InputMemoryStream

***************************************/

Burger::Flash::Stream::Stream(const Manager *pFlashPlayer,InputMemoryStream* pInput) :
	m_pStream(pInput),
	m_pFlashPlayer(pFlashPlayer),
	m_TagStack(),
	m_uBitBucket(0),
	m_uBitsRemaining(0)
{
}

/*! ************************************

	\brief Release data

	\note This class did not take ownership of the InputMemoryStream. 
	It will not release the Manager nor the InputMemoryStream on shutdown

	\sa Stream(const Manager *,InputMemoryStream *)

***************************************/

Burger::Flash::Stream::~Stream()
{
}

/*! ************************************

	\fn const Manager *Burger::Flash::Stream::GetManager(void) const
	\brief Get the parent manager class

	Some readers need access to the parent class, this function
	grants that access.

	\sa Manager

***************************************/

/*! ************************************

	\fn void Burger::Flash::Stream::ByteAlign(void)
	\brief Byte align the data stream

	Forces the data stream to align to the next 8 bit boundary.
	This internal function will clear out and remaining bits left
	over from a bit stream parse from calls to GetWord(Word) or
	GetInt(Word)

	\sa GetWord(Word) or GetInt(Word)

***************************************/

/*! ************************************

	\fn InputMemoryStream * Burger::Flash::Stream::GetStream(void) const
	\brief Get the InputMemoryStream pointer

	\return The pointer to the InputMemoryStream the class is using to read in data.
	\sa InputMemoryStream

***************************************/

/*! ************************************

	\fn Word Burger::Flash::Stream::GetBoolean(void)
	\brief Get a single bit from the stream.

	Extract a single bit from the stream and return it as a \ref TRUE or \ref FALSE

	\return \ref TRUE or \ref FALSE
	\sa ByteAlign(void) or GetWord(Word)

***************************************/

/*! ************************************

	\brief Read a one to five bytes to create a 32 bit integer

	Read in 1 to 5 bytes from the data stream. This
	variable length encoding is used by Flash to compress
	32 bit integers.

	This code was lovingly ripped off from the file
	http://wwwimages.adobe.com/www.adobe.com/content/dam/Adobe/en/devnet/swf/pdf/swf-file-format-spec.pdf
	at page 17.

	\return An unsigned integer decompressed from the byte stream
	\sa GetWord32(void), GetWord(Word) or GetBoolean(void)

***************************************/

Word32 BURGER_API Burger::Flash::Stream::GetEncodedU32(void)
{
	m_uBitsRemaining = 0;
	InputMemoryStream *pStream = m_pStream;
	Word32 uResult = pStream->GetByte();
	if (uResult & 0x80) {
		uResult = (uResult ^ 0x80) | (pStream->GetByte() << 7U);
		if (uResult & 0x4000) {
			uResult = (uResult ^ 0x4000) | (pStream->GetByte() << 14U);
			if (uResult & 0x200000) {
				uResult = (uResult ^ 0x200000) | (pStream->GetByte() << 21U);
				if (uResult & 0x10000000) {
					uResult = (uResult ^ 0x10000000) | (pStream->GetByte() << 28U);
				}
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Read a number of bits from the stream

	Read in 0 to 32 bits from the data stream. This
	will assert if a number higher than 32 is requested.

	\param uBitCount The number of bits to parse (0-32).
	\return An unsigned integer of the requested number of bits.
	\sa GetInt(Word), ByteAlign(void) or GetBoolean(void)

***************************************/

Word BURGER_API Burger::Flash::Stream::GetWord(Word uBitCount)
{
	BURGER_ASSERT(uBitCount < 33U);
	Word32 uResult = 0;
	if (uBitCount) {
		Word uBitsRemaining = m_uBitsRemaining;
		Word uBitBucket = m_uBitBucket;
		do {
			if (uBitsRemaining) {
				if (uBitCount >= uBitsRemaining) {
					// Consume all the unused bits.
					uResult |= (uBitBucket << (uBitCount - uBitsRemaining));
					uBitCount -= uBitsRemaining;
					uBitsRemaining = 0;
				} else {
					// Consume some of the unused bits.
					uResult |= (uBitBucket >> (uBitsRemaining - uBitCount));
					// mask off the bits we consumed.
					uBitBucket &= ((1 << (uBitsRemaining - uBitCount)) - 1);
					uBitsRemaining -= uBitCount;
					// We're done.
					uBitCount = 0;
				}
			} else {
				// Refill the bit bucket
				uBitBucket = m_pStream->GetByte();
				uBitsRemaining = 8;
			}
		} while (uBitCount);
		m_uBitsRemaining = uBitsRemaining;
		m_uBitBucket = uBitBucket;
	}
	return uResult;
}

/*! ************************************

	\brief Read a number of signed bits from the stream

	Read in 0 to 32 bits from the data stream. This
	will assert if a number higher than 32 is requested.

	\param uBitCount The number of bits to parse (0-32).
	\return An signed integer of the requested number of bits.
	\sa GetWord(Word), ByteAlign(void) or GetBoolean(void)

***************************************/

Int	BURGER_API Burger::Flash::Stream::GetInt(Word uBitCount)
{
	Int	iResult = static_cast<Int32>(GetWord(uBitCount));
	// Sign extend.
	if (iResult & (1 << (uBitCount - 1))) {
		iResult |= -1 << uBitCount;
	}
	return iResult;
}

/*! ************************************

	\fn float Burger::Flash::Stream::GetFixedAsFloat(void)
	\brief Read a 16.16 fixed point integer as a float

	Parse a 32 bit 16.16 fixed point number and convert it to a float
	and then return the resulting float.

	\return Floating point version of the 16.16 fixed point number.
	\sa GetFloat16(void), GetFloat(void) or GetDouble(void)

***************************************/

/*! ************************************

	\brief Read a 16 bit float

	Read in a compressed 16 bit float and convert it into a 32 bit
	floating point number.

	\return Floating point version of the 16 bit floating point number.
	\sa GetFixedAsFloat(void), GetFloat(void) or GetDouble(void)

***************************************/

float BURGER_API Burger::Flash::Stream::GetFloat16(void)
{
	m_uBitsRemaining = 0;
	Word32 uPacked = m_pStream->GetShort();
	// Get the sign
	Word32 uFloat = (uPacked & 0x8000U) << 16U;
	// Get the exponent
	Word32 uExponent = uPacked & 0x7C00U;
	if (uExponent) {
		uFloat |= ((uExponent>>10U) + (127 - 16)) << 23U;
	}
	// Get the mantissa
	uFloat |= (uPacked & 0x3FF) << 13U;
	// Convert into a float
	return static_cast<const float *>(static_cast<const void *>(&uFloat))[0];
}

/*! ************************************

	\fn float Burger::Flash::Stream::GetFloat(void)
	\brief Read a 32 bit floating point number

	Parse a little endian 32 bit floating point number from
	the byte stream.

	\return 32 bit floating point number from the byte stream.
	\sa GetFloat16(void), GetFixedAsFloat(void) or GetDouble(void)

***************************************/

/*! ************************************

	\fn double Burger::Flash::Stream::GetDouble(void)
	\brief Read a 64 bit floating point number

	Parse a little endian 4 bit floating point number from
	the byte stream.

	\return 64 bit floating point number from the byte stream.
	\sa GetFloat16(void), GetFixedAsFloat(void) or GetFloat(void)

***************************************/

/*! ************************************

	\fn Word8 Burger::Flash::Stream::GetByte(void)
	\brief Read an 8 bit unsigned integer

	Parse a single unsigned byte from the byte stream.

	\return An 8 bit unsigned integer.
	\sa GetInt8(void)

***************************************/

/*! ************************************

	\fn Int8 Burger::Flash::Stream::GetInt8(void)
	\brief Read an 8 bit signed integer

	Parse a single signed byte from the byte stream.

	\return An 8 bit signed integer.
	\sa GetByte(void)

***************************************/

/*! ************************************

	\fn Word16 Burger::Flash::Stream::GetShort(void)
	\brief Read a 16 bit unsigned integer

	Parse a 16 bit unsigned integer from the byte stream.

	\return A 16 bit unsigned integer.
	\sa GetInt16(void)

***************************************/

/*! ************************************

	\fn Int16 Burger::Flash::Stream::GetInt16(void)
	\brief Read a 16 bit signed integer

	Parse a 16 bit signed integer from the byte stream.

	\return A 16 bit signed integer.
	\sa GetShort(void)

***************************************/

/*! ************************************

	\fn Word32 Burger::Flash::Stream::GetWord32(void)
	\brief Read a 32 bit unsigned integer

	Parse a 32 bit unsigned integer from the byte stream.

	\return A 32 bit unsigned integer.
	\sa GetInt32(void)

***************************************/

/*! ************************************

	\fn Int32 Burger::Flash::Stream::GetInt32(void)
	\brief Read a 32 bit signed integer

	Parse a 32 bit signed integer from the byte stream.

	\return A 32 bit signed integer.
	\sa GetWord32(void)

***************************************/

/*! ************************************

	\fn Int32 Burger::Flash::Stream::GetEncodedInt32(void)
	\brief Read a byte compressed 32 bit signed integer

	Call GetEncodedU32(void) and return the value as a signed
	integer.

	\return A 32 bit signed integer.
	\sa GetEncodedU32(void) or GetEncodedU30(void)

***************************************/

/*! ************************************

	\fn Word32 Burger::Flash::Stream::GetEncodedU30(void)
	\brief Read a byte compressed 30 bit unsigned integer

	Parse a 30 bit unsigned integer from the byte stream
	using the byte packing used in GetEncodedU32(void).

	\return A 32 bit unsigned integer.
	\sa GetEncodedU32(void) or GetEncodedInt32(void)

***************************************/

/*! ************************************

	\brief Read a byte compressed 16 bit unsigned integer

	Read a single byte, if the byte is equal to 0xFF, read
	in two more bytes and return the resulting 16 bit number,
	otherwise return the original single byte.

	\return A 16 bit unsigned integer.
	\sa GetEncodedU32(void)

***************************************/

Word BURGER_API Burger::Flash::Stream::GetVariableCount(void)
{
	m_uBitsRemaining = 0; 
	Word uResult = m_pStream->GetByte();
	if (uResult == 0xFF) {
		uResult = m_pStream->GetShort();
	}
	return uResult;
}

/*! ************************************

	\brief Read in a zero terminated "C" string.

	Read from the data stream until a zero is found and capture
	the resulting string into the output.

	\param pOutput Pointer to a String to store the parsed string data.
	\sa ReadPString(String *) or ReadString(WordPtr,String *)

***************************************/

void BURGER_API Burger::Flash::Stream::ReadString(String *pOutput)
{
	m_uBitsRemaining = 0;
	m_pStream->GetString(pOutput);
}

/*! ************************************

	\brief Read in a pascal string.

	Read a single byte as a string length. Read in
	the following bytes into the output string. If
	the string length was zero, the output string is
	set to an empty string.

	\param pOutput Pointer to a String to store the parsed string data.
	\sa ReadString(String *) or ReadString(WordPtr,String *)

***************************************/

void BURGER_API Burger::Flash::Stream::ReadPString(String *pOutput)
{
	m_uBitsRemaining = 0;
	char Buffer[256];
	m_pStream->GetPString(Buffer,sizeof(Buffer));
	pOutput->Set(Buffer);
}

/*! ************************************

	\brief Read in a specific number of bytes as a string.

	Given a string length, read in
	the following bytes into the output string. If
	the string length was zero, the output string is
	set to an empty string.

	\param uLength Number of bytes to read into the output string buffer.
	\param pOutput Pointer to a String to store the parsed string data.
	\sa ReadString(String *) or ReadPString(String *)

***************************************/

void BURGER_API Burger::Flash::Stream::ReadString(WordPtr uLength,String *pOutput)
{
	m_uBitsRemaining = 0;
	pOutput->SetBufferSize(uLength);
	if (uLength) {
		m_pStream->Get(pOutput->GetPtr(),uLength);
	}
}

/*! ************************************

	\fn WordPtr Burger::Flash::Stream::GetMark(void) const
	\brief Return the current location of the byte stream.

	\return The location in the stream where parsing is currently at.
	\sa SetMark(WordPtr)

***************************************/

/*! ************************************

	\brief Set the location of the byte stream.

	Move the mark position within the byte stream to parse at
	a different location.

	\param uMark New location in the byte stream to parse from.
	\sa GetMark(void) const

***************************************/

void BURGER_API Burger::Flash::Stream::SetMark(WordPtr uMark)
{
	m_uBitsRemaining = 0;
	// If we're in a tag, make sure we're not seeking outside the tag.
#if defined(_DEBUG)
	if (m_TagStack.size() > 0) {
		BURGER_ASSERT(uMark <= m_TagStack.back());
	}
#endif
	// Pass the mark to the InputMemoryStream
	m_pStream->SetMark(uMark);
}

/*! ************************************

	\brief Return the file position of the end of the current tag.

	Return the file mark at the end of the data chunk
	from the last time StartTag(void) was called

	\return Input data mark from the last data tag.
	\sa StartTag(void) or CloseTag(void)

***************************************/

WordPtr	BURGER_API Burger::Flash::Stream::GetTagEndPosition(void) const
{
	BURGER_ASSERT(m_TagStack.size() > 0);
	return m_TagStack.back();
}

/*! ************************************

	\brief Open a data tag

	Read in 16 or 48 bits to get the next tag ID and the
	size of the data chunk. The end of chunk mark is pushed
	onto a stack so when CloseTag(void) is called, the
	marks can be checked for parsing errors.

	\return Current tag ID (A 10 bit unsigned number)
	\sa CloseTag(void) or GetTagEndPosition(void) const

***************************************/

Word BURGER_API Burger::Flash::Stream::StartTag(void)
{
	m_uBitsRemaining = 0;
	Word uTagHeader = m_pStream->GetShort();
	Word uTagType = uTagHeader >> 6U;
	Word uTagLength = uTagHeader & 0x3FU;
	if (uTagLength == 0x3FU) {
		uTagLength = m_pStream->GetWord32();
	}
	if (m_pFlashPlayer->GetVerboseParsingFlag()) {
		Debug::Message("Tag type = %u, Tag length = %u\n",uTagType,uTagLength);
	}
	// Remember where the end of the tag is, so we can
	// fast-forward past it when we're done reading it.
	m_TagStack.push_back(m_pStream->GetMark() + uTagLength);
	return uTagType;
}

/*! ************************************

	\brief Close a data tag

	Pops the last data tag end mark off of an internal stack
	and sets the input mark to this location. If
	Manager::GetVerboseParsingFlag(void) const flag is
	\ref TRUE, it will output a console message notifying
	if there's a file mark mismatch, which is useful in debugging
	flash files.

	\return Current tag ID (A 10 bit unsigned number)
	\sa OpenTag(void) or GetTagEndPosition(void) const

***************************************/

void BURGER_API Burger::Flash::Stream::CloseTag(void)
{
	BURGER_ASSERT(m_TagStack.size() > 0);
	// Get the last saved file mark
	WordPtr uEndMark = m_TagStack.back();
	m_TagStack.pop_back();
	// Data parse error?
	if (uEndMark != m_pStream->GetMark()) {
		// Spew out debugging information
		if (m_pFlashPlayer->GetVerboseParsingFlag()) {
			Debug::Message( "Tag is not correctly read, tag length is not respected\n");
		}
	}
	// Set the mark
	m_pStream->SetMark(uEndMark);
	m_uBitsRemaining = 0;
}