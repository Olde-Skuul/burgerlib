/***************************************

	Library to handle xml files (XML)

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilexml.h"
#include "brstringfunctions.h"
#include "brglobalmemorymanager.h"
#include "brfilemanager.h"
#include "brnumberstring.h"
#include "brnumberstringhex.h"
#include "brfloatingpoint.h"
#include "brfixedpoint.h"
#include "brutf8.h"

#if defined(BURGER_MSVC)
#pragma warning(disable:4355)		// "this" pointer used in initializer
#endif

/*! ************************************

	\class Burger::FileXML::Generic
	\brief Base class for XML file objects

	This base class is used to contain objects for XML files. It is
	used as is as a root object to start and end a linked list loop.

	\sa FileXML, FileXML::Element, FileXML::Comment or FileXML::Attribute

***************************************/

/*! ************************************

	\enum Burger::FileXML::Generic::eType
	\brief Enumeration to identify derived classes

	To remove the need for Run Time Type Information, all derived classes must
	self-identify by assigning themselves with a unique enumeration so
	a Generic class can be upcast to the true derived class.

	\sa FileXML, FileXML::Element, FileXML::Comment or FileXML::Attribute 

***************************************/

/*! ************************************

	\brief Initialize a generic object
	\param uType Type enumeration for the derived class

***************************************/

Burger::FileXML::Generic::Generic(eType uType) : 
	m_Link(this),			// Initialize the linked list parent
	m_eType(uType)			// Set the type
{
}

/*! ************************************

	\brief Call the destructor for a generic object

	Unlink itself from the linked list and release resources

	\note This is a base class, so the destructor is virtual

***************************************/

Burger::FileXML::Generic::~Generic() 
{
	// m_Link unlinks on destruction
}

/*! ************************************

	\brief Parse data from a data stream

	Parse data from the stream for this object and return zero if
	no error, or non-zero on error

	\param pInput Pointer to an input stream
	\return Error code of zero for no error or non-zero on error

***************************************/

Word Burger::FileXML::Generic::Parse(InputMemoryStream * /* pInput */)
{
	return 0;
}

/*! ************************************

	\brief Write data to a data stream

	Write the contents of this object into the XML file

	\param pOutput Pointer to an output stream
	\param uDepth Number of tabs to output before saving out this record
	\return Error code of zero for no error or non-zero on error

***************************************/

Word Burger::FileXML::Generic::Save(OutputMemoryStream * /* pOutput */,Word /* uDepth */) const
{
	return 0;
}

/*! ************************************

	\fn Burger::FileXML::Generic *Burger::FileXML::Generic::GetNext(void) const
	\brief Get the pointer to the next object in the list

	Follow the linked list forward and get the item's pointer

	\return Pointer to the next Generic object in the list
	\sa GetPrevious(void) const

***************************************/

/*! ************************************

	\fn Burger::FileXML::Generic *Burger::FileXML::Generic::GetPrevious(void) const
	\brief Get the pointer to the previous object in the list

	Follow the linked list backward and get the item's pointer

	\return Pointer to the previous Generic object in the list
	\sa GetNext(void) const

***************************************/

/*! ************************************

	\fn Burger::FileXML::Generic::eType *Burger::FileXML::Generic::GetType(void) const
	\brief Get the enumeration of the derived class' type

	Returns the enumeration that will identify the type of derived class from Generic
	this class is. If it's not derived, the value will be \ref XML_ROOT

	\return \ref eType enumeration
	\sa eType or Generic

***************************************/

/*! ************************************

	\fn void Burger::FileXML::Generic::InsertBefore(Generic *pGeneric)
	\brief Insert this object before this one in the linked list

	Add this object into the linked list BEFORE this object. If this is the root
	object, it's effectively adding the new object to the end of the list.

	\param pGeneric Pointer to the Generic object to insert in the list before this one
	\sa InsertAfter(Generic *)

***************************************/

/*! ************************************

	\fn void Burger::FileXML::Generic::InsertAfter(Generic *pGeneric)
	\brief Insert this object after this one in the linked list

	Add this object into the linked list AFTER this object. If this is the root
	object, it's effectively adding the new object to the start of the list.

	\param pGeneric Pointer to the Generic object to insert in the list after this one
	\sa InsertBefore(Generic *)

***************************************/

/*! ************************************

	\brief Follow the linked list of items to the next Element

	Starting from the supplied object, iterate over the linked list until
	either an Element is found or a Root object is found. If it's
	a Root object, return \ref NULL. Otherwise, return the pointer
	to the Element object.

	\return \ref NULL if there are no more Element objects or a valid Element pointer
	\sa GetNextItem(void) const

***************************************/

Burger::FileXML::Element * BURGER_API Burger::FileXML::Generic::GetNextElement(void) const
{
	const Generic *pGeneric = this;		// Start with the current object
	Element *pElement = NULL;			// Assume no elements found
	eType uType;
	do {
		// Iterate to the next object
		pGeneric = pGeneric->GetNext();
		// Get the type
		uType = pGeneric->GetType();
		if (uType==XML_ELEMENT) {
			pElement = static_cast<Element *>(const_cast<Generic *>(pGeneric));
			break;
		}
	// Not an "end of list" record?
	} while (uType!=XML_ROOT);
	return pElement;
}

/*! ************************************

	\brief Follow the linked list of items to the next named Element

	Starting from the supplied object, iterate over the linked list until
	either an Element with the requested name is found or a Root object is found. If it's
	a Root object, return \ref NULL. Otherwise, return the pointer
	to the Element object.

	\param pElementName Pointer to a "C" string with the name of the requested Element
	\return \ref NULL if there are no more Element objects or a valid Element pointer
	\sa GetNextItem(void) const or GetNextElement(void) const

***************************************/

Burger::FileXML::Element * BURGER_API Burger::FileXML::Generic::GetNextElement(const char *pElementName) const
{
	const Generic *pGeneric = this;		// Start with the current object
	Element *pElement = NULL;			// Assume no elements found
	eType uType;
	do {
		// Iterate to the next object
		pGeneric = pGeneric->GetNext();
		// Get the type
		uType = pGeneric->GetType();
		if (uType==XML_ELEMENT) {
			if (!StringCompare(pElementName,static_cast<const Element *>(pGeneric)->GetName())) {
				pElement = static_cast<Element *>(const_cast<Generic *>(pGeneric));
				break;
			}
		}
		// Not an "end of list" record?
	} while (uType!=XML_ROOT);
	return pElement;
}

/*! ************************************

	\brief Follow the linked list of items to the next object

	Starting from the supplied object, iterate over the linked list one
	entry and if it's a Root object, return \ref NULL. Otherwise, return the pointer
	to the Generic object.

	\return \ref NULL if there are no more objects or a valid Generic pointer
	\sa GetNextElement(void) const

***************************************/

Burger::FileXML::Generic * BURGER_API Burger::FileXML::Generic::GetNextItem(void) const
{
	Generic *pGeneric = GetNext();		// Start with the next object
	Generic *pResult = NULL;			// Assume end of list
	if (pGeneric->GetType()!=XML_ROOT) {	// Valid?
		pResult = pGeneric;
	}
	return pResult;
}


/*! ************************************

	\class Burger::FileXML::Root
	\brief Linked list root object

	For every object that can contain other object, this object is the
	anchor that is the start and end of the linked list of contained object.

	This object contains no data per se, only a list of other objects.

	\note This will identify with \ref eType of \ref XML_ROOT

	\sa Burger::FileXML, Burger::FileXML::Generic, Burger::FileXML::Element or Burger::FileXML::Declaration 

***************************************/

/*! ************************************

	\brief Create an empty root object

***************************************/

Burger::FileXML::Root::Root() : 
	Generic(XML_ROOT)
{
}

/*! ************************************

	\brief Call the destructor for a Root object

	Dispose of all objects still under this object's control

***************************************/

Burger::FileXML::Root::~Root()
{
	DeleteList();
}

/*! ************************************

	\brief Parse XML objects from an XML text stream

	Parse the text and create XML objects based on the input. If
	there is an error in parsing, return a non-zero error code.

	Parsing will continue until the end of input or a <// end parse token is found.
	
	\note This function can be called recursively due to Elements parsing Elements within
	themselves.

	\param pInput Pointer to an input stream
	\param bAllowRawText If \ref TRUE, then non-encapsulated text is allowed 
		to be parsed since an Element is currently being parsed.
	\return Error code of zero for no error or non-zero on error
	\sa SaveList(OutputMemoryStream *,Word) const

***************************************/

Word BURGER_API Burger::FileXML::Root::ParseList(InputMemoryStream *pInput,Word bAllowRawText)
{
	// Assume error
	Word uResult = TRUE;
	for (;;) {
		// Skip the whitespace
		pInput->ParseBeyondWhiteSpace();
		// Get a character. It can only be a '<' or a line end mark
		Word uTemp = pInput->GetByte();
		if (uTemp=='<') {
			// Since it's a '<', then process the token
			Generic *pGeneric;
			if (pInput->IsStringMatch("!--")) {
				// Create a comment
				pGeneric = Comment::New(pInput);
			} else if (pInput->IsStringMatchCase("?xml")) {
				// Create Declarator
				pGeneric = Declaration::New(pInput);
			} else if (pInput->IsStringMatch("![CDATA[")) {
				// Create a CDATA record
				pGeneric = CData::New(pInput);
			} else if (pInput->IsStringMatch("/")) {
				uResult = FALSE;
				pInput->SkipBack(2);		// Allow the higher level parser detect this in the stream
				break;
			} else if (pInput->IsStringMatch("!")) {
				// Create a DTD record
				pGeneric = Element::New(pInput);
			} else {
				// Create an element
				pGeneric = Element::New(pInput);
			}
			// Append this new record at the end of the file
			if (!pGeneric) {
				break;
			}
			InsertBefore(pGeneric);
		} else {
			// End of data?
			if (!uTemp) {
				uResult = FALSE;
				break;
			}
			// If not a space character, see if it can be parsed as a text string
			if ((uTemp>=128) || !(g_AsciiTestTable[uTemp]&ASCII_SPACE)) {
				// Undo the fetch
				pInput->SkipBack(1);
				// Text found?
				if (!bAllowRawText) {
					break;			// Error!
				}
				// Create a text object
				Generic *pGeneric = RawText::New(pInput);
				if (!pGeneric) {
					break;
				}
				InsertBefore(pGeneric);
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Output all objects in a linked list

	Assume the current object is a root object and iterate over all of the 
	other XML objects linked to it and save the contents to an output stream.

	\param pOutput Pointer to an output stream
	\param uDepth Number of tabs to output before saving out this record
	\return Error code of zero for no error or non-zero on error
	\sa Save(OutputMemoryStream *,Word) const

***************************************/

Word BURGER_API Burger::FileXML::Root::SaveList(OutputMemoryStream *pOutput,Word uDepth) const
{
	Generic *pGeneric = GetNext();
	Word uResult = FALSE;		// Assume no error
	if (pGeneric!=this) {
		do {
			// Save the contents of all the other entries
			uResult |= pGeneric->Save(pOutput,uDepth);
			pGeneric = pGeneric->GetNext();
		} while (pGeneric!=this);
	}
	return uResult;
}

/*! ************************************

	\brief Dispose of all objects in a linked list

	Assume the current object is a root object and iterate over all of the 
	other XML objects linked to it and delete them.

***************************************/

void BURGER_API Burger::FileXML::Root::DeleteList(void)
{
	Generic *pGeneric = GetNext();
	if (pGeneric!=this) {
		do {
			Generic *pNext = pGeneric->GetNext();
			Delete(pGeneric);
			// Deleting the object removes it from the list, 
			// so reload from the list the "next" object
			// to continue the iterations
			pGeneric = pNext;
		} while (pGeneric!=this);
	}
}

/*! ************************************

	\brief Traverse the linked list and find a specific type

	Iterate over the XML objects linked list until an object of type 
	\ref XML_ROOT is found and return \ref NULL. If the element of the requested type is
	found, return the pointer to the Element.

	\param uType Type enumeration to scan for.
	\return \ref NULL if the element was not found or a valid pointer to the 
		an Element with the requested type.

***************************************/

Burger::FileXML::Generic * BURGER_API Burger::FileXML::Root::FindType(eType uType) const
{
	Generic *pResult = NULL;
	Generic *pGeneric = GetNext();
	if (pGeneric != this) {
		do {
			if (pGeneric->GetType()==uType) {
				pResult = pGeneric;
				break;
			}
			pGeneric = pGeneric->GetNext();
		} while (pGeneric!=this);
	}
	return pResult;
}

/*! ************************************

	\brief Traverse the linked list and delete all objects of a specific type

	Iterate over the XML objects linked list and delete all objects of the
	requested type.

	\param uType Type enumeration to scan for.

***************************************/

void BURGER_API Burger::FileXML::Root::DeleteType(eType uType)
{
	// Types of XML_ROOT are not allocated
	if (uType!=XML_ROOT) {
		Generic *pGeneric = GetNext();
		if (pGeneric!=this) {
			do {
				Generic *pNext = pGeneric->GetNext();
				if (pGeneric->GetType()==uType) {
					Delete(pGeneric);
				}
				pGeneric = pNext;
			} while (pGeneric!=this);
		}
	}
}

/*! ************************************

	\brief Traverse the linked list and find a specific named Element

	Iterate over the XML objects linked list until an object of type 
	\ref XML_ROOT is found and abort. Return \ref NULL if the element is not
	found or the pointer to the next occurrence of the named Element using
	a case sensitive string compare.

	\param pElementName Pointer to a "C" string of the name of the element to look for.
	\return \ref NULL if the element was not found or a valid pointer to the 
		first occurrence of the an Element with the requested name.

***************************************/

const Burger::FileXML::Element * BURGER_API Burger::FileXML::Root::FindElement(const char *pElementName) const
{
	const Element *pElement = NULL;
	const Generic *pGeneric = GetNext();
	if (pGeneric->GetType()!=XML_ROOT) {
		do {
			if (pGeneric->GetType()==XML_ELEMENT) {
				if (!StringCaseCompare(pElementName,static_cast<const Element *>(pGeneric)->GetName())) {
					pElement = static_cast<const Element *>(pGeneric);
					break;
				}
			}
			pGeneric = pGeneric->GetNext();
		} while (pGeneric->GetType()!=XML_ROOT);
	}
	return pElement;
}

/*! ************************************

	\brief Traverse the linked list and find a specific named Element

	Iterate over the XML objects linked list until an object of type 
	\ref XML_ROOT is found and abort. Return \ref NULL if the element is not
	found or the pointer to the next occurrence of the named Element using
	a case sensitive string compare.

	\param pElementName Pointer to a "C" string of the name of the element to look for.
	\param bAlwaysCreate \ref TRUE if the object can't be found, create an empty record instead
	\return \ref NULL if the element was not found or a valid pointer to the 
		first occurrence of the an Element with the requested name.

***************************************/

Burger::FileXML::Element * BURGER_API Burger::FileXML::Root::FindElement(const char *pElementName,Word bAlwaysCreate)
{
	Element *pElement = NULL;
	Generic *pGeneric = GetNext();
	if (pGeneric->GetType()!=XML_ROOT) {
		do {
			if (pGeneric->GetType()==XML_ELEMENT) {
				if (!StringCaseCompare(pElementName,static_cast<const Element *>(pGeneric)->GetName())) {
					pElement = static_cast<Element *>(pGeneric);
					break;
				}
			}
			pGeneric = pGeneric->GetNext();
		} while (pGeneric->GetType()!=XML_ROOT);
	}
	if (bAlwaysCreate && !pElement) {
		pElement = AddElement(pElementName);
	}
	return pElement;
}

/*! ************************************

	\brief Allocate memory for a new Element and append it to the end of this list.

	No attempt is made to determine if there are other elements with the same name.

	\param pElementName Pointer to a "C" string of the name of the element to create.
	\return \ref NULL if there was a memory allocation error or a valid pointer to the 
		newly created Element.

***************************************/

Burger::FileXML::Element * BURGER_API Burger::FileXML::Root::AddElement(const char *pElementName)
{
	Element *pElement = Element::New(pElementName);
	if (pElement) {
		InsertBefore(pElement);
	}
	return pElement;
}

/*! ************************************

	\brief Traverse the linked list and delete a specific named Element

	\note This will only delete the FIRST occurrence of a named element. If
	there are multiple copies, the remaining elements will be retained.

	\param pElementName Pointer to a "C" string of the name of the element to look for.
	\sa DeleteElements()

***************************************/

void BURGER_API Burger::FileXML::Root::DeleteElement(const char *pElementName)
{
	Delete(FindElement(pElementName));
}

/*! ************************************

	\brief Traverse the linked list and delete all Element objects of a specific name

	Iterate over the XML objects linked list and delete all Element objects of the
	requested name.

	\param pElementName Pointer to a "C" string to the Element's name (Can be multiple elements)
	\sa DeleteElement()

***************************************/

void BURGER_API Burger::FileXML::Root::DeleteElements(const char *pElementName)
{
	Generic *pGeneric = GetNext();
	if (pGeneric!=this) {
		do {
			Generic *pNext = pGeneric->GetNext();
			if (pGeneric->GetType()==XML_ELEMENT) {
				if (!StringCompare(pElementName,static_cast<const Element *>(pGeneric)->GetName())) {
					Delete(pGeneric);
				}
			}
			pGeneric = pNext;
		} while (pGeneric!=this);
	}
}


/*! ************************************

	\brief Traverse the linked list and find a specific named Attribute

	Iterate over the XML objects linked list until an object of type 
	\ref XML_ROOT is found and abort. Return \ref NULL if the element is not
	found or the pointer to the next occurrence of the named Attribute using
	a case sensitive string compare.

	\param pAttributeName Pointer to a "C" string of the name of the attribute to look for.
	\return \ref NULL if the attribute was not found or a valid pointer to the 
		occurrence of an Attribute with the requested name.

***************************************/

const Burger::FileXML::Attribute * BURGER_API Burger::FileXML::Root::FindAttribute(const char *pAttributeName) const
{
	const Attribute *pAttribute = NULL;
	const Generic *pGeneric = GetNext();
	if (pGeneric->GetType()!=XML_ROOT) {
		do {
			if (pGeneric->GetType()==XML_ATTRIBUTE) {
				if (!StringCaseCompare(pAttributeName,static_cast<const Attribute *>(pGeneric)->GetKey())) {
					pAttribute = static_cast<const Attribute *>(pGeneric);
					break;
				}
			}
			pGeneric = pGeneric->GetNext();
		} while (pGeneric->GetType()!=XML_ROOT);
	}
	return pAttribute;
}
/*! ************************************

	\brief Traverse the linked list and find a specific named Attribute

	Iterate over the XML objects linked list until an object of type 
	\ref XML_ROOT is found and abort. Return \ref NULL if the element is not
	found or the pointer to the next occurrence of the named Attribute using
	a case sensitive string compare.

	\param pAttributeName Pointer to a "C" string of the name of the attribute to look for.
	\param bAlwaysCreate \ref TRUE if the object can't be found, create an empty record instead
	\return \ref NULL if the attribute was not found or a valid pointer to the 
		occurrence of an Attribute with the requested name.

***************************************/

Burger::FileXML::Attribute * BURGER_API Burger::FileXML::Root::FindAttribute(const char *pAttributeName,Word bAlwaysCreate)
{
	Attribute *pAttribute = NULL;
	Generic *pGeneric = GetNext();
	if (pGeneric->GetType()!=XML_ROOT) {
		do {
			if (pGeneric->GetType()==XML_ATTRIBUTE) {
				if (!StringCaseCompare(pAttributeName,static_cast<const Attribute *>(pGeneric)->GetKey())) {
					pAttribute = static_cast<Attribute *>(pGeneric);
					break;
				}
			}
			pGeneric = pGeneric->GetNext();
		} while (pGeneric->GetType()!=XML_ROOT);
	}
	if (bAlwaysCreate && !pAttribute) {
		pAttribute = AddAttribute(pAttributeName);
	}
	return pAttribute;
}

/*! ************************************

	\brief Allocate memory for a new Attribute and append it to the end of this list.

	If an attribute of the same name exists in the host linked list, the
	pointer to the pre-existing attribute is updated.

	\param pAttributeName Pointer to a "C" string of the name of the attribute to create.
	\param pValue Pointer to a "C" string of the name of the value for the attribute.
	\return \ref NULL if there was a memory allocation error or a valid pointer to the 
		newly created Attribute.

***************************************/

Burger::FileXML::Attribute * BURGER_API Burger::FileXML::Root::AddAttribute(const char *pAttributeName,const char *pValue)
{
	// Does it already exist in the list?
	Attribute *pAttribute = FindAttribute(pAttributeName);
	if (!pAttribute) {
		// Create it
		pAttribute = Attribute::New(pAttributeName,pValue);
		if (pAttribute) {
			InsertBefore(pAttribute);
		}
	} else {
		// Update it
		pAttribute->SetValue(pValue);
	}
	return pAttribute;
}

/*! ************************************

	\brief Traverse the linked list and delete a specific named Attribute

	\param pAttributeName Pointer to a "C" string of the name of the attribute to look for.

***************************************/

void BURGER_API Burger::FileXML::Root::DeleteAttribute(const char *pAttributeName)
{
	Delete(FindAttribute(pAttributeName));
}

/*! ************************************

	\brief Traverse the linked list and find the first occurrence of RawText

	Iterate over the XML objects linked list until an object of type 
	\ref XML_ROOT is found and abort. Return \ref NULL if the element is not
	found or the pointer to the next occurrence of the named RawText using
	a case sensitive string compare.

	\return \ref NULL if the raw text was not found or a valid pointer to the 
		occurrence of the RawText found.

***************************************/

const Burger::FileXML::RawText * BURGER_API Burger::FileXML::Root::FindRawText(void) const
{
	const RawText *pRawText = NULL;
	const Generic *pGeneric = GetNext();
	if (pGeneric->GetType()!=XML_ROOT) {
		do {
			if (pGeneric->GetType()==XML_TEXT) {
				pRawText = static_cast<const RawText *>(pGeneric);
				break;
			}
			pGeneric = pGeneric->GetNext();
		} while (pGeneric->GetType()!=XML_ROOT);
	}
	return pRawText;
}

/*! ************************************

\brief Traverse the linked list and find the first occurrence of RawText

	Iterate over the XML objects linked list until an object of type 
	\ref XML_ROOT is found and abort. Return \ref NULL if the element is not
	found or the pointer to the next occurrence of the named RawText using
	a case sensitive string compare.

	\param bAlwaysCreate \ref TRUE if the object can't be found, create an empty record instead
	\return \ref NULL if the raw text was not found or a valid pointer to the 
		occurrence of the RawText if found or created.

***************************************/

Burger::FileXML::RawText * BURGER_API Burger::FileXML::Root::FindRawText(Word bAlwaysCreate)
{
	RawText *pRawText = NULL;
	Generic *pGeneric = GetNext();
	if (pGeneric->GetType()!=XML_ROOT) {
		do {
			if (pGeneric->GetType()==XML_TEXT) {
				pRawText = static_cast<RawText *>(pGeneric);
				break;
			}
			pGeneric = pGeneric->GetNext();
		} while (pGeneric->GetType()!=XML_ROOT);
	}
	if (bAlwaysCreate && !pRawText) {
		pRawText = AddRawText();
	}
	return pRawText;
}

/*! ************************************

	\brief Allocate memory for a new RawText and append it to the end of this list.

	This will append a new record without checking if one already exists

	\param pValue Pointer to a "C" string of the value of the new text.
	\return \ref NULL if there was a memory allocation error or a valid pointer to the 
		newly created RawText.

***************************************/

Burger::FileXML::RawText * BURGER_API Burger::FileXML::Root::AddRawText(const char *pValue)
{
	// Create it
	RawText *pRawText = RawText::New(pValue);
	if (pRawText) {
		InsertBefore(pRawText);
	}
	return pRawText;
}

/*! ************************************

	\brief Traverse the linked list and delete the first RawText entry located

***************************************/

void BURGER_API Burger::FileXML::Root::DeleteRawText(void)
{
	// Any text?
	Delete(FindRawText());
}


/*! ************************************

	\class Burger::FileXML::Comment
	\brief Comment text object

	For every <!-- --> record in an XML file, store
	the text in one of these objects. It will not be translated
	and is retained so when the file is saved, the comments will be intact.

	\note This text can contain "illegal" characters like \\n and '&'

	\note This will identify with \ref eType of \ref XML_COMMENT

	\sa Burger::FileXML, Burger::FileXML::Generic, Burger::FileXML::Element or Burger::FileXML::Declaration 

***************************************/

/*! ************************************

	\brief Create an empty comment object

***************************************/

Burger::FileXML::Comment::Comment() : 
	Generic(XML_COMMENT),
	m_Comment()
{
}

/*! ************************************

	\brief Create a comment object with text

	\param pComment Text to store in this newly created object

***************************************/

Burger::FileXML::Comment::Comment(const char *pComment) : 
	Generic(XML_COMMENT),
	m_Comment(pComment)
{
}

/*! ************************************

	\brief Call the destructor for a Comment object

	Unlink itself from the linked list and release the contained string

***************************************/

Burger::FileXML::Comment::~Comment()
{
	// Release the string
}

/*! ************************************

	\brief Parse an XML comment object

	Given a data string that's starting right after the initial <!-- in
	an XML file, parse out the comment and remove the trailing -->. 
	Return an error if a --> was not found before the data
	stream was exhausted.

	\note No translation is done to the text at all. It's likely it can contain illegal
		characters like line feeds and ampersands.

	\param pInput Pointer to the input text stream
	\return Zero if no error, non-zero on error
	\sa Save(OutputMemoryStream *)

***************************************/

Word Burger::FileXML::Comment::Parse(InputMemoryStream *pInput)
{
	// Clear out the old data
	m_Comment.Clear();

	// Assume error
	Word uResult = TRUE;
	// Capture the start so the length can be determined
	WordPtr uMark = pInput->GetMark();
	Word uPrevious1 = 0;		// Test for the --
	Word uPrevious2 = 0;
	for (;;) {
		// Fetch a byte
		Word uTemp = pInput->GetByte();
		// End of the data?
		if (!uTemp) {
			break;
		}
		// Found the closing mark?
		if (uTemp=='>') {
			// Only if it's preceded by -- will it be considered legal
			if ((uPrevious1=='-') && (uPrevious2=='-')) {
				WordPtr uFinalSize = (pInput->GetMark()-uMark)-3;
				if (uFinalSize) {
					pInput->SetMark(uMark);
					m_Comment.SetBufferSize(uFinalSize);
					pInput->Get(m_Comment.GetPtr(),uFinalSize);
					m_Comment.NormalizeLineFeeds();
					pInput->SkipForward(3);		// Skip past the -->
				}
				uResult = FALSE;
				break;
			}
		}
		uPrevious1 = uPrevious2;
		uPrevious2 = uTemp;
	}
	// On error, undo all parsing
	if (uResult) {
		pInput->SetMark(uMark);
	}
	return uResult;
}

/*! ************************************

	\brief Write out an XML comment object

	Encapsulate a comment record with <!-- -->\\n and send it out
	the data stream

	\param pOutput Pointer to the output text stream
	\param uDepth Number of tabs to output for formatting before writing out the comment
	\return Zero if no error, non-zero on error
	\sa Parse(InputMemoryStream *)

***************************************/

Word Burger::FileXML::Comment::Save(OutputMemoryStream *pOutput,Word uDepth) const
{
	Word uResult = pOutput->AppendTabs(uDepth);
	uResult |= pOutput->Append("<!--");
	uResult |= pOutput->Append(m_Comment.GetPtr());
	uResult |= pOutput->Append("-->\n");
	return uResult;
}

/*! ************************************

	\fn const char *Burger::FileXML::Comment::GetText(void) const
	\brief Return a pointer to the contained comment string

	\sa SetText(const char *) or SetText(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileXML::Comment::SetText(const char *pInput)
	\brief Set a new comment string

	Replace the contained comment string with a new one

	\param pInput Pointer to a "C" string
	\sa GetText(void) const or SetText(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileXML::Comment::SetText(const String *pInput)
	\brief Set a new comment string

	Replace the contained comment string with a new one

	\param pInput Pointer to a String class instance
	\sa GetText(void) const or SetText(const char *)

***************************************/

/*! ************************************

	\brief Create a new Comment structure and initialize it from an input file

	Parse the input file after the starting <!-- and up to and including the ending -->
	Will return \ref NULL on a parsing error or if it's out of memory

	\note No translation is done to the text at all. It's likely it can contain illegal
		characters like line feeds and ampersands.

	\param pInput Pointer to an input stream at the byte after the <!--
	\return \ref NULL on error or a valid Comment record on a successful parse

***************************************/

Burger::FileXML::Comment * BURGER_API Burger::FileXML::Comment::New(InputMemoryStream *pInput)
{
	Comment *pComment = new (Alloc(sizeof(Comment))) Comment;
	if (pComment) {
		if (pComment->Parse(pInput)) {
			Delete(pComment);
			pComment = NULL;
		}
	}
	return pComment;
}

/*! ************************************

	\brief Create a new Comment structure and initialize it with a string

	Will return \ref NULL on if it's out of memory.

	\param pComment Text to store in this newly created object
	\return \ref NULL on error or a valid Comment record

***************************************/

Burger::FileXML::Comment * BURGER_API Burger::FileXML::Comment::New(const char *pComment)
{
	return new (Alloc(sizeof(Comment))) Comment(pComment);
}





/*! ************************************

	\class Burger::FileXML::CData
	\brief Untouched text object intended for application parsing

	For every <![CDATA[ ]]> record in an XML file, store
	the text in one of these objects. It will not be translated
	and is retained so when the file is saved, the comments will be intact.

	\note This will identify with \ref eType of \ref XML_CDATA

	\sa FileXML, FileXML::Generic, FileXML::Element or FileXML::Comment 

***************************************/

/*! ************************************

	\brief Create an empty CDATA object

***************************************/

Burger::FileXML::CData::CData() : 
	Generic(XML_CDATA),
	m_CData()
{
}

/*! ************************************

	\brief Create a CDATA object with text

	\param pCData Text to store in this newly created object

***************************************/

Burger::FileXML::CData::CData(const char *pCData) : 
	Generic(XML_CDATA),
	m_CData(pCData)
{
}

/*! ************************************

	\brief Call the destructor for a CDATA object

	Unlink itself from the linked list and release the contained string

***************************************/

Burger::FileXML::CData::~CData()
{
	// Release the string
}

/*! ************************************

	\brief Parse an XML CDATA object

	Given a data string that's starting right after the initial <![CDATA[ in
	an XML file, parse out the data and remove the trailing ]]>. 
	Return an error if a ]]> was not found before the data
	stream was exhausted.

	\param pInput Pointer to the input text stream
	\return Zero if no error, non-zero on error
	\sa Save(OutputMemoryStream *)

***************************************/

Word Burger::FileXML::CData::Parse(InputMemoryStream *pInput)
{
	// Clear out the old data
	m_CData.Clear();

	// Assume error
	Word uResult = TRUE;
	// Capture the start so the length can be determined
	WordPtr uMark = pInput->GetMark();
	Word uPrevious1 = 0;		// Test for the --
	Word uPrevious2 = 0;
	for (;;) {
		// Fetch a byte
		Word uTemp = pInput->GetByte();
		// End of the data?
		if (!uTemp) {
			break;
		}
		// Found the closing mark?
		if (uTemp=='>') {
			// Only if it's preceded by -- will it be considered legal
			if ((uPrevious1==']') && (uPrevious2==']')) {
				WordPtr uFinalSize = (pInput->GetMark()-uMark)-3;
				if (uFinalSize) {
					pInput->SetMark(uMark);
					m_CData.SetBufferSize(uFinalSize);
					pInput->Get(m_CData.GetPtr(),uFinalSize);
					m_CData.NormalizeLineFeeds();
					pInput->SkipForward(3);
				}
				uResult = FALSE;
				break;
			}
		}
		uPrevious1 = uPrevious2;
		uPrevious2 = uTemp;
	}
	// On error, undo all parsing
	if (uResult) {
		pInput->SetMark(uMark);
	}
	return uResult;
}

/*! ************************************

	\brief Write out an XML CDATA object

	Encapsulate a CDATA record with <![CDATA[ ]]>\\n and send it out
	the data stream

	\param pOutput Pointer to the output text stream
	\param uDepth Number of tabs to output for formatting before writing out the CDATA
	\return Zero if no error, non-zero on error
	\sa Parse(InputMemoryStream *)

***************************************/

Word Burger::FileXML::CData::Save(OutputMemoryStream *pOutput,Word uDepth) const
{
	Word uResult = pOutput->AppendTabs(uDepth);
	uResult |= pOutput->Append("<![CDATA[");
	uResult |= pOutput->Append(m_CData.GetPtr());
	uResult |= pOutput->Append("]]>\n");
	return uResult;
}

/*! ************************************

	\fn const char *Burger::FileXML::CData::GetText(void) const
	\brief Return a pointer to the contained CDATA string

	\sa SetText(const char *) or SetText(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileXML::CData::SetText(const char *pInput)
	\brief Set a new CDATA string

	Replace the contained CDATA string with a new one

	\param pInput Pointer to a "C" string
	\sa GetText(void) const or SetText(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileXML::CData::SetText(const String *pInput)
	\brief Set a new CDATA string

	Replace the contained CDATA string with a new one

	\param pInput Pointer to a String class instance
	\sa GetText(void) const or SetText(const char *)

***************************************/

/*! ************************************

	\brief Create a new CDATA structure and initialize it from an input file

	Parse the input file after the starting <![CDATA[ and up to and including the ending ]]>
	Will return \ref NULL on a parsing error or if it's out of memory

	\param pInput Pointer to an input stream at the byte after the <![CDATA[
	\return \ref NULL on error or a valid CData record on a successful parse

***************************************/

Burger::FileXML::CData * BURGER_API Burger::FileXML::CData::New(InputMemoryStream *pInput)
{
	CData *pCData = new (Alloc(sizeof(CData))) CData;
	if (pCData) {
		if (pCData->Parse(pInput)) {
			Delete(pCData);
			pCData = NULL;
		}
	}
	return pCData;
}

/*! ************************************

	\brief Create a new CDATA structure and initialize it with a string

	Will return \ref NULL if it's out of memory

	\param pCData Text to store in this newly created object
	\return \ref NULL on error or a valid CData record

***************************************/

Burger::FileXML::CData * BURGER_API Burger::FileXML::CData::New(const char *pCData)
{
	return new (Alloc(sizeof(CData))) CData(pCData);
}




/*! ************************************

	\class Burger::FileXML::Attribute
	\brief XML Attribute key / value pair

	Some XML elements have attached attributes. Each attribute is attached
	to an element in the form of attribute="value"

	For parsing, ' is valid as is ". Due to the nature of some XML
	files, it will parse attribute=value as long as the value has
	no spaces. It will write out the value with quotes in all cases.

	\note This will identify with \ref eType of \ref XML_ATTRIBUTE

	\sa FileXML, FileXML::Generic, FileXML::Declaration or FileXML::Element 

***************************************/

/*! ************************************

	\brief Create an empty attribute object

***************************************/

Burger::FileXML::Attribute::Attribute() :
	Generic(XML_ATTRIBUTE),
	m_Key(),
	m_Value()
{
}

/*! ************************************

	\brief Call the destructor for a Attribute object

	Unlink itself from the linked list and release the contained strings

***************************************/

Burger::FileXML::Attribute::~Attribute()
{
}

/*! ************************************

	\brief Parse an XML Attribute object

	\param pInput Pointer to the input text stream
	\return Zero if no error, non-zero on error
	\sa Save(OutputMemoryStream *)

***************************************/

Word Burger::FileXML::Attribute::Parse(InputMemoryStream *pInput)
{
	Word uResult = TRUE;
	WordPtr uMark = pInput->GetMark();
	// Extract the attribute's name
	if (!ReadXMLName(&m_Key,pInput)) {
		pInput->ParseBeyondWhiteSpace();
		// Locate the required '='
		Word uTemp = pInput->GetByte();
		if (uTemp=='=') {
			pInput->ParseBeyondWhiteSpace();
			// Read in the attribute data
			if (!ReadXMLText(&m_Value,pInput)) {
				uResult = DecodeXMLString(&m_Value);	// Success!!!
			}
		}
	}
	// On failure, clear this data and restore the stream
	if (uResult) {
		pInput->SetMark(uMark);
		m_Key.Clear();
		m_Value.Clear();
	}
	return uResult;
}

/*! ************************************

	\brief Write out an XML Attribute object

	\param pOutput Pointer to the output text stream
	\param uDepth Number of tabs to output for formatting before writing out the comment
	\return Zero if no error, non-zero on error
	\sa Parse(InputMemoryStream *)

***************************************/

Word Burger::FileXML::Attribute::Save(OutputMemoryStream *pOutput,Word /* uDepth */) const
{
	Word uResult = pOutput->Append(m_Key.GetPtr());
	uResult |= pOutput->Append("=\"");
	uResult |= SaveXMLString(pOutput,m_Value.GetPtr());
	uResult |= pOutput->Append('"');
	// If the next item in the linked list is not a root object, then append a space
	if (GetNext()->GetType()!=XML_ROOT) {
		uResult |= pOutput->Append(' ');
	}
	return uResult;
}

/*! ************************************

	\fn const char *Burger::FileXML::Attribute::GetKey(void) const
	\brief Return a pointer to the key string for the attribute

	\sa SetKey(const char *) or SetKey(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileXML::Attribute::SetKey(const char *pInput)
	\brief Set a new attribute key

	Replace the contained attribute key string with a new one

	\param pInput Pointer to a "C" string
	\sa GetKey(void) const or SetKey(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileXML::Attribute::SetKey(const String *pInput)
	\brief Set a new attribute key

	Replace the contained attribute key with a new one

	\param pInput Pointer to a String class instance
	\sa GetKey(void) const or SetKey(const char *)

***************************************/


/*! ************************************

	\fn const char *Burger::FileXML::Attribute::GetValue(void) const
	\brief Return a pointer to the value string for the attribute

	\sa SetValue(const char *) or SetValue(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileXML::Attribute::SetValue(const char *pInput)
	\brief Set a new attribute value

	Replace the contained attribute value string with a new one

	\param pInput Pointer to a "C" string
	\sa GetValue(void) const or SetValue(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileXML::Attribute::SetValue(const String *pInput)
	\brief Set a new attribute value

	Replace the contained attribute value with a new one

	\param pInput Pointer to a String class instance
	\sa GetValue(void) const or SetValue(const char *)

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::FileXML::Attribute::GetBoolean(Word bDefault) const
	\brief Return a boolean

	Calls String::GetBoolean(Word) on the value text and returns the result.

	\param bDefault Value to return on parse failure
	\sa SetBoolean(Word)

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::FileXML::Attribute::SetBoolean(Word bValue) const
	\brief Return a boolean

	Calls String::SetYesNo(Word) on the value text and returns the result.

	\param bValue Value to pass to String::SetYesNo(Word)
	\sa SetBoolean(Word)

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::FileXML::Attribute::GetWord(Word uDefault,Word uMin,Word uMax) const
	\brief Return an unsigned integer value.

	Calls String::GetWord(Word,Word,Word) on the value text and returns the result.

	\param uDefault Value to return on error
	\param uMin Minimum acceptable value
	\param uMax Maximum acceptable value
	\sa SetWord(Word)

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::Attribute::SetWord(Word uValue)
	\brief Set an unsigned integer value.

	Calls String::SetWord(Word) on the value text

	\param uValue Value to store as an unsigned integer string
	\sa SetWordHex(Word) or GetWord(Word,Word,Word) const

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::Attribute::SetWordHex(Word uValue)
	\brief Set an unsigned integer value as hex.

	Calls String::SetWordHex(Word) on the value text

	\param uValue Value to store as an unsigned integer hex string
	\sa SetWord(Word) or GetWord(Word,Word,Word) const

***************************************/

/*! ************************************

	\fn BURGER_INLINE Int Burger::FileXML::Attribute::GetInt(Int iDefault,Int iMin,Int iMax) const
	\brief Return a signed integer value.

	Calls String::GetInt(Int,Int,Int) on the value text and returns the result.

	\param iDefault Value to return on error
	\param iMin Minimum acceptable value
	\param iMax Maximum acceptable value
	\sa SetInt(Int)

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::Attribute::SetInt(Int iValue)
	\brief Set a signed integer value.

	Calls String::SetInt(Int) on the value text

	\param iValue Value to store as an integer string
	\sa GetInt(Int,Int,Int) const

***************************************/

/*! ************************************

	\fn BURGER_INLINE float Burger::FileXML::Attribute::GetFloat(float fDefault) const
	\brief Return a floating point value.

	Calls String::GetFloat(float) on the value text and returns the result.

	\param fDefault Value to return on error
	\sa SetFloat(float)

***************************************/

/*! ************************************

	\fn BURGER_INLINE float Burger::FileXML::Attribute::GetFloat(float fDefault,float fMin,float fMax) const
	\brief Return a floating point value.

	Calls String::GetFloat(float,float,float) on the value text and returns the result.

	\param fDefault Value to return on error
	\param fMin Minimum acceptable value
	\param fMax Maximum acceptable value
	\sa SetFloat(float)

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::Attribute::SetFloat(float fValue)
	\brief Set a 32 bit floating point value.

	Calls String::SetFloat(float) on the value text

	\param fValue Value to store as an integer string
	\sa GetFloat(float,float,float) const

***************************************/

/*! ************************************

	\fn BURGER_INLINE double Burger::FileXML::Attribute::GetDouble(double dDefault) const
	\brief Return a 64 bit floating point value.

	Calls String::GetDouble(double) on the value text and returns the result.

	\param dDefault Value to return on error
	\sa SetDouble(double)

***************************************/

/*! ************************************

	\fn BURGER_INLINE double Burger::FileXML::Attribute::GetDouble(double dDefault,double dMin,double dMax) const
	\brief Return a 64 bit floating point value.

	Calls String::GetDouble(double,double,double) on the value text and returns the result.

	\param dDefault Value to return on error
	\param dMin Minimum acceptable value
	\param dMax Maximum acceptable value
	\sa SetDouble(double)

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::Attribute::SetDouble(double dValue)
	\brief Set a 64 bit floating point value.

	Calls String::SetDouble(double) on the value text

	\param dValue Value to store as an integer string
	\sa GetDouble(double,double,double) const

***************************************/

/*! ************************************

	\brief Create a new Attribute structure and initialize it from an input file

	Parse the input file starting at a key, followed by an '=' and then the
	quoted value
	Will return \ref NULL on a parsing error or if it's out of memory

	\param pInput Pointer to an input stream at the start of the key string
	\return \ref NULL on error or a valid Attribute record on a successful parse

***************************************/

Burger::FileXML::Attribute * BURGER_API Burger::FileXML::Attribute::New(InputMemoryStream *pInput)
{
	Attribute *pAttribute = new (Alloc(sizeof(Attribute))) Attribute;
	if (pAttribute) {
		if (pAttribute->Parse(pInput)) {
			Delete(pAttribute);
			pAttribute = NULL;
		}
	}
	return pAttribute;
}

/*! ************************************

	\brief Create a new Attribute structure and initialize it with a key value

	Initialize a new Attribute with an empty value.

	Will return \ref NULL if it's out of memory

	\param pInput Pointer to a "C" string with the key value.
	\param pValue Pointer to a "C" string with the value. Can be \ref NULL for no value.
	\return \ref NULL on error or a valid Attribute record on a successful parse

***************************************/

Burger::FileXML::Attribute * BURGER_API Burger::FileXML::Attribute::New(const char *pInput,const char *pValue)
{
	Attribute *pAttribute = new (Alloc(sizeof(Attribute))) Attribute;
	if (pAttribute) {
		pAttribute->SetKey(pInput);
		pAttribute->SetValue(pValue);
	}
	return pAttribute;
}

/*! ************************************

	\brief Create or dispose of an attribute record

	If a pointer to an attribute is maintained, use this function to
	create / delete / modify an existing attribute. If the key
	is \ref NULL or an empty string or the data is \ref NULL, the
	attribute will be disposed of.

	If the key / value is valid and the attribute pointer is \ref NULL,
	a new attribute will be created and appended to the end of the
	pRoot linked list. Otherwise, the current attribute will have
	the key/value pair updated to the new value.

	Will return \ref NULL if the attribute was disposed on, the
	"this" pointer on update or a new pointer if an attribute
	was created.

	\code
	static Attribute *g_pAttribute = NULL;

	Element *pElement;
	g_pAttribute = g_pAttribute->Update(pElement->GetRoot(),"key","newvalue");
	g_pAttribute = g_pAttribute->Update(pElement->GetRoot(),"keytoremove",NULL);
	\endcode

	\param pRoot Pointer to parent's Generic linked list in the event of attribute creation. If \ref NULL, a new item will not be added to any linked list.
	\param pKey Pointer to a "C" string with the key.
	\param pValue Pointer to a "C" string with the value.
	\return \ref NULL on error or a valid Attribute record on a valid key/value pair

***************************************/

Burger::FileXML::Attribute *BURGER_API Burger::FileXML::Attribute::Update(Root *pRoot,const char *pKey,const char *pValue)
{
	// Delete the record?
	if (!pKey || !pKey[0] || !pValue) {
		Delete(this);
		return NULL;
	}
	// Not allocated already?
	if (this==NULL) {
		// Create the new attribute
		Attribute *pAttribute = Attribute::New(pKey,pValue);
		if (pAttribute && pRoot) {
			pRoot->InsertBefore(pAttribute);
		}
		return pAttribute;
	}
	// Easy way!
	SetKey(pKey);
	SetValue(pValue);
	return this;
}



/*! ************************************

	\class Burger::FileXML::Declaration
	\brief XML Declaration object

	Sometimes an XML file has a <?xml ?> record for declaring the text encoding.
	This object contains this information. XML files should only have one 
	of these records at the start of the file. It's not good practice to have
	multiple records nor having it not be the first record in the file.

	\note This will identify with \ref eType of \ref XML_DECLARATION

	\sa FileXML, FileXML::Generic, FileXML::Comment or FileXML::Element 

***************************************/

/*! ************************************

	\brief Create an empty declaration object

***************************************/

Burger::FileXML::Declaration::Declaration() :
	Generic(XML_DECLARATION),
	m_Encoding(),			// No specific encoding
	m_fVersion(1.0f),		// Default version
	m_bStandalone(2)		// 2 means uninitialized
{
}

/*! ************************************

	\brief Create a declaration object with supplied defaults

***************************************/

Burger::FileXML::Declaration::Declaration(float fVersion,const char *pEncoding,Word bStandalone) :
	Generic(XML_DECLARATION),
	m_Encoding(pEncoding),
	m_fVersion(fVersion),
	m_bStandalone(bStandalone)
{
}

/*! ************************************

	\brief Call the destructor for a Declaration object

	Unlink itself from the linked list and release the contained strings

***************************************/

Burger::FileXML::Declaration::~Declaration()
{
}

/*! ************************************

	\brief Parse an XML Declaration object

	\param pInput Pointer to the input text stream
	\return Zero if no error, non-zero on error
	\sa Save(OutputMemoryStream *)

***************************************/

Word Burger::FileXML::Declaration::Parse(InputMemoryStream *pInput)
{
	m_bStandalone = 2;
	m_fVersion = 1.0f;
	m_Encoding.Clear();
	Attribute TempAttribute;
	Word uResult = TRUE;
	for (;;) {
		// Look for the net token
		pInput->ParseBeyondWhiteSpace();
		if (pInput->IsStringMatch("?>")) {
			uResult = FALSE;
			break;
		}
		// Parse an attribute
		if (TempAttribute.Parse(pInput)) {
			// Uh oh...
			break;
		}
		// Special case, test for known attributes
		const char *pKey = TempAttribute.GetKey();
		if (!StringCaseCompare(pKey,"version")) {
			m_fVersion = Abs(AsciiToFloat(TempAttribute.GetValue()));
		} else if (!StringCaseCompare(pKey,"encoding")) {
			m_Encoding = TempAttribute.GetValue();
		} else if (!StringCaseCompare(pKey,"standalone")) {
			m_bStandalone = AsciiToBoolean(TempAttribute.GetValue());
		}
	}
	return uResult;
}

/*! ************************************

	\brief Write out an XML Declaration object

	\param pOutput Pointer to the output text stream
	\param uDepth Number of tabs to output for formatting before writing out the comment
	\return Zero if no error, non-zero on error
	\sa Parse(InputMemoryStream *)

***************************************/

Word Burger::FileXML::Declaration::Save(OutputMemoryStream *pOutput,Word uDepth) const
{
	Word uResult = pOutput->AppendTabs(uDepth);
	uResult |= pOutput->Append("<?xml");

	// If the version is a real floating point number, print it in the form of x.xx

	if (IsFinite(m_fVersion)) {
		uResult |= pOutput->Append(" version=\"");

		// Force the value to be in the format of x.xx
		float fInt = RoundToZero(m_fVersion);
		uResult |= pOutput->AppendAscii(FloatToIntRound(fInt));
		uResult |= pOutput->Append('.');
		float fFrac = RoundToZero((m_fVersion-fInt)*10.f);		// 10 gets 1 digits of fraction
		uResult |= pOutput->AppendAscii(FloatToIntRound(fFrac));
		uResult |= pOutput->Append('"');
	}

	// If there is a string, output the encoding
	if (m_Encoding.GetLength()) {
		uResult |= pOutput->Append(" encoding=\"");
		uResult |= SaveXMLString(pOutput,m_Encoding.GetPtr());
		uResult |= pOutput->Append('"');
	}

	// If enabled, output 'yes' or 'no'
	if (m_bStandalone<2) {
		uResult |= pOutput->Append(" standalone=\"");
		const char *pString;
		if (m_bStandalone) {
			pString = g_YesString;
		} else {
			pString = g_NoString;
		}
		uResult |= pOutput->Append(pString);
		uResult |= pOutput->Append('"');
	}
	uResult |= pOutput->Append(" ?>\n");
	return uResult;
}

/*! ************************************

	\fn float Burger::FileXML::Declaration::GetVersion(void) const
	\brief Return the declaration version

	Usually this value is 1.0f

	\sa SetVersion(float) 

***************************************/

/*! ************************************

	\brief Sets the declaration version

	\param fInput Version number of the declaration
	\sa GetVersion() const

***************************************/

void BURGER_API Burger::FileXML::Declaration::SetVersion(float fInput)
{
	m_fVersion = Abs(fInput);
}

/*! ************************************

	\fn const char *Burger::FileXML::Declaration::GetEncoding(void) const
	\brief Return the declaration encoding string

	Usually it's UTF-8 or ISO-8859-1

	\sa SetEncoding(const char *) or SetEncoding(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileXML::Declaration::SetEncoding(const char *pInput)
	\brief Sets the declaration encoding

	Replace the contained encoding value string with a new one

	\param pInput Pointer to the string containing the encoding or an empty string.
	\sa GetEncoding(void) const or SetEncoding(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileXML::Declaration::SetEncoding(const String *pInput)
	\brief Sets the declaration encoding

	Replace the contained encoding value string with a new one

	\param pInput Pointer to the string containing the encoding or an empty string.
	\sa GetEncoding(void) const or SetEncoding(const char *)

***************************************/


/*! ************************************

	\fn Word Burger::FileXML::Declaration::GetStandalone(void) const
	\brief Return the standalone boolean

	The XML default is \ref FALSE.

	\return \ref TRUE if it's standalone or \ref FALSE if not. 
	\sa SetStandalone(Word) 

***************************************/

/*! ************************************

	\fn void Burger::FileXML::Declaration::SetStandalone(Word bInput)
	\brief Sets the declaration standalone value

	There's a special value of 2 that will set the standalone value to "false" and
	disable the ability to save this attribute to the output file.

	\param bInput \ref TRUE for standalone, \ref FALSE for not standalone, 
		2 for not standalone and disable the entry on write
	\sa GetStandalone() const

***************************************/

/*! ************************************

	\brief Create a new Declaration structure and initialize it from an input file

	Parse the input file after the starting <?xml and up to and including the ending ?>
	Will return \ref NULL on a parsing error or if it's out of memory

	\param pInput Pointer to an input stream at the byte after the <?xml
	\return \ref NULL on error or a valid Declaration record on a successful parse

***************************************/

Burger::FileXML::Declaration * BURGER_API Burger::FileXML::Declaration::New(InputMemoryStream *pInput)
{
	Declaration *pDeclaration = new (Alloc(sizeof(Declaration))) Declaration;
	if (pDeclaration) {
		if (pDeclaration->Parse(pInput)) {
			Delete(pDeclaration);
			pDeclaration = NULL;
		}
	}
	return pDeclaration;
}

/*! ************************************

	\brief Create a new Declaration structure and initialize it

	Parse the input file after the starting <?xml and up to and including the ending ?>
	Will return \ref NULL on a parsing error or if it's out of memory

	\param fVersion Version number of the declaration
	\param pEncoding Pointer to the string containing the encoding or an empty string.
	\param bStandalone \ref TRUE for standalone, \ref FALSE for not standalone, 
		2 for not standalone and disable the entry on write
	\return \ref NULL on error or a valid Declaration record on a successful parse

***************************************/

Burger::FileXML::Declaration * BURGER_API Burger::FileXML::Declaration::New(float fVersion,const char *pEncoding,Word bStandalone)
{
	return new (Alloc(sizeof(Declaration))) Declaration(fVersion,pEncoding,bStandalone);
}






/*! ************************************

	\class Burger::FileXML::RawText
	\brief Raw text object

	In between Element markers can exist raw text, store
	the text in one or more of these objects.

	\code
	<Element>Text that is stored in a RawText object</Element>
	\endcode

	\note This will identify with \ref eType of \ref XML_TEXT

	\sa FileXML, FileXML::Generic, FileXML::Element or FileXML::Declaration 

***************************************/

/*! ************************************

	\brief Create an empty raw text object

***************************************/

Burger::FileXML::RawText::RawText() : 
	Generic(XML_TEXT),
	m_Text()
{
}

/*! ************************************

	\brief Create a raw text object with default text

	\param pText Text to store in this newly created object

***************************************/

Burger::FileXML::RawText::RawText(const char *pText) : 
	Generic(XML_TEXT),
	m_Text(pText)
{
}

/*! ************************************

	\brief Call the destructor for a raw text object

	Unlink itself from the linked list and release the contained string

***************************************/

Burger::FileXML::RawText::~RawText()
{
	// Release the string
}

/*! ************************************

	\brief Parse raw text between XML objects

	Given a data string that's starting outside of an encapsulated 
	element token, parse out the raw text until a '>' or '<' is found.
	Trailing whitespace is removed from the text string.

	\param pInput Pointer to the input text stream
	\return Zero if no error, non-zero on error
	\sa Save(OutputMemoryStream *)

***************************************/

Word Burger::FileXML::RawText::Parse(InputMemoryStream *pInput)
{
	// Clear out the old data
	m_Text.Clear();

	// Assume error
	Word uResult = TRUE;
	// Capture the start so the length can be determined
	WordPtr uMark = pInput->GetMark();
	WordPtr uEndMark = uMark;
	for (;;) {
		// Fetch a byte
		Word uTemp = pInput->GetByte();
		// End of the data?
		if (!uTemp) {
			break;
		}
		// Found the closing mark?
		if (uTemp=='>' || uTemp=='<') {
			// Only if it's preceded by -- will it be considered legal
			uResult = FALSE;
			pInput->SkipBack(1);
			WordPtr uFinalSize = (uEndMark-uMark);
			if (uFinalSize) {
				WordPtr uDone = pInput->GetMark();
				pInput->SetMark(uMark);
				m_Text.SetBufferSize(uFinalSize);
				pInput->Get(m_Text.GetPtr(),uFinalSize);
				uResult = DecodeXMLString(&m_Text);
				m_Text.NormalizeLineFeeds();
				pInput->SetMark(uDone);
			}
			break;
		}
		// Allowable character?
		if ((uTemp>=128) || (!(g_AsciiTestTable[uTemp]&ASCII_SPACE))) {
			uEndMark = pInput->GetMark();
		}
	}
	// On error, undo all parsing
	if (uResult) {
		pInput->SetMark(uMark);
	}
	return uResult;
}

/*! ************************************

	\brief Write out a raw text object

	Output raw text as is. Only print leading tabs if the previously output object was
	not a \ref XML_ROOT or \ref XML_TEXT. Append a \\n if the next object is not \ref XML_ROOT
	or \ref XML_TEXT.

	\param pOutput Pointer to the output text stream
	\param uDepth Number of tabs to output for formatting before writing out the comment
	\return Zero if no error, non-zero on error
	\sa Parse(InputMemoryStream *)

***************************************/

Word Burger::FileXML::RawText::Save(OutputMemoryStream *pOutput,Word uDepth) const
{
	Word uResult = 0;
	eType Type = GetPrevious()->GetType();
	if ((Type!=XML_ROOT) && (Type!=XML_TEXT)) {
		uResult = pOutput->AppendTabs(uDepth);
	}
	uResult |= SaveXMLString(pOutput,m_Text.GetPtr());
	Type = GetNext()->GetType();
	if ((Type!=XML_ROOT) && (Type!=XML_TEXT)) {
		uResult |= pOutput->Append("\n");
	}
	return uResult;
}

/*! ************************************

	\fn BURGER_INLINE const char *Burger::FileXML::RawText::GetText(void) const
	\brief Return a pointer to the contained text string

	\sa SetText(const char *) or SetText(const String *)

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::RawText::SetText(const char *pInput)
	\brief Set a new string

	Replace the contained string with a new one

	\param pInput Pointer to a "C" string
	\sa GetText(void) const or SetText(const String *)

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::RawText::SetText(const String *pInput)
	\brief Set a new string

	Replace the contained string with a new one

	\param pInput Pointer to a String class instance
	\sa GetText(void) const or SetText(const char *)

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::FileXML::RawText::GetBoolean(Word bDefault) const
	\brief Return a boolean

	Calls String::GetBoolean(Word) on the text and returns the result.

	\param bDefault Value to return on parse failure
	\sa SetBoolean(Word)

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::FileXML::RawText::SetBoolean(Word bValue) const
	\brief Return a boolean

	Calls String::SetYesNo(Word) on the text and returns the result.

	\param bValue Value to pass to String::SetYesNo(Word)
	\sa SetBoolean(Word)

***************************************/

/*! ************************************

	\fn BURGER_INLINE Word Burger::FileXML::RawText::GetWord(Word uDefault,Word uMin,Word uMax) const
	\brief Return an unsigned integer value.

	Calls String::GetWord(Word,Word,Word) on the text and returns the result.

	\param uDefault Value to return on error
	\param uMin Minimum acceptable value
	\param uMax Maximum acceptable value
	\sa SetWord(Word)

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::RawText::SetWord(Word uValue)
	\brief Set an unsigned integer value.

	Calls String::SetWord(Word) on the text

	\param uValue Value to store as an unsigned integer string
	\sa SetWordHex(Word) or GetWord(Word,Word,Word) const

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::RawText::SetWordHex(Word uValue)
	\brief Set an unsigned integer value as hex.

	Calls String::SetWordHex(Word) on the text

	\param uValue Value to store as an unsigned integer hex string
	\sa SetWord(Word) or GetWord(Word,Word,Word) const

***************************************/

/*! ************************************

	\fn BURGER_INLINE Int Burger::FileXML::RawText::GetInt(Int iDefault,Int iMin,Int iMax) const
	\brief Return a signed integer value.

	Calls String::GetInt(Int,Int,Int) on the text and returns the result.

	\param iDefault Value to return on error
	\param iMin Minimum acceptable value
	\param iMax Maximum acceptable value
	\sa SetInt(Int)

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::RawText::SetInt(Int iValue)
	\brief Set a signed integer value.

	Calls String::SetInt(Int) on the text

	\param iValue Value to store as an integer string
	\sa GetInt(Int,Int,Int) const

***************************************/

/*! ************************************

	\fn BURGER_INLINE float Burger::FileXML::RawText::GetFloat(float fDefault) const
	\brief Return a floating point value.

	Calls String::GetFloat(float) on the text and returns the result.

	\param fDefault Value to return on error
	\sa SetFloat(float)

***************************************/

/*! ************************************

	\fn BURGER_INLINE float Burger::FileXML::RawText::GetFloat(float fDefault,float fMin,float fMax) const
	\brief Return a floating point value.

	Calls String::GetFloat(float,float,float) on the text and returns the result.

	\param fDefault Value to return on error
	\param fMin Minimum acceptable value
	\param fMax Maximum acceptable value
	\sa SetFloat(float)

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::RawText::SetFloat(float fValue)
	\brief Set a 32 bit floating point value.

	Calls String::SetFloat(float) on the text

	\param fValue Value to store as an integer string
	\sa GetFloat(float,float,float) const

***************************************/

/*! ************************************

	\fn BURGER_INLINE double Burger::FileXML::RawText::GetDouble(double dDefault) const
	\brief Return a 64 bit floating point value.

	Calls String::GetDouble(double) on the text and returns the result.

	\param dDefault Value to return on error
	\sa SetDouble(double)

***************************************/

/*! ************************************

	\fn BURGER_INLINE double Burger::FileXML::RawText::GetDouble(double dDefault,double dMin,double dMax) const
	\brief Return a 64 bit floating point value.

	Calls String::GetDouble(double,double,double) on the text and returns the result.

	\param dDefault Value to return on error
	\param dMin Minimum acceptable value
	\param dMax Maximum acceptable value
	\sa SetDouble(double)

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::RawText::SetDouble(double dValue)
	\brief Set a 64 bit floating point value.

	Calls String::SetDouble(double) on the text

	\param dValue Value to store as an integer string
	\sa GetDouble(double,double,double) const

***************************************/

/*! ************************************

	\brief Create a new RawText structure and initialize it from an input file

	Parse the input file after whitespace and it will copy the text
	until a '>' or a '<' is found. Trailing whitespace will be removed.

	Will return \ref NULL on a parsing error or if it's out of memory

	\param pInput Pointer to an input stream at the byte after whitespace.
	\return \ref NULL on error or a valid RawText record on a successful parse

***************************************/

Burger::FileXML::RawText * BURGER_API Burger::FileXML::RawText::New(InputMemoryStream *pInput)
{
	RawText *pRawText = new (Alloc(sizeof(RawText))) RawText;
	if (pRawText) {
		if (pRawText->Parse(pInput)) {
			Delete(pRawText);
			pRawText = NULL;
		}
	}
	return pRawText;
}

/*! ************************************

	\brief Create a new RawText structure and initialize it.

	Copy the text string into the newly created RawText object.

	Will return \ref NULL on a parsing error or if it's out of memory

	\param pText Pointer to text string to initialize this object with.
	\return \ref NULL on error or a valid RawText record

***************************************/

Burger::FileXML::RawText * BURGER_API Burger::FileXML::RawText::New(const char *pText)
{
	return new (Alloc(sizeof(RawText))) RawText(pText);
}






/*! ************************************

	\class Burger::FileXML::Element
	\brief XML Element object

	XML files mostly are populated by XML elements that are declared
	with either a &lt;foobar /&gt; or a &lt;foobar&gt;&lt;/foobar&gt; string. These
	objects can contain other objects.

	\note This will identify with \ref eType of \ref XML_ELEMENT

	\sa FileXML, FileXML::Generic, FileXML::Comment or FileXML::Declaration 

***************************************/

/*! ************************************

	\brief Create an empty element object

***************************************/

Burger::FileXML::Element::Element() :
	Generic(XML_ELEMENT),
	m_Attributes(),
	m_Root(),
	m_Name()
{
}

/*! ************************************

	\brief Create an empty element object
	\param pName Pointer to a "C" string for the element's name

***************************************/

Burger::FileXML::Element::Element(const char *pName) :
	Generic(XML_ELEMENT),
	m_Attributes(),
	m_Root(),
	m_Name(pName)
{
}

/*! ************************************

	\brief Call the destructor for a Element object

	Unlink itself from the linked list and release the contained strings

***************************************/

Burger::FileXML::Element::~Element()
{
}

/*! ************************************

	\brief Parse an XML Element object

	\param pInput Pointer to the input text stream
	\return Zero if no error, non-zero on error
	\sa Save(OutputMemoryStream *)

***************************************/

Word Burger::FileXML::Element::Parse(InputMemoryStream *pInput)
{
	Word uResult = TRUE;
	m_Root.DeleteList();
	m_Attributes.DeleteList();

	pInput->ParseBeyondWhiteSpace();
	// Get the element's name
	uResult = ReadXMLName(&m_Name,pInput);
	if (!uResult) {
		uResult = TRUE;
		for (;;) {
			// Look for the net token
			pInput->ParseBeyondWhiteSpace();
			// Does this item have no data?
			if (pInput->IsStringMatch("/>")) {
				uResult = FALSE;
				break;
			}
			// Found data!
			if (pInput->IsStringMatch(">")) {
				// Parse the data contained in the element pair
				m_Root.ParseList(pInput,TRUE);
				// Was it properly terminated?
				if (pInput->IsStringMatch("</")) {
					pInput->ParseBeyondWhiteSpace();
					String EndName;
					// Verify that the ending tag is a match to the start tag.
					if (!ReadXMLName(&EndName,pInput)) {
						if (!StringCaseCompare(m_Name.GetPtr(),EndName.GetPtr())) {
							pInput->ParseBeyondWhiteSpace();
							// Properly closed?
							if (pInput->IsStringMatch(">")) {
								// We are good to go!!!
								uResult = FALSE;
							}
						}
					}
				}
				break;
			}
			// Parse an attribute
			Attribute *pAttribute = Attribute::New(pInput);
			if (!pAttribute) {
				// Uh oh...
				break;
			}
			m_Attributes.InsertBefore(pAttribute);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Write out an XML Element object

	\param pOutput Pointer to the output text stream
	\param uDepth Number of tabs to output for formatting before writing out the element
	\return Zero if no error, non-zero on error
	\sa Parse(InputMemoryStream *)

***************************************/

Word Burger::FileXML::Element::Save(OutputMemoryStream *pOutput,Word uDepth) const
{
	Word uResult = pOutput->AppendTabs(uDepth);
	uResult |= pOutput->Append("<");
	uResult |= pOutput->Append(m_Name.GetPtr());
	// Any attributes?
	if (m_Attributes.GetNext()!=&m_Attributes) {
		uResult |= pOutput->Append(' ');
		uResult |= m_Attributes.SaveList(pOutput,uDepth);
	}

	// Is there any data contained besides the attributes?
	if (m_Root.GetNext()!=&m_Root) {
		// Close the tag
		uResult |= pOutput->Append(">");

		// Allow raw text object to immediately follow the element
		// so <foo>text</foo> is resulting output.

		if (m_Root.GetNext()->GetType()!=Generic::XML_TEXT) {
			uResult |= pOutput->Append("\n");
		}
		// Dump the contents
		uResult |= m_Root.SaveList(pOutput,uDepth+1);
		// Last entry text? If so, no tabs
		if (m_Root.GetPrevious()->GetType()!=Generic::XML_TEXT) {
			uResult |= pOutput->AppendTabs(uDepth);
		}
		// Output the closing element </foo>
		uResult |= pOutput->Append("</");
		uResult |= pOutput->Append(m_Name.GetPtr());
		uResult |= pOutput->Append(">\n");
	} else {
		// End the tag now <foo />
		uResult |= pOutput->Append("/>\n");
	}
	return uResult;
}

/*! ************************************

	\fn BURGER_INLINE Root * Burger::FileXML::Element::GetRoot(void)
	\brief Return the root object

	Return the root object that contains all objects this Element for manual
	parsing.
	
	\return Pointer to the root object contained in this Element.
	\sa GetRoot(void) const

***************************************/

/*! ************************************

	\fn BURGER_INLINE const Root * Burger::FileXML::Element::GetRoot(void) const
	\brief Return the const root object

	Return the root object that contains all objects this Element for manual
	parsing.
	
	\return Pointer to the root object contained in this Element.
	\sa GetRoot(void)

***************************************/

/*! ************************************

	\fn BURGER_INLINE const Element *Burger::FileXML::Element::FindElement(const char *pElementName) const
	\brief Find a named XML Element.

	Traverse the linked list for an XML Element record with the matching 
	name using a case sensitive search. Return \ref NULL if the record wasn't
	found.
	
	\param pElementName	Pointer to a "C" string to the element to search for.
	\return Pointer to an XML Element or \ref NULL if not found
	\sa DeleteElement() or AddElement()

***************************************/

/*! ************************************

	\fn BURGER_INLINE Element *Burger::FileXML::Element::FindElement(const char *pElementName,Word bAlwaysCreate)
	\brief Find a named XML Element.

	Traverse the linked list for an XML Element record with the matching 
	name using a case sensitive search. Return \ref NULL if the record wasn't
	found and bAlwaysCreate is zero. If bAlwaysCreate is not zero and the 
	record didn't exist, create one.
	
	\param pElementName	Pointer to a "C" string to the element to search for.
	\param bAlwaysCreate \ref TRUE to create the Element if one didn't already exist
	\return Pointer to an XML Element or \ref NULL if not found
	\sa DeleteElement() or AddElement()

***************************************/

/*! ************************************

	\fn BURGER_INLINE Element *Burger::FileXML::Element::AddElement(const char *pElementName)
	\brief Add a new Element record.

	Create a new Element and place it as the last record in the Element's list.
	
	\note XML allows multiple Elements with the same name.
		This function will not check if any preexisting Element records possess the same name.

	\param pElementName	Pointer to a "C" string to the element to search for.
	\return Pointer to an XML Element or \ref NULL if out of memory
	\sa FindElement(const char *,Word) or DeleteElement()

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::Element::DeleteElement(const char *pElementName)
	\brief Delete an Element record.

	Traverse the linked list for the first Element with the supplied name
	and dispose of it. It will stop after the first deletion. 
	If there are multiple records with the same name, the duplicates will remain.

	\param pElementName	Pointer to a "C" string to the element to search for.
	\sa DeleteElements(const char *), FindElement(const char *,Word) or AddElement(const char *)

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::Element::DeleteElements(const char *pElementName)
	\brief Delete all Element records with this name.

	Traverse the linked list for the all Element records with the supplied name and dispose of them.

	\param pElementName	Pointer to a "C" string to the element to search for.
	\sa DeleteElement(const char *), FindElement(const char *,Word) or AddElement(const char *)

***************************************/

/*! ************************************

	\fn BURGER_INLINE Attribute *Burger::FileXML::Element::FindAttribute(const char *pAttributeName,Word bAlwaysCreate)
	\brief Traverse the linked list and find a specific named Attribute.

	Iterate over the Element's linked list until an Attribute is found. 
	Return \ref NULL if the Attribute is not found or the pointer to the
	occurrence of the named Attribute using a case sensitive string compare.
	
	\param pAttributeName Pointer to a "C" string to the Attribute to search for.
	\param bAlwaysCreate \ref TRUE to create the Attribute if one didn't already exist
	\return Pointer to an XML Attribute or \ref NULL if not found
	\sa DeleteAttribute() or AddAttribute()

***************************************/

/*! ************************************

	\fn BURGER_INLINE Attribute *Burger::FileXML::Element::AddAttribute(const char *pAttributeName,const char *pValue)
	\brief Add a new Attribute record.

	Create a new Attribute and place it as the last record in the Element's Attribute list.
	
	\note If the Attribute already exists, the contents will be updated with the new pValue.

	\param pAttributeName Pointer to a "C" string to the Attribute to search for.
	\param pValue Pointer to a "C" string for the data field for the Attribute. \ref NULL yields an empty string.
	\return Pointer to an XML Element or \ref NULL if out of memory
	\sa FindAttribute(const char *,Word) or DeleteAttribute()

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::Element::DeleteAttribute(const char *pAttributeName)
	\brief Delete an Attribute record.

	Traverse the linked list for the Attribute with the supplied name
	and dispose of it.

	\param pAttributeName Pointer to a "C" string to the Attribute to search for.
	\sa FindAttribute(const char *,Word) or AddAttribute(const char *,const char *)

***************************************/

/*! ************************************

	\fn const char *Burger::FileXML::Element::GetName(void) const
	\brief Return a pointer to the element's name

	\sa SetName(const char *) or SetName(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileXML::Element::SetName(const char *pInput)
	\brief Set the name of the element

	Replace the element's name with the supplied "C" string.

	\param pInput Pointer to a "C" string
	\sa GetName(void) const or SetName(const String *)

***************************************/

/*! ************************************

	\fn void Burger::FileXML::Element::SetName(const String *pInput)
	\brief Set the name of the element

	Replace the element's name with the supplied String.

	\param pInput Pointer to a String class instance
	\sa GetName(void) const or SetName(const char *)

***************************************/

/*! ************************************

	\brief Return a boolean value from the Element's RawText (\ref TRUE or \ref FALSE)

	Find the first RawText field stored in the Element and parse it as a Boolean
	with "true", "yes" or non zero returning \ref TRUE and "false", "no" or 0 returning
	\ref FALSE.

	If it's none of the above, then return the default value

	\param bDefault Value to return on error
	\return \ref TRUE or \ref FALSE or bDefault
	\sa SetBoolean(Word) or AsciiToBoolean(const char *,Word)

***************************************/

Word BURGER_API Burger::FileXML::Element::GetBoolean(Word bDefault) const
{
	const RawText *pRawText = m_Root.FindRawText();
	if (pRawText) {
		bDefault = pRawText->GetBoolean(bDefault);
	}
	return bDefault;
}

/*! ************************************

	\brief Set the RawText in the Element to a boolean value

	If the input value is zero, set the string to "no",
	otherwise set the string to "yes". It will not
	store numeric values.

	\param bValue Value to store as a string
	\sa GetBoolean(Word) const

***************************************/

void BURGER_API Burger::FileXML::Element::SetBoolean(Word bValue)
{
	RawText *pRawText = m_Root.FindRawText(TRUE);
	if (pRawText) {
		pRawText->SetBoolean(bValue);
	}
}

/*! ************************************

	\brief Return an unsigned integer value from the Element's RawText

	Find the first RawText field stored in the Element and
	parse it as a 32 bit unsigned integer or
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
	\sa GetInt(Int,Int,Int) const, SetWord(Word), SetWordHex(Word), or AsciiToWord(const char *,Word,Word,Word)

***************************************/

Word BURGER_API Burger::FileXML::Element::GetWord(Word uDefault,Word uMin,Word uMax) const
{
	const RawText *pRawText = m_Root.FindRawText();
	if (pRawText) {
		uDefault = pRawText->GetWord(uDefault,uMin,uMax);
	}
	return uDefault;
}

/*! ************************************

	\brief Set an unsigned integer value to the Element's RawText

	Convert the input into an unsigned integer UTF-8 string 
	and set the value to this string

	\param uValue Value to store as an unsigned integer string
	\sa GetWord(Word,Word,Word) const or SetWordHex(Word)

***************************************/

void BURGER_API Burger::FileXML::Element::SetWord(Word uValue)
{
	RawText *pRawText = m_Root.FindRawText(TRUE);
	if (pRawText) {
		pRawText->SetWord(uValue);
	}
}

/*! ************************************

	\brief Set an unsigned integer value as hex to the Element's RawText

	Convert the input into a "C" style hex
	string in the format of 0x12345678 and
	store this string as the value

	\param uValue Value to store as an unsigned integer hex string
	\sa GetWord(Word,Word,Word) const or SetWord(Word)

***************************************/

void BURGER_API Burger::FileXML::Element::SetWordHex(Word uValue)
{
	RawText *pRawText = m_Root.FindRawText(TRUE);
	if (pRawText) {
		pRawText->SetWordHex(uValue);
	}
}

/*! ************************************

	\brief Return a signed integer value from the Element's RawText

	Find the first RawText field stored in the Element and
	parse it as a 32 bit signed integer or
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
	\sa GetWord(Word,Word,Word) const, SetInt(Int) or AsciiToInteger(const char *,Int,Int,Int)

***************************************/

Int BURGER_API Burger::FileXML::Element::GetInt(Int iDefault,Int iMin,Int iMax) const
{
	const RawText *pRawText = m_Root.FindRawText();
	if (pRawText) {
		iDefault = pRawText->GetInt(iDefault,iMin,iMax);
	}
	return iDefault;
}

/*! ************************************

	\brief Set a signed integer value to the Element's RawText

	Convert the input into an signed integer UTF-8 string 
	and set the value to this string

	\param iValue Value to store as a signed integer string
	\sa GetInt(Int,Int,Int) const or SetWord(Word)

***************************************/

void BURGER_API Burger::FileXML::Element::SetInt(Int iValue)
{
	RawText *pRawText = m_Root.FindRawText(TRUE);
	if (pRawText) {
		pRawText->SetInt(iValue);
	}
}

/*! ************************************

	\brief Return a floating point value from the Element's RawText

	Find the first RawText field stored in the Element and
	parse it as a 32 bit floating point
	numeric value and if successful, return it.
	If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param fDefault Value to return on error
	\return Value or fDefault
	\sa GetDouble(double) const, SetFloat(float) or AsciiToFloat(const char *,float)

***************************************/

float BURGER_API Burger::FileXML::Element::GetFloat(float fDefault) const
{
	const RawText *pRawText = m_Root.FindRawText();
	if (pRawText) {
		fDefault = pRawText->GetFloat(fDefault);
	}
	return fDefault;
}

/*! ************************************

	\brief Return a floating point value from the Element's RawText

	Find the first RawText field stored in the Element and
	parse it as a 32 bit floating point
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

float BURGER_API Burger::FileXML::Element::GetFloat(float fDefault,float fMin,float fMax) const
{
	const RawText *pRawText = m_Root.FindRawText();
	if (pRawText) {
		fDefault = pRawText->GetFloat(fDefault,fMin,fMax);
	}
	return fDefault;
}

/*! ************************************

	\brief Set a 32 bit floating point value to the Element's RawText

	Convert the input into an floating point representation
	of a UTF-8 string and set the value to this string

	\param fValue Value to store as a floating point string
	\sa GetFloat(float,float,float) const or SetDouble(double)

***************************************/

void BURGER_API Burger::FileXML::Element::SetFloat(float fValue)
{
	RawText *pRawText = m_Root.FindRawText(TRUE);
	if (pRawText) {
		pRawText->SetFloat(fValue);
	}
}

/*! ************************************

	\brief Return a 64 bit floating point value from the Element's RawText

	Find the first RawText field stored in the Element and
	parse it as a 64 bit floating point
	numeric value and if successful, return it.
	If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param dDefault Value to return on error
	\return Value or dDefault
	\sa GetFloat(float) const, SetDouble(double) or AsciiToDouble(const char *,double)

***************************************/

double BURGER_API Burger::FileXML::Element::GetDouble(double dDefault) const
{
	const RawText *pRawText = m_Root.FindRawText();
	if (pRawText) {
		dDefault = pRawText->GetDouble(dDefault);
	}
	return dDefault;
}

/*! ************************************

	\brief Return a 64 bit floating point value from the Element's RawText

	Find the first RawText field stored in the Element and
	parse it as a 64 bit floating point
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

double BURGER_API Burger::FileXML::Element::GetDouble(double dDefault,double dMin,double dMax) const
{
	const RawText *pRawText = m_Root.FindRawText();
	if (pRawText) {
		dDefault = pRawText->GetDouble(dDefault,dMin,dMax);
	}
	return dDefault;
}

/*! ************************************

	\brief Set a 64 bit floating point value to the Element's RawText

	Convert the input into an floating point representation
	of a UTF-8 string and set the value to this string

	\param dValue Value to store as a 64 bit floating point string
	\sa GetDouble(double,double,double) const or SetFloat(float)

***************************************/

void BURGER_API Burger::FileXML::Element::SetDouble(double dValue)
{
	RawText *pRawText = m_Root.FindRawText(TRUE);
	if (pRawText) {
		pRawText->SetDouble(dValue);
	}
}

/*! ************************************

	\brief Locate and return the Element's RawText

	Return the string pointer to the Element's first RawText object.

	If it's not present, then return the default value

	\param pDefault String to return on error
	\return Pointer to the RawText string or pDefault
	\sa SetString(const char *)

***************************************/

const char *BURGER_API Burger::FileXML::Element::GetString(const char *pDefault) const
{
	const RawText *pRawText = m_Root.FindRawText();
	if (pRawText) {
		pDefault = pRawText->GetText();
	}
	return pDefault;
}

/*! ************************************

	\brief Set the Element's RawText object

	Replace the Element's first RawText object's text with this string.

	If there isn't a RawText object found, create one with this string.

	\param pValue Pointer to a "C" string to copy into the RawText object
	\sa GetString(const char *) const

***************************************/

void BURGER_API Burger::FileXML::Element::SetString(const char *pValue)
{
	RawText *pRawText = m_Root.FindRawText(TRUE);
	if (pRawText) {
		pRawText->SetText(pValue);
	}
}

/*! ************************************

	\brief Return a boolean value from an Attribute (\ref TRUE or \ref FALSE)

	Find the Attribute with the supplied Attribute name that's stored 
	in the Element and parse it as a Boolean with "true", "yes" or 
	non zero returning \ref TRUE and "false", "no" or 0 returning
	\ref FALSE.

	If it's none of the above, then return the default value

	\param pAttributeName "C" string for the name of the Attribute to parse
	\param bDefault Value to return on error
	\return \ref TRUE or \ref FALSE or bDefault
	\sa AttributeSetBoolean(const char *,Word) or AsciiToBoolean(const char *,Word)

***************************************/

Word BURGER_API Burger::FileXML::Element::AttributeGetBoolean(const char *pAttributeName,Word bDefault) const
{
	const Attribute *pAttribute = m_Attributes.FindAttribute(pAttributeName);
	if (pAttribute) {
		bDefault = pAttribute->GetBoolean(bDefault);
	}
	return bDefault;
}

/*! ************************************

	\brief Set an Attribute to a boolean value

	If the input value is zero, set the string to "no",
	otherwise set the string to "yes". It will not
	store numeric values.

	If the Attribute already exists, it's updated. Otherwise, it will be created

	\param pAttributeName Pointer to a "C" string with the name of the Attribute
	\param bValue Value to store as a string
	\sa AttributeGetBoolean(const char *,Word) const

***************************************/

void BURGER_API Burger::FileXML::Element::AttributeSetBoolean(const char *pAttributeName,Word bValue)
{
	Attribute *pAttribute = m_Attributes.FindAttribute(pAttributeName,TRUE);
	if (pAttribute) {
		pAttribute->SetBoolean(bValue);
	}
}

/*! ************************************

	\brief Return an unsigned integer value from an Attribute 

	Find the Attribute with the supplied Attribute name that's stored 
	in the Element and parse it as a 32 bit unsigned integer or
	hex value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234

	\param pAttributeName Pointer to a "C" string with the name of the Attribute
	\param uDefault Value to return on error
	\param uMin Minimum acceptable value
	\param uMax Maximum acceptable value
	\return Value in between uMin and uMax or uDefault
	\sa AttributeGetInt(const char *,Int,Int,Int) const, AttributeSetWord(const char *,Word),
		AttributeSetWordHex(const char *,Word), or AsciiToWord(const char *,Word,Word,Word)

***************************************/

Word BURGER_API Burger::FileXML::Element::AttributeGetWord(const char *pAttributeName,Word uDefault,Word uMin,Word uMax) const
{
	const Attribute *pAttribute = m_Attributes.FindAttribute(pAttributeName);
	if (pAttribute) {
		uDefault = pAttribute->GetWord(uDefault,uMin,uMax);
	}
	return uDefault;
}

/*! ************************************

	\brief Set an unsigned integer value to the Element's Attribute's value

	Convert the input into an unsigned integer UTF-8 string 
	and set the value to this string

	\param pAttributeName Pointer to a "C" string with the name of the Attribute
	\param uValue Value to store as an unsigned integer string
	\sa AttributeGetWord(const char *,Word,Word,Word) const or AttributeSetWordHex(const char *,Word)

***************************************/

void BURGER_API Burger::FileXML::Element::AttributeSetWord(const char *pAttributeName,Word uValue)
{
	Attribute *pAttribute = m_Attributes.FindAttribute(pAttributeName,TRUE);
	if (pAttribute) {
		pAttribute->SetWord(uValue);
	}
}

/*! ************************************

	\brief Set an unsigned integer value as hex to the Element's Attribute's value

	Convert the input into a "C" style hex
	string in the format of 0x12345678 and
	store this string as the value

	\param pAttributeName Pointer to a "C" string with the name of the Attribute
	\param uValue Value to store as an unsigned integer hex string
	\sa AttributeGetWord(const char *,Word,Word,Word) const or AttributeSetWord(const char *,Word)

***************************************/

void BURGER_API Burger::FileXML::Element::AttributeSetWordHex(const char *pAttributeName,Word uValue)
{
	Attribute *pAttribute = m_Attributes.FindAttribute(pAttributeName,TRUE);
	if (pAttribute) {
		pAttribute->SetWordHex(uValue);
	}
}

/*! ************************************

	\brief Return a signed integer value from an Attribute 

	Find the Attribute with the supplied Attribute name that's stored 
	in the Element and parse it as a 32 bit signed integer or
	hex value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234. 0xFFFFFFFF will be converted
	to -1.

	\param pAttributeName Pointer to a "C" string with the name of the Attribute
	\param iDefault Value to return on error
	\param iMin Minimum acceptable value
	\param iMax Maximum acceptable value
	\return Value in between iMin and iMax or iDefault
	\sa AttributeGetWord(const char *,Word,Word,Word) const, AttributeSetInt(const char *,Int) or AsciiToInteger(const char *,Int,Int,Int)

***************************************/

Int BURGER_API Burger::FileXML::Element::AttributeGetInt(const char *pAttributeName,Int iDefault,Int iMin,Int iMax) const
{
	const Attribute *pAttribute = m_Attributes.FindAttribute(pAttributeName);
	if (pAttribute) {
		iDefault = pAttribute->GetInt(iDefault,iMin,iMax);
	}
	return iDefault;
}

/*! ************************************

	\brief Set a signed integer value to the Element's Attribute's value

	Convert the input into an signed integer UTF-8 string 
	and set the value to this string

	\param pAttributeName Pointer to a "C" string with the name of the Attribute
	\param iValue Value to store as a signed integer string
	\sa AttributeGetInt(const char *,Int,Int,Int) const or AttributeSetWord(const char *,Word)

***************************************/

void BURGER_API Burger::FileXML::Element::AttributeSetInt(const char *pAttributeName,Int iValue)
{
	Attribute *pAttribute = m_Attributes.FindAttribute(pAttributeName,TRUE);
	if (pAttribute) {
		pAttribute->SetInt(iValue);
	}
}

/*! ************************************

	\brief Return a floating point value from an Attribute

	Find the Attribute with the supplied Attribute name that's stored 
	in the Element and parse it as a 32 bit floating point
	numeric value and if successful, return it.
	If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param pAttributeName Pointer to a "C" string with the name of the Attribute
	\param fDefault Value to return on error
	\return Value or fDefault
	\sa AttributeGetDouble(const char *,double) const, AttributeSetFloat(const char *,float) or AsciiToFloat(const char *,float)

***************************************/

float BURGER_API Burger::FileXML::Element::AttributeGetFloat(const char *pAttributeName,float fDefault) const
{
	const Attribute *pAttribute = m_Attributes.FindAttribute(pAttributeName);
	if (pAttribute) {
		fDefault = pAttribute->GetFloat(fDefault);
	}
	return fDefault;
}

/*! ************************************

	\brief Return a floating point value from an Attribute

	Find the Attribute with the supplied Attribute name that's stored 
	in the Element and parse it as a 32 bit floating point
	numeric value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param pAttributeName Pointer to a "C" string with the name of the Attribute
	\param fDefault Value to return on error
	\param fMin Minimum acceptable value
	\param fMax Maximum acceptable value
	\return Value in between fMin and fMax or fDefault
	\sa AttributeGetDouble(const char *,double,double,double) const, AttributeSetFloat(const char *,float) or AsciiToFloat(const char *,float,float,float)

***************************************/

float BURGER_API Burger::FileXML::Element::AttributeGetFloat(const char *pAttributeName,float fDefault,float fMin,float fMax) const
{
	const Attribute *pAttribute = m_Attributes.FindAttribute(pAttributeName);
	if (pAttribute) {
		fDefault = pAttribute->GetFloat(fDefault,fMin,fMax);
	}
	return fDefault;
}

/*! ************************************

	\brief Set a 32 bit floating point value to the Element's Attribute's value

	Convert the input into an floating point representation
	of a UTF-8 string and set the value to this string

	\param pAttributeName Pointer to a "C" string with the name of the Attribute
	\param fValue Value to store as a floating point string
	\sa AttributeGetFloat(const char *,float,float,float) const or AttributeSetDouble(const char *,double)

***************************************/

void BURGER_API Burger::FileXML::Element::AttributeSetFloat(const char *pAttributeName,float fValue)
{
	Attribute *pAttribute = m_Attributes.FindAttribute(pAttributeName,TRUE);
	if (pAttribute) {
		pAttribute->SetFloat(fValue);
	}
}

/*! ************************************

	\brief Return a 64 bit floating point value from an Attribute

	Find the Attribute with the supplied Attribute name that's stored 
	in the Element and parse it as a 64 bit floating point
	numeric value and if successful, return it.
	If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param pAttributeName Pointer to a "C" string with the name of the Attribute
	\param dDefault Value to return on error
	\return Value or dDefault
	\sa AttributeGetFloat(const char *,float) const, AttributeSetDouble(const char *,double) or AsciiToDouble(const char *,double)

***************************************/

double BURGER_API Burger::FileXML::Element::AttributeGetDouble(const char *pAttributeName,double dDefault) const
{
	const Attribute *pAttribute = m_Attributes.FindAttribute(pAttributeName);
	if (pAttribute) {
		dDefault = pAttribute->GetDouble(dDefault);
	}
	return dDefault;
}


/*! ************************************

	\brief Return a 64 bit floating point value from an Attribute

	Find the Attribute with the supplied Attribute name that's stored 
	in the Element and parse it as a 64 bit floating point
	numeric value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param pAttributeName Pointer to a "C" string with the name of the Attribute
	\param dDefault Value to return on error
	\param dMin Minimum acceptable value
	\param dMax Maximum acceptable value
	\return Value in between dMin and dMax or dDefault
	\sa AttributeGetFloat(const char *,float,float,float) const, AttributeSetDouble(const char *,double) or AsciiToDouble(const char *,double,double,double)

***************************************/

double BURGER_API Burger::FileXML::Element::AttributeGetDouble(const char *pAttributeName,double dDefault,double dMin,double dMax) const
{
	const Attribute *pAttribute = m_Attributes.FindAttribute(pAttributeName);
	if (pAttribute) {
		dDefault = pAttribute->GetDouble(dDefault,dMin,dMax);
	}
	return dDefault;
}

/*! ************************************

	\brief Set a 64 bit floating point value to the Element's Attribute's value

	Convert the input into an floating point representation
	of a UTF-8 string and set the value to this string

	\param pAttributeName Pointer to a "C" string with the name of the Attribute
	\param dValue Value to store as a 64 bit floating point string
	\sa AttributeGetDouble(const char *,double,double,double) const or AttributeSetFloat(const char *,float)

***************************************/

void BURGER_API Burger::FileXML::Element::AttributeSetDouble(const char *pAttributeName,double dValue)
{
	Attribute *pAttribute = m_Attributes.FindAttribute(pAttributeName,TRUE);
	if (pAttribute) {
		pAttribute->SetDouble(dValue);
	}
}

/*! ************************************

	\brief Locate and return the Element's Attribute value text

	Return the pointer to the value of Element's Attribute object.

	If it's not present, then return the default value

	\param pAttributeName Pointer to a "C" string with the name of the Attribute
	\param pDefault String to return on error
	\return Pointer to the RawText string or pDefault
	\sa AttributeSetString(const char *,const char *)

***************************************/

const char *BURGER_API Burger::FileXML::Element::AttributeGetString(const char *pAttributeName,const char *pDefault) const
{
	const Attribute *pAttribute = m_Attributes.FindAttribute(pAttributeName);
	if (pAttribute) {
		pDefault = pAttribute->GetValue();
	}
	return pDefault;
}

/*! ************************************

	\brief Set the Element's Attribute object's value

	Replace the Element's Attribute's value string with this string.

	If there isn't a Attribute object found, create one with this string.

	\param pAttributeName Pointer to a "C" string with the name of the Attribute
	\param pValue Pointer to a "C" string to copy into the RawText object
	\sa AttributeGetString(const char *,const char *) const

***************************************/

void BURGER_API Burger::FileXML::Element::AttributeSetString(const char *pAttributeName,const char *pValue)
{
	Attribute *pAttribute = m_Attributes.FindAttribute(pAttributeName,TRUE);
	if (pAttribute) {
		pAttribute->SetValue(pValue);
	}
}

/*! ************************************

	\brief Return a boolean value from an Element's RawText (\ref TRUE or \ref FALSE)

	Find the Element with the supplied Element name that's stored 
	in this Element and parse the RawText as a Boolean with "true", "yes" or 
	non zero returning \ref TRUE and "false", "no" or 0 returning
	\ref FALSE.

	If it's none of the above, then return the default value

	\param pElementName "C" string for the name of the Element to parse
	\param bDefault Value to return on error
	\return \ref TRUE or \ref FALSE or bDefault
	\sa ElementSetBoolean(const char *,Word) or AsciiToBoolean(const char *,Word)

***************************************/

Word BURGER_API Burger::FileXML::Element::ElementGetBoolean(const char *pElementName,Word bDefault) const
{
	const Element *pElement = m_Root.FindElement(pElementName);
	if (pElement) {
		bDefault = pElement->GetBoolean(bDefault);
	}
	return bDefault;
}

/*! ************************************

	\brief Set an Element's RawText to a boolean value

	If the input value is zero, set the RawText attached
	to the Element to "no",
	otherwise set the RawText to "yes". It will not
	store numeric values.

	If the Element already exists, it's updated. Otherwise, it will be created

	\param pElementName Pointer to a "C" string with the name of the Element
	\param bValue Value to store as a string
	\sa ElementGetBoolean(const char *,Word) const

***************************************/

void BURGER_API Burger::FileXML::Element::ElementSetBoolean(const char *pElementName,Word bValue)
{
	Element *pElement = m_Root.FindElement(pElementName,TRUE);
	if (pElement) {
		pElement->SetBoolean(bValue);
	}
}


/*! ************************************

	\brief Return an unsigned integer value from an Element's RawText

	Find an Element with the supplied name and from the
	RawText that's stored in it parse it as a 32 bit unsigned integer or
	hex value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234

	\param pElementName Pointer to a "C" string with the name of the Element
	\param uDefault Value to return on error
	\param uMin Minimum acceptable value
	\param uMax Maximum acceptable value
	\return Value in between uMin and uMax or uDefault
	\sa ElementGetInt(const char *,Int,Int,Int) const, ElementSetWord(const char *,Word), ElementSetWordHex(const char *,Word), or AsciiToWord(const char *,Word,Word,Word)

***************************************/

Word BURGER_API Burger::FileXML::Element::ElementGetWord(const char *pElementName,Word uDefault,Word uMin,Word uMax) const
{
	const Element *pElement = m_Root.FindElement(pElementName);
	if (pElement) {
		uDefault = pElement->GetWord(uDefault,uMin,uMax);
	}
	return uDefault;
}

/*! ************************************

	\brief Set an unsigned integer value into a named Element's RawText

	Convert the input into an unsigned integer UTF-8 string 
	and set the value to this string

	\param pElementName Pointer to a "C" string with the name of the Element
	\param uValue Value to store as an unsigned integer string
	\sa ElementGetWord(const char *,Word,Word,Word) const or ElementSetWordHex(const char *,Word)

***************************************/

void BURGER_API Burger::FileXML::Element::ElementSetWord(const char *pElementName,Word uValue)
{
	Element *pElement = m_Root.FindElement(pElementName,TRUE);
	if (pElement) {
		pElement->SetWord(uValue);
	}
}

/*! ************************************

	\brief Set an unsigned integer value as hex into a named Element's RawText

	Convert the input into a "C" style hex
	string in the format of 0x12345678 and
	store this string as the value

	\param pElementName Pointer to a "C" string with the name of the Element
	\param uValue Value to store as an unsigned integer hex string
	\sa ElementGetWord(const char *,Word,Word,Word) const or ElementSetWord(const char *,Word)

***************************************/

void BURGER_API Burger::FileXML::Element::ElementSetWordHex(const char *pElementName,Word uValue)
{
	Element *pElement = m_Root.FindElement(pElementName,TRUE);
	if (pElement) {
		pElement->SetWordHex(uValue);
	}
}

/*! ************************************

	\brief Return a signed integer value from an Element's RawText

	Find an Element with the supplied name and from the
	RawText that's stored in it parse it as a 32 bit signed integer or
	hex value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	Hex strings are acceptable input in the form
	of $1234 and 0x1234. 0xFFFFFFFF will be converted
	to -1.

	\param pElementName Pointer to a "C" string with the name of the Element
	\param iDefault Value to return on error
	\param iMin Minimum acceptable value
	\param iMax Maximum acceptable value
	\return Value in between iMin and iMax or iDefault
	\sa ElementGetWord(const char *,Word,Word,Word) const, ElementSetInt(const char *,Int) or AsciiToInteger(const char *,Int,Int,Int)

***************************************/

Int BURGER_API Burger::FileXML::Element::ElementGetInt(const char *pElementName,Int iDefault,Int iMin,Int iMax) const
{
	const Element *pElement = m_Root.FindElement(pElementName);
	if (pElement) {
		iDefault = pElement->GetInt(iDefault,iMin,iMax);
	}
	return iDefault;
}

/*! ************************************

	\brief Set a signed integer value into a named Element's RawText

	Convert the input into an signed integer UTF-8 string 
	and set the value to this string

	\param pElementName Pointer to a "C" string with the name of the Element
	\param iValue Value to store as a signed integer string
	\sa ElementGetInt(const char *,Int,Int,Int) const or ElementSetWord(const char *,Word)

***************************************/

void BURGER_API Burger::FileXML::Element::ElementSetInt(const char *pElementName,Int iValue)
{
	Element *pElement = m_Root.FindElement(pElementName,TRUE);
	if (pElement) {
		pElement->SetInt(iValue);
	}
}

/*! ************************************

	\brief Return a floating point value from an Element's RawText

	Find an Element with the supplied name and from the
	RawText that's stored in it parse it as a 32 bit floating point
	numeric value and if successful, return it.
	If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param pElementName Pointer to a "C" string with the name of the Element
	\param fDefault Value to return on error
	\return Value or fDefault
	\sa ElementGetDouble(const char *,double) const, ElementSetFloat(const char *,float) or AsciiToFloat(const char *,float)

***************************************/

float BURGER_API Burger::FileXML::Element::ElementGetFloat(const char *pElementName,float fDefault) const
{
	const Element *pElement = m_Root.FindElement(pElementName);
	if (pElement) {
		fDefault = pElement->GetFloat(fDefault);
	}
	return fDefault;
}

/*! ************************************

	\brief Return a floating point value from an Element's RawText

	Find an Element with the supplied name and from the
	RawText that's stored in it parse it as a 32 bit floating point
	numeric value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param pElementName Pointer to a "C" string with the name of the Element
	\param fDefault Value to return on error
	\param fMin Minimum acceptable value
	\param fMax Maximum acceptable value
	\return Value in between fMin and fMax or fDefault
	\sa ElementGetDouble(const char *,double,double,double) const, ElementSetFloat(const char *,float) or AsciiToFloat(const char *,float,float,float)

***************************************/

float BURGER_API Burger::FileXML::Element::ElementGetFloat(const char *pElementName,float fDefault,float fMin,float fMax) const
{
	const Element *pElement = m_Root.FindElement(pElementName);
	if (pElement) {
		fDefault = pElement->GetFloat(fDefault,fMin,fMax);
	}
	return fDefault;
}

/*! ************************************

	\brief Set a 32 bit floating point value into a named Element's RawText

	Convert the input into an floating point representation
	of a UTF-8 string and set the value to this string

	\param pElementName Pointer to a "C" string with the name of the Element
	\param fValue Value to store as a floating point string
	\sa ElementGetFloat(const char *,float,float,float) const or ElementSetDouble(const char *,double)

***************************************/

void BURGER_API Burger::FileXML::Element::ElementSetFloat(const char *pElementName,float fValue)
{
	Element *pElement = m_Root.FindElement(pElementName,TRUE);
	if (pElement) {
		pElement->SetFloat(fValue);
	}
}

/*! ************************************

	\brief Return a 64 bit floating point value from an Element's RawText

	Find an Element with the supplied name and from the
	RawText that's stored in it parse it as a 64 bit floating point
	numeric value and if successful, return it.
	If it's not a number, return the default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param pElementName Pointer to a "C" string with the name of the Element
	\param dDefault Value to return on error
	\return Value or dDefault
	\sa ElementGetFloat(const char *,float) const, ElementSetDouble(const char *,double) or AsciiToDouble(const char *,double)

***************************************/

double BURGER_API Burger::FileXML::Element::ElementGetDouble(const char *pElementName,double dDefault) const
{
	const Element *pElement = m_Root.FindElement(pElementName);
	if (pElement) {
		dDefault = pElement->GetDouble(dDefault);
	}
	return dDefault;
}

/*! ************************************

	\brief Return a 64 bit floating point value from an Element's RawText

	Find an Element with the supplied name and from the
	RawText that's stored in it parse it as a 64 bit floating point
	numeric value and if successful, test it against
	the valid range and return the value clamped
	to that range. If it's not a number, return the
	default.

	If NaN or Inf is detected, it will be converted to a zero
	to prevent floating point issues.

	\param pElementName Pointer to a "C" string with the name of the Element
	\param dDefault Value to return on error
	\param dMin Minimum acceptable value
	\param dMax Maximum acceptable value
	\return Value in between dMin and dMax or dDefault
	\sa ElementGetFloat(const char *,float,float,float) const, ElementSetDouble(const char *,double) or AsciiToDouble(const char *,double,double,double)

***************************************/

double BURGER_API Burger::FileXML::Element::ElementGetDouble(const char *pElementName,double dDefault,double dMin,double dMax) const
{
	const Element *pElement = m_Root.FindElement(pElementName);
	if (pElement) {
		dDefault = pElement->GetDouble(dDefault,dMin,dMax);
	}
	return dDefault;
}

/*! ************************************

	\brief Set a 64 bit floating point value into a named Element's RawText

	Convert the input into an floating point representation
	of a UTF-8 string and set the value to this string

	\param pElementName Pointer to a "C" string with the name of the Element
	\param dValue Value to store as a 64 bit floating point string
	\sa ElementGetDouble(const char *,double,double,double) const or ElementSetFloat(const char *,float)

***************************************/

void BURGER_API Burger::FileXML::Element::ElementSetDouble(const char *pElementName,double dValue)
{
	Element *pElement = m_Root.FindElement(pElementName,TRUE);
	if (pElement) {
		pElement->SetDouble(dValue);
	}
}

/*! ************************************

	\brief Locate and return the named Element's RawText text

	Return the pointer to the value of the named Element's first RawText object.

	If it's not present, then return the default value

	\param pElementName Pointer to a "C" string with the name of the Element
	\param pDefault String to return on error
	\return Pointer to the RawText string or pDefault
	\sa ElementSetString(const char *,const char *)

***************************************/

const char *BURGER_API Burger::FileXML::Element::ElementGetString(const char *pElementName,const char *pDefault) const
{
	const Element *pElement = m_Root.FindElement(pElementName);
	if (pElement) {
		pDefault = pElement->GetString(pDefault);
	}
	return pDefault;
}

/*! ************************************

	\brief Set the the named Element's RawText string

	Replace the named Element's first RawText string with this string.

	If there isn't a Element object by this name found, create one with this string.

	\param pElementName Pointer to a "C" string with the name of the Element
	\param pValue Pointer to a "C" string to copy into the RawText object
	\sa ElementGetString(const char *,const char *) const

***************************************/

void BURGER_API Burger::FileXML::Element::ElementSetString(const char *pElementName,const char *pValue)
{
	Element *pElement = m_Root.FindElement(pElementName,TRUE);
	if (pElement) {
		pElement->SetString(pValue);
	}
}

/*! ************************************

	\brief Create a new Element structure and initialize it from an input file

	Parse the input file after the starting < and up to and including the ending />
	or > followed by internal data and the following &lt;/endname&gt;

	Will return \ref NULL on a parsing error or if it's out of memory

	\param pInput Pointer to an input stream at the byte after the <
	\return \ref NULL on error or a valid Element record on a successful parse

***************************************/

Burger::FileXML::Element * BURGER_API Burger::FileXML::Element::New(InputMemoryStream *pInput)
{
	Element *pElement = new (Alloc(sizeof(Element))) Element;
	if (pElement) {
		if (pElement->Parse(pInput)) {
			Delete(pElement);
			pElement = NULL;
		}
	}
	return pElement;
}

/*! ************************************

	\brief Create a new Element structure and initialize its name

	Allocate memory for a new Element and set the name.

	\param pName Pointer to a "C" string with the name of the element
	\return \ref NULL on error or a valid Element record

***************************************/

Burger::FileXML::Element * BURGER_API Burger::FileXML::Element::New(const char *pName)
{
	return new (Alloc(sizeof(Element))) Element(pName);
}



/*! ************************************

	\class Burger::FileXML
	\brief Reads and writes XML files

	This class will read and write out XML text files

	\code
	<?xml version="1.0" ?>
	<!-- This is a sample of the contents -->
	<!-- of a legal XML file -->

	<!-- Section without a name, access by passing "" as the section name -->
	<Data>1</Data>
	<String>Hello there</String>

	<!-- Section, access with the string "Foobar" (It's case insensitive) -->
	<FOOBAR attribute="55">
		<Data>2</Data>
		<Hi>Hi</Hi>
	</FOOBAR>
	<!-- Another section with duplicate names -->
	<foobar2>
		<Data>2</Data>
		<Float>3.52</Float>
	</foobar2>
	\endcode

	To read the above XML file, here's some sample code.

	\code
	FileXML FooFile;
	if (!FooFile.LoadFile("9:foo.xml")) {
		FileXML::Element *pElement = FooFile.FindElement("FOOBAR");
		if (pElement) {
			Word uData = pElement->ElementGetWord("Data",0,0,255);
			const char *pHi = pElement->ElementGetString("Hi","Not found");
			Word uAttribute = pElement->AttributeGetWord("attribute",0);
		}
		pElement = LoadFile.FindElement("Data");
		Word uData2 = pElement->GetWord(0,0,255);
	}
	\endcode

	\sa Burger::FileINI

***************************************/

/*! ************************************

	\brief Initialize an XML file parser

	XML Files default with Byte Order Marks disabled.

***************************************/

Burger::FileXML::FileXML(void) :
	m_Root(),
	m_bUTF8ByteMark(FALSE)
{
}

/*! ************************************

	\brief Dispose of an XML file parser

	Release all objects.

	Calling Shutdown() performs this action manually.

	\sa Shutdown(void)

***************************************/

Burger::FileXML::~FileXML()
{
}

/*! ************************************

	\brief Allocate and initialize a FileXML

	\return A pointer to an empty FileXML structure or \ref NULL if out of memory
	\sa New(InputMemoryStream *,Word), New(const char *,Word) or New(Filename *,Word)

***************************************/

Burger::FileXML * BURGER_API Burger::FileXML::New(void)
{
	// Allocate the memory
	return new (Alloc(sizeof(FileXML))) FileXML();
}

/*! ************************************

	\brief Create a new XML file class and initialize it with entries from a text file on disk

	Open the file and read in the entries into a newly allocated FileXML structure.

	Return \ref NULL if the file cannot be opened and the bAlwaysCreate flag is zero

	\param pFilename Pointer to a "C" string of a valid Burgerlib filename
	\param bAlwaysCreate \ref TRUE if the file can't be opened, return an empty record instead
	\return A pointer to an empty FileXML structure or \ref NULL if out of memory
	\sa New(InputMemoryStream *,Word), New(void) or New(Filename *,Word)

***************************************/

Burger::FileXML * BURGER_API Burger::FileXML::New(const char *pFilename,Word bAlwaysCreate)
{
	// Convert to a filename object
	Filename NewName(pFilename);
	return New(&NewName,bAlwaysCreate);
}

/*! ************************************

	\brief Create a new XML file class and initialize it with entries from a text file on disk

	Open the file and read in the entries into a newly allocated FileXML structure.

	Return \ref NULL if the file cannot be opened and the bAlwaysCreate flag is zero

	\param pFilename Pointer to a Filename record
	\param bAlwaysCreate \ref TRUE if the file can't be opened, return an empty record instead
	\return A pointer to an empty FileXML structure or \ref NULL if out of memory
	\sa New(InputMemoryStream *,Word), New(void) or New(const char *,Word)

***************************************/

Burger::FileXML * BURGER_API Burger::FileXML::New(Filename *pFilename,Word bAlwaysCreate)
{
	InputMemoryStream Stream;
	FileXML *pResult;
	// Load into a stream
	if (!Stream.Open(pFilename)) {
		// Create the record
		pResult = New(&Stream,bAlwaysCreate);
	} else if (!bAlwaysCreate) {
		pResult = NULL;
	} else {
		// Create an empty record on missing file or file read error
		pResult = New();
	}
	return pResult;
}

/*! ************************************

	\brief Create a new XML file class and initialize it with entries from a text file in a memory stream

	Read in the entries from the InputMemoryStream into a newly allocated FileXML structure.

	Return \ref NULL if the text stream cannot be parsed and the bAlwaysCreate flag is zero

	\note In the event of a parse failure, if bAlwaysCreate is \ref TRUE, return the FileXML
	record with all the objects that successfully parsed (An incomplete parsing)

	\param pInput Pointer to a InputMemoryStream record that has the text file image
	\param bAlwaysCreate \ref TRUE if the file can't be parsed, return an empty record instead
	\return A pointer to an empty FileXML structure or \ref NULL if out of memory
	\sa New(Filename *,Word), New(void) or New(const char *,Word)

***************************************/

Burger::FileXML * BURGER_API Burger::FileXML::New(InputMemoryStream *pInput,Word bAlwaysCreate)
{
	FileXML *pResult = New();	// Init the structure
	if (pResult) {
		// Fill in the data
		if (pResult->Init(pInput)) {
			// Failure? Destroy or return a class instance anyways?
			if (!bAlwaysCreate) {
				// On error, delete it
				Delete(pInput);
				pInput = NULL;
			}
		}
	}
	return pResult;
}

/*! ************************************

	\brief Initialize the records with a file on disk

	Purge all records from the class and load in
	new data from a file on disk using a Burgerlib formatted filename.

	\param pFilename Pointer to a "C" string of a valid Burgerlib filename
	\return Zero on success, or non-zero on failure
	\sa Init(InputMemoryStream *) or Init(const char *)

***************************************/

Word BURGER_API Burger::FileXML::Init(const char *pFilename)
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

Word BURGER_API Burger::FileXML::Init(Filename *pFilename)
{
	// Purge
	Shutdown();
	InputMemoryStream Stream;
	Word uResult = 10;
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

	\param pInput Pointer to an image of the XML text file in memory
	\return Zero on success, or non-zero on failure
	\sa Init(Filename *) or Init(const char *)

***************************************/

Word BURGER_API Burger::FileXML::Init(InputMemoryStream *pInput)
{
	// Purge any data in the file record
	Shutdown();

	// Detect a byte order mark
	if (pInput->IsDataMatch(UTF8::ByteOrderMark,3)) {
		// Retain it
		m_bUTF8ByteMark = TRUE;
	}
	// Parse the XML file
	return m_Root.ParseList(pInput,FALSE); 
}

/*! ************************************

	\brief Dispose of the contents of the structure

	Every record is released from memory and the Byte Order Mark flag is cleared

***************************************/

void BURGER_API Burger::FileXML::Shutdown(void)
{
	m_Root.DeleteList();
	m_bUTF8ByteMark = FALSE;
}

/*! ************************************

	\brief Save an XML file

	Serialize the data contained in the entire class 
	as an XML text file.

	\note If any text encoding other than UTF8 is used, then ensure 
	that the Byte Order Mark is disabled so other parsers won't get confused.

	\param pOutput Output string to store the UTF-8 file image of the data
	\return Zero on success, non zero on error

***************************************/

Word BURGER_API Burger::FileXML::Save(OutputMemoryStream *pOutput) const
{
	Word uResult=FALSE;
	if (m_bUTF8ByteMark) {
		uResult = pOutput->Append(UTF8::ByteOrderMark,3);
	}
	if (!uResult) {
		uResult = m_Root.SaveList(pOutput,0);
	}
	return uResult;
}

/*! ************************************

	\fn BURGER_INLINE Word Burger::FileXML::GetUTF8ByteMark(void) const
	\brief Get the flag for writing a UTF8 byte mark

	If this flag is non-zero, the file read in (Or to be written) had
	a three byte UTF8 Byte Order Mark.

	\return Zero on if no Byte Order Mark was found or will be written, non zero on if so.
	\sa SetUTF8ByteMark(Word) or Save(OutputMemoryStream *) const

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::SetUTF8ByteMark(Word bUTF8ByteMark)
	\brief Set the flag for writing a UTF8 byte mark

	If this flag is non-zero, the file written out will have
	a three byte UTF8 Byte Order Mark first.

	\param bUTF8ByteMark Zero on if no Byte Order Mark will be written, non zero on if so.
	\sa GetUTF8ByteMark(void) const or Save(OutputMemoryStream *) const

***************************************/

/*! ************************************

	\brief Extract an XML label from an input stream.

	Using XML rules, extract a string from the input stream and store
	it in the output string.

	The first character must be a letter, an underscore or a high ASCII
	character, followed by alphanumeric, high ASCII, underscores, '-', '.' and ':'

	\param pOutput Pointer to a String to contain the output
	\param pInput Pointer to an input stream that's pointing to the start of the label

***************************************/

Word BURGER_API Burger::FileXML::ReadXMLName(String *pOutput,InputMemoryStream *pInput)
{
	Word uResult = TRUE;
	WordPtr uMark = pInput->GetMark();
	Word uTemp = pInput->GetByte();
	// The first character of a name MUST be an underscore or an ASCII letter (Assume >128 is UTF8)
	if ((uTemp>=128) ||
		(g_AsciiTestTable[uTemp]&(ASCII_UPPER|ASCII_LOWER)) ||
		(uTemp=='_')) {
		// Continue until it hits a delimiter
		do {
			uTemp = pInput->GetByte();
		} while ((uTemp>=128) ||
				(g_AsciiTestTable[uTemp]&(ASCII_UPPER|ASCII_LOWER|ASCII_DIGIT)) ||
				(uTemp=='_') ||
				(uTemp=='-') ||
				(uTemp=='.') ||
				(uTemp==':'));
		// Get the size of the string parsed
		WordPtr uSize = (pInput->GetMark()-uMark)-1;
		pOutput->SetBufferSize(uSize);
		pInput->SetMark(uMark);
		// Copy the string
		pInput->Get(pOutput->GetPtr(),uSize);
		// No error
		uResult = 0;
	} else {
		pInput->SetMark(uMark);
		pOutput->Clear();
	}
	return uResult;
}

/*! ************************************

	\brief Extract a XML text from an input stream.

	Extract a quoted string from the input stream.

	\param pOutput Pointer to a String to contain the output
	\param pInput Pointer to an input stream that's pointing to the start of the label

***************************************/

Word BURGER_API Burger::FileXML::ReadXMLText(String *pOutput,InputMemoryStream *pInput)
{
	Word uResult = TRUE;
	WordPtr uMark = pInput->GetMark();

	// Test if quoted
	Word uTemp = pInput->GetByte();
	if ((uTemp=='\'') || (uTemp=='"')) {
		Word uDelimiter = uTemp;				// Use this for the quote
		do {
			uTemp = pInput->GetByte();			// Get the next character
			if (uTemp==uDelimiter) {
				// Get the size of the string parsed
				WordPtr uSize = (pInput->GetMark()-uMark)-2;
				pOutput->SetBufferSize(uSize);
				pInput->SetMark(uMark+1);
				// Copy the string
				pInput->Get(pOutput->GetPtr(),uSize);
				pInput->SkipForward(1);
				// No error
				uResult = 0;
				break;
			}
		} while (uTemp);

	// According to the standard, it should fail. However, too many files have non-quoted
	// attributes, so handle it with space delimiters
	} else {
		while ((uTemp>=128) ||
			((uTemp!='/') && (uTemp!='>') && (!(g_AsciiTestTable[uTemp]&ASCII_SPACE)))) {
			uTemp = pInput->GetByte();
		}
		WordPtr uSize = (pInput->GetMark()-uMark)-1;
		if (uSize) {
			pOutput->SetBufferSize(uSize);
			pInput->SetMark(uMark);
			// Copy the string
			pInput->Get(pOutput->GetPtr(),uSize);
			// No error
			uResult = 0;
		}
	}
	if (uResult) {
		pInput->SetMark(uMark);
		pOutput->Clear();
	}
	return uResult;
}

/*! ************************************

	\brief Output a string in XML encoding

	XML files use certain characters for the file format. To use
	these characters as text, they need to be encoded. This
	function will take a text string and output it
	in XML format.

	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>ASCII</th><th>Code</th></tr>
	<tr><td>&amp;</td><td>&amp;amp;</td></tr>
	<tr><td>&apos;</td><td>&amp;apos;</td></tr>
	<tr><td>&quot;</td><td>&amp;quot;</td></tr>
	<tr><td>&gt;</td><td>&amp;gt;</td></tr>
	<tr><td>&lt;</td><td>&amp;lt;</td></tr>
	</table>

	\param pOutput Pointer to a String to contain the output
	\param pInput Pointer to an input stream that's pointing to the start of the label
	\return Zero if no errors, non-zero for a save error
	\sa DecodeXMLString()

***************************************/

Word BURGER_API Burger::FileXML::SaveXMLString(OutputMemoryStream *pOutput,const char *pInput)
{
	Word uResult = 0;		// Assume okay
	// Work as unsigned.
	const Word8 *pWork = reinterpret_cast<const Word8 *>(pInput);
	if (pWork) {
		do {
			Word uTemp = pWork[0];
			if (!uTemp) {
				break;
				// Ampersand already inserted?
			} else if (uTemp=='&') {
				if ((pWork[1] == '#') &&
					(pWork[2] == 'x')) {
					do {
						// Save the string to the point of ';' or end
						uTemp = pWork[0];
						if (!uTemp) {
							break;
						}
						++pWork;
						uResult = pOutput->Append(static_cast<char>(uTemp));
						if (uResult) {			// Abort on error
							break;
						}
					} while (uTemp!=';');		// Reached the end of output?
					if (!uTemp) {
						break;
					}
				} else {
					uResult = pOutput->Append("&amp;");
				}
			} else if (uTemp=='<') {
				uResult = pOutput->Append("&lt;");
			} else if (uTemp=='>') {
				uResult = pOutput->Append("&gt;");
			} else if (uTemp=='"') {
				uResult = pOutput->Append("&quot;");
			} else if (uTemp=='\'') {
				uResult = pOutput->Append("&apos;");
			} else if (uTemp<32) {
				uResult = pOutput->Append("&#");
				uResult |= pOutput->AppendAscii(static_cast<Word32>(uTemp));
				uResult |= pOutput->Append(';');
			} else {
				uResult = pOutput->Append(static_cast<char>(uTemp));
			}
			if (!uTemp) {
				break;
			}
			++pWork;
		} while (!uResult);
	}
	return uResult;
}

/*! ************************************

	\brief Decode a string using XML encoding

	XML files use certain characters for the file format. To use
	these characters as text, they need to be encoded. This
	function will take a text string encoded in XML format and
	decode it to a UTF-8 string.

	\htmlonly
	<table border="1" style="margin-right:auto;margin-left:auto;text-align:center;width:80%">
	<tr><th>ASCII</th><th>Code</th></tr>
	<tr><td>&amp;</td><td>&amp;amp;</td></tr>
	<tr><td>&apos;</td><td>&amp;apos;</td></tr>
	<tr><td>&quot;</td><td>&amp;quot;</td></tr>
	<tr><td>&gt;</td><td>&amp;gt;</td></tr>
	<tr><td>&lt;</td><td>&amp;lt;</td></tr>
	<tr><td>\n</td><td>&amp;#10;</td></tr>
	<tr><td>&#x7F8E;</td><td>&amp;#x7F8E;</td></tr>
	</table>
	\endhtmlonly

	\param pInput Pointer to a String to convert
	\return Zero if no errors occurred in decoding, non-zero if there was an issue with the string.
	\sa SaveXMLString()

***************************************/

Word BURGER_API Burger::FileXML::DecodeXMLString(String *pInput)
{
	Word uResult = 0;
	Word8 *pSource = reinterpret_cast<Word8 *>(pInput->GetPtr());
	Word uTemp = pSource[0];
	if (uTemp) {
		Word8 *pDest = pSource;
		do {
			++pSource;
			// Found an entity?
			if (uTemp=='&') {
				Word uReplace = 0;
				// Check the hard coded ones first
				if (!MemoryCompare("amp",pSource,3)) {
					uReplace = '&';
					pSource+=3;
				} else if (!MemoryCompare("lt",pSource,2)) {
					uReplace = '<';
					pSource+=2;
				} else if (!MemoryCompare("gt",pSource,2)) {
					uReplace = '>';
					pSource+=2;
				} else if (!MemoryCompare("quot",pSource,4)) {
					uReplace = '"';
					pSource+=4;
				} else if (!MemoryCompare("apos",pSource,4)) {
					uReplace = '\'';
					pSource+=4;
				}
				// A hard coded match!
				if (uReplace) {
					pDest[0] = static_cast<Word8>(uReplace);
					++pDest;
					// Remove the trailing ';'
					if (pSource[0]==';') {
						++pSource;
					}
				} else {
					if (pSource[0]!='#') {
						uResult = TRUE;		// Parsing error!!!
					} else {
						++pSource;	// Accept the '#'
						Word32 uUTF32;
						const char *pEnd;
						// Try if it's numerically encoded
						if (pSource[0]=='x') {
							// Force hex decoding
							pSource[0]='$';
						}
						// Convert the ascii
						uUTF32 = AsciiToInteger(reinterpret_cast<const char *>(pSource),&pEnd);
						// Get the new marker
						pSource = const_cast<Word8*>(reinterpret_cast<const Word8*>(pEnd));
						// Removed any trailing ';'
						if (pSource[0]==';') {
							++pSource;
						}
						// Convert from UTF32 to UFT8
						WordPtr uChunk = UTF8::FromUTF32(reinterpret_cast<char *>(pDest),uUTF32);
						// Invalid conversion (And nulls are not allowed)
						if (!uChunk || !uUTF32) {
							// Parse error
							uResult = TRUE;
						} else {
							// Accept the UTF-8 output
							pDest+=uChunk;
						}
					}
				}
			} else {
				// Normal character
				pDest[0] = static_cast<Word8>(uTemp);
				++pDest;
			}
			uTemp = pSource[0];
		} while (uTemp);
		// Done parsing, resize the buffer if needed
		pInput->SetBufferSize(static_cast<WordPtr>(reinterpret_cast<char*>(pDest)-pInput->GetPtr()));	
	}
	// Return error if any
	return uResult;
}

/*! ************************************

	\brief Find the XML declaration
	
	Traverse the linked list for an XML declaration. Return
	\ref NULL if the record wasn't found and bAlwaysCreate is zero. If
	bAlwaysCreate is not zero and the record didn't exist, create one.
	
	\param bAlwaysCreate \ref TRUE to create the section if one didn't already exist
	\return Pointer to an XML declaration or \ref NULL if not found
	\sa DeleteDeclaration() or AddDeclaration()

***************************************/

Burger::FileXML::Declaration *BURGER_API Burger::FileXML::FindDeclaration(Word bAlwaysCreate)
{
	// Assume nothing found
	Generic *pGeneric = m_Root.FindType(Generic::XML_DECLARATION);
	Declaration *pResult = NULL;
	if (pGeneric) {
		pResult = static_cast<Declaration *>(pGeneric);
	}
	if (!pResult && bAlwaysCreate) {
		pResult = Declaration::New(1.0f);
		if (pResult) {
			// Make it FIRST!
			m_Root.InsertAfter(pResult);
		}
	}
	return pResult;
}

/*! ************************************

	\brief Add an XML declaration record
	
	Traverse the linked list for an XML declaration and if one exists, update it.
	Otherwise, create a new record and place it as the first record in the XML file
	
	\return Pointer to an XML declaration or \ref NULL if not found
	\sa FindDeclaration() or DeleteDeclaration()

***************************************/

Burger::FileXML::Declaration * BURGER_API Burger::FileXML::AddDeclaration(float fVersion,const char *pEncoding,Word bStandalone)
{
	Declaration *pDeclaration = FindDeclaration(TRUE);
	if (pDeclaration) {
		pDeclaration->SetVersion(fVersion);
		pDeclaration->SetEncoding(pEncoding);
		pDeclaration->SetStandalone(bStandalone);
	}
	return pDeclaration;
}

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::DeleteDeclaration(void)
	\brief Delete the XML declaration record(s)
	
	Traverse the linked list for a XML declaration(s) and dispose of them.
	
	\return Pointer to an XML declaration or \ref NULL if not found
	\sa FindDeclaration() or AddDeclaration()

***************************************/

/*! ************************************

	\fn BURGER_INLINE Root * Burger::FileXML::GetRoot(void)
	\brief Return the root object

	Return the root object that contains all objects in an XML file for manual
	parsing.
	
	\return Pointer to the root XML object.
	\sa GetRoot(void) const

***************************************/

/*! ************************************

	\fn BURGER_INLINE const Root * Burger::FileXML::GetRoot(void) const
	\brief Return the const root object

	Return the root object that contains all objects in an XML file for manual
	parsing.
	
	\return Pointer to the root XML object.
	\sa GetRoot(void)

***************************************/

/*! ************************************

	\fn BURGER_INLINE const Element * Burger::FileXML::FindElement(const char *pElementName) const
	\brief Find a named XML Element
	
	Traverse the linked list for an XML Element record with the matching name
	using a case sensitive search. Return
	\ref NULL if the record wasn't found.
	
	\param pElementName Pointer to a "C" string to the element to search for.
	\return Pointer to an XML Element or \ref NULL if not found
	\sa DeleteElement() or AddElement()

***************************************/

/*! ************************************

	\fn BURGER_INLINE Element * Burger::FileXML::FindElement(const char *pElementName,Word bAlwaysCreate)
	\brief Find a named XML Element
	
	Traverse the linked list for an XML Element record with the matching name
	using a case sensitive search. Return
	\ref NULL if the record wasn't found and bAlwaysCreate is zero. If
	bAlwaysCreate is not zero and the record didn't exist, create one.
	
	\param pElementName Pointer to a "C" string to the element to search for.
	\param bAlwaysCreate \ref TRUE to create the section if one didn't already exist
	\return Pointer to an XML Element or \ref NULL if not found
	\sa DeleteElement() or AddElement()

***************************************/

/*! ************************************

	\fn BURGER_INLINE Element * Burger::FileXML::AddElement(const char *pElementName)
	\brief Add a new Element record
	
	Create a new Element and place it as the last record in the XML file.

	\note XML allows multiple Elements with the same name. This function will not check if any
	preexisting Element records possess the same name.
	
	\param pElementName Pointer to a "C" string to the element to search for.
	\return Pointer to an XML Element or \ref NULL if out of memory
	\sa FindElement(const char *,Word) or DeleteElement()

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::DeleteElement(const char *pElementName)
	\brief Delete an Element record
	
	Traverse the linked list for the first Element with the supplied name
	and dispose of it. It will stop after the first deletion. If there
	are multiple records with the same name, the duplicates will remain.
	
	\param pElementName Pointer to a "C" string to the element to search for.
	\sa DeleteElements(const char *), FindElement(const char *,Word) or AddElement(const char *)

***************************************/

/*! ************************************

	\fn BURGER_INLINE void Burger::FileXML::DeleteElements(const char *pElementName)
	\brief Delete all Element records with this name
	
	Traverse the linked list for the all Element records with the supplied name
	and dispose of them.
	
	\param pElementName Pointer to a "C" string to the element to search for.
	\sa DeleteElement(const char *), FindElement(const char *,Word) or AddElement(const char *)

***************************************/

/*! ************************************

	\fn BURGER_INLINE Element Burger::FileXML::GetFirstElement(void) const
	\brief Return the pointer to the first Element object

	Scan the XML file for the first Element object at the root level. If an Element is found
	return the pointer to the Element object. Return \ref NULL if there
	are no Element objects.

	\return \ref NULL if there are no Element objects or the first Element object pointer
	\sa GetFirstItem(void) const

***************************************/

/*! ************************************

	\fn BURGER_INLINE Generic Burger::FileXML::GetFirstItem(void) const
	\brief Return the pointer to the first object

	Return the first object in the XML file. If the file is empty, return \ref NULL

	\return \ref NULL if there are no objects or a valid Generic pointer
	\sa GetFirstElement(void) const

***************************************/

