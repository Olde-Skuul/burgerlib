/***************************************

	Flash player Adobe Virtual Machine Version 2 support
	
	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#ifndef __BRFLASHAVM2_H__
#define __BRFLASHAVM2_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSIMPLEARRAY_H__
#include "brsimplearray.h"
#endif

#ifndef __BRSMARTPOINTER_H__
#include "brsmartpointer.h"
#endif

#ifndef __BRFLASHSTREAM_H__
#include "brflashstream.h"
#endif

#ifndef __BRARRAY_H__
#include "brarray.h"
#endif

/* BEGIN */
namespace Burger { namespace Flash {
class ABC_Definition;
struct OptionDetail_t {
	enum eKind {
		CONSTANT_Undefined=0x00,		///< Unused 
		CONSTANT_Utf8=0x01,				///< UTF8 string
		CONSTANT_Int=0x03,				///< Signed 32 bit integer
		CONSTANT_UInt=0x04,				///< Unsigned 32 bit integer
		CONSTANT_PrivateNs=0x05,		///< Private namespace to the class
		CONSTANT_Double=0x06,			///< 64 bit floating point
		CONSTANT_Namespace=0x08,		///< Global namespace
		CONSTANT_False=0x0A,			///< Boolean \ref FALSE
		CONSTANT_True=0x0B,				///< Boolean \ref TRUE
		CONSTANT_Null=0x0C,				///< \ref NULL pointer
		CONSTANT_PackageNamespace=0x16,	///< Namespace exclusive to the package
		CONSTANT_PackageInternalNs=0x17,	///< Local package namespace
		CONSTANT_ProtectedNamespace=0x18,	///< Protected namespace
		CONSTANT_ExplicitNamespace=0x19,	///< Explicit namespace
		CONSTANT_StaticProtectedNs=0x1A		///< Static protected namespace
	};
	Word32 m_uValue;			///< Index into the constant pool
	eKind m_eKind;				///< Type of variable found in the constant pool
	void BURGER_API Read(Stream *pStream);
};
struct ItemInfo_t {
	Word m_uKey;			///< Key value
	Word m_uValue;			///< Value attached to the key
	void BURGER_API Read(Stream* pStream);
};
class TraitsInfo : public WeakAndStrongBase {
public:
	enum eKind {
		Trait_Slot = 0,		///< Data is found in trait_slot
		Trait_Method = 1,	///< Data is found in trait_method
		Trait_Getter = 2,	///< Data is found in trait_method
		Trait_Setter = 3,	///< Data is found in trait_method
		Trait_Class = 4,	///< Data is found in trait_class
		Trait_Function = 5,	///< Data is found in trait_function
		Trait_Const = 6,	///< Data is found in trait_slot
		Trait_Invalid = 255	///< Invalid record
	};

	enum eAttribute {
		ATTR_Final = 0x1,	///< Is used with Trait_Method, Trait_Getter and Trait_Setter. It marks a method that cannot be overridden by a sub-class
		ATTR_Override = 0x2,///< Is used with Trait_Method, Trait_Getter and Trait_Setter. It marks a method that has been overridden in this class
		ATTR_Metadata = 0x4	///< Is used to signal that the fields metadata_count and metadata follow the data field in the traits_info entry
	};

	int m_uName;			///< Index to the multiname constant pool
	eKind m_eKind;			///< Specific kind of data contained in this class
	Word m_eAttribute;		///< Attributes for how to handle overrides

	// data
	union {
		struct {
			Word m_uSlotID;		///< Position identification for this trait
			Word m_uTypeName;	///< Index into the multiname array for this item's name
			Word m_uVIndex;		///< Index into the constant pool for this data
			Flash::OptionDetail_t::eKind m_eKind;	///< Kind of data in the constant pool
		} trait_slot;			///< Used by eKind \ref Trait_Slot and \ref Trait_Const

		struct {
			Word m_uSlotID;		///< Position identification for this trait
			Word m_uClassIndex;	///< Index into the class array
		} trait_class;			///< Used by eKind \ref Trait_Class

		struct {
			Word m_uSlotID;		///< Position identification for this trait
			Word m_uFunction;	///< Index into the function array
		} trait_function;		///< Used by eKind \ref Trait_Function

		struct {
			Word m_uDispatchID;	///< Index into the dispatcher array
			Word m_uMethodIndex;	///< Index into the method array
		} trait_method;			///< Used by eKind \ref Trait_Method, \ref Trait_Getter or \ref Trait_Setter
	};
	SimpleArray<Word> m_uMetadataArray;	///< Array of indexes into the metadata array
	TraitsInfo();
	virtual ~TraitsInfo();
	void BURGER_API Read(Stream *pStream,ABC_Definition *pABCDefinition);
};
class MetadataInfo : public WeakAndStrongBase {
public:
	Word m_uName;									///< Name index into the string array
	SimpleArray<ItemInfo_t> m_ItemInfoArray;		///< Array of key / value pairs
	MetadataInfo();
	virtual ~MetadataInfo();
	void BURGER_API Read(Stream* pStream);
};
class ExceptionInfo : public WeakAndStrongBase {
public:
	Word m_uFrom;		///< The starting position in the code field from which the exception is enabled.
	Word m_uTo;			///< The ending position in the code field after which the exception is disabled.
	Word m_uTarget;		///< The position in the code field to which control should jump if an exception of type exc_type is encountered
	Word m_uExceptionType;	///< An index into the string array of the constant pool that identifies the name of the type of exception
	Word m_uVariableName;	///< This index into the string array of the constant pool defines the name of the variable that is to receive the exception object
	ExceptionInfo();
	virtual ~ExceptionInfo();
	void BURGER_API Read(Stream* pStream);
};
class InstanceInfo : public WeakAndStrongBase {
public:
	enum eFlags {
		CONSTANT_ClassSealed = 0x01,	///< The class is sealed: properties can not be dynamically added to instances of the class.
		CONSTANT_ClassFinal = 0x02,		///< The class is final: it cannot be a base class for any other class.
		CONSTANT_ClassInterface = 0x04,	///< The class is an interface.
		CONSTANT_ClassProtectedNs = 0x08	///< The class uses its protected namespace and the m_uProtectedNamespace field is present in the InstanceInfo class.
	};
	WeakPointer<ABC_Definition> m_pParentABCDefinition;		///< Parent Adobe Byte Code
	SimpleArray<Word> m_uInterfaceArray;								///< Array of interface indexes
	ClassArray<SmartPointer<TraitsInfo> > m_pTraitArray;				///< Array of instance traits
	Word m_uName;				///< Name of this instance
	Word m_uSuperName;			///< Name of the parent class this derives from
	Word m_uFlags;				///< See \ref eFlags
	Word m_uProtectedNamespace;	///< If protected, this contains the index to the protected namespace
	Word m_uInitializationIndex;	///< Index to the initialization function
	InstanceInfo();
	virtual ~InstanceInfo();
	void BURGER_API Read(Stream *pStream,ABC_Definition *pABCDefinition);
};
class ClassInfo : public WeakAndStrongBase {
public:
	WeakPointer<ABC_Definition> m_pParentABCDefinition;
	ClassArray<SmartPointer<TraitsInfo> > m_pTraitArray;
	Word m_uClassInit;
	ClassInfo();
	virtual ~ClassInfo();
	void BURGER_API Read(Stream* pStream,ABC_Definition *pABCDefinition);
};
class ScriptInfo : public WeakAndStrongBase {
public:
	ClassArray<SmartPointer<TraitsInfo> > m_pTraitArray;
	Word m_uScriptInit;
	ScriptInfo();
	virtual ~ScriptInfo();
	void BURGER_API Read(Stream* pStream,ABC_Definition *pABCDefinition);
};

}}
/* END */

#endif
