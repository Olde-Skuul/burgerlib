/***************************************

	Memory manager for Playstation Vita

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __PSVITA_MEMORY_H__
#define __PSVITA_MEMORY_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef _STDLIB
#include <stdlib.h>
#endif

// Playstation Vita needs these calls so the Vita libraries can use the app's
// memory manager

namespace std {
class bad_alloc;
struct nothrow_t;
}

extern "C" {
void user_malloc_init(void);
void user_malloc_finalize(void);
void* user_malloc(size_t size);
void user_free(void* ptr);
void* user_calloc(size_t nelem, size_t size);
void* user_realloc(void* ptr, size_t size);
void* user_memalign(size_t boundary, size_t size);
void* user_reallocalign(void* ptr, size_t size, size_t boundary);
int user_malloc_stats(struct malloc_managed_size* mmsize);
int user_malloc_stats_fast(struct malloc_managed_size* mmsize);
size_t user_malloc_usable_size(void* ptr);
void user_malloc_for_tls_init(void);
void user_malloc_for_tls_finalize(void);
void* user_malloc_for_tls(size_t size);
void user_free_for_tls(void* ptr);
};
void* user_new(std::size_t size) throw(std::bad_alloc);
void* user_new(std::size_t size, const std::nothrow_t& x) throw();
void* user_new_array(std::size_t size) throw(std::bad_alloc);
void* user_new_array(std::size_t size, const std::nothrow_t& x) throw();
void user_delete(void* ptr) throw();
void user_delete(void* ptr, const std::nothrow_t& x) throw();
void user_delete_array(void* ptr) throw();
void user_delete_array(void* ptr, const std::nothrow_t& x) throw();

/* BEGIN */
/* END */
#endif
