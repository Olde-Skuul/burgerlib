#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Build rules for the makeprojects suite of build tools.

This file is parsed by the cleanme, buildme, rebuildme and makeprojects
command line tools to clean, build and generate project files.

When any of these tools are invoked, this file is loaded and parsed to
determine special rules on how to handle building the code and / or data.
"""

# pylint: disable=unused-argument
# pylint: disable=consider-using-f-string
# pylint: disable=global-statement
# pylint: disable=too-many-lines

from __future__ import absolute_import, print_function, unicode_literals

import os
from burger import copy_file_if_needed, get_windows_host_type, \
    is_codewarrior_mac_allowed, get_sdks_folder, clean_directories, \
    clean_files, is_under_git_control, clean_xcode, delete_directory
from makeprojects.config import _HLSL_MATCH, _GLSL_MATCH, _X360SL_MATCH, \
    _VITACG_MATCH
from makeprojects import PlatformTypes, IDETypes, ProjectTypes

# Check if git is around
_GIT_FOUND = None

# Default project name to use instead of the name of the working directory
PROJECT_NAME = "burger"

# Type of the project, default is ProjectTypes.tool
PROJECT_TYPE = ProjectTypes.library

# Recommended IDE for the project. Default is IDETypes.default()
# PROJECT_IDE = IDETypes.vs2022

# If set to True, ``buildme -r``` will not parse directories in this folder.
NO_RECURSE = True

# ``cleanme`` will assume only the function ``clean()`` is used if False.
# Overrides PROCESS_PROJECT_FILES
CLEANME_PROCESS_PROJECT_FILES = False

# List of projects to generate if makeprojects is invoked
# without any parameters, default create recommended
# project for the host machine
MAKEPROJECTS = [
    # Create the evergreen projects

    # Windows
    {"platform": "windows",
     "name": "burger",
     "type": "library",
     "ide": ("vs2003", "vs2005", "vs2008", "vs2010", "vs2012",
      "vs2013", "vs2015", "vs2017", "vs2019", "vs2022",
      "watcom",
      "codewarrior50",
      "codeblocks")},
    {"platform": "windows",
     "name": "unittests",
     "type": "console",
     "ide": ("vs2003", "vs2005", "vs2008", "vs2010", "vs2012",
      "vs2013", "vs2015", "vs2017", "vs2019", "vs2022",
      "watcom",
      "codewarrior50",
      "codeblocks")},

    # MSDOS X32 and DOS4GW
    {"platform": "msdos",
     "name": "burger",
     "type": "library",
     "ide": ("watcom", "codeblocks")},
    {"platform": "msdos4gw",
     "name": "unittests",
     "type": "console",
     "ide": "watcom"},

    # Linux
    {"platform": "linux",
     "name": "burger",
     "type": "library",
     "ide": "make"},
    {"platform": "linux",
     "name": "unittests",
     "type": "console",
     "ide": "make"},

    # MacOS X
    {"platform": "macosx",
     "name": "burger",
     "type": "library",
     "ide": ("xc3", "x14")},
    {"platform": "macosx",
     "name": "unittests",
     "type": "console",
     "ide": ("xc3", "x14")},

    # iOS
    {"platform": "macosx",
     "name": "burger",
     "type": "library",
     "ide": "xc3"},

    # Android / nVidia Shield
    {"platform": "android",
     "name": "burger",
     "type": "library",
     "ide": "vs2022"},
    {"platform": "shield",
     "name": "burger",
     "type": "library",
     "ide": "vs2015"},

    # Sony platforms
    {"platform": "ps3",
     "name": "burger",
     "type": "library",
     "ide": "vs2015"},
    {"platform": "ps3",
     "name": "unittests",
     "type": "app",
     "ide": "vs2015"},

    # Microsoft platforms
    {"platform": "xbox",
     "name": "burger",
     "type": "library",
     "ide": "vs2003"},
    {"platform": "xbox",
     "name": "unittests",
     "type": "console",
     "ide": "vs2003"},
    {"platform": "xbox360",
     "name": "burger",
     "type": "library",
     "ide": "vs2010"},
    {"platform": "xbox360",
     "name": "unittests",
     "type": "console",
     "ide": "vs2010"},

    # Stadia (RIP)
    {"platform": "stadia",
     "name": "burger",
     "type": "library",
     "ide": "vs2022"}
]

# Build folder
BUILD_FOLDER = os.path.dirname(os.path.abspath(__file__))

# Folder of the source code
SOURCE_FOLDER = os.path.join(os.path.dirname(BUILD_FOLDER), "source")

# Folder of the platform specific source code
PLATFORMS_FOLDER = os.path.join(SOURCE_FOLDER, "platforms")

# Allow creating projects for "forbidden" platforms
# If you need this source code, contact Rebecca Heineman
# at becky@oldeskuul.com and offer proof that you're a licensed
# platform developer, and she'll send you the folder

# Is PS4/PS5 support found?
if os.path.isdir(os.path.join(PLATFORMS_FOLDER, "ps4")):
    MAKEPROJECTS.extend([
        {
            "platform": "ps4",
            "name": "burger",
            "type": "library",
            "ide": "vs2022"
        },
        {
            "platform": "ps4",
            "name": "unittests",
            "type": "app",
            "ide": "vs2022"
        },

        {
            "platform": "ps5",
            "name": "burger",
            "type": "library",
            "ide": "vs2022"
        },
        {
            "platform": "ps5",
            "name": "unittests",
            "type": "app",
            "ide": "vs2022"
        }])

# Is Playstation Vita support found?
if os.path.isdir(os.path.join(PLATFORMS_FOLDER, "psvita")):
    MAKEPROJECTS.extend([
        {
            "platform": "vita",
            "name": "burger",
            "type": "library",
            "ide": "vs2015"
        },
        {
            "platform": "vita",
            "name": "unittests",
            "type": "console",
            "ide": "vs2015"
        }])

# Is Nintendo WiiU support found?
if os.path.isdir(os.path.join(PLATFORMS_FOLDER, "wiiu")):
    MAKEPROJECTS.extend([
        {
            "platform": "wiiu",
            "name": "burger",
            "type": "library",
            "ide": "vs2015"
        },
        {
            "platform": "wiiu",
            "name": "unittests",
            "type": "app",
            "ide": "vs2015"
        }])

# Is Nintendo Switch support found?
if os.path.isdir(os.path.join(PLATFORMS_FOLDER, "switch")):
    MAKEPROJECTS.extend([
        {
            "platform": "switch64",
            "name": "burger",
            "type": "library",
            "ide": "vs2022"
        },
        {
            "platform": "switch64",
            "name": "unittests",
            "type": "app",
            "ide": "vs2022"
        }])

# Is Microsoft Xbox ONE support found?
if os.path.isdir(os.path.join(PLATFORMS_FOLDER, "xboxone")):
    MAKEPROJECTS.extend([
        {
            "platform": "xboxone",
            "name": "burger",
            "type": "library",
            "ide": "vs2017"
        },
        {
            "platform": "xboxone",
            "name": "unittests",
            "type": "console",
            "ide": "vs2017"
        },

        {
            "platform": "xboxgdk",
            "name": "burger",
            "type": "library",
            "ide": "vs2022"
        },
        {
            "platform": "xboxgdk",
            "name": "unittests",
            "type": "console",
            "ide": "vs2022"
        },

        {
            "platform": "xboxonex",
            "name": "burger",
            "type": "library",
            "ide": "vs2022"
        },
        {
            "platform": "xboxonex",
            "name": "unittests",
            "type": "console",
            "ide": "vs2022"
        }])

# List of Codewarrior library files to move to sdks/windows/burgerlib
WINDOWS_LIB_FILES = (
    "burgerc50w32rel.lib",
    "burgerc50w32dbg.lib",
    "burgerc50w32int.lib"
)

# List of Codewarrior library files to move to sdks/mac/burgerlib
MAC68K_LIB_FILES = (
    "burgerc58mac68krel.lib",
    "burgerc58mac68kdbg.lib",
    "burgerc58mac68kint.lib",
    "burgerc58mac68kfardbg.lib",
    "burgerc58mac68kfarint.lib",
    "burgerc58mac68kfarrel.lib",
    "burgerc58mac68kfpudbg.lib",
    "burgerc58mac68kfpuint.lib",
    "burgerc58mac68kfpurel.lib",
    "burgerc58mac68kfarfpudbg.lib",
    "burgerc58mac68kfarfpuint.lib",
    "burgerc58mac68kfarfpurel.lib",
    "burgerc58car68kfarfpudbg.lib",
    "burgerc58car68kfarfpuint.lib",
    "burgerc58car68kfarfpurel.lib"
)

# Directories for the cross platform source code
BURGER_LIB_SOURCE = (
    "../source",
    "../source/ansi",
    "../source/audio",
    "../source/commandline",
    "../source/compression",
    "../source/file",
    "../source/flashplayer",
    "../source/graphics",
    "../source/graphics/effects",
    "../source/graphics/shaders",
    "../source/input",
    "../source/lowlevel",
    "../source/math",
    "../source/memory",
    "../source/network",
    "../source/random",
    "../source/text"
)

# PS3 specific code
BURGER_LIB_PS3 = (
    "../source/platforms/ps3",
    "../source/asm/ps3_asm"
)

# PS4 specific code
BURGER_LIB_PS4 = (
    "../source/platforms/ps4",
    "../source/asm/ps4_asm"
)

# PS5 specific code
BURGER_LIB_PS5 = (
    "../source/platforms/ps5",
)

# PS Vita specific code
BURGER_LIB_VITA = (
    "../source/platforms/psvita",
    "../source/graphics/shadersvita"
)

# Nintendo WiiU specific code
BURGER_LIB_WIIU = (
    "../source/platforms/wiiu",
    "../source/asm/wiiu_asm"
)

# Nintendo Switch specific code
BURGER_LIB_SWITCH = (
    "../source/platforms/switch",
    "../source/graphics/vulkan",
    "../source/asm/switch_asm"
)

# Microsoft Xbox classic specific code
BURGER_LIB_XBOX = (
    "../source/platforms/xbox",
)

# Microsoft Xbox 360 specific code
BURGER_LIB_XBOX_360 = (
    "../source/platforms/xbox360",
    "../source/graphics/shadersxbox360",
    "../source/asm/xbox360asm"
)

# Microsoft Xbox ONE specific code
BURGER_LIB_XBOX_ONE = (
    "../source/platforms/xboxone",
    "../source/graphics/shadersxboxone",
    "../source/asm/xboxoneasm"
)

# Apple macOS X specific code
BURGER_LIB_MACOSX = (
    "../source/platforms/macosx",
    "../source/graphics/shadersopengl"
)

# Apple macOS 7-9 and Carbon specific code
BURGER_LIB_MAC = (
    "../source/platforms/mac",
    "../source/graphics/shadersopengl"
)

# Apple iOS specific code
BURGER_LIB_IOS = (
    "../source/platforms/ios",
    "../source/graphics/shadersopengl"
)

# Darwin specific code
BURGER_LIB_DARWIN = (
    "../source/platforms/darwin",
)

# Unix specific code
BURGER_LIB_UNIX = (
    "../source/platforms/unix",
)

# Microsoft MSDos specific code
BURGER_LIB_DOS = (
    "../source/platforms/msdos",
)

# Microsoft Windows specific code
BURGER_LIB_WINDOWS = (
    "../source/platforms/windows",
    "../source/graphics/shadersdx9",
    "../source/graphics/shadersopengl",
    "../source/graphics/vulkan"
)

# nVidia Shield specific code
BURGER_LIB_SHIELD = (
    "../source/platforms/shield",
)

# Android specific code
BURGER_LIB_ANDROID = (
    "../source/platforms/android",
    "../source/graphics/shadersopengl",
    "../source/graphics/vulkan"
)

# Stadia specific code
BURGER_LIB_STADIA = (
    "../source/platforms/stadia",
)

# Linux specific code
BURGER_LIB_LINUX = (
    "../source/platforms/linux",
    "../source/graphics/shadersopengl",
    "../source/graphics/vulkan"
)

# Generated source file
BURGER_LIB_GENERATED = (
    "../source/generated/version.h",
)

########################################


def is_git():
    """
    Detect if perforce or git is source control
    """

    global _GIT_FOUND

    if _GIT_FOUND is None:
        _GIT_FOUND = is_under_git_control(BUILD_FOLDER)
    return _GIT_FOUND

########################################


def sdks_folder():
    """
    Return the location of the sdks folder

    Check if under github, if so, it's stored locally, otherwise, it's in the
    BURGER_SDKS folder
    """

    if is_git():
        return "../sdks/"
    return "$(BURGER_SDKS)/"

########################################


def window_opengl_folder():
    """
    Return the location of the OpenGL folder for Windows

    Check if under github, if so, it's stored locally, otherwise, it's in the
    BURGER_SDKS folder
    """

    return sdks_folder() + "windows/opengl"

########################################


def steam_folder():
    """
    Return the location of the Steam SDKS folder

    Check if under github, if so, it's stored locally, otherwise, it's in the
    BURGER_SDKS folder
    """

    return sdks_folder() + "steamworks/public/steam"

########################################


def x32_folder():
    """
    Return the location of the FlashTex DOS X32 folder

    Check if under github, if so, it's stored locally, otherwise, it's in the
    BURGER_SDKS folder
    """

    return sdks_folder() + "msdos/x32/include"

########################################


def clean(working_directory):
    """
    Delete temporary files.

    This function is called by ``cleanme`` to remove temporary files.

    On exit, return 0 for no error, or a non zero error code if there was an
    error to report.

    Args:
        working_directory
            Directory this script resides in.

    Returns:
        None if not implemented, otherwise an integer error code.
    """

    # This directory has read only files
    delete_directory(os.path.join(working_directory, "Durango"), True)
    delete_directory(os.path.join(working_directory,
                                  "Gaming.Xbox.XboxOne.x64"), True)

    # Clean these directories
    clean_directories(working_directory,
                      (".vscode", "appfolder", "temp", "ipch", "bin", "JSON",
                       "SRV", ".vs", "*_Data", "* Data",
                       "__pycache__", "Debug"))

    clean_files(working_directory, (".DS_Store", "*.suo", "*.user", "*.ncb",
                                    "*.err", "*.sdf", "*.layout.cbTemp",
                                    "*.VC.db", "*.pyc", "*.pyo", "*.XGD"))

    clean_xcode(working_directory)


########################################


def postbuild(working_directory, configuration):
    """
    Issue build commands after all IDE projects have been built.

    This function can assume all other build projects have executed for final
    deployment or cleanup

    On exit, return 0 for no error, or a non zero error code if there was an
    error to report.

    Args:
        working_directory
            Directory this script resides in.

        configuration
            Configuration to build, ``all`` if no configuration was requested.

    Returns:
        None if not implemented, otherwise an integer error code.
    """

    # Only process if perforce is source control
    if is_git():
        return 0

    # Copy the windows version of the CodeWarrior libraries.
    error = 0

    # If the host can run CodeWarrior for Windows (Codewarrior 9.4 Mac or
    # Windows) then copy the generated windows library files

    if get_windows_host_type() or is_codewarrior_mac_allowed():

        # Update the codewarrior files

        sdks = get_sdks_folder()
        windowsburgerbase = os.path.join(sdks, "windows", "burgerlib")
        outputfolder = os.path.join(working_directory, "bin")
        for item in WINDOWS_LIB_FILES:
            error = copy_file_if_needed(
                os.path.join(outputfolder, item),
                os.path.join(windowsburgerbase, item), perforce=True)
            if error:
                break

    # If the host can run CodeWarrior for MacOS Carbon (Codewarrior 10 Mac)
    # then copy the generated Mac 68k library files

    if not error and is_codewarrior_mac_allowed():

        # Update the codewarrior files

        sdks = get_sdks_folder()
        macburgerbase = os.path.join(sdks, "mac", "burgerlib")
        outputfolder = os.path.join(working_directory, "bin")
        for item in MAC68K_LIB_FILES:
            error = copy_file_if_needed(
                os.path.join(outputfolder, item),
                os.path.join(macburgerbase, item), perforce=True)
            if error:
                break

    return error

########################################


def watcom_stripcomments(line_list):
    """
    Force stripcomments.exe to a specific path for Watcom

    Makeprojects will generate the watcom project file and pass it to
    this function where it will replace all occurances of stripcomments
    with a prefix to ensure it will use the local copy found in github

    Note: If the tool is in the path, then remove the line that sets the
    solution.post_process variable so this isn't performed.

    Args:
        line_list: List of lines of text
    Return:
        List of adjusted lines
    """
    return [x.replace("stripcomments",
                      "..\\tools\\windows\\x64\\stripcomments")
            for x in line_list]

########################################


def make_stripcomments(line_list):
    """
    Force stripcomments to a specific path for GNU Make

    Makeprojects will generate the make project file and pass it to
    this function where it will replace all occurances of stripcomments
    with a prefix to ensure it will use the local copy found in github

    Note: If the tool is in the path, then remove the line that sets the
    solution.post_process variable so this isn't performed.

    Args:
        line_list: List of lines of text
    Return:
        List of adjusted lines
    """
    return [x.replace("stripcomments",
                      "../tools/linux/stripcomments")
            for x in line_list]


########################################


def watcom_rules(project):
    """
    Handle special cases for Visual Studio 2005 and 2008

    Assume these IDEs are building only for Windows
    """

    ide = project.solution.ide
    if ide is IDETypes.watcom:
        platform = project.platform

        # Asm functions for all platforms
        project.source_folders_list.append("../source/asm/wasm")

        if platform.is_windows():

            # Open Watcom defaults to Windows 95
            # Since Burgerlib requires Windows XP at the minimum,
            # Alert the windows headers in Watcom to expose the
            # Windows XP values

            project.define_list.extend((
                # Set to Windows XP
                "WINVER=0x0600",
                "_WIN32_WINNT=0x0600",
                "NTDDI_VERSION=0x05010000",
                # Needed to allow D3D11.h to compile
                "D3D11_NO_HELPERS",
                # Needed for the OpenGL headers to compile
                "GLUT_DISABLE_ATEXIT_HACK"))

            project.include_folders_list.extend((
                "$(DXSDK_DIR)/Include",
                sdks_folder() + "windows/opengl",
                sdks_folder() + "steamworks/public/steam"
            ))

            # This is a required environment variable
            project.env_variable_list.append("DXSDK_DIR")

            # If building on git, stripcomments.exe may not be on the path
            # Force it
            if is_git():
                project.solution.post_process = watcom_stripcomments

        # Handle the DOS version
        if platform.is_msdos():

            # Add the assembly functions for gluing to the dos extender
            project.source_folders_list.append("../source/asm/wasm/msdos")

########################################


def codewarrior_rules_project(project):
    """
    Handle special cases for Metrowerks Codewarrior

    Args:
        project: Project
    """

    ide = project.solution.ide

    # Codewarrior for x86 has some intrisics missing
    if ide.is_codewarrior():

        # Only add on
        platform = project.platform
        if platform.is_windows():

            # Add all the files
            project.source_folders_list.append("../source/asm/mwx86asm")


########################################


def codewarrior_rules(configuration):
    """
    Handle special cases for Metrowerks Codewarrior

    Args:
        configuration: Project configuration
    """

    platform = configuration.platform
    ide = configuration.ide

    # Codewarrior needs to have their folders BEFORE the CodeWarrior folders.
    if platform.is_windows() and ide.is_codewarrior():

        # Include these BEFORE the default headers
        configuration.library_folders_list[0:0] = [
            window_opengl_folder(),
            # The DirectX Headers need sal.h, supply it manually for VS 2003
            "../source/platforms/windows/vc7compat",
            # Direct X June 2010 sdk
            "$(DXSDK_DIR)/Include",
            steam_folder()]

        # Fix to allow dwrite.h to compile on Codewarrior
        configuration.define_list.append("DWRITE_NO_WINDOWS_H")

        # If at Olde Skuul, use the BURGER_SDKS variable
        if not is_git:
            configuration.cw_environment_variables.append("BURGER_SDKS")

        # Everyone has to have DirectX June 2010
        configuration.cw_environment_variables.append("DXSDK_DIR")

########################################


def vs2003_stripcomments(line_list):
    """
    Force stripcomments.exe to a specific path for Visual Studio 2003

    Makeprojects will generate the Visual studio project file and pass it to
    this function where it will replace all occurances of stripcomments
    with a prefix to ensure it will use the local copy found in github

    Note: If the tool is in the path, then remove the line that sets the
    solution.post_process variable so this isn't performed.

    Args:
        line_list: List of lines of text
    Return:
        List of adjusted lines
    """

    if is_git():
        strip_comments = "..\\tools\\windows\\x64\\stripcomments"
    else:
        strip_comments = "$(BURGER_SDKS)\\windows\\bin\\x64\\stripcomments"

    return [x.replace("stripcomments", strip_comments)
            for x in line_list]

########################################


def vs2003_rules(project):
    """
    Handle special cases for Visual Studio 2003

    Assume these IDEs are building only for Windows
    """

    ide = project.solution.ide
    # Add assembly files to vs2003
    if ide is IDETypes.vs2003:

        # Add missing intrinsics for 2003
        project.source_folders_list.append(
            "../source/asm/masm/vs2003")
        project.source_folders_list.append(
            "../source/asm/masm/vs2003_2005")
        project.source_folders_list.append(
            "../source/asm/masm/vs2003_2008")

        if project.platform.is_windows():
            # stripcomments.exe is on the path, but VS 2003 doesn't use it
            # So, replace the call to where the binary actually is
            project.solution.post_process = vs2003_stripcomments

            # The DirectX Headers need sal.h, supply it manually for VS 2003
            project.source_folders_list.append(
                "../source/platforms/windows/vc7compat")

            # Use the DirectX SDK. MUST BE INCLUDED BEFORE Windows SDK!
            project.include_folders_list.append("$(DXSDK_DIR)/Include")

            # Use the Windows SDK installed from VS 2005 because the one
            # that shipped with VS 2003 is too old to handle the DirectX
            # SDK
            project.include_folders_list.append(
                "C:/Program Files/Microsoft SDKs/Windows/v6.0A/Include")

########################################


def vs2005_2008_rules(project):
    """
    Handle special cases for Visual Studio 2005 and 2008

    Assume these IDEs are building only for Windows
    """

    ide = project.solution.ide
    # Enable masm for Visual Studio 2005 and 2008
    if ide in (IDETypes.vs2005, IDETypes.vs2008):

        # Enable glsl
        project.vs_rules.append("../ide_plugins/vs2005_2008/glsl.rules")

        # Enable hlsl
        project.vs_rules.append("../ide_plugins/vs2005_2008/hlsl.rules")

        # Enable masm64
        project.vs_rules.append("../ide_plugins/vs2005_2008/masm64.rules")

        # This is a missing intrinsic
        project.source_folders_list.append(
            "../source/asm/masm/vs2003_2008")
        project.source_folders_list.append(
            "../source/asm/masm64/vs2005_2008")

        # Include the DirectX SDK, June 2010
        project.include_folders_list.append("$(DXSDK_DIR)/Include")

    # __cpuindex() is available on 2008, but not 2005
    if ide is IDETypes.vs2005:
        project.source_folders_list.append(
            "../source/asm/masm/vs2003_2005")
        project.source_folders_list.append(
            "../source/asm/masm64/vs2005")

########################################


def gnu_clang_rules(project):
    """
    Handle special cases for GNU/Clang compilers

    Stadia, Linux, Switch, MacOSX, iOS, Android
    """

    ide = project.solution.ide
    if ide is IDETypes.make:
        project.source_folders_list.append(
            "../source/asm/gnux64")


########################################


def find_generated_source(source_files, working_directory, directory, match):
    """
    Create entries for generated shader headers.

    Args:
        source_files: List to append the found files
        working_directory: Directory to anchor the found files.
        directory: Directory to scan
        match: Regular expression for filename match.
    """

    # Get the absolute path for lookups
    abs_path = os.path.abspath(os.path.join(working_directory, directory))
    for item in os.listdir(abs_path):
        if match.match(item):
            # Convert to header
            new_name = os.path.splitext(item)[0] + ".h"

            # Save in "generated" folder
            source_files.append(os.path.join(abs_path, "generated", new_name))


########################################


def project_settings(project):
    """
    Set the custom attributes for each configuration.

    Args:
        configuration: Configuration to modify.
    """

    # pylint: disable=too-many-branches
    # pylint: disable=too-many-statements

    project.solution.perforce = not is_git()

    platform = project.platform
    ide = project.solution.ide

    # Initialize lists
    project.source_folders_list.extend(BURGER_LIB_SOURCE)
    project.source_files_list.extend(BURGER_LIB_GENERATED)

    # Windows specific files
    if platform.is_windows():
        project.source_folders_list.extend(BURGER_LIB_WINDOWS)

        # Add the assembly code for Windows
        if ide.is_visual_studio():

            # VS 2007-2010 default to 8087 math. All others use SSE2
            if ide in (IDETypes.vs2003, IDETypes.vs2005,
                       IDETypes.vs2008, IDETypes.vs2010):
                project.source_folders_list.append(
                    "../source/asm/masm/vs2003_2010")

            # 8087 control for all x86 targets
            project.source_folders_list.append(
                "../source/asm/masm")

        # Add in the headers for Windows, but there be dragons
        if not ide.is_codewarrior() and ide is not IDETypes.watcom:

            # For OpenGL support since the compiler has an old sdk
            project.include_folders_list.append(window_opengl_folder())

            # For legacy directx 9 support with modern IDEs
            if ide in (IDETypes.vs2017, IDETypes.vs2019, IDETypes.vs2022):
                project.include_folders_list.append(
                    "../source/platforms/windows/directx9")

            # Older IDEs need DirectX from the June 2010 SDK
            if ide in (IDETypes.vs2010, IDETypes.vs2012, IDETypes.vs2013,
                       IDETypes.vs2015):
                project.include_folders_list.append("$(DXSDK_DIR)/Include")

            if ide is IDETypes.codeblocks:
                project.include_folders_list.append(
                    "$(BURGER_SDKS)/windows/windows5")
                project.include_folders_list.append(
                    "$(BURGER_SDKS)/windows/directx9")
                project.define_list.append("GLUT_DISABLE_ATEXIT_HACK")

        if ide is not IDETypes.watcom:
            project.define_list.extend([
                "_CRT_NONSTDC_NO_WARNINGS",
                "_CRT_SECURE_NO_WARNINGS",
                "GLUT_NO_LIB_PRAGMA",
                "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1"])

        find_generated_source(
            project.source_files_list,
            project.working_directory,
            "../source/platforms/windows",
            _HLSL_MATCH)

    # MS/DOS
    if platform.is_msdos():
        project.source_folders_list.extend(BURGER_LIB_DOS)
        project.include_folders_list.append(x32_folder())

    # Linux
    if platform is PlatformTypes.linux:
        project.source_folders_list.extend(BURGER_LIB_LINUX)
        # If building on git, stripcomments.exe may not be on the path
        # Force it
        if is_git():
            project.solution.post_process = make_stripcomments

    if platform is PlatformTypes.stadia:
        project.source_folders_list.extend(BURGER_LIB_STADIA)

    # Apple platforms
    if platform.is_macosx():
        project.source_folders_list.extend(BURGER_LIB_MACOSX)
        project.source_folders_list.append(
            "../source/asm/xcodeasm")

    if platform.is_ios():
        project.source_folders_list.extend(BURGER_LIB_IOS)

    if platform.is_macos():
        project.source_folders_list.extend(BURGER_LIB_MAC)

    if platform.is_darwin():
        project.source_folders_list.extend(BURGER_LIB_DARWIN)

    # Sony platforms
    if platform is PlatformTypes.ps3:
        project.source_folders_list.extend(BURGER_LIB_PS3)

    if platform is PlatformTypes.ps4:
        project.source_folders_list.extend(BURGER_LIB_PS4)

    if platform is PlatformTypes.ps5:
        project.source_folders_list.extend(BURGER_LIB_PS4)
        project.source_folders_list.extend(BURGER_LIB_PS5)

    if platform is PlatformTypes.vita:
        project.source_folders_list.extend(BURGER_LIB_VITA)

        # PSVita assembly code
        project.source_folders_list.append(
            "../source/asm/psvita_asm")

        project.vs_props.append(
            "$(VCTargetsPath)\\BuildCustomizations\\vitacg.props")
        project.vs_targets.append(
            "$(VCTargetsPath)\\BuildCustomizations\\vitacg.targets")
        find_generated_source(
            project.source_files_list,
            project.working_directory,
            "../source/graphics/shadersvita",
            _VITACG_MATCH)

    # Microsoft platforms
    if platform is PlatformTypes.xbox:
        project.source_folders_list.extend(BURGER_LIB_XBOX)
        project.source_folders_list.append(
            "../source/asm/masm/vs2003_2010")

        # 8087 control for all x86 targets
        project.source_folders_list.append(
            "../source/asm/masm")

    if platform is PlatformTypes.xbox360:
        project.source_folders_list.extend(BURGER_LIB_XBOX_360)
        project.vs_props.append(
            "$(VCTargetsPath)\\BuildCustomizations\\x360sl.props")
        project.vs_targets.append(
            "$(VCTargetsPath)\\BuildCustomizations\\x360sl.targets")
        find_generated_source(
            project.source_files_list, project.working_directory,
            "../source/graphics/shadersxbox360",
            _X360SL_MATCH)

    if platform.is_xboxone():
        project.source_folders_list.extend(BURGER_LIB_XBOX_ONE)

    # Android
    if platform.is_android():
        project.source_folders_list.extend(BURGER_LIB_ANDROID)

    if platform is PlatformTypes.shield:
        # Override for Shield, even though it's an Android
        # platform
        project.source_folders_list.extend(BURGER_LIB_SHIELD)

    # Nintendo
    if platform is PlatformTypes.wiiu:
        project.source_folders_list.extend(BURGER_LIB_WIIU)

    if platform.is_switch():
        project.source_folders_list.extend(BURGER_LIB_SWITCH)

    # Unix platforms
    if platform.is_darwin() or platform.is_android() or \
            platform in (PlatformTypes.linux, PlatformTypes.stadia):
        project.source_folders_list.extend(BURGER_LIB_UNIX)

    # Folder to store the output
    platform_folder = platform.get_platform_folder()

    # Add property files for unittests or burgerlib
    if project.name == "unittests":
        project.source_folders_list.append("../unittest")

        if platform.is_xboxone():
            project.source_files_list.extend((
                "../unittest/xboxone/unittestxboxone.appxmanifest",
                "../unittest/xboxone/Logo.png",
                "../unittest/xboxone/SmallLogo.png",
                "../unittest/xboxone/SplashScreen.png",
                "../unittest/xboxone/StoreLogo.png",
                "../unittest/xboxone/WideLogo.png"
            ))

        if platform is PlatformTypes.linux:
            project.libraries_list.append("GL")
            project.libraries_list.append("uuid")

    else:
        if not is_git():
            project.deploy_folder = \
                "$(BURGER_SDKS)/{}/burgerlib".format(platform_folder)

    # Enable OpenGL extensions
    if platform.is_windows() or platform.is_android() or \
            platform is PlatformTypes.linux:
        project.vs_props.append(
            "$(ProjectDir)..\\ide_plugins\\vs2010\\glsl.props")
        project.vs_targets.append(
            "$(ProjectDir)..\\ide_plugins\\vs2010\\glsl.targets")
        find_generated_source(
            project.source_files_list,
            project.working_directory,
            "../source/graphics/shadersopengl",
            _GLSL_MATCH)

    # Enable hlsl extensions
    if platform.is_windows():
        project.vs_props.append(
            "$(ProjectDir)..\\ide_plugins\\vs2010\\hlsl.props")
        project.vs_targets.append(
            "$(ProjectDir)..\\ide_plugins\\vs2010\\hlsl.targets")
        find_generated_source(
            project.source_files_list,
            project.working_directory,
            "../source/graphics/shadersdx9",
            _HLSL_MATCH)

    # Enable Steam
    if platform.is_windows() or platform.is_macosx() or \
            platform is PlatformTypes.linux:
        if not ide.is_codewarrior() and ide is not IDETypes.watcom:
            project.include_folders_list.append(steam_folder())

    # Hack to allow compilation of dbus on Darwin
    if platform.is_darwin():
        project.include_folders_list.append("../source/platforms/unix")

    # Default to Unicode APIs on Windows
    if platform.is_windows() or platform.is_xbox():
        project.vs_CharacterSet = "Unicode"

    # Add in x64 burgerlib assembly
    if ide.is_visual_studio():
        if platform.is_windows():
            # VS 2003 doesn't handle 64 bit code
            if ide is not IDETypes.vs2003:
                project.source_folders_list.append(
                    "../source/asm/masm64")
            # ARM code is available on Visual Studio 2017+
            if ide >= IDETypes.vs2017:
                project.source_folders_list.append(
                    "../source/asm/marmasm64")

    project.custom_rules = {
        "ps*.hlsl":
        {"HeaderFileName": r"%(RootDir)%(Directory)Generated\%(FileName).h",
         "VariableName": "g_%(FileName)",
         "TargetProfile": "ps_2_0"},

        "vs*.hlsl":
        {"HeaderFileName": r"%(RootDir)%(Directory)Generated\%(FileName).h",
         "VariableName": "g_%(FileName)",
         "TargetProfile": "vs_2_0"},

        "*.glsl":
        {"ObjectFileName": r"%(RootDir)%(Directory)Generated\%(FileName).h"},

        "ps*.x360sl":
        {"HeaderFileName": r"%(RootDir)%(Directory)Generated\%(FileName).h",
         "VariableName": "g_%(FileName)",
         "TargetProfile": "ps_2_0"},

        "vs20*.x360sl":
        {"HeaderFileName": r"%(RootDir)%(Directory)Generated\%(FileName).h",
         "VariableName": "g_%(FileName)",
         "TargetProfile": "vs_2_0"},

        "ps*.vitacg":
        {"HeaderFileName": r"%(RootDir)%(Directory)Generated\%(FileName).h",
         "TargetProfile": "sce_fp_psp2"},

        "vs*.vitacg":
        {"HeaderFileName": r"%(RootDir)%(Directory)Generated\%(FileName).h",
         "TargetProfile": "sce_vp_psp2"}
    }

    # Add rules needed to build for Watcom
    watcom_rules(project)

    # Add rules needed to build for CodeWarrior
    codewarrior_rules_project(project)

    # Add rules needed to build for Visual Studio 2003
    vs2003_rules(project)

    # Add rules needed to build for Visual Studio 2005/2008
    vs2005_2008_rules(project)

    # Add rules needed for gnu/clang based projects
    gnu_clang_rules(project)

    if not is_git():
        # This is a required environment variable
        project.env_variable_list.append("BURGER_SDKS")

########################################


def configuration_settings(configuration):
    """
    Set the custom attributes for each configuration.

    Args:
        configuration: Configuration to modify.
    """

    platform = configuration.platform
    ide = configuration.ide

    # BECKY This is a HACK!!
    # brtestbrendian.cpp crashes the watcom compiler
    # if debugging is turned on. Will fix later
    if ide is IDETypes.watcom:
        configuration.debug = False

    # Use fastcall convention on windows 32 bit Intel
    if platform is PlatformTypes.win32:
        configuration.fastcall = True

    # Handle all the special cases for codewarrior
    codewarrior_rules(configuration)

    # Special case for Direct X SDK
    if ide is IDETypes.vs2017 and \
            (platform in (PlatformTypes.win32, PlatformTypes.win64)):
        configuration.include_folders_list.append("$(DXSDK_DIR)/Include")

    # Switch for unittests needs metadata, add it here
    if platform.is_switch():
        if configuration.project.name == "unittests":
            if platform is PlatformTypes.switch64:
                configuration.switch_meta_source = (
                    "..\\unittest\\switch\\switch.arm64.nmeta")
            else:
                configuration.switch_meta_source = (
                    "..\\unittest\\switch\\switch.arm32.nmeta")

    # Where are the sdks?
    if is_git():
        sdks_dir = "../sdks/"
    else:
        sdks_dir = os.getenv("BURGER_SDKS")

    # Add in Directplay for Windows intel 32 bit
    # platform. All others are skipped
    if platform is PlatformTypes.win32:

        # Tell the library that only the headers are needed
        configuration.directplay_headers_only = True
        configuration.library_rules_list.append(
            os.path.join(sdks_dir, "windows", "dplay"))

    # Add in DirectDraw for Windows platforms
    if platform.is_windows():
        configuration.directdraw_headers_only = True
        configuration.library_rules_list.append(
            os.path.join(sdks_dir, "windows", "ddraw"))

    return 0

########################################


# If called as a command line and not a class, perform the build
if __name__ == "__main__":
    import sys
    sys.exit(
        postbuild(os.path.dirname(os.path.abspath(__file__)), "all"))
