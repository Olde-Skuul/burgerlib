Building Burgerlib {#page_building}
==================

Windows host {#page_windows}
------------

Installing Burgerlib on a Win32 machine.

\section win32copysdk Step 1: Copying the SDKs onto your host machine.

Create a folder on your hard drive to copy all of the SDKs that you will be using. The usual place is C:\\SDKs but you can place them anywhere you wish.

Once the folder is created, you need to copy and unzip these basic SDKs into their respective folders. The links below will download the current stable versions of the respective folder.

* <a href="http://www.burgerbecky.com/burgerlib/w32burger.zip">Burgerlib
    for Win32</a> into w32burger.

* <a href="http://www.burgerbecky.com/burgerlib/w32directx9.zip">DirectX 9
    and 10 headers for Win32</a> into w32directx9.

* <a href="http://www.burgerbecky.com/burgerlib/w32platformsdk.zip">Win32
    headers from Microsoft for all compilers</a> into w32platformsdk.

* <a href="http://www.burgerbecky.com/burgerlib/w32opengl.zip">OpenGL for
    Win32</a> into w32opengl.

* <a href="http://www.burgerbecky.com/burgerlib/w32qt7sdk.zip">Quicktime 7
    headers for Win32</a> into w32qt7sdk.

For best use, an environment variable needs to be set up to point to the SDKs directory. The easiest way is to edit your AUTOEXEC.BAT file. Editing your system registry is acceptable as well, but this way it is easier to modify if you decide to move the folder somewhere else.

Insert this line into your C:\\AUTOEXEC.BAT file.

```C
SET BURGER_SDKS=C:\SDKs
```

\section win32accesssdk Step 2: Accessing the SDKs from the environment.

For each target in your Win32 project, you need to add these folders into the include and link include directories so the headers and the libraries can be found. Please note the use of quotes, it will allow the variable \$(BURGER_SDKS) to contain a space.

\subsection win32vc71 Include directories for Visual Studio .NET 2003.

In Configuration Properties: C/C++ : General : Additional Include Directories...

```C
"$(BURGER_SDKS)\windows\burger";"$(BURGER_SDKS)\windows\opengl";"$(BURGER_SDKS)\windows\directx9";"$(BURGER_SDKS)\windows\windows5"
```

In Configuration Properties: Linker : General : Additional Library Directories...

```C
"$(BURGER_SDKS)\windows\burger";"$(BURGER_SDKS)\windows\opengl";"$(BURGER_SDKS)\windows\directx9"
```

\subsection win32cw9 Access paths for Codewarrior 9.4.

In the menu bar, select Edit : Preferences... : General : Source Trees...

Create the name of "BURGER_SDKS" and set the path to the folder where the SDKS are.

The typical combination is Name = BURGER_SDKS and the environment variable is SDKS which usually resolves to C:\\SDKS.

For each project, in Settings : Target Settings Panels: Target: Access Paths<br>

```C
{BURGER_SDKS}windows\burger
{BURGER_SDKS}windows\opengl
{BURGER_SDKS}windows\directx}
{BURGER_SDKS}windows\windows5
```

\section win32selectlib Step 3: Include the proper library for your target.

Include only one of these libraries for the build target.

\subsection win32selectlibvc71 Libraries for Visual Studio .NET 2003.

* burgervc7w32dbg.lib for no optimizations and debug information.
* burgervc7w32int.lib for full optimizations and debug information.
* burgervc7w32rel.lib for full optimizations and no debug information.

\subsection win32selectlibvc8 Libraries for Visual Studio .NET 2005.

* burgervc8w32dbg.lib for no optimizations and debug information.
* burgervc8w32int.lib for full optimizations and debug information.
* burgervc8w32rel.lib for full optimizations and no debug information.

\subsection win32selectlibcw9 Libraries for Codewarrior 9.4.

* burgerc50w32dbg.lib for no optimizations and debug information.
* burgerc50w32int.lib for full optimizations and debug information.
* burgerc50w32del.lib for full optimizations and no debug information.

\subsection win32selectlibwat Libraries for Open Watcom 1.9.

* burgerwatw32dbg.lib for no optimizations and debug information.
* burgerwatw32int.lib for full optimizations and debug information.
* burgerwatw32rel.lib for full optimizations and no debug information.

### Building with Visual Studio 2008 {#page_vs2008faq}

Q: The error "`__cpuindex()` is not an intrinsic" is appearing when building Burgerlib.

A: Install Service Pack 1 for Visual Studio 2008 to get rid of this error.


macOS host {#page_mac}
----------

\subsection macosinstallsoftware Step 1: Installing development software

\subsubsection powermacinstall Using a PowerPC Mac running with MacOSX Leopard

Install these applications

* <a
    href="http://support.apple.com/downloads/DL761/en_US/QuickTime770_Leopard.dmg">Quicktime
    7.7 for Leopard</a>

* <a
    href="ftp://ftp.perforce.com/perforce/r11.1/bin.macosx105u/P4V.dmg">Perforce
    Visual Client 11.1 for Power Mac</a>

* <a
    href="http://adcdownload.apple.com/Developer_Tools/xcode_3.1.4_developer_tools/xcode314_2809_developerdvd.dmg">XCode
    3.1.4 for MacOSX Leopard PowerPC</a>, may need to be logged into Apple's
    Developer Connection to get access

* <a
    href="https://www.python.org/ftp/python/2.7.8/python-2.7.8-macosx10.5.dmg">Python
    2.7.8 for MacOSX Leopard PowerPC</a>

* <a href="http://pine.barebones.com/freeware/TextWrangler_3.5.3.dmg">Text
    Wrangler for MacOSX Leopard PowerPC</a>

\subsubsection intelmacinstall Using an Intel Mac macOS Mavericks or higher

Install these applications

* <a
    href="ftp://ftp.perforce.com/perforce/r11.1/bin.macosx105u/P4V.dmg">Perforce
    Visual Client 11.1 for Power Mac</a>

* XCode for MacOSX via the app store

* <a
    href="https://www.python.org/ftp/python/2.7.8/python-2.7.8-macosx10.5.dmg">Python
    2.7.8 for MacOSX Leopard PowerPC</a>

* Text Wrangler for MacOSX Via the app store

\subsection macoscopysdk Step 2: Copying the SDKs onto your host machine.

Create a folder on your hard drive to copy all of the SDKs that you will be using. The usual place is SDKS off of your boot volume, but you can place them anywhere you wish.

Once the folder is created, you need to copy and uncompress these basic SDKS into their respective folders. The links below will download the current stable versions of the respective folder.

* <a
    href="http://www.burgerbecky.com/burgerlib/macburger.sitx">Burgerlib for
    MacOS</a> into macburger.

* <a
    href="http://www.burgerbecky.com/burgerlib/macgamesprockets.sitx">Gamesprockets
    for MacOS 9</a> into macgamesprockets.

* <a href="http://www.burgerbecky.com/burgerlib/macglide.sitx">Glide for
    MacOS 9 (Obsolete)</a> into macglide.

* <a href="http://www.burgerbecky.com/burgerlib/macopengl.sitx">OpenGL for
    MacOS 9</a> into macopengl.

* <a href="http://www.burgerbecky.com/burgerlib/macqt6sdk.sitx">Quicktime
    for MacOS 9</a> into macqt6sdk.

* <a href="http://www.burgerbecky.com/burgerlib/macstdclib.sitx">StdCLib
    support for Classic and OSX.</a> into macstdclib.

\subsection macosaccesssdk Step 3: Accessing the SDKs in the build environment.

\subsubsection macoscw9 Access paths for Codewarrior 8.3 or 10.0.

In the menu bar, select Edit : Preferences... : General : Source Trees...

Create the name of "BURGER_SDKS" and set the path to the folder where the SDKS are.

The typical combination is Name = BURGER_SDKS and the Path = /SDKs.

For each project, in Settings : Target Settings Panels: Target: Access Paths

```C
{BURGER_SDKS}macburger
{BURGER_SDKS}macopengl
{BURGER_SDKS}macgamesprockets
```

\subsection macosselectlib Step 4: Include the proper library for your target.

Include only one of these libraries for the build target.

\subsubsection macosselectlibc10 Libraries for Codewarrior 10.0 / PowerPC.

* burgerc10macppcdbg.lib for no optimizations and debug information
    (Classic only).

* burgerc10macppcdbf.lib for full optimizations and debug information
    (Classic only).

* burgerc10macppcrel.lib for full optimizations and no debug information
    (Classic only).

* burgerc10carppcdbg.lib for no optimizations and debug information
    (Carbon for OS9 and OSX).

* burgerc10carppcdbf.lib for full optimizations and debug information
    (Carbon for OS9 and OSX).

* burgerc10carppcrel.lib for full optimizations and no debug information
    (Carbon for OS9 and OSX).

* burgerc10osxdbg.lib for no optimizations and debug information (OSX
    Mach-O).

* burgerc10osxdbf.lib for full optimizations and debug information (OSX
    Mach-O).

* burgerc10osxrel.lib for full optimizations and no debug information (OSX
    Mach-O).

\subsubsection macosselectlibcw8 Codewarrior 8.3 / 68k classic only.

* burgercw868kdbg.lib for no optimizations and debug information.

* burgercw868kdbf.lib for full optimizations and debug information.

* burgercw868krel.lib for full optimizations and no debug information.

* burgercw868kdbgfar.lib for no optimizations and debug information (Far
    data).

* burgercw868kdbffar.lib for full optimizations and debug information (Far
    data).

* burgercw868krelfar.lib for full optimizations and no debug information
    (Far data).

* burgercw868kcfmdbg.lib for no optimizations and debug information (Code
    fragment).

* burgercw868kcfmdbf.lib for full optimizations and debug information
    (Code fragment).

* burgercw868kcfmrel.lib for full optimizations and no debug information
    (Code fragment).

* burgercw868kcfmdbgfar.lib for no optimizations and debug information
    (Code fragment, Far data).

* burgercw868kcfmdbffar.lib for full optimizations and debug information
    (Code fragment, Far data).

* burgercw868kcfmrelfar.lib for full optimizations and no debug
    information (Code fragment, Far data).
