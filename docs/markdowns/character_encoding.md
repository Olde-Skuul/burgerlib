# Character Encoding rules

## UTF-8

All 8 bit strings used within Burgerlib are assumed to be encoded in UTF-8 unless the specific function's documentation denotes otherwise. This is to ensure internationalization of all applications using Burgerlib and it's Burgerlib's responsibility when communicating with the underlying platform for proper character set translation.

This also is enforced with filenames and interaction with environment variables, where they are all converted to the native character encoding whereas an application using Burgerlib will assume it's been already translated to UTF-8

## Examples of use

### UTF-8 to UTF-16 or UTF-32

To convert to a 16 or 32 bit string, use the Burger::String16 or Burger::String32 classes and simply use the Burger::String16::assign(const char\*) function to translate to a 16/32 bit string.

The functions Burger::UTF16::translate_from_UTF8(uint16_t\*, uintptr_t, const char\*) and Burger::UTF32::translate_from_UTF8(uint32_t\*, uintptr_t, const char\*) perform the same function but to literal buffers.

Note that these functions do not check for invalid UTF-8 strings. Use the Burger::UTF8::is_valid(const char\*, uintptr_t) function for testing when a foreign string needs to be verified for validity.

### Converting to a UTF-8 string

The simplest was is to create a Burger::String and use the Burger::String::assign(const uint16_t\*) function to quickly translate to UTF-8 or Burger::String::assign(const uint32_t\*) to convert from UTF-32.

The functions Burger::UTF8::from_UTF16(char\* , uintptr_t, const uint16_t\*) and Burger::UTF8::from_UTF32(char\*, uintptr_t, const uint32_t\*) perform the same function but to literal buffers.

### UTF-8 filenames

Normally, characters such as © (Copyright) and ¿ (Spanish question) are stored as standandard UTF-8 encoded characters. But if used for filenames or text for the host's dialogs or console output, they don't show up properly due to differences in character encoding on the host platform. Any function that handles filenames or console/window output in Burgerlib will automatically translate to the native character set, if the character is available.

```cpp
    // Output a text file with a copyright symbol

    // Burgerlib will remove the copyright
    // on file systems that don't support it
    // but will remap it on file systems that do

    // If your text editor doesn't support UTF-8 character
    // sets, use \xA9 in your C String

    // The example below, only the filename is translated
    // The string being saved is not translated, so any UTF-8 characters
    // written, will be written as is.

    Burger::FileManager::save_file(
        "My good stuff©", "Good stuff", 10);
```

## Character encoding tables

* [ISO Latin 1 character set.](isolatin1.htm) Used for translating from UTF-8 to the filename encoding of ISO-9660 standard.
* [MacOS Roman US character set.](macromanus.htm) Used in translating from the MacOS Classic 8 bit character set for United States English to UTF-8.
* [Windows 437 character set.](windows437.htm) Used for translating from the United States English MS DOS character set to UTF-8.
* [Windows 1252 character set.](windows1252.htm) Used for translating from Windows-1252, common in Windows XP and 8 bit Windows APIs to UTF-8.
