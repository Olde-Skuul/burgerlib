Compiler issues {#page_compiler_issues}
===============

Burgerlib is compiled on a myriad of compilers, and as always, not all compilers are the same.
To use burgerlib is simplicity itself. Just add this to the beginning of your source files or headers (Or both).

\section templates Template differences

Modern compilers support the concept of **Substitution Failure Is Not An Error** or SFINAE for short. This is the backbone of modern compilers, however, Burgerlib supports older compilers that do not fully support SFINAE. The Apple MrC and SC compilers do not support SFINAE so there should be no attempt to use complex templates on them. Open Watcom 1.9 has support for SFINAE, but there are some known bugs so some Burgerlib templates are not available, while most are.

### Unsupported on Open Watcom 1.9

* is_function
* enable_if
* remove_const on arrays
* remove_volatile on arrays
* remove_cv on arrays
* add_const on arrays
* add_volatile on arrays
* add_cv on arrays

### Unsupported on Metrowerks 68K

* enable_if
* remove_const on arrays
* remove_volatile on arrays
* remove_cv on arrays
* is_function

\section multibyte_constants Multibyte constants

All compilers except the Open Watcom compiler support the wide character format where 'ABCD' is a valid 4 byte constant. If this is used on Open Watcom, it will create a ``char 'A'`` without issuing a warning or error. If this syntax is used in source code, it will fail. The Burgerlib class FourCharacterCode_t is used to ensure cross compatiblity with Watcom for multibyte character codes.

Another issue is that most compilers treat multibyte constants as signed except the Metrowerks 68K compiler. That compiler assumes the value is unsigned.

\section pointer_width Pointer with vs register width

Most platforms use pointers that are as wide as the register width. The exceptions are the Xbox 360 and the Playstation 3. Both platforms use a 64 bit CPU, but the memory address space is limited to 32 bits. So uintptr_t is a 32 bit wide value to save runtime space. Be aware of this and always use uintptr_t as an integer type that can also hold a pointer.
