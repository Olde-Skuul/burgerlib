# File system

## General practices

Since file paths are not consistent across platforms, such as whether ``/`` or ``\`` is a pathname delimiter, Burgerlib uses a standardized pathname system using ``:`` as a pathname delimiter.

## Filename formatting

If a pathname starts with a ``:``, it's considered a full pathname and what follows is the drive name, followed by all the directories, separated by colons until the last entry which is either a filename or directory name.

If a pathname starts with a ``.``, it's considered a device name or number. This is considered obsolete in Burgerlib with the exception of Windows pathnames where it will be remapped to a drive letter. All other platforms, this will map to a device, but it's strongly discouraged.

If a pathname starts with a number or with one of these characters, followed by a colon, it's considered a prefix. Numeric values of 0-31, and the characters ``@``, ``$``, and ``*``, are valid. All other values are considered errors.

If the pathname doesn't follow the above guidelines, it's considered a partial pathname and will be prepended with the current working directory which is prefix 8.

## Prefixes

Unlike most operating systems, Burgerlib allows the use of multiple simultaneous working directories where the filename makes its selection by prepending the pathname with a specific prefix. The Burgerlib file system will replace the prefix with the directory assigned to that prefix and then perform the requested file operation. This makes it easy for an application to assign different directories for data types and the file system will manage finding data from a filename.

Below are the preassigned prefixes.

* ``8:`` Current working directory
* ``9:`` Directory of the executable
* ``*:`` Directory of the boot volume
* ``$:`` Directory of the system directory (Considered obsolete)
* ``@:`` Directory of the user's folder
* ``0:`` through ``7:`` Application assigned
* ``10:`` through ``31:`` Application assigned

### 8: Current working directory

When the application is launched, this is the directory of the shell or launcher. This is likely not the directory the application resides, but is likely where the command line parameters are referring to when partial pathnames are passed in.

### 9: Directory of the executable

Directory where the application binary is located. It will always be where the binary resides, so the application can quickly know it's only location for accessing itself or data files that reside in the same directory as the application.

### *: Boot volume

On Windows, this is usually ``.D2`` which translates to ``C:\``. On Linux or MacOSX it's likely ``:`` which becomes ``/``. Under normal circumstances, this is a protected folder, so unless the application has root access, writing to this directory will likely fail. It was a holdover from Windows 95 where it was common to install application data on a folder on the boot volume.

### $: System Directory

On Windows, this usually translates to ``.D2:Windows`` and MacOS 7-9 is ``:Macintosh HD:System Folder:``. This is a holdover from when MacOS classic and Window 3.1/95 applications would store user data in the system folder instead of a user folder. This directory only has meaning on GS/OS and MacOS 7-9 platforms, all other platforms should not use this directory.

### \@: User folder

On systems that don't allow multiple users, such as MSDOS, GS/OS and MacOS 7-9, this is the same as the System Directory ``$:``. On all other systems, this points to the ``~`` folder of the currently logged in user. This is the preferred directory to store saved application data.

### Changing prefixes

To change, or get the information about a prefix, call Burger::FileManager::get_prefix(Burger::Filename\*, uint_t) or Burger::FileManager::set_prefix(uint_t, const char\*). The only restriction on setting a prefix is that the pathname would be considered valid.

### Why colons?

Burgerlib was originally created in 1979 on an Apple ][ and in 1986 it was upgraded for an Apple IIgs using GS/OS. Pathnames on MacOS 6 and GS/OS used colons as pathname delimiters, and GS/OS used pathname prefixes. These systems were made part of Burgerlib and has been retained since 1986.

## Examples of use

```cpp

    // Find the application's music folder
    // and assign prefix 20 to it.
    Filename Temp;
    Burger::FileManager::get_prefix(&Temp, 9);
    Temp.join("Music");
    Burger::FileManager::set_prefix(20, &Temp);

    // Or the faster way...
    // Since a prefix can use an already set prefix.
    Burger::FileManager::set_prefix(20, "9:Music")

    // Save a file to the users folder
    Burger::FileManager::save_text_file(
        "@:MyApp:MyPrefs.txt", "started=1", 9)

    // Load a data file that's next to my application
    void *pBuffer;
    uintptr_t uLength;
    pBuffer = Burger::FileManager::load_file(
        "9:data.dat", &uLength);

```

## Caveats

While Burgerlib does what it can to translate UTF-8 to the underlying file system's character encoding, make sure that if UTF-8 is used, that the operating system on the platform the application is run on can support the characters after remapping. Burgerlib will replace unsupported characters with either underscores or question marks depending on which character is the right one for the underlying operating system.

If the application is to be run on Windows 10, Mac OS X, or modern Linux, the operating system uses UTF-8 as a default, so character encoding is not a concern. If the application is to be run on a classic OS should the set of characters be limited to ensure compatibility.
