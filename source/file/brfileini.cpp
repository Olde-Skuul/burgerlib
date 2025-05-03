/***************************************

    Library to handle windows style configuration files (INI)

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfileini.h"
#include "brstringfunctions.h"
#include "brglobalmemorymanager.h"
#include "brfilemanager.h"
#include "brnumberstring.h"
#include "brnumberstringhex.h"
#include "brfloatingpoint.h"

#if defined(BURGER_WATCOM)
// Disable 'sizeof' operand contains compiler generated information
#pragma disable_message(549)
#endif

/*! ************************************

	\class Burger::FileINI::Generic
	\brief Base class for INI file line objects

	Base class used to contain line objects for INI files. This is
	used as is as a root object to start and end a linked list loop.

	\sa Burger::FileINI, Burger::FileINI::Entry, Burger::FileINI::Comment or Burger::FileINI::Section 

***************************************/

/*! ************************************

	\enum Burger::FileINI::Generic::eType
	\brief Enumeration to identify derived classes

	To remove the need for Run Time Type Information, all derived classes must
	self-identify by assigning themselves with a unique enumeration so
	a Generic class can be upcast to the true derived class.

	\sa Burger::FileINI, Burger::FileINI::Entry, Burger::FileINI::Comment or Burger::FileINI::Section 

***************************************/

/*! ************************************

	\brief Initialize a generic object
	\param uType Type enumeration for the derived class

***************************************/

Burger::FileINI::Generic::Generic(eType uType) : 
	// "this" pointer used in initializer
	BURGER_MSVC_SUPPRESS(4355)
	m_Link(this),			// Initialize the linked list parent
	m_eType(uType)			// Set the type
{
}

/*! ************************************

	\brief Call the destructor for a generic object

	Unlink itself from the linked list and release resources

	\note This is a base class, so the destructor is virtual

***************************************/

Burger::FileINI::Generic::~Generic() 
{
	// m_Link unlinks on destruction
}

/*! ************************************

	\fn Burger::FileINI::Generic *Burger::FileINI::Generic::GetNext(void) const
	\brief Get the pointer to the next object in the list

	Follow the linked list forward and get the item's pointer

	\return Pointer to the next Generic object in the list
	\sa GetPrevious(void) const

***************************************/

/*! ************************************

	\fn Burger::FileINI::Generic *Burger::FileINI::Generic::GetPrevious(void) const
	\brief Get the pointer to the previous object in the list

	Follow the linked list backward and get the item's pointer

	\return Pointer to the previous Generic object in the list
	\sa GetNext(void) const

***************************************/

/*! ************************************

	\fn Burger::FileINI::Generic::eType *Burger::FileINI::Generic::GetType(void) const
	\brief Get the enumeration of the derived class' type

	Returns the enumeration that will identify the type of derived class from Generic
	this class is. If it's not derived, the value will be \ref LINEENTRY_ROOT

	\return \ref eType enumeration
	\sa eType or Generic

***************************************/

/*! ************************************

	\fn void Burger::FileINI::Generic::InsertBefore(Generic *pGeneric)
	\brief Insert this object before this one in the linked list

	Add this object into the linked list BEFORE this object. If this is the root
	object, it's effectively adding the new object to the end of the list.

	\param pGeneric Pointer to the Generic object to insert in the list before this one
	\sa InsertAfter(Generic *)

***************************************/

/*! ************************************

	\fn void Burger::FileINI::Generic::InsertAfter(Generic *pGeneric)
	\brief Insert this object after this one in the linked list

	Add this object into the linked list AFTER this object. If this is the root
	object, it's effectively adding the new object to the start of the list.

	\param pGeneric Pointer to the Generic object to insert in the list after this one
	\sa InsertBefore(Generic *)

***************************************/




/*! ************************************

	\class Burger::FileINI::Comment
	\brief Untouched text object

	For every line in an INI file that's not part of the data, store
	the text in one of these objects. It will not be parsed and yet
	retained so when the file is saved, the comments will be intact.

	\note This will identify with \ref eType of \ref LINEENTRY_COMMENT

	\sa Burger::FileINI, Burger::FileINI::Entry, Burger::FileINI::Generic or Burger::FileINI::Section 

***************************************/

/*! ************************************

	\brief Create an empty comment object

***************************************/

Burger::FileINI::Comment::Comment() : 
	Generic(LINEENTRY_COMMENT),
	m_Comment()
{
}

/*! ************************************

	\brief Create a comment object with text

	\param pComment Text to store in this newly created object

***************************************/

Burger::FileINI::Comment::Comment(const char *pComment) : 
	Generic(LINEENTRY_COMMENT),
	m_Comment(pComment)
{
}

/*! ************************************

	\brief Call the destructor for a Comment object

	Unlink itself from the linked list and release the contained string

***************************************/

Burger::FileINI::Comment::~Comment()
{
	// Release the string
}

/*! ************************************

	\fn const char *Burger::FileINI::Comment::GetText(void) const
	\brief Return a pointer to the contained comment string

	\sa SetText(const char *) or SetText(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileINI::Comment::SetText(const char *pInput)
	\brief Set a new comment string

	Replace the contained comment string with a new one

	\param pInput Pointer to a "C" string
	\sa GetText(void) const or SetText(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileINI::Comment::SetText(const String *pInput)
	\brief Set a new comment string

	Replace the contained comment string with a new one

	\param pInput Pointer to a String class instance
	\sa GetText(void) const or SetText(const char *)

***************************************/

/*! ************************************

	\brief Allocate an empty comment object

***************************************/

Burger::FileINI::Comment * BURGER_API Burger::FileINI::Comment::new_object(void)
{
	// Allocate the memory
	return new (allocate_memory(sizeof(Comment))) Comment();
}

/*! ************************************

	\brief Allocate a comment object with text

	\param pComment Text to store in this newly created object

***************************************/

Burger::FileINI::Comment * BURGER_API Burger::FileINI::Comment::new_object(const char *pComment)
{
	// Allocate the memory
	return new (allocate_memory(sizeof(Comment))) Comment(pComment);
}



/*! ************************************

	\class Burger::FileINI::Entry
	\brief Key / Value pair

	INI files have data entries in the form of..

	\code
	Data = 1
	String = "Hello world"
	Number = 7.3232
	\endcode

	This class will contain the string preceding the '=' as a "Key" and the
	data after the '=' as the value. All values are stored as strings and 
	the method of query will determine if it is to be treated as an integer,
	float or string object.

	\note This will identify with \ref eType of \ref LINEENTRY_ENTRY

	\sa Burger::FileINI, Burger::FileINI::Comment, Burger::FileINI::Generic or Burger::FileINI::Section 

***************************************/

/*! ************************************

	\brief Create an empty key value pair object

***************************************/

Burger::FileINI::Entry::Entry() :
	Generic(LINEENTRY_ENTRY),
	m_Key(),
	m_Value()
{
}

/*! ************************************

	\brief Create a key value pair object initialized with a key and a value

	\param pKey "C" string containing the new key
	\param pValue "C" string containing the data attached to the key

***************************************/

Burger::FileINI::Entry::Entry(const char *pKey,const char *pValue) :
	Generic(LINEENTRY_ENTRY),
	m_Key(pKey),
	m_Value(pValue)
{
}

/*! ************************************

	\brief Call the destructor for an Entry object

	Unlink itself from the linked list and release the contained strings

***************************************/

Burger::FileINI::Entry::~Entry()
{
}

/*! ************************************

	\fn const char *Burger::FileINI::Entry::GetKey(void) const
	\brief Return a pointer to the contained key string

	\sa GetValue(void) const, SetKey(const char *) or SetKey(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileINI::Entry::SetKey(const char *pInput)
	\brief Set a new key string

	Replace the contained key string with a new one

	\param pInput Pointer to a "C" string
	\sa SetValue(const char *), GetKey(void) const or SetKey(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileINI::Entry::SetKey(const String *pInput)
	\brief Set a new key string

	Replace the contained key string with a new one

	\param pInput Pointer to a String class instance
	\sa SetValue(const String *), GetKey(void) const or SetKey(const char *)

***************************************/

/*! ************************************

	\fn const char *Burger::FileINI::Entry::GetValue(void) const
	\brief Return a pointer to the contained value string

	\sa GetKey(void) const, SetValue(const char *) or SetValue(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileINI::Entry::SetValue(const char *pInput)
	\brief Set a new value string

	Replace the contained value string with a new one

	\param pInput Pointer to a "C" string
	\sa SetKey(const char *), GetValue(void) const or SetValue(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileINI::Entry::SetValue(const String *pInput)
	\brief Set a new value string

	Replace the contained value string with a new one

	\param pInput Pointer to a String class instance
	\sa SetKey(const String *), GetValue(void) const or SetValue(const char *)

***************************************/

/*! ************************************

	\brief Return a boolean value (\ref TRUE or \ref FALSE)

	Scan the value string and if it matches the word TRUE or FALSE
	(Case insensitive), it will return \ref TRUE or \ref FALSE. If
	the value is numeric, it will return \ref TRUE if the
	value is not zero and \ref FALSE if not.

	If it's none of the above, then return the default value

	\param bDefault Value to return on error
	\return \ref TRUE or \ref FALSE or bDefault
	\sa SetBoolean(uint_t) or AsciiToBoolean(const char *,uint_t)

***************************************/

uint_t BURGER_API Burger::FileINI::Entry::GetBoolean(uint_t bDefault) const
{
	return m_Value.GetBoolean(bDefault);
}

/*! ************************************

	\brief Set a boolean value (\ref TRUE or \ref FALSE)

	If the input value is zero, set the string to "FALSE",
	otherwise set the string to "TRUE". It will not
	store numeric values.

	\param bValue Value to store as a string
	\sa GetBoolean(uint_t) const

***************************************/

void BURGER_API Burger::FileINI::Entry::SetBoolean(uint_t bValue)
{
	m_Value.SetTrueFalse(bValue);
}

/*! ************************************

	\brief Return an unsigned integer value

	Scan the value string as a 32 bit unsigned integer or
	hex value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234

	\param uDefault Value to return on error
	\param uMin Minimum acceptable value
	\param uMax Maximum acceptable value
	\return Value in between uMin and uMax or uDefault
	\sa GetInt(int_t,int_t,int_t) const, SetWord(uint_t), SetWordHex(uint_t), or AsciiToWord(const char *,uint_t,uint_t,uint_t)

***************************************/

uint_t BURGER_API Burger::FileINI::Entry::GetWord(uint_t uDefault,uint_t uMin,uint_t uMax) const
{
	return AsciiToWord(m_Value.c_str(),uDefault,uMin,uMax);
}

/*! ************************************

	\brief Set an unsigned integer value

	Convert the input into an unsigned integer UTF-8 string 
	and set the value to this string

	\param uValue Value to store as an unsigned integer string
	\sa GetWord(uint_t,uint_t,uint_t) const or SetWordHex(uint_t)

***************************************/

void BURGER_API Burger::FileINI::Entry::SetWord(uint_t uValue)
{
	m_Value.SetWord(uValue);
}

/*! ************************************

	\brief Set an unsigned integer value as hex

	Convert the input into a "C" style hex
	string in the format of 0x12345678 and
	store this string as the value

	\param uValue Value to store as an unsigned integer hex string
	\sa GetWord(uint_t,uint_t,uint_t) const or SetWord(uint_t)

***************************************/

void BURGER_API Burger::FileINI::Entry::SetWordHex(uint_t uValue)
{
	m_Value.SetWordHex(uValue);
}

/*! ************************************

	\brief Return a signed integer value

	Scan the value string as a 32 bit signed integer or
	hex value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234. 0xFFFFFFFF will be converted
	to -1.

	\param iDefault Value to return on error
	\param iMin Minimum acceptable value
	\param iMax Maximum acceptable value
    \return Value in between iMin and iMax or iDefault
    \sa GetWord(uint_t,uint_t,uint_t) const, SetInt(int_t) or AsciiToInteger(const char *,int_t,int_t,int_t)

***************************************/

int_t BURGER_API Burger::FileINI::Entry::GetInt(int_t iDefault,int_t iMin,int_t iMax) const
{
	return m_Value.GetInt(iDefault,iMin,iMax);
}

/*! ************************************

	\brief Set a signed integer value

	Convert the input into an signed integer UTF-8 string 
	and set the value to this string

	\param iValue Value to store as a signed integer string
	\sa GetInt(int_t,int_t,int_t) const or SetWord(uint_t)

***************************************/

void BURGER_API Burger::FileINI::Entry::SetInt(int_t iValue)
{
	m_Value.SetInt(iValue);
}

/*! ************************************

	\brief Return a floating point value

	Scan the value string as a 32 bit floating point
	numeric value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param fDefault Value to return on error
	\param fMin Minimum acceptable value
	\param fMax Maximum acceptable value
	\return Value in between fMin and fMax or fDefault
	\sa GetDouble(double,double,double) const, SetFloat(float) or AsciiToFloat(const char *,float,float,float)

***************************************/

float BURGER_API Burger::FileINI::Entry::GetFloat(float fDefault,float fMin,float fMax) const
{
	return m_Value.GetFloat(fDefault,fMin,fMax);
}

/*! ************************************

	\brief Set a 32 bit floating point value

	Convert the input into an floating point representation
	of a UTF-8 string and set the value to this string

	\param fValue Value to store as a floating point string
	\sa GetFloat(float,float,float) const or SetDouble(double)

***************************************/

void BURGER_API Burger::FileINI::Entry::SetFloat(float fValue)
{
	m_Value.SetFloat(fValue);
}

/*! ************************************

	\brief Return a 64 bit floating point value

	Scan the value string as a 64 bit floating point
	numeric value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param dDefault Value to return on error
	\param dMin Minimum acceptable value
	\param dMax Maximum acceptable value
	\return Value in between dMin and dMax or dDefault
	\sa GetFloat(float,float,float) const, SetDouble(double) or AsciiToDouble(const char *,double,double,double)

***************************************/

double BURGER_API Burger::FileINI::Entry::GetDouble(double dDefault,double dMin,double dMax) const
{
	return m_Value.GetDouble(dDefault,dMin,dMax);
}

/*! ************************************

	\brief Set a 64 bit floating point value

	Convert the input into an floating point representation
	of a UTF-8 string and set the value to this string

	\param dValue Value to store as a 64 bit floating point string
	\sa GetDouble(double,double,double) const or SetFloat(float)

***************************************/

void BURGER_API Burger::FileINI::Entry::SetDouble(double dValue)
{
	m_Value.SetDouble(dValue);
}

/*! ************************************

	\brief Locate and return a quoted string

	Return the string without enclosing quotes. The
	string can have quotes if double quotes were
	used.

	If it's none of the above, then return the default value

	\param pOutput Pointer to a String class to hold the resulting string
	\param pDefault String to return on error
	\sa SetString(const char *,const char *)

***************************************/

void BURGER_API Burger::FileINI::Entry::GetString(String *pOutput,const char *pDefault) const
{
	uintptr_t uLength = m_Value.length();
	// No string?
	if (!uLength) {
		// Use the default
		pOutput->assign(pDefault);
	} else {
		char Buffer[128];
		char *pBuffer = Buffer;
		// Will the buffer hold?
		if (uLength>=sizeof(Buffer)) {
			pBuffer = static_cast<char *>(allocate_memory(uLength+1));
		}
		ParseQuotedString(pBuffer,uLength+1,m_Value.c_str());
		pOutput->assign(pBuffer);
		if (pBuffer!=Buffer) {
			free_memory(pBuffer);
		}
	}
}

/*! ************************************

	\brief Locate an entry and set a new quoted string

	Search the entries in the Section and if found
	convert the input into an quoted string.

	If not found, create the entry with the data.

	\param pValue String to encapsulate with quotes
	\sa GetString(String *,const char *,const char *) const

***************************************/

void BURGER_API Burger::FileINI::Entry::SetString(const char *pValue)
{
	// Valid input?
	if (pValue && pValue[0]) {
		// Start with a quote
		m_Value = '"';
		// Any quotes?
		const char *pQuote = StringCharacter(pValue,'"');
		if (pQuote) {
			do {
				// Copy up the part that's not quoted
				m_Value.append(pValue,static_cast<uintptr_t>(pQuote-pValue));
				// Insert the double quote
				m_Value.append("\"\"",2);
				// Skip past the quote
				pValue = pQuote+1;
				// Keep looking
				pQuote = StringCharacter(pValue,'"');
			} while (pQuote);
		}
		// Append the remainder
		m_Value += pValue;
		// End quote
		m_Value += '"';
	} else {
		// End now
		m_Value.clear();
	}
}

/*! ************************************

	\brief Allocate an empty key value pair object

***************************************/

Burger::FileINI::Entry * BURGER_API Burger::FileINI::Entry::new_object(void)
{
	// Allocate the memory
	return new (allocate_memory(sizeof(Entry))) Entry();
}

/*! ************************************

	\brief Allocate a key value pair object initialized with a key and a value

	\param pKey "C" string containing the new key
	\param pValue "C" string containing the data attached to the key

***************************************/

Burger::FileINI::Entry * BURGER_API Burger::FileINI::Entry::new_object(const char *pKey,const char *pValue)
{
	// Allocate the memory
	return new (allocate_memory(sizeof(Entry))) Entry(pKey,pValue);
}


/*! ************************************

	\class Burger::FileINI::Section
	\brief INI section chunk

	INI files have data entries in the form of..

	\code
	Nameless = 1234
	[Section]
	Data = 1
	String = "Hello world"
	Number = 7.3232
	[NewSection]
	Data = 3
	\endcode

	When braces are found, a Section class is created and all entries following
	it will be inserted into the section and all data queries are
	done through the section. Since sections contain localized data,
	entry names can be reused because name collisions are avoided.

	Before the first section is found, a nameless section exists to hold data that
	isn't part of a data section. To access that data, pass an empty string as the
	section name.

	\note This will identify with \ref eType of \ref LINEENTRY_SECTION

	\sa Burger::FileINI, Burger::FileINI::Comment, Burger::FileINI::Generic or Burger::FileINI::Entry 

***************************************/

/*! ************************************

	\brief Create an empty Section object

***************************************/

Burger::FileINI::Section::Section() : 
	Generic(LINEENTRY_SECTION),
	m_Root(LINEENTRY_ROOT),
	m_SectionName()
{
}

/*! ************************************

	\brief Create a Section object initialized with a name

	\param pSection "C" string containing the new section name

***************************************/

Burger::FileINI::Section::Section(const char *pSection) : 
	Generic(LINEENTRY_SECTION),
	m_Root(LINEENTRY_ROOT),
	m_SectionName(pSection)
{
}

/*! ************************************

	\brief Call the destructor for a Section object

	Unlink itself from the linked list and release the contained strings
	and objects

***************************************/

Burger::FileINI::Section::~Section()
{
	Generic *pGeneric = m_Root.GetNext();
	if (pGeneric!=&m_Root) {
		// Dispose of all the objects in the list
		do {
			delete_object(pGeneric);
			// Since the object unlinked, pull from the root
			pGeneric = m_Root.GetNext();
		} while (pGeneric!=&m_Root);
	}
}

/*! ************************************

	\brief Save a script file

	Serialize the data contained in the section as a
	chunk meant for a Windows style INI text file.

	\param pOutput Output string to store the UTF-8 file image of the data
	\return Zero on success, non zero on error

***************************************/

uint_t BURGER_API Burger::FileINI::Section::Save(OutputMemoryStream *pOutput) const
{
	uint_t uResult=FALSE;
	// Is it nameless?
	const char *pName = m_SectionName.c_str();
	if (pName[0]) {
		// Output the section name
		pOutput->Append('[');
		pOutput->Append(pName);
		pOutput->Append("]\n");
	}
	const Generic *pEntry = m_Root.GetNext();
	if (pEntry!=&m_Root) {
		// Dispose of all the objects in the list
		do {
			// Upcast based on what it is
			switch (pEntry->GetType()) {
			case LINEENTRY_ENTRY:
				// Convert to "key = value"
				pOutput->Append(static_cast<const Entry *>(pEntry)->GetKey());
				pOutput->Append(" = ");
				pOutput->Append(static_cast<const Entry *>(pEntry)->GetValue());
				pOutput->Append('\n');
				break;
			case LINEENTRY_COMMENT:
				// Save as is
				pOutput->Append(static_cast<const Comment *>(pEntry)->GetText());
				pOutput->Append('\n');
				break;
			default:
				break;
			}
			pEntry = pEntry->GetNext();
		} while (pEntry!=&m_Root);
	}
	return uResult;
}

/*! ************************************

	\fn const char *Burger::FileINI::Section::GetName(void) const
	\brief Return a pointer to the section's name

	\sa SetName(const char *) or SetName(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileINI::Section::SetName(const char *pInput)
	\brief Set a new name for the section

	Replace the section name string with a new one

	\param pInput Pointer to a "C" string
	\sa GetName(void) const or SetName(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileINI::Section::SetName(const String *pInput)
	\brief Set a new name for the section

	Replace the section name string with a new one

	\param pInput Pointer to a String class instance
	\sa GetName(void) const or SetName(const char *)

***************************************/

/*! ************************************

	\brief Create a new entry and append it to the end of the section

	\param pKey Pointer to a "C" string for the new entry's key
	\param pValue Pointer to a "C" string for the new entry's value
	\return Pointer to the new entry or \ref NULL on an out of memory situation
	\sa FindEntry(const char *,uint_t) or AddComment(const char *)

***************************************/

Burger::FileINI::Entry * BURGER_API Burger::FileINI::Section::AddEntry(const char *pKey,const char *pValue)
{
	Entry *pEntry = Entry::new_object(pKey,pValue);
	if (pEntry) {
		m_Root.InsertBefore(pEntry);
	}
	return pEntry;
}

/*! ************************************

	\brief Locate an entry and return it if found

	\param pKey Pointer to a "C" of the key desired
	\return Pointer to the located entry or \ref NULL if the entry wasn't found
	\sa FindEntry(const char *,uint_t)

***************************************/

Burger::FileINI::Entry * BURGER_API Burger::FileINI::Section::FindEntry(const char *pKey) const
{
	const Generic *pGeneric = m_Root.GetNext();
	Entry *pResult = NULL;
	if (pGeneric!=&m_Root) {
		// Only match entry objects (Skip comment objects)
		do {
			if (pGeneric->GetType()==LINEENTRY_ENTRY) {
				if (!StringCaseCompare(static_cast<const Entry *>(pGeneric)->GetKey(),pKey)) {
					pResult = const_cast<Entry *>(static_cast<const Entry *>(pGeneric));
					break;
				}
			}
			pGeneric = pGeneric->GetNext();
		} while (pGeneric!=&m_Root);
	}
	return pResult;
}

/*! ************************************

	\brief Locate an entry and return it or create it if necessary

	\param pKey Pointer to a "C" of the key desired
	\param bAlwaysCreate If non-zero, and if the entry wasn't found, create a new entry with this desired key
	\return Pointer to the located key or \ref NULL if the key wasn't found or not enough memory to create a new entry
	\sa FindEntry(const char *) const

***************************************/

Burger::FileINI::Entry * BURGER_API Burger::FileINI::Section::FindEntry(const char *pKey,uint_t bAlwaysCreate)
{
	const Generic *pGeneric = m_Root.GetNext();
	Entry *pResult = NULL;
	const Generic *pLastEntry = &m_Root;
	if (pGeneric!=pLastEntry) {
		// Only match entry objects (Skip comment objects)
		do {
			if (pGeneric->GetType()==LINEENTRY_ENTRY) {
				if (!StringCaseCompare(static_cast<const Entry *>(pGeneric)->GetKey(),pKey)) {
					pResult = const_cast<Entry *>(static_cast<const Entry *>(pGeneric));
					break;
				}
				pLastEntry = pGeneric;
			}
			pGeneric = pGeneric->GetNext();
		} while (pGeneric!=&m_Root);
	}
	// Not found and create flag set?
	if (!pResult && bAlwaysCreate) {
		// Create it with no data
		pResult = Entry::new_object(pKey,"");
		if (pResult) {
			// Insert it after the last located entry (But before the last comments)
			// This will allow any spacing from the entries to the next section to be
			// maintained
			const_cast<Generic *>(pLastEntry)->InsertAfter(pResult);
		}
	}
	return pResult;
}

/*! ************************************

	\brief Create a new comment and append it to the end of the section

	\param pComment Pointer to a "C" string for the new entry's data
	\return Pointer to the new comment or \ref NULL on an out of memory situation
	\sa FindEntry(const char *,uint_t) or AddEntry(const char *,const char *)

***************************************/

Burger::FileINI::Comment * BURGER_API Burger::FileINI::Section::AddComment(const char *pComment)
{
	Comment *pEntry = Comment::new_object(pComment);
	if (pEntry) {
		m_Root.InsertBefore(pEntry);
	}
	return pEntry;
}

/*! ************************************

	\brief Locate an entry and return a pointer to the value string

	\param pKey Pointer to a "C" of the key desired
	\return Pointer to the "C" string located or \ref NULL if the key wasn't found
	\sa FindEntry(const char *) const, SetValue(const char *,const char *) or SetValue(const char *,const String *)

***************************************/

const char * BURGER_API Burger::FileINI::Section::GetValue(const char *pKey) const
{
	Entry *pEntry = FindEntry(pKey);
	if (pEntry) {
		return pEntry->GetValue();
	}
	return NULL;
}

/*! ************************************

	\brief Locate an entry and set a new value string

	Search the entries in the Section and if found
	replace the contained value string with a new one.

	If not found, create the entry with the data.

	\param pKey Pointer to a "C" of the key desired
	\param pInput Pointer to a "C" string of the new value
	\sa GetValue(const char *) const or SetValue(const char *,const String *)

***************************************/

void BURGER_API Burger::FileINI::Section::SetValue(const char *pKey,const char *pInput)
{
	Entry *pEntry = FindEntry(pKey,TRUE);
	if (pEntry) {
		pEntry->SetValue(pInput);
	}
}

/*! ************************************

	\brief Locate an entry and set a new value string

	Search the entries in the Section and if found
	replace the contained value string with a new one.

	If not found, create the entry with the data.

	\param pKey Pointer to a "C" of the key desired
	\param pInput Pointer to a String containing the new value
	\sa GetValue(const char *) const or SetValue(const char *,const char *)

***************************************/

void BURGER_API Burger::FileINI::Section::SetValue(const char *pKey,const String *pInput)
{
	Entry *pEntry = FindEntry(pKey,TRUE);
	if (pEntry) {
		pEntry->SetValue(pInput);
	}
}

/*! ************************************

	\brief Locate and return a boolean value (\ref TRUE or \ref FALSE)

	Search the entries in the Section and if found
	scan the value string and if it matches the word TRUE or FALSE
	(Case insensitive), it will return \ref TRUE or \ref FALSE. If
	the value is numeric, it will return \ref TRUE if the
	value is not zero and \ref FALSE if not.

	If it's none of the above, then return the default value

	\param pKey Pointer to a "C" of the key desired
	\param bDefault Value to return on error
	\return \ref TRUE or \ref FALSE or bDefault
	\sa SetBoolean(const char *,uint_t)

***************************************/

uint_t BURGER_API Burger::FileINI::Section::GetBoolean(const char *pKey,uint_t bDefault) const
{
	const Entry *pEntry = FindEntry(pKey);
	if (pEntry) {
		bDefault = pEntry->GetBoolean(bDefault);
	}
	return bDefault;
}

/*! ************************************

	\brief Locate an entry and set a new boolean string

	Search the entries in the Section and if found
	replace the contained value string with a new one for a boolean.

	If not found, create the entry with the data.

	\param pKey Pointer to a "C" of the key desired
	\param bValue Zero to store "FALSE" or non zero to store "TRUE"
	\sa GetBoolean(const char *,uint_t) const

***************************************/

void BURGER_API Burger::FileINI::Section::SetBoolean(const char *pKey,uint_t bValue)
{
	Entry *pEntry = FindEntry(pKey,TRUE);
	if (pEntry) {
		pEntry->SetBoolean(bValue);
	}
}

/*! ************************************

	\brief Locate and return an unsigned integer value

	Search the entries in the Section and if found
	scan the value string as a 32 bit unsigned integer or
	hex value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234

	If it's none of the above, then return the default value

	\param pKey Pointer to a "C" of the key desired
	\param uDefault Value to return on error
	\param uMin Minimum acceptable value
	\param uMax Maximum acceptable value
	\return Value in between uMin and uMax or uDefault
	\sa SetWord(const char *,uint_t) or GetInt(const char *,int_t,int_t,int_t) const

***************************************/

uint_t BURGER_API Burger::FileINI::Section::GetWord(const char *pKey,uint_t uDefault,uint_t uMin,uint_t uMax) const
{
	const Entry *pEntry = FindEntry(pKey);
	if (pEntry) {
		uDefault = pEntry->GetWord(uDefault,uMin,uMax);
	}
	return uDefault;
}

/*! ************************************

	\brief Locate an entry and set a new unsigned integer string

	Search the entries in the Section and if found
	replace the contained value string with a new one for an unsigned integer.

	If not found, create the entry with the data.

	\param pKey Pointer to a "C" of the key desired
	\param uValue Value to convert to an unsigned integer UTF-8 string
	\sa GetWord(const char *,uint_t,uint_t,uint_t) const and SetWordHex(const char *,uint_t)

***************************************/

void BURGER_API Burger::FileINI::Section::SetWord(const char *pKey,uint_t uValue)
{
	Entry *pEntry = FindEntry(pKey,TRUE);
	if (pEntry) {
		pEntry->SetWord(uValue);
	}
}

/*! ************************************

	\brief Locate an entry and set a new unsigned integer string

	Search the entries in the Section and if found
	replace the contained value string with a new one for a hex string for the unsigned integer.

	If not found, create the entry with the data.

	\param pKey Pointer to a "C" of the key desired
	\param uValue Value to convert to a hex string
	\sa GetWord(const char *,uint_t,uint_t,uint_t) const and SetWord(const char *,uint_t)

***************************************/

void BURGER_API Burger::FileINI::Section::SetWordHex(const char *pKey,uint_t uValue)
{
	Entry *pEntry = FindEntry(pKey,TRUE);
	if (pEntry) {
		pEntry->SetWordHex(uValue);
	}
}

/*! ************************************

	\brief Locate and return an unsigned integer value

	Search the entries in the Section and if found
	scan the value string as a 32 bit signed integer or
	hex value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234. 0xFFFFFFFF will be converted
	to -1.

	If it's none of the above, then return the default value

	\param pKey Pointer to a "C" of the key desired
	\param iDefault Value to return on error
	\param iMin Minimum acceptable value
	\param iMax Maximum acceptable value
	\return Value in between iMin and iMax or iDefault
	\sa SetInt(const char *,int_t) or GetWord(const char *,uint_t,uint_t,uint_t) const

***************************************/

int_t BURGER_API Burger::FileINI::Section::GetInt(const char *pKey,int_t iDefault,int_t iMin,int_t iMax) const
{
	Entry *pEntry = FindEntry(pKey);
	if (pEntry) {
		iDefault = pEntry->GetInt(iDefault,iMin,iMax);
	}
	return iDefault;
}

/*! ************************************

	\brief Locate an entry and set a new signed integer string

	Search the entries in the Section and if found
	convert the input into an signed integer UTF-8 string 
	and set the value to this string

	If not found, create the entry with the data.

	\param pKey Pointer to a "C" of the key desired
	\param iValue Value to convert to an signed integer UTF-8 string
	\sa GetInt(const char *,int_t,int_t,int_t) const and SetWord(const char *,uint_t)

***************************************/

void BURGER_API Burger::FileINI::Section::SetInt(const char *pKey,int_t iValue)
{
	Entry *pEntry = FindEntry(pKey,TRUE);
	if (pEntry) {
		pEntry->SetInt(iValue);
	}
}

/*! ************************************

	\brief Locate and return a 32 bit floating point value

	Search the entries in the Section and if found
	scan the value string as a 32 bit floating point
	numeric value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	If it's none of the above, then return the default value

	\param pKey Pointer to a "C" of the key desired
	\param fDefault Value to return on error
	\param fMin Minimum acceptable value
	\param fMax Maximum acceptable value
	\return Value in between fMin and fMax or fDefault
	\sa SetFloat(const char *,float) or GetDouble(const char *,double,double,double) const

***************************************/

float BURGER_API Burger::FileINI::Section::GetFloat(const char *pKey,float fDefault,float fMin,float fMax) const
{
	const Entry *pEntry = FindEntry(pKey);
	if (pEntry) {
		fDefault = pEntry->GetFloat(fDefault,fMin,fMax);
	}
	return fDefault;
}

/*! ************************************

	\brief Locate an entry and set a new 32 bit floating point string

	Search the entries in the Section and if found
	convert the input into an floating point representation
	of a UTF-8 string and set the value to this string

	If not found, create the entry with the data.

	\param pKey Pointer to a "C" of the key desired
	\param fValue Value to store as a floating point string
	\sa GetFloat(const char *,float,float,float) const and SetDouble(const char *,double)

***************************************/

void BURGER_API Burger::FileINI::Section::SetFloat(const char *pKey,float fValue)
{
	Entry *pEntry = FindEntry(pKey,TRUE);
	if (pEntry) {
		pEntry->SetFloat(fValue);
	}
}

/*! ************************************

	\brief Locate and return a 64 bit floating point value

	Search the entries in the Section and if found
	scan the value string as a 64 bit floating point
	numeric value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	If it's none of the above, then return the default value

	\param pKey Pointer to a "C" of the key desired
	\param dDefault Value to return on error
	\param dMin Minimum acceptable value
	\param dMax Maximum acceptable value
	\return Value in between dMin and dMax or dDefault
	\sa SetDouble(const char *,double) or GetFloat(const char *,float,float,float) const

***************************************/

double BURGER_API Burger::FileINI::Section::GetDouble(const char *pKey,double dDefault,double dMin,double dMax) const
{
	const Entry *pEntry = FindEntry(pKey);
	if (pEntry) {
		dDefault = pEntry->GetDouble(dDefault,dMin,dMax);
	}
	return dDefault;
}

/*! ************************************

	\brief Locate an entry and set a new 64 bit floating point string

	Search the entries in the Section and if found
	convert the input into an floating point representation
	of a UTF-8 string and set the value to this string

	If not found, create the entry with the data.

	\param pKey Pointer to a "C" of the key desired
	\param dValue Value to store as a floating point string
	\sa GetDouble(const char *,double,double,double) const and SetFloat(const char *,float)

***************************************/

void BURGER_API Burger::FileINI::Section::SetDouble(const char *pKey,double dValue)
{
	Entry *pEntry = FindEntry(pKey,TRUE);
	if (pEntry) {
		pEntry->SetDouble(dValue);
	}
}

/*! ************************************

	\brief Locate and return a quoted string

	Search the entries in the Section and if found
	return the string without enclosing quotes. The
	string can have quotes if double quotes were
	used.

	If it's none of the above, then return the default value

	\param pOutput Pointer to a String class to hold the resulting string
	\param pKey Pointer to a "C" of the key desired
	\param pDefault String to return on error
	\sa SetString(const char *,const char *)

***************************************/

void BURGER_API Burger::FileINI::Section::GetString(String *pOutput,const char *pKey,const char *pDefault) const
{
	const Entry *pEntry = FindEntry(pKey);
	if (pEntry) {
		pEntry->GetString(pOutput,pDefault);
	} else {
		pOutput->assign(pDefault);
	}
}

/*! ************************************

	\brief Locate an entry and set a new quoted string

	Search the entries in the Section and if found
	convert the input into an quoted string.

	If not found, create the entry with the data.

	\param pKey Pointer to a "C" of the key desired
	\param pValue String to encapsulate with quotes
	\sa GetString(String *,const char *,const char *) const

***************************************/

void BURGER_API Burger::FileINI::Section::SetString(const char *pKey,const char *pValue)
{
	Entry *pEntry = FindEntry(pKey,TRUE);
	if (pEntry) {
		pEntry->SetString(pValue);
	}
}

/*! ************************************

	\brief Allocate an empty Section object

***************************************/

Burger::FileINI::Section * BURGER_API Burger::FileINI::Section::new_object(void)
{
	// Allocate the memory
	return new (allocate_memory(sizeof(Section))) Section();
}

/*! ************************************

	\brief Allocate a Section object initialized with a name

	\param pSection "C" string containing the new section name

***************************************/

Burger::FileINI::Section * BURGER_API Burger::FileINI::Section::new_object(const char *pSection)
{
	// Allocate the memory
	return new (allocate_memory(sizeof(Section))) Section(pSection);
}

/*! ************************************

	\class Burger::FileINI
	\brief Reads and writes INI files

	This class will read and write out INI text files
	Care is taken that all comments and extra, non-parsable, data is
	retained so when the file is modified and written back out,
	all of the extra data is untouched.

	\code
	# This is a sample of the contents
	# of a legal Windows INI file

	// C style comments are okay!

	# Section without a name, access by passing "" as the section name
	Data = 1
	String = "Hello there"

	# Section, access with the string "Foobar" (It's case insensitive)
	[FOOBAR]
	Data = 2
	Hi = "Hi"

	# Another section with duplicate names
	[foobar2]
	Data = 2
	Float = 3.52

	\endcode

	\sa Burger::FileXML

***************************************/

/*! ************************************

	\brief Initialize an INI file parser

***************************************/

Burger::FileINI::FileINI(void) :
	m_Root(Generic::LINEENTRY_ROOT)
{
}

/*! ************************************

	\brief Dispose of an INI file parser
	\sa Shutdown(void)

***************************************/

Burger::FileINI::~FileINI()
{
	Shutdown();
}

/*! ************************************

	\brief Allocate and initialize a FileINI

	\return A pointer to an empty FileINI structure or \ref NULL if out of memory
	\sa new_object(InputMemoryStream *,uint_t), new_object(const char *,uint_t) or new_object(Filename *,uint_t)

***************************************/

Burger::FileINI * BURGER_API Burger::FileINI::new_object(void)
{
	// Allocate the memory
	return new (allocate_memory(sizeof(FileINI))) FileINI();
}

/*! ************************************

	\brief Create a new pref file record and initialize it with entries from a text file on disk

	Open the file and read in the entries into a newly allocated FileINI structure.

	Return \ref NULL if the file cannot be opened and the bAlwaysCreate flag is zero

	\param pFilename Pointer to a "C" string of a valid Burgerlib filename
	\param bAlwaysCreate \ref TRUE if the file can't be opened, return an empty record instead
	\return A pointer to an empty FileINI structure or \ref NULL if out of memory
	\sa new_object(InputMemoryStream *,uint_t), new_object(void) or new_object(Filename *,uint_t)

***************************************/

Burger::FileINI * BURGER_API Burger::FileINI::new_object(const char *pFilename,uint_t bAlwaysCreate)
{
	// Convert to a filename object
	Filename NewName(pFilename);
	return new_object(&NewName,bAlwaysCreate);
}

/*! ************************************

	\brief Create a new pref file record and initialize it with entries from a text file on disk

	Open the file and read in the entries into a newly allocated FileINI structure.

	Return \ref NULL if the file cannot be opened and the bAlwaysCreate flag is zero

	\param pFilename Pointer to a Filename record
	\param bAlwaysCreate \ref TRUE if the file can't be opened, return an empty record instead
	\return A pointer to an empty FileINI structure or \ref NULL if out of memory
	\sa new_object(InputMemoryStream *,uint_t), new_object(void) or new_object(const char *,uint_t)

***************************************/

Burger::FileINI * BURGER_API Burger::FileINI::new_object(Filename *pFilename,uint_t bAlwaysCreate)
{
	InputMemoryStream Stream;
	FileINI *pResult;
	// Load into a stream
	if (!Stream.Open(pFilename)) {
		// Create the record
		pResult = new_object(&Stream,bAlwaysCreate);
	} else if (!bAlwaysCreate) {
		pResult = NULL;
	} else {
		// Create an empty record on missing file or file read error
		pResult = new_object();
	}
	return pResult;
}

/*! ************************************

	\brief Create a new pref file record and initialize it with entries from a text file on disk

	Read in the entries from the InputMemoryStream into a newly allocated FileINI structure.

	Return \ref NULL if the file cannot be opened and the bAlwaysCreate flag is zero

	\param pInput Pointer to a InputMemoryStream record that has the text file image
	\param bAlwaysCreate \ref TRUE if the file can't be opened, return an empty record instead
	\return A pointer to an empty FileINI structure or \ref NULL if out of memory
	\sa new_object(Filename *,uint_t), new_object(void) or new_object(const char *,uint_t)

***************************************/

Burger::FileINI * BURGER_API Burger::FileINI::new_object(InputMemoryStream *pInput,uint_t bAlwaysCreate)
{
	FileINI *pResult = new_object();	// Init the structure
	if (pResult) {
		// Fill in the data
		if (pResult->Init(pInput)) {
			// Failure? Destroy or return a class instance anyways?
			if (!bAlwaysCreate) {
				// On error, delete it
				delete_object(pInput);
				pInput = NULL;
			}
		}
	}
	return pResult;
}

/*! ************************************

	\brief Initialize the records with a file on disk

	Purge all records from the class and load in
	new data from a file on disk.

	\param pFilename Pointer to a "C" string of a valid Burgerlib filename
	\return Zero on success, or non-zero on failure
	\sa Init(InputMemoryStream *) or Init(const char *)

***************************************/

uint_t BURGER_API Burger::FileINI::Init(const char *pFilename)
{
	// Convert to a filename object
	Filename NewName(pFilename);
	return Init(&NewName);
}

/*! ************************************

	\brief Initialize the records with a file on disk

	Purge all records from the class and load in
	new data from a file on disk.

	\param pFilename Pointer to a Filename structure
	\return Zero on success, or non-zero on failure
	\sa Init(InputMemoryStream *) or Init(const char *)

***************************************/

uint_t BURGER_API Burger::FileINI::Init(Filename *pFilename)
{
	// Purge
	Shutdown();
	InputMemoryStream Stream;
	uint_t uResult = 10;
	// Load into a stream
	if (!Stream.Open(pFilename)) {
		// Create the record
		uResult = Init(&Stream);
	}
	return uResult;
}

/*! ************************************

	\brief Initialize the records with an InputMemoryStream

	Purge all records from the class and load in
	new data from a memory stream

	\param pInput Pointer to an image of the text file in memory
	\return Zero on success, or non-zero on failure
	\sa Init(Filename *) or Init(const char *)

***************************************/

uint_t BURGER_API Burger::FileINI::Init(InputMemoryStream *pInput)
{
	// Purge
	Shutdown();

	char OneLine[1024];		// Work buffer for a single line of text
	char LineCopy[1024];	// Copy of the line
	Section *pSection = NULL;

	uint_t uResult = 0;
	while (pInput->BytesRemaining()) {			// Any input?
		// Grab a single line of input
		pInput->GetString(OneLine,sizeof(OneLine));

		// Clean it up
		string_copy(LineCopy,OneLine);
		StripLeadingAndTrailingWhiteSpace(LineCopy);

		// Let's parse the text (I can mangle it)

		uint_t uLetter = reinterpret_cast<uint8_t *>(LineCopy)[0];
		if (uLetter=='[') {		// [Section]?
			char *pEnd = StringCharacter(LineCopy,']');	// Section end char?
			if (pEnd) {
				pEnd[0] = 0;							// End the string here
				pSection = AddSection(LineCopy+1);		// Make a section
				continue;
			}
		}
		if (!pSection) {
			pSection = Section::new_object();
			m_Root.InsertAfter(pSection);
		}
		// Is this a Foo = Bar entry?
		// Skip C++ and ASM style comments

		if ((uLetter!='=') && 
			(uLetter!=';') && 
			(uLetter!='#') && 
			!(uLetter=='/' && LineCopy[1]=='/')) {
			char *pParameter = StringCharacter(LineCopy,'=');	// Is there an equals in the middle?
			if (pParameter) {
				pParameter[0] = 0;		// Remove the equals
				++pParameter;

				// Remove whitespace again
				StripTrailingWhiteSpace(LineCopy);
				StripLeadingWhiteSpace(pParameter);
				// Is there a token name?
				if (LineCopy[0]) {
					pSection->AddEntry(LineCopy,pParameter);
					continue;		// Next line
				}
			}
		}
		// It's a comment, save the line in its untouched form
		pSection->AddComment(OneLine);
	}
	return uResult;		// Return the structure 
}

/*! ************************************

	\brief Dispose of the contents of the structure

	Every record is released from memory

***************************************/

void BURGER_API Burger::FileINI::Shutdown(void)
{
	Generic *pGeneric = m_Root.GetNext();
	if (pGeneric!=&m_Root) {
		// Dispose of all the objects in the list
		do {
			delete_object(pGeneric);
			// delete_object() unlinks the entry, fetch from the root
			pGeneric = m_Root.GetNext();
		} while (pGeneric!=&m_Root);
	}
}

/*! ************************************

	\brief Save a script file to disk

	Serialize the data contained in the entire class 
	as a Windows style INI text file and save
	it to a file.

	\param pFilename Pointer to a Burgerlib Path name
	\return Zero on success, non zero on error

***************************************/

uint_t BURGER_API Burger::FileINI::Save(const char *pFilename) const
{
	OutputMemoryStream Output;
	uint_t uResult = Save(&Output);
	if (!uResult) {
		uResult = Output.SaveFile(pFilename);
	}
	return uResult;
}

/*! ************************************

	\brief Save a script file to disk

	Serialize the data contained in the entire class 
	as a Windows style INI text file and save
	it to a file.

	\param pFilename Pointer to a \ref Filename instance
	\return Zero on success, non zero on error

***************************************/

uint_t BURGER_API Burger::FileINI::Save(Filename *pFilename) const
{
	OutputMemoryStream Output;
	uint_t uResult = Save(&Output);
	if (!uResult) {
		uResult = Output.SaveFile(pFilename);
	}
	return uResult;
}

/*! ************************************

	\brief Save a script file

	Serialize the data contained in the entire class 
	as a Windows style INI text file.

	\param pOutput Output string to store the UTF-8 file image of the data
	\return Zero on success, non zero on error

***************************************/

uint_t BURGER_API Burger::FileINI::Save(OutputMemoryStream *pOutput) const
{
	uint_t uResult=FALSE;
	const Generic *pSection = m_Root.GetNext();
	if (pSection!=&m_Root) {
		// Iterate and save the data in all sections
		do {
			if (pSection->GetType()==Generic::LINEENTRY_SECTION) {
				uResult = static_cast<const Section *>(pSection)->Save(pOutput);
				if (uResult) {
					break;
				}
			}
			pSection = pSection->GetNext();
		} while (pSection!=&m_Root);
	}
	return uResult;
}

/*! ************************************

	\brief Create a new section and append it to the end of the INI file

	Allocate a new Section and name it with pSectionName. Append
	the new Section at the end of the INI file records

	\param pSectionName Pointer to a "C" string of the new section to create
	\sa FindSection(const char *,uint_t) or DeleteSection(const char *)

***************************************/

Burger::FileINI::Section * BURGER_API Burger::FileINI::AddSection(const char *pSectionName)
{
	Section *pSection = Section::new_object(pSectionName);
	if (pSection) {
		// Insert at the end of the list
		m_Root.InsertBefore(pSection);
	}
	return pSection;
}

/*! ************************************

	\brief Find a Section
	
	Traverse the linked list for a section that has the requested name. Return
	\ref NULL if the record wasn't found and bAlwaysCreate is zero. If
	bAlwaysCreate is not zero and the record didn't exist, create one with
	the requested name.
	
	\param pSectionName Pointer to a "C" string of the new section to locate
	\param bAlwaysCreate \ref TRUE to create the section if one didn't already exist
	\return Pointer to a Section with the requested name or \ref NULL if not found
	\sa DeleteSection(const char *) or AddSection(const char *)

***************************************/

Burger::FileINI::Section * BURGER_API Burger::FileINI::FindSection(const char *pSectionName,uint_t bAlwaysCreate)
{
	Generic *pGeneric = m_Root.GetNext();
	Section *pResult = NULL;
	if (pGeneric!=&m_Root) {
		// Iterate over all the objects in the list
		do {
			if (pGeneric->GetType()==Generic::LINEENTRY_SECTION) {
				if (!StringCaseCompare(static_cast<const Section *>(pGeneric)->GetName(),pSectionName)) {
					pResult = static_cast<Section *>(pGeneric);
					break;
				}
			}
			pGeneric = pGeneric->GetNext();
		} while (pGeneric!=&m_Root);
	}
	if (!pResult && bAlwaysCreate) {
		pResult = AddSection(pSectionName);
	}
	return pResult;
}

/*! ************************************

	\brief Delete a named Section
	
	Traverse the linked list for a section
	that has the requested name and delete it and all
	data attached to it.

	\param pSectionName Pointer to a "C" string of the new section to locate
	\sa FindSection(const char *,uint_t) or AddSection(const char *)

***************************************/

void BURGER_API Burger::FileINI::DeleteSection(const char *pSectionName)
{
	Section *pSection = FindSection(pSectionName);
	delete_object(pSection);
}
