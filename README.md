# Welcome to Burgerlib {#mainpage}

## The only low level library you'll ever need

Copyright 1995-2023 by Rebecca Ann Heineman [becky@burgerbecky.com](mailto:becky@burgerbecky.com). Email all suggestions, corrections, optimizations and insults to her and she'll be eternally grateful.

It is released under an MIT Open Source license. Please see [**LICENSE**](https://raw.githubusercontent.com/Olde-Skuul/burgerlib/master/LICENSE) for license details. Yes, you can use it in a commercial title without paying anything, just give me a credit.

Please? It's not like I'm asking you for money!

---

## Online Doxygen Documentation

[Documentation is found here.](http://www.burgerbecky.com/burgerlib/docs)

**Yes, click the link above. DO IT! BOOKMARK IT!**

To build documentation locally, in the ``docs`` folder, type "``buildme -docs``". Make sure that ``doxygen``, ``dot``, and ``pandoc`` are in the path. The HTML documentation will be found in **docs/temp/burgerlibdoxygen/index.htm**

---

## Python is required

[Python can be downloaded here.](https://www.python.org/downloads/)

After Python is installed, run ``pip install -r requirements.txt`` from the root folder. This will ensure ``buildme`` and other python scripts needed by the build system are present.

Once it is installed, run ``buildme`` to build everything in the root or projects folders, or ``buildme whatever.sln`` to build a specific project file. ``buildme`` will build all project files. ``cleanme`` will clean a folder of all temporary files.

## Windows prerequisites

### Open Watcom 1.9 for Dos and Windows 32 targets

To build with Open Watcom, download and install the compiler and set the environment variable ``WATCOM`` to point to the installation folder, usually ``C:\WATCOM``

[Download Open Watcom 1.9 here](http://openwatcom.org/ftp/install/open-watcom-c-win32-1.9.exe)

``` text
SHA-1 236ac33ebd463006be4ecd83d7ebea1c026eb55a
```

### Codewarrior 9.4 for Windows 32 target

To build with Metrowerks Codewarrior, download and install the compiler. To install, first run ``CW_Windows_9.2.exe`` and then install ``CW_Windows_9.4_Update.exe``. After installation, copy the file ``license.dat`` over the file **C:\Program Files (x86)\Metrowerks\CodeWarrior\license.dat** to update the license to a permanent one.

[Download Codewarrior 9.4 for Windows here](https://archive.org/details/cwpro9)

``` text
SHA-1 2a8a839aa34a0e7b8400004788f0cbc3db03f90e for CW_Windows_9.2.exe
SHA-1 6da8be8022bf96b9387b6b14980944092ed2fc46 for CW_Windows_9.4_Update.exe
```

### Current Visual Studio IDE

Download Visual Studio at [Visual Studio website](https://visualstudio.com). Visual Studio 2022 is recommended, however versions from 2003, 2005, all the way to 2022 is supported.

### Visual Studio 2003

Download these three ISO files,

1. [Disk 1](https://archive.org/download/vsnet2003/MSDN%20Visual%20Studio%20NET%202003%20-%20Enterprise%20Architect%20%28Disc%201%29%28Disc%202082%29%28May%202003%29%28X09-51498%29.ISO)
2. [Disk 2](https://archive.org/download/vsnet2003/MSDN%20Visual%20Studio%20NET%202003%20-%20Enterprise%20Architect%20%28Disc%202%29%28Disc%202083%29%28May%202003%29%28X09-51499%29.ISO)
3. [Prerequisites](https://archive.org/download/vsnet2003/MSDN%20Visual%20Studio%20NET%202003%20Prerequisites%20%28Disc%202072%29%28May%202003%29%28X09-51482%29.ISO)
4. [Service Pack 1](https://archive.org/download/Visual-Studio-.NET-2003-SP-1/VS7.1sp1-KB918007-X86.exe.zip)

---

## Tools locations for download

### Visual Studio code

[Download Visual Studio code here](https://code.visualstudio.com/)

### Doxygen

[Download Doxygen for the host platform from here.](https://www.doxygen.nl/download.html)

### Graphviz

[Download Graphviz from here.](https://graphviz.org/download/)

### Pandoc

[Download Pandoc from here.](https://pandoc.org/installing.html)

### 7Zip

[Download 7Zip from here.](https://7-zip.org/)

### DirectX 9.0c SDK June 2010

This is needed to build Burgerlib Windows using older compilers.

[Download DirectX 9.0c SDK June 2010 here](https://www.microsoft.com/en-us/download/details.aspx?id=6812)

``` text
SHA-1 495ad05ae5fc4ecd76b53a69a35bacb66d958608
```

### Vulkan SDK

Needed for Windows builds.

[Download Vulkan SDK here](https://www.lunarg.com/vulkan-sdk/)

---

## Sections of interest

* \ref page_what_is_burgerlib
* \ref page_building
* \ref page_using_burgerlib
* \ref page_compiler_issues
* \ref page_defines
* \ref page_platform_types
* \ref page_white_papers

---

## Credits

* Rebecca Ann Heineman - most of the codebase
* Gary S. Brown - CRC32B
* Jean-loup Gailly - Original Zlib compressor
* Mark Adler - Original Zlib decompressor
* Thatcher Ulrich - Hash template
* Daniel Julius Bernstein - djb2 hash algorithm
* Matt Pritchard - SafePrint
