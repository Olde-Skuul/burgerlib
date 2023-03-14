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

from __future__ import absolute_import, print_function, unicode_literals

import os
from burger import copy_file_if_needed, get_windows_host_type, \
    is_codewarrior_mac_allowed, get_sdks_folder, clean_directories, \
    clean_files, is_under_git_control
from makeprojects.config import _HLSL_MATCH, _GLSL_MATCH, _X360SL_MATCH, \
    _VITACG_MATCH
from makeprojects import PlatformTypes, IDETypes, ProjectTypes, makeprojects

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

CLEANME_PROCESS_PROJECT_FILES = False

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
)

# PS4 specific code
BURGER_LIB_PS4 = (
    "../source/platforms/ps4",
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
)

# Nintendo Switch specific code
BURGER_LIB_SWITCH = (
    "../source/platforms/switch",
    "../source/graphics/vulkan"
)

# Microsoft Xbox 360 specific code
BURGER_LIB_XBOX_360 = (
    "../source/platforms/xbox360",
    "../source/graphics/shadersxbox360"
)

# Microsoft Xbox ONE specific code
BURGER_LIB_XBOX_ONE = (
    "../source/platforms/xboxone",
    "../source/graphics/shadersxboxone"
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

# List of projects to build
ARG_LISTS = [
    ("windows", "burger", "library",
     ["vs2003", "vs2005", "vs2008", "vs2010", "vs2012",
      "vs2013", "vs2015", "vs2017", "vs2019", "vs2022",
      "watcom",
      "codewarrior50"]),
    ("windows", "unittests", "console",
     ["vs2003", "vs2005", "vs2008", "vs2010", "vs2012",
      "vs2013", "vs2015", "vs2017", "vs2019", "vs2022",
      "watcom",
      "codewarrior50"]),
    ("ps3", "burger", "library", ["vs2015"]),
    ("ps4", "burger", "library", ["vs2022"]),
    ("ps5", "burger", "library", ["vs2022"]),
    ("vita", "burger", "library", ["vs2015"]),
    ("vita", "unittests", "console", ["vs2015"]),
    ("xbox360", "burger", "library", ["vs2010"]),
    ("xbox360", "unittests", "console", ["vs2010"]),
    ("xboxone", "burger", "library", ["vs2017"]),
    ("xboxone", "unittests", "console", ["vs2017"]),
    ("xboxgdk", "burger", "library", ["vs2022"]),
    ("xboxonex", "burger", "library", ["vs2022"]),
    ("wiiu", "burger", "library", ["vs2013"]),
    ("switch", "burger", "library", ["vs2017"]),
    ("switch", "unittests", "app", ["vs2017"]),
    ("shield", "burger", "library", ["vs2015"]),
    ("android", "burger", "library", ["vs2022"]),
    ("stadia", "burger", "library", ["vs2022"]),
    ("msdos", "burger", "library", ["watcom"]),
    ("msdos4gw", "unittests", "console", ["watcom"]),
    ("macosx", "burger", "library", ["xcode3", "xcode14"]),
    ("macosx", "unittests", "console", ["xcode3", "xcode14"]),
    ("ios", "burger", "library", ["xcode3", "xcode5"]),
    ("linux", "burger", "library", ["make"]),
    ("linux", "unittests", "console", ["make"]),
    ("msdos", "burger", "library", ["codeblocks"]),
    ("windows", "burger", "library", ["codeblocks"]),
    ("windows", "unittests", "console", ["codeblocks"])
]

########################################


def is_git(working_directory):
    """
    Detect if perforce or git is source control
    """

    global _GIT_FOUND

    if _GIT_FOUND is None:
        _GIT_FOUND = is_under_git_control(working_directory)
    return _GIT_FOUND

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

    clean_directories(working_directory, (".vscode", "appfolder", "temp", "ipch", "bin", "JSON",
                                          "Durango", "SRV", ".vs", "*_Data", "* Data",
                                          "__pycache__"))

    clean_files(working_directory, (".DS_Store", "*.suo", "*.user", "*.ncb",
                                    "*.err", "*.sdf", "*.layout.cbTemp", "*.VC.db",
                                    "*.pyc", "*.pyo"))

########################################


def do_generate(working_directory):
    """
    Generate all the project files.

    Args:
        working_directory: Directory to place the project files.
    """

    # ARG_LISTS = [
    #    ("switch", "burger", "library", ["vs2017"])
    # ]

    for item in ARG_LISTS:
        args = ["-p", item[0], "-n", item[1],
            "-t", item[2], "-g", ""]
        for ide in item[3]:
            args[-1] = ide
            makeprojects(working_directory=working_directory, args=args)
    return 0


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
    if is_git(working_directory):
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

    # Too many branches
    # Too many statements
    # pylint: disable=R0912,R0915

    project.solution.perforce = not is_git(project.working_directory)

    platform = project.platform
    ide = project.solution.ide

    # Initialize lists
    source_folders_list = list(BURGER_LIB_SOURCE)
    include_folders_list = []
    source_files_list = list(BURGER_LIB_GENERATED)
    library_folders_list = []
    vs_targets = []
    vs_props = []
    vs_rules = []

    # Windows specific files
    if platform.is_windows():
        source_folders_list.extend(BURGER_LIB_WINDOWS)

        if not ide.is_codewarrior():
            include_folders_list.extend([
                "$(BURGER_SDKS)/windows/perforce",
                "$(BURGER_SDKS)/windows/directx9",
                "$(BURGER_SDKS)/windows/opengl"])

        if ide in (IDETypes.watcom, IDETypes.codeblocks):
            include_folders_list.append("$(BURGER_SDKS)/windows/windows5")

        # Visual Studio 2019 doesn"t have the old Win XP headers
        # if ide >= IDETypes.vs2017:
        # include_folders_list.append(
        # "$(BURGER_SDKS)/windows/windows5")

        project.define_list = [
            "_CRT_NONSTDC_NO_WARNINGS",
            "_CRT_SECURE_NO_WARNINGS",
            "GLUT_NO_LIB_PRAGMA",
            "_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1"]

        find_generated_source(
            source_files_list,
            project.working_directory,
            "../source/windows",
            _HLSL_MATCH)

    # MS/DOS
    if platform.is_msdos():
        source_folders_list.extend(BURGER_LIB_DOS)
        include_folders_list.append("$(BURGER_SDKS)/msdos/x32")

    # Linux
    if platform is PlatformTypes.linux:
        source_folders_list.extend(BURGER_LIB_LINUX)

    # Apple platforms
    if platform.is_macosx():
        source_folders_list.extend(BURGER_LIB_MACOSX)

    if platform.is_ios():
        source_folders_list.extend(BURGER_LIB_IOS)

    if platform.is_macos():
        source_folders_list.extend(BURGER_LIB_MAC)

    if platform.is_darwin():
        source_folders_list.extend(BURGER_LIB_DARWIN)

    # Sony platforms
    if platform is PlatformTypes.ps3:
        source_folders_list.extend(BURGER_LIB_PS3)

    if platform is PlatformTypes.ps4:
        source_folders_list.extend(BURGER_LIB_PS4)

    if platform is PlatformTypes.ps5:
        source_folders_list.extend(BURGER_LIB_PS5)

    if platform is PlatformTypes.vita:
        source_folders_list.extend(BURGER_LIB_VITA)

        vs_props.append("$(VCTargetsPath)\\BuildCustomizations\\vitacg.props")
        vs_targets.append(
            "$(VCTargetsPath)\\BuildCustomizations\\vitacg.targets")
        find_generated_source(
            source_files_list,
            project.working_directory,
            "../source/graphics/shadersvita",
            _VITACG_MATCH)

    # Microsoft platforms
    if platform is PlatformTypes.xbox360:
        source_folders_list.extend(BURGER_LIB_XBOX_360)
        vs_props.append("$(VCTargetsPath)\\BuildCustomizations\\x360sl.props")
        vs_targets.append(
            "$(VCTargetsPath)\\BuildCustomizations\\x360sl.targets")
        find_generated_source(
            source_files_list, project.working_directory,
            "../source/graphics/shadersxbox360",
            _X360SL_MATCH)

    if platform.is_xboxone():
        source_folders_list.extend(BURGER_LIB_XBOX_ONE)
        # vs_props.append("$(VCTargetsPath)\\BuildCustomizations\\x360sl.props")
        # vs_targets.append(
        #    "$(VCTargetsPath)\\BuildCustomizations\\x360sl.targets")
        # find_generated_source(
        #    source_files_list, working_directory,
        #    "../source/graphics/shadersxbox360",
        #    _X360SL_MATCH)

    # Android
    if platform.is_android():
        source_folders_list.extend(BURGER_LIB_ANDROID)

    if platform is PlatformTypes.shield:
        # Override for Shield, even though it's an Android
        # platform
        source_folders_list.extend(BURGER_LIB_SHIELD)

    if platform is PlatformTypes.stadia:
        source_files_list.extend(BURGER_LIB_STADIA)

    # Nintendo
    if platform is PlatformTypes.wiiu:
        source_folders_list.extend(BURGER_LIB_WIIU)

    if platform is PlatformTypes.switch:
        source_folders_list.extend(BURGER_LIB_SWITCH)

    # Unix platforms
    if platform.is_darwin() or platform.is_android() or \
            platform in (PlatformTypes.linux, PlatformTypes.stadia):
        source_folders_list.extend(BURGER_LIB_UNIX)

    # Folder to store the output
    platform_folder = platform.get_platform_folder()

    # Add property files for unittests or burgerlib
    if project.name == "unittests":
        source_folders_list.append("../unittest")

        if platform.is_xboxone():
            source_files_list.append(
                "../unittest/xboxone/unittestxboxone.appxmanifest")

        library_folders_list.append(
            "$(BURGER_SDKS)/{}/burgerlib".format(platform_folder))
        if platform is PlatformTypes.linux:
            project.libraries_list.append("GL")

    else:
        if not is_git(project.working_directory):
            project.deploy_folder = \
                "$(BURGER_SDKS)/{}/burgerlib".format(platform_folder)

    if platform.is_windows():
        if ide.is_codewarrior() or project.name == "unittests":
            library_folders_list.extend([
                "$(BURGER_SDKS)/windows/perforce",
                "$(BURGER_SDKS)/windows/opengl",
                "$(BURGER_SDKS)/windows/directx9"
            ])

    # Enable OpenGL extensions
    if platform.is_windows() or platform.is_android() or \
            platform is PlatformTypes.linux:
        vs_props.append(
            "$(VCTargetsPath)\\BuildCustomizations\\glsl.props")
        vs_targets.append(
            "$(VCTargetsPath)\\BuildCustomizations\\glsl.targets")
        vs_rules.append("glsl.rules")
        find_generated_source(
            source_files_list, project.working_directory,
            "../source/graphics/shadersopengl",
            _GLSL_MATCH)

    # Enable hlsl extensions
    if platform.is_windows():
        vs_props.append(
            "$(VCTargetsPath)\\BuildCustomizations\\hlsl.props")
        vs_targets.append("$(VCTargetsPath)\\BuildCustomizations\\hlsl.targets")
        vs_rules.append("hlsl.rules")
        find_generated_source(
            source_files_list,
            project.working_directory,
            "../source/graphics/shadersdx9",
            _HLSL_MATCH)

    # Enable Steam
    if platform.is_windows() or platform.is_macosx() or platform is PlatformTypes.linux:
        if ide.is_codewarrior():
            library_folders_list.append("$(BURGER_SDKS)/steamworks/public/steam")
        else:
            include_folders_list.append("$(BURGER_SDKS)/steamworks/public/steam")

    # Store the values into the project
    project.source_folders_list = source_folders_list
    project.include_folders_list = include_folders_list
    project.source_files_list = source_files_list
    project.library_folders_list = library_folders_list
    project.vs_props = vs_props
    project.vs_targets = vs_targets
    project.vs_rules = vs_rules
    if platform.is_windows():
        project.cw_environment_variables = ["BURGER_SDKS"]

    # Default to Unicode APIs on Windows
    if platform.is_windows() or platform.is_xbox():
        project.vs_CharacterSet = "Unicode"

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


########################################

def configuration_settings(configuration):
    """
    Set the custom attributes for each configuration.

    Args:
        configuration: Configuration to modify.
    """

    # Use fastcall convention on windows 32 bit Intel
    if configuration.platform is PlatformTypes.win32:
        configuration.fastcall = True
    return 0

########################################


# If called as a command line and not a class, perform the build
if __name__ == "__main__":
    import sys
    sys.exit(do_generate(os.path.dirname(os.path.abspath(__file__))))
