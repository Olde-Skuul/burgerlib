/***************************************

	Flash player action buffer
	
	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#ifndef __BRFLASHACTIONVALUE_H__
#define __BRFLASHACTIONVALUE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSTATICRTTI_H__
#include "brstaticrtti.h"
#endif

#ifndef __BRSMARTPOINTER_H__
#include "brsmartpointer.h"
#endif

#ifndef __BRSTRING_H__
#include "brstring.h"
#endif

#ifndef __BRFLASHMANAGER_H__
#include "brflashmanager.h"
#endif

#ifndef __BRFLOATINGPOINT_H__
#include "brfloatingpoint.h"
#endif

/* BEGIN */
namespace Burger { namespace Flash {
class ActionScriptFunction;
class ActionScriptObject;
class ActionScriptValue;
class OldFlashManager;
class ActionScriptSFunction;

class ActionScriptProperty : public WeakAndStrongBase {
	SmartPointer<ActionScriptFunction> m_GetFunction;		///< Function object to call when data is to be retrieved
	SmartPointer<ActionScriptFunction> m_SetFunction;		///< Function object to call when data is being updated
	BURGER_RTTI_IN_CLASS();
public:
	ActionScriptProperty(const ActionScriptValue &rGetFunction,const ActionScriptValue &rSetFunction);
	virtual ~ActionScriptProperty();
	void SetFunction(ActionScriptObject *pTargetObject,const ActionScriptValue &rValue);
	void GetFunction(ActionScriptObject *pTargetObject,ActionScriptValue *pValue) const;
	void GetFunction(const ActionScriptValue &rTargetObject,ActionScriptValue *pValue) const;
	void Print(void) const;
};
class ActionScriptValue {
public:
	// flags defining the level of protection of a value
	enum eValueFlag {
		DONT_ENUM = 0x01,
		DONT_DELETE = 0x02,
		READ_ONLY = 0x04
	};
private:
	enum eType {
		UNDEFINED,
		BOOLEAN,
		NUMBER,
		STRING,
		OBJECT,
		PROPERTY
	};
	SmartPointer<ActionScriptObject> m_pObject;	///< Object that this variable belongs to
	SmartPointer<ActionScriptObject> m_pPropertyTarget;	///< Property target for this variable
	SmartPointer<ActionScriptProperty> m_pProperty;		///< Property for this variable

	mutable String m_String;	///< String variable
	double m_dNumber;			///< Numeric value (As double)
	Word m_bBool;				///< Boolean value
	eType m_eType;				///< Type of data contained
	mutable Word m_uFlags;		///< eValueFlag flags
public:
	// constructors
	ActionScriptValue();
	ActionScriptValue(const ActionScriptValue &rInput);
	ActionScriptValue(const char *pInput);
	ActionScriptValue(bool bInput);
	ActionScriptValue(int iInput);
	ActionScriptValue(float fInput);
	ActionScriptValue(double dInput);
	ActionScriptValue(ActionScriptObject* pObject);
	ActionScriptValue(Manager *pManager,ActionScriptCFunctionProc pProc);
	ActionScriptValue(ActionScriptSFunction *pInput);
	ActionScriptValue(const ActionScriptValue &rGetFunction,const ActionScriptValue &rSetFunction);
	~ActionScriptValue();

	void ReleaseReferences(void);
	const char *ToDebugString(void) const;
	const char *ToCharPtr(void) const;
	const String &ToString(void) const;
	double ToDouble(void) const;
	int ToInt(void) const { return (int) ToDouble(); };
	float ToFloat(void) const { return (float) ToDouble(); };
	Word ToBool(void) const;
	ActionScriptFunction* CastToFunction(void) const;
	ActionScriptObject* CastToObject(void) const;
	ActionScriptProperty* CastToProperty(void) const;
	void SetString(const String &rInput);
	void SetString(const char *pInput);
	void SetDouble(double dInput);
	void SetBool(Word bInput);
	void SetInt(int iInput) { SetDouble(static_cast<double>(iInput)); }
	void SetNaN(void) { SetDouble(g_dNan); }
	void SetAsObject(ActionScriptObject *pObject);
	void SetAsCFunction(Manager *pManager,ActionScriptCFunctionProc pProc);
	void SetUndefined(void);
	void SetNULL(void) { SetAsObject(NULL); }

	void SetProperty(const ActionScriptValue &rInput);
	void GetProperty(ActionScriptValue *pScript) const;
	void GetProperty(const ActionScriptValue &rInput,ActionScriptValue *pScript) const;
	const ActionScriptObject *GetPropertyTarget(void) const;
	void SetPropertyTarget(ActionScriptObject *pObject);

	void operator=(const ActionScriptValue &rInput);
	Word operator==(const ActionScriptValue &rInput) const;
	Word operator!=(const ActionScriptValue &rInput) const;
	Word operator<(double dInput) const { return ToDouble() < dInput; }
	void operator+=(double dInput) { SetDouble(ToDouble() + dInput); }
	void operator-=(double dInput) { SetDouble(ToDouble() - dInput); }
	void operator*=(double dInput) { SetDouble(ToDouble() * dInput); }
	void operator/=(double dInput) { SetDouble(ToDouble() / dInput); }
	void operator&=(int iInput) { SetInt(int(ToDouble()) & iInput); }
	void operator|=(int iInput) { SetInt(int(ToDouble()) | iInput); }
	void operator^=(int iInput) { SetInt(int(ToDouble()) ^ iInput); }
	void shl(int iInput) { SetInt(int(ToDouble()) << iInput); }
	void asr(int iInput) { SetInt(int(ToDouble()) >> iInput); }
	void lsr(int iInput) { SetInt(static_cast<int>((Word32(ToDouble()) >> iInput))); }

	Word IsFunction(void) const;
	BURGER_INLINE Word IsBool(void) const { return m_eType == BOOLEAN; }
	BURGER_INLINE Word IsString(void) const { return m_eType == STRING; }
	BURGER_INLINE Word IsNumber(void) const { return m_eType == NUMBER && IsNan(m_dNumber) == false; }
	BURGER_INLINE Word IsObject(void) const { return m_eType == OBJECT; }
	BURGER_INLINE Word IsProperty(void) const { return m_eType == PROPERTY; }
	BURGER_INLINE Word IsNULL(void) const { return m_eType == OBJECT && m_pObject == NULL; }
	BURGER_INLINE Word IsUndefined(void) const { return m_eType == UNDEFINED; }
	const char* GetTypeOf(void) const;
	Word IsInstanceOf(const ActionScriptFunction *pFunction) const;
	Word FindProperty(const String &rInput,ActionScriptValue *pValue);
	Word FindPropertyOwner(const String &rInput,ActionScriptValue *pValue);
	BURGER_INLINE Word IsEnum(void) const { return static_cast<Word>(m_uFlags & DONT_ENUM ? FALSE : TRUE); }
	BURGER_INLINE Word IsReadOnly(void) const { return static_cast<Word>(m_uFlags & READ_ONLY ? TRUE : FALSE); }
	BURGER_INLINE Word IsProtected(void) const { return static_cast<Word>(m_uFlags & DONT_DELETE ? TRUE : FALSE); }
	BURGER_INLINE Word GetFlags(void) const { return m_uFlags; }
	BURGER_INLINE void SetFlags(Word flags) const  { m_uFlags = flags; }
	static Word CompareEqual(const ActionScriptValue &rInput1,const ActionScriptValue &rInput2);
	static ActionScriptValue RelationalCompareEqual(const ActionScriptValue &rInput1,const ActionScriptValue &rInput2);
};
}}
/* END */

#endif
