// A temporary header to supplement specstrings.h with things that currently 
// cannot be put into specstrings.h.
// This is designed to be included at the beginning of a compilation unit
// (via force-include) if necessary.

#ifndef SPECSTRING_SUPP_H // [
#define SPECSTRING_SUPP_H

#if !defined(__midl) && defined(_PREFAST_) && _MSC_VER >= 1000 // [

#include "sal_supp.h"

#ifdef  __cplusplus
extern "C" {
#endif

__ANNOTATION(SAL_bound(void));
__ANNOTATION(SAL_range(__int64,__int64));
__ANNOTATION(SAL_allocator(void));   //??? resolve with PFD
__ANNOTATION(SAL_file_parser(__AuToQuOtE __xin char *, __xin char *));
__ANNOTATION(SAL_source_code_content(__xin char *));
__ANNOTATION(SAL_analysisHint(__AuToQuOtE char *));
__ANNOTATION(SAL_untrusted_data_source(__AuToQuOtE char *));
__ANNOTATION(SAL_untrusted_data_source_this(__AuToQuOtE char *));
__ANNOTATION(SAL_validated(__AuToQuOtE char *));
__ANNOTATION(SAL_validated_this(__AuToQuOtE char *));
__ANNOTATION(SAL_encoded(void));
__ANNOTATION(SAL_adt(__AuToQuOtE __xin char *, __AuToQuOtE __xin char *));
__ANNOTATION(SAL_add_adt_property(__AuToQuOtE __xin char *, __AuToQuOtE __xin char *));
__ANNOTATION(SAL_remove_adt_property(__AuToQuOtE __xin char *, __AuToQuOtE __xin char *));
__ANNOTATION(SAL_transfer_adt_property_from(__AuToQuOtE __xin char *));
__ANNOTATION(SAL_post_type(__AuToQuOtE __xin char *));
__ANNOTATION(SAL_volatile(void));
__ANNOTATION(SAL_nonvolatile(void));
__ANNOTATION(SAL_nullTerminated(enum __SAL_YesNo));



#define ___group(annotes)													\
	__declspec("SAL_begin") annotes __drv_declspec("SAL_end")

#define ___nop(x) x

#define __static_context(ctx, annotes)										\
	__declspec("SAL_context(" SPECSTRINGIZE(ctx)")") ___group(___nop(annotes))

#define __failure(x) __static_context(SAL_return_convention,			\
	__declspec("SAL_failure(" SPECSTRINGIZE(x) ")"))
// __success should look like that when we can override it; in the meantime
// the parser allows it in POST.

#define __applyTo_return(annotes) 											\
	__static_context(SAL_return_convention, annotes )

#define __on_failure(annotes) 												\
	__static_context(SAL_failed, __post ___group(annotes) )

enum __SAL_failureKind{__failureUnspecified = 0, __failureUndefined = 1};

__ANNOTATION(SAL_failureDefault(enum __SAL_failureKind));
#define __failureDefault(kind) __static_context(SAL_return_convention,	\
	__declspec("SAL_failureDefault(" SPECSTRINGIZE(kind) ")"))

__ANNOTATION(SAL_valueUndefined());

#define __valueUndefined __declspec("SAL_valueUndefined()")

#ifdef  __cplusplus
}
#endif

#else // ][

#define __failure(x)
#define __static_context(ctx, annotes)
#define __applyTo_return(annotes) 
#define __on_failure(annotes)
#define __failureDefault(kind)
#define __valueUndefined

#endif // ]

#endif // ]

