// A temporary header to supplement sal.h with things that currently 
// cannot be put into sal.h.
// This is designed to be included at the beginning of a compilation unit
// (via force-include) if necessary.

#ifndef SAL_SUPP_H // [
#define SAL_SUPP_H

#if !defined(__midl) && defined(_PREFAST_) && _MSC_VER >= 1000 // [

// Declare a function to be an annotation or primop (respectively).
// Done this way so that they don't appear in the regular compiler's
// namespace.
#define __ANNOTATION(fun)          __declspec("SAL_annotation") void __SA_##fun
 
#define __PRIMOP(type, fun)        __declspec("SAL_primop") type __SA_##fun

// We're tying to phase this out, someday.  The parser quotes the param.
#define __AuToQuOtE                         __declspec("SAL_AuToQuOtE")

// Normally the parser does some simple type checking of annotation params,
// defer that check to the plugin.
#define __deferTypecheck                    __declspec("SAL_deferTypecheck")

__ANNOTATION(SAL_useHeader(void));
#define __useHeader                               __declspec("SAL_useHeader()")

__ANNOTATION(SAL_entrypoint(__AuToQuOtE char *, __AuToQuOtE char *));
__ANNOTATION(SAL_success(__int64));

// Only appears in model files, but needs to be declared.
//???
__ANNOTATION(SAL_TypeName(__AuToQuOtE char *));

// These are needed because the declarations below need to be after __in
// (etc.) is defined.  Once this file is merged then the 'x' vesions should
// be the standard ones.
#define __xin __declspec("SAL_pre") __declspec("SAL_valid") __declspec("SAL_pre") __declspec("SAL_deref") __declspec("SAL_readonly")

__ANNOTATION(SAL_blocksOn(__xin void*));

//__declspec("SAL_pre")
//__declspec("SAL_valid")
//__declspec("SAL_pre")
//__declspec("SAL_deref")
//__declspec("SAL_readonly")
__PRIMOP(char *, macroValue$(__xin char *));
__PRIMOP(int, macroDefined$(__xin char *));
__PRIMOP(char *, strstr$(__xin char *, __xin char *));

#else // ][

#define __ANNOTATION(fun)
#define __PRIMOP(type, fun)
#define __AuToQuOtE
#define __deferTypecheck
#define __useHeader

#endif // ]

#endif // ]

