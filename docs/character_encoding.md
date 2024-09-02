# Character Encoding rules

## UTF-8

All 8 bit strings used within Burgerlib are assumed to be encoded in UTF-8 unless the specific function's documatation denotes otherwise. This is to ensure internationalization of all applications using Burgerlib and it's Burgerlib's responsibility when comminicating with the underlying platform for proper character set translation.

This also is enforced with filenames and interaction with environment variables, where they are all converted to the native character encoding whereas an application using Burgerlib will assume it's been already translated to UTF-8

## Character encoding tables

* [ISO Latin 1 character set. (CDROM)](isolatin1.htm)
* [MacOS Roman US character set. (MacOS 7-9)](macromanus.htm)
* [Windows 437 character set. (MSDOS)](windows437.htm)
* [Windows 1252 character set. (Windows)](windows1252.htm)