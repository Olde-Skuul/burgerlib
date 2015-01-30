/***************************************

	Flash player Adobe Virtual Machine Version 2 support
	
	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#include "brflashavm2.h"
#include "brflashmanager.h"
#include "brdebug.h"

/*! ************************************

	\struct Burger::Flash::OptionDetail_t
	\brief Optional class parameters

	The OptionDetail_t entry is used to define the default
	values for the optional parameters of the method.

	Full documentation is found on page 25 of
	the file avm2overview.pdf

	\sa OptionInfo

***************************************/

/*! ************************************

	\brief Read from an input stream

	Parse the data from the stream to fill in the structure.

	\param pStream Pointer to the input stream
	\sa ReferenceCounter

***************************************/

void BURGER_API Burger::Flash::OptionDetail_t::Read(Stream *pStream)
{
	m_uValue = pStream->GetEncodedU30();
	m_eKind = static_cast<eKind>(pStream->GetByte());
}



/*! ************************************

	\struct Burger::Flash::ItemInfo_t
	\brief Optional key/value pairs

	The ItemInfo_t entry is used to define a
	key / value pair for the MetaDataInfo
	class.

	Full documentation is found on page 27 of
	the file avm2overview.pdf

	\sa MetadataInfo

***************************************/

/*! ************************************

	\brief Read from an input stream

	Parse the data from the stream to fill in the structure.

	\param pStream Pointer to the input stream
	\sa MetadataInfo

***************************************/

void BURGER_API Burger::Flash::ItemInfo_t::Read(Stream *pStream)
{
	m_uKey = pStream->GetEncodedU30();
	m_uValue = pStream->GetEncodedU30();
}




/*! ************************************

	\class Burger::Flash::TraitsInfo
	\brief Optional class traits

	A trait is a fixed property of an object or class; it has a
	name, a type, and some associated data. The
	TraitsInfo structure bundles these data.

	Full documentation is found on page 29 of
	the file avm2overview.pdf

	\sa OptionInfo

***************************************/

/*! ************************************

	\brief Standard constructor

	Initialize to defaults

	\sa TraitsInfo

***************************************/

Burger::Flash::TraitsInfo::TraitsInfo() :
	m_uName(0),
	m_eKind(Trait_Invalid),
	m_eAttribute(0),
	m_uMetadataArray()
{
	trait_slot.m_uSlotID = 0;
	trait_slot.m_uTypeName = 0;
	trait_slot.m_uVIndex = 0;
	trait_slot.m_eKind = OptionDetail_t::CONSTANT_Undefined;
}

/*! ************************************

	\brief Standard destructor

	Release any allocated data

	\sa TraitsInfo()

***************************************/

Burger::Flash::TraitsInfo::~TraitsInfo()
{
}

/*! ************************************

	\fn void Burger::Flash::TraitsInfo::Read(Flash::Stream* pStream,gameswf::ABC_Definition *pABCDefinition)
	\brief Read in the data from the file stream

	Parse this record from the input stream

	\param pStream Pointer to the input stream
	\param pABCDefinition Pointer to the parent Adobe Byte Code
	
***************************************/




/*! ************************************

	\struct Burger::Flash::MetadataInfo
	\brief Data to represent metadata_info

	The MetadataInfo entry is used to define all
	of the key / value pair for an
	ABC file.

	Full documentation is found on page 27 of
	the file avm2overview.pdf

	\sa ItemInfo_t

***************************************/

/*! ************************************

	\brief Standard constructor

	Initialize the array

***************************************/

Burger::Flash::MetadataInfo::MetadataInfo() :
	m_uName(0),
	m_ItemInfoArray()
{

}

/*! ************************************

	\brief Standard destructor

	Release all memory

***************************************/

Burger::Flash::MetadataInfo::~MetadataInfo()
{
}

/*! ************************************

	\brief Read from an input stream

	Parse the data from the stream to fill in the structure.

	\param pStream Pointer to the input stream
	\sa ItemInfo_t::Read(Stream *)

***************************************/

void BURGER_API Burger::Flash::MetadataInfo::Read(Stream *pStream)
{
	m_uName = pStream->GetEncodedU30();
	Word uCount = pStream->GetEncodedU30();
	m_ItemInfoArray.reserve(uCount);
	if (uCount) {
		ItemInfo_t *pData = m_ItemInfoArray.GetPtr();
		do {
			pData->Read(pStream);
			++pData;
		} while (--uCount);
	}
}



/*! ************************************

	\class Burger::Flash::ExceptionInfo
	\brief Data to represent exception_info

	The ExceptionInfo entry is used to define
	an exception handler

	Full documentation is found on page 34 of
	the file avm2overview.pdf

***************************************/

/*! ************************************

	\brief Standard constructor

	Initialize the WeakPointer data

***************************************/

Burger::Flash::ExceptionInfo::ExceptionInfo() :
	m_uFrom(0),
	m_uTo(0),
	m_uTarget(0),
	m_uExceptionType(0),
	m_uVariableName(0)
{
}

/*! ************************************

	\brief Standard destructor

	Signal all \ref WeakPointer references

***************************************/

Burger::Flash::ExceptionInfo::~ExceptionInfo()
{
}

/*! ************************************

	\brief Read from an input stream

	Parse the data from the stream to fill in the structure.

	\param pStream Pointer to the input stream

***************************************/

void BURGER_API Burger::Flash::ExceptionInfo::Read(Stream* pStream)
{
	m_uFrom = pStream->GetEncodedU30();
	m_uTo = pStream->GetEncodedU30();
	m_uTarget = pStream->GetEncodedU30();
	m_uExceptionType = pStream->GetEncodedU30();
	m_uVariableName = pStream->GetEncodedU30();
}




/*! ************************************

	\class Burger::Flash::InstanceInfo
	\brief Data to represent instance_info

	The InstanceInfo entry is used to define
	interfaces and traits for a class instance

	Full documentation is found on page 28 of
	the file avm2overview.pdf

***************************************/

/*! ************************************

	\brief Standard constructor

	Initialize the WeakPointer data

***************************************/

Burger::Flash::InstanceInfo::InstanceInfo() :
	m_pParentABCDefinition(),
	m_uInterfaceArray(),
	m_pTraitArray(),
	m_uName(0),
	m_uSuperName(0),
	m_uFlags(0),
	m_uProtectedNamespace(0),
	m_uInitializationIndex(0)
{
}

/*! ************************************

	\brief Standard destructor

	Signal all \ref WeakPointer references and
	release all memory

***************************************/

Burger::Flash::InstanceInfo::~InstanceInfo()
{
}

/*! ************************************

	\fn void Burger::Flash::InstanceInfo::Read(Stream* pStream,gameswf::ABC_Definition *pABCDefinition)
	\brief Read from an input stream

	Parse the data from the stream to fill in the structure.

	\param pStream Pointer to the input stream
	\param pABCDefinition Pointer to the connected ABC_Definition

***************************************/



/*! ************************************

	\class Burger::Flash::ClassInfo
	\brief Data to represent class_info

	The ClassInfo entry is used to define
	characteristics of an ActionScript 3.0 class.

	Full documentation is found on page 31 of
	the file avm2overview.pdf

***************************************/

/*! ************************************

	\brief Standard constructor

	Initialize the WeakPointer data

***************************************/

Burger::Flash::ClassInfo::ClassInfo() :
	m_pParentABCDefinition(),
	m_pTraitArray(),
	m_uClassInit(0)
{
}

/*! ************************************

	\brief Standard destructor

	Signal all \ref WeakPointer references and
	release all memory

***************************************/

Burger::Flash::ClassInfo::~ClassInfo()
{
}




/*! ************************************

	\class Burger::Flash::ScriptInfo
	\brief Data to represent script_info

	The ScriptInfo entry is used to define
	characteristics of an ActionScript 3.0 class.

	Full documentation is found on page 32 of
	the file avm2overview.pdf

***************************************/

/*! ************************************

	\brief Standard constructor

	Initialize the WeakPointer data

***************************************/

Burger::Flash::ScriptInfo::ScriptInfo() :
	m_pTraitArray(),
	m_uScriptInit(0)
{
}

/*! ************************************

	\brief Standard destructor

	Signal all \ref WeakPointer references and
	release all memory

***************************************/

Burger::Flash::ScriptInfo::~ScriptInfo()
{
}