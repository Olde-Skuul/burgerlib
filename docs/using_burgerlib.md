Using Burgerlib {#page_using_burgerlib}
===============

\section introduction Introduction

To use burgerlib is simplicity itself. Just add this to the beginning of your source files or headers (Or both).

```C
// The #ifndef __BURGER__ is optional, but useful to prevent double includes
// if included from a user supplied header.
#ifndef __BURGER__
#include <burger.h>
#endif
```
