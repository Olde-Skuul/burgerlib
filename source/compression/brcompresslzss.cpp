/***************************************

	Compress using LZSS

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brcompresslzss.h"

/*! ************************************

	\class Burger::CompressLZSS
	\brief Compress data using LZSS encoding

	Lempel Ziv Storer Szymanski (LZSS) encoding is explained here.

	http://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Storer%E2%80%93Szymanski

	This compression performs string compares from the previous 4096
	bytes of the data stream and if a match of 3 to 18 bytes are
	found, a sixteen bit index token is encoded to perform a copy
	from the previous data to the current data.

	The format is as follows, a byte is encoded with 8 bits with a
	one meaning an 8 bit value follows or a zero meaning a 16 bit index
	follows. After 8 samples have been parsed, another 8 bit byte
	will be fetched and the process is repeated.

	The 16 bit token has the upper 4 bits encoded with 0-15 converted
	to 3-18 as a byte count and the lower 12 bits is negative sign extended
	and added to the current output pointer and the bytes are 
	copied from the previously decompressed data to the current buffer

	\sa Burger::DecompressLZSS

***************************************/

/*! ************************************

	\brief Removes a node from the binary tree

	Prunes an entry from the binary string match tree

	\param uNodeNumber Ring buffer index (0-(CompressLZSS::RINGBUFFERSIZE-1))
	\sa InsertNode(Word)

***************************************/

void Burger::CompressLZSS::DeleteNode(WordPtr uNodeNumber)
{
	// Valid node?
	if (m_RootBranch[uNodeNumber] != NOTUSED) {
		WordPtr uClipNum;
		// Is there a right branch?
		if (m_RightBranch[uNodeNumber] == NOTUSED) {
			// Clip just the left branch
			uClipNum = m_LeftBranch[uNodeNumber];

		// Is there a left branch?
		} else if (m_LeftBranch[uNodeNumber] == NOTUSED) {
			// Clip just the right branch
			uClipNum = m_RightBranch[uNodeNumber];
		} else {

			// Clip both sides
			uClipNum = m_LeftBranch[uNodeNumber];
			WordPtr uRight = m_RightBranch[uClipNum];
			// Shall I prune the right side of the tree?
			if (uRight != NOTUSED) {
				do {
					// Follow to the end of the tree
					uClipNum = uRight;			// Clip this one?
					uRight = m_RightBranch[uClipNum];
				} while (uRight != NOTUSED);

				// Remove from the root
				uRight = m_LeftBranch[uClipNum];
				WordPtr uTempRoot = m_RootBranch[uClipNum];
				m_RightBranch[uTempRoot] = static_cast<Word>(uRight);	// Set the new right parent
				m_RootBranch[uRight] = static_cast<Word>(uTempRoot);	// Set the new root

				uRight = m_LeftBranch[uNodeNumber];
				m_LeftBranch[uClipNum] = static_cast<Word>(uRight);		// Clip the left
				m_RootBranch[uRight] = static_cast<Word>(uClipNum);		// Set the left's new parent
			}
			// Right side is clipped, finish up
			uRight = m_RightBranch[uNodeNumber];
			m_RightBranch[uClipNum] = static_cast<Word>(uRight);		// Attach my tree to the right's end
			m_RootBranch[uRight] = static_cast<Word>(uClipNum);		// Set the new parent
		}

		// ClipNum has the branch to remove
		WordPtr uRoot = m_RootBranch[uNodeNumber];		// Cache a value for speed

		// Reset the parent
		m_RootBranch[uClipNum] = static_cast<Word>(uRoot);
		m_RootBranch[uNodeNumber] = NOTUSED;	// Zap this tree
		if (m_RightBranch[uRoot] == static_cast<Word>(uNodeNumber)) {	// Finish up
			m_RightBranch[uRoot] = static_cast<Word>(uClipNum);		// Trim the right
		} else {
			m_LeftBranch[uRoot] = static_cast<Word>(uClipNum);			// Trim the left
		}
	}
}

/*! ************************************

	\brief Scans the node in the ring buffer for a previous match

	Inserts string of length CompressLZSS::MAXMATCHLENGTH,
	CompressLZSS::m_RingBuffer[?..?+ CompressLZSS::MAXMATCHLENGTH -1],
	into one of the trees (CompressLZSS::m_RingBuffer[uNodeNumber]'th tree) and returns the
	longest-match position and length via the variables
	CompressLZSS::m_uMatchOffset and CompressLZSS::m_uMatchSize. 
	If CompressLZSS::m_uMatchSize = CompressLZSS::MAXMATCHLENGTH,
	then removes the old node in favor of the new one,
	because the old one will be deleted sooner.

	\note uNodeNumber plays double role, as tree node and position in buffer.

	\param uNodeNumber Ring buffer index (0-(CompressLZSS::RINGBUFFERSIZE-1))

	\sa DeleteNode(Word)

***************************************/

void Burger::CompressLZSS::InsertNode(WordPtr uNodeNumber)
{
	// Zap the children nodes now
	m_RightBranch[uNodeNumber] = NOTUSED;
	m_LeftBranch[uNodeNumber] = NOTUSED;

	const Word8 *pRingBufferKey = &m_RingBuffer[uNodeNumber];

	// Get the parent node number via hash table
	WordPtr uParentNodeNumber = static_cast<WordPtr>(pRingBufferKey[0])+(RINGBUFFERSIZE+1U);
	WordPtr uMatchSize = 0;		// No match yet
	int iTestValue = 1;		// Assume a right branch initially

	// Stay until a match is found
	for (;;) {

		// Right branch?
		WordPtr uNewParent;
		if (iTestValue >= 0) {
			// Right branch
			uNewParent = m_RightBranch[uParentNodeNumber];
			if (uNewParent == NOTUSED) {
				m_RightBranch[uParentNodeNumber] = static_cast<Word>(uNodeNumber);		// Set new right branch
				m_RootBranch[uNodeNumber] = static_cast<Word>(uParentNodeNumber);		// Mark root
				m_uMatchSize = static_cast<Word>(uMatchSize);
				return;			// Exit NOW!
			}
		} else {
			// Left branch?
			uNewParent = m_LeftBranch[uParentNodeNumber];
			if (uNewParent == NOTUSED) {
				m_LeftBranch[uParentNodeNumber] = static_cast<Word>(uNodeNumber);		// Mark the new left branch
				m_RootBranch[uNodeNumber] = static_cast<Word>(uParentNodeNumber);		// Mark root
				m_uMatchSize = static_cast<Word>(uMatchSize);
				return;			// Exit NOW!
			}
		}

		// New parent node
		uParentNodeNumber = uNewParent;
		WordPtr uMatchCount = 1;		// Init string length
		const Word8 *pString = &m_RingBuffer[uParentNodeNumber];
		do {
			// Perform a signed compare to branch left/right
			iTestValue = static_cast<int>(pRingBufferKey[uMatchCount]);
			iTestValue = iTestValue-static_cast<int>(pString[uMatchCount]);
			if (iTestValue) {	// Is it different?
				break;			// Get out
			}
		} while (++uMatchCount<MAXMATCHLENGTH);

		if (uMatchCount > uMatchSize) {			// Larger match?
			m_uMatchOffset = static_cast<Word>(uParentNodeNumber);	// Set the mark
			uMatchSize = static_cast<Word>(uMatchCount);			// Set the length
			if (uMatchCount >= MAXMATCHLENGTH) {	// Maximum length!
				break;					// Get out of the main loop!
			}
		}
	}

	m_uMatchSize = static_cast<Word>(uMatchSize);
	// I have a full match!
	// Replace the old string with this one since it's newer

	WordPtr uRootNode = m_RootBranch[uParentNodeNumber];
	WordPtr uLeftNode = m_LeftBranch[uParentNodeNumber];
	WordPtr uRightNode = m_RightBranch[uParentNodeNumber];
	m_RootBranch[uNodeNumber] = static_cast<Word>(uRootNode);	// Set the new parent
	m_LeftBranch[uNodeNumber] = static_cast<Word>(uLeftNode);
	m_RightBranch[uNodeNumber] = static_cast<Word>(uRightNode);

	m_RootBranch[uLeftNode] = static_cast<Word>(uNodeNumber);
	m_RootBranch[uRightNode] = static_cast<Word>(uNodeNumber);
	m_RootBranch[uParentNodeNumber] = NOTUSED;	// Remove Parent

	if (m_RightBranch[uRootNode] == static_cast<Word>(uParentNodeNumber)) {
		m_RightBranch[uRootNode] = static_cast<Word>(uNodeNumber);
	} else {
		m_LeftBranch[uRootNode] = static_cast<Word>(uNodeNumber);
	}
}

/*! ************************************

	\brief Init the binary tree needed for the compression system

	For i = 0 to CompressLZSS::RINGBUFFERSIZE - 1, CompressLZSS::m_RightBranch[i] and
	CompressLZSS::m_LeftBranch[i] will be the right and left children of node i.
	These nodes need not be initialized.
	Also, CompressLZSS::m_RootBranch[i] is the parent of node i. These are initialized to
	CompressLZSS::NOTUSED
	For i = 0 to 255, CompressLZSSm_RightBranch[CompressLZSS::RINGBUFFERSIZE + i + 1] is the root of the tree
	for strings that begin with character i. These are initialized
	to CompressLZSS::NOTUSED.

	The hash is 8 bit, hence 256 hash entries

	\sa Init(void)

***************************************/

void Burger::CompressLZSS::InitTrees(void)
{
	// Initialize the hash
	WordPtr uCount = 256;
	Word *pWork = &m_RightBranch[RINGBUFFERSIZE+1];
	do {
		pWork[0] = NOTUSED;		// Zap the hash table
		++pWork;
	} while (--uCount);

	// Initialize the root indexes

	pWork = &m_RootBranch[0];
	uCount = RINGBUFFERSIZE;
	do {
		pWork[0] = NOTUSED;
		++pWork;
	} while (--uCount);

	// No data is in the cache
	m_uCachedLength = 0;
}

/*! ************************************

	\brief Initialize the compressor to defaults

***************************************/

Burger::CompressLZSS::CompressLZSS() :
	Compress()
{
	m_uSignature.u = Signature;
	InitTrees();
}

/***************************************

	Initialize

***************************************/

Burger::CompressLZSS::eError Burger::CompressLZSS::Init(void)
{
	// Clear any previous output
	m_Output.Clear();
	InitTrees();
	return COMPRESS_OKAY;
}

/***************************************

	Compress using LZSS

***************************************/

Burger::CompressLZSS::eError Burger::CompressLZSS::Process(const void *pInput,WordPtr uInputLength)
{
	if (uInputLength) {

		// Read MAXMATCHLENGTH bytes into the last MAXMATCHLENGTH bytes of the buffer
		// Note: If not possible to fill the buffer, then exit early with the data in the cache

		if (m_uCachedLength<MAXMATCHLENGTH) {
			do {
				if (!uInputLength) {
					return COMPRESS_OKAY;
				}
				m_RingBuffer[m_uCachedLength+(RINGBUFFERSIZE - MAXMATCHLENGTH)] = static_cast<const Word8 *>(pInput)[0];
				pInput = static_cast<const Word8 *>(pInput) + 1;
				--uInputLength;		// Use input
			} while (++m_uCachedLength<MAXMATCHLENGTH);

			// Encode the very first byte, which can only be 
			// a raw byte.

			// Init the ring buffer indexes

			InsertNode(RINGBUFFERSIZE - MAXMATCHLENGTH);		// Insert it in and get m_uMatchSize
			m_uBitMaskOffset = m_Output.GetSize();				// Offset is the first byte in the stream
			m_Output.Append(static_cast<Word8>(1));
			m_Output.Append(static_cast<Word8>(m_RingBuffer[RINGBUFFERSIZE - MAXMATCHLENGTH]));	// Send uncoded.
			m_bOrMask = 1U<<1U;			// Initialize for the NEXT bit
			m_uMatchSize = 1;			// Force 1 byte for future loop
			m_bBitMask = 1;				// 'send one byte' flag
			m_uPreviousMatchSize = 1;	// Length of the last match
			m_uMatchIterator = 0;		// Begin the iteration check
			m_uSourceIndex = 0;			// Index to begin compression
			m_uDestIndex = RINGBUFFERSIZE - MAXMATCHLENGTH;

			// Any data to handle?
			if (!uInputLength) {
				return COMPRESS_OKAY;
			}
		}

		// Process all incoming data!

		do {
			// Update the binary tree if a node is to be purged
			// since it may have dropped past the RINGBUFFERSIZE offset
			do {
				if (!uInputLength) {
					return COMPRESS_OKAY;
				}
				Word uInputByte = static_cast<const Word8 *>(pInput)[0];		// Get a byte from the input stream
				pInput = static_cast<const Word8 *>(pInput) + 1;
				--uInputLength;
				DeleteNode(m_uSourceIndex);		// Delete old strings and read new bytes
				m_RingBuffer[m_uSourceIndex] = static_cast<Word8>(uInputByte);

				// If the position is near the end of buffer, extend the buffer to make
				// string comparison easier.

				if (m_uSourceIndex < (MAXMATCHLENGTH-1)) {
					m_RingBuffer[m_uSourceIndex + RINGBUFFERSIZE] = static_cast<Word8>(uInputByte);
				}

				// Since this is a ring buffer, increment the position modulo RINGBUFFERSIZE.

				++m_uSourceIndex;
				++m_uDestIndex;
				m_uSourceIndex = m_uSourceIndex & (RINGBUFFERSIZE - 1);
				m_uDestIndex = m_uDestIndex & (RINGBUFFERSIZE - 1);
				InsertNode(m_uDestIndex);	// Register the string in RingBuffer[r..r+MAXMATCHLENGTH-1]
			} while (++m_uMatchIterator<m_uPreviousMatchSize);

			// Time to emit a token

			// Insert a bit mask if one isn't pending

			if (!m_bOrMask) {
				m_uBitMaskOffset = m_Output.GetSize();
				m_Output.Append(static_cast<Word8>(0));
				m_bOrMask = 1;
				m_bBitMask = 0;
			}

			// Should it be encoded or raw?
			if (m_uMatchSize < (MINMATCHLENGTH+1)) {
					
				// Not long enough match. Send one byte.
				m_uMatchSize = 1;				// Force 1 byte for future loop
				m_bBitMask |= m_bOrMask;		// 'send one byte' flag
				m_Output.Append(static_cast<Word8>(m_RingBuffer[m_uDestIndex]));	// Send uncoded.
			} else {

				// Send position and length pair. Note MatchSize > MINMATCHLENGTH.

				Word uNewToken = m_uMatchOffset-m_uDestIndex;
				uNewToken = uNewToken&0xFFF;
				uNewToken = uNewToken|((m_uMatchSize-(MINMATCHLENGTH+1))<<12);
				// Note: This is put in the stream as little endian!!
				m_Output.Append(static_cast<Word16>(uNewToken));
			}

			m_bOrMask <<=1;				// Shift mask left one bit.
			// All filled?
			if (!m_bOrMask) {
				// Overwrite the value that's present in the stream (Stored a zero)
				m_Output.Overwrite(&m_bBitMask,1,m_uBitMaskOffset);
			}
			m_uPreviousMatchSize = m_uMatchSize;	// Length of the last match
			m_uMatchIterator = 0;
		} while (uInputLength);	// until length of string to be processed is zero
	}
	return COMPRESS_OKAY;
}

/***************************************

	Wrap up any pending bitmask writes

***************************************/

Burger::CompressLZSS::eError Burger::CompressLZSS::Finalize(void)
{
	// Finally, insert the whole string just read. The
	// global variables MatchSize and MatchOffset are set.
	if (m_uCachedLength) {
		do {
			// Update the binary tree if a node is to be purged
			// since it may have dropped past the RINGBUFFERSIZE offset
			do {					
				// After the end of text, no need to read, but
				// buffer may not be empty.

				DeleteNode(m_uSourceIndex);
				++m_uSourceIndex;
				++m_uDestIndex;
				// Wrap the indexes
				m_uSourceIndex = m_uSourceIndex & (RINGBUFFERSIZE - 1);
				m_uDestIndex = m_uDestIndex & (RINGBUFFERSIZE - 1);
				// Any new data to stream?
				if (m_uCachedLength) {
					--m_uCachedLength;
					InsertNode(m_uDestIndex);
				}
			} while (++m_uMatchIterator<m_uPreviousMatchSize);
			if (!m_uCachedLength) {
				break;
			}
			// MatchSize may be spuriously long near the end of text.

			if (m_uMatchSize > m_uCachedLength) {		// uPipedLen<=MAXMATCHLENGTH
				m_uMatchSize = m_uCachedLength;
			}

			// Insert a bit mask if one isn't pending

			if (!m_bOrMask) {
				m_bOrMask = 1;
				m_bBitMask = 0;
				m_uBitMaskOffset = m_Output.GetSize();
				m_Output.Append(static_cast<Word8>(0));
			}
	
			// Should it be encoded or raw?
			if (m_uMatchSize < (MINMATCHLENGTH+1)) {

				// Not long enough match. Send one byte.
				m_uMatchSize = 1;				// Force 1 byte for future loop
				m_bBitMask |= m_bOrMask;		// 'send one byte' flag
				m_Output.Append(static_cast<Word8>(m_RingBuffer[m_uDestIndex]));	// Send uncoded.
			} else {

				// Send position and length pair. Note MatchSize > MINMATCHLENGTH.
					
				Word uNewToken = m_uMatchOffset-m_uDestIndex;
				uNewToken = uNewToken&0xFFF;
				uNewToken = uNewToken|((m_uMatchSize-(MINMATCHLENGTH+1))<<12);
				// Note: This is put in the stream as little endian!!
				m_Output.Append(static_cast<Word16>(uNewToken));
			}

			m_bOrMask <<=1;				// Shift mask left one bit.
			// All filled?
			if (!m_bOrMask) {
				// Overwrite the value that's present in the stream (Stored a zero)
				m_Output.Overwrite(&m_bBitMask,1,m_uBitMaskOffset);
			}
			m_uPreviousMatchSize = m_uMatchSize;	// Length of the last match
			m_uMatchIterator = 0;
		} while (m_uCachedLength);	// until length of string to be processed is zero
	}

	// Final cleanup by updating the bit mask if needed

	if (m_bOrMask) {
		m_Output.Overwrite(&m_bBitMask,1,m_uBitMaskOffset);
	}
	return COMPRESS_OKAY;
}
