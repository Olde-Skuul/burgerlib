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

from __future__ import absolute_import, print_function, unicode_literals

import sys
import os

from burger import make_version_header, get_sdks_folder, \
    create_folder_if_needed, run_command, compare_files, \
    copy_file_if_needed, clean_directories, clean_files, \
    is_write_protected, get_tool_path

# ``cleanme`` will assume only the function ``clean()`` is used if False.
# Overrides PROCESS_PROJECT_FILES
CLEANME_PROCESS_PROJECT_FILES = False

# Check if git is around
_GIT_FOUND = None

# Check if running on Readthedocs
_ON_RTD = os.environ.get("READTHEDOCS", None) == "True"

# Folders for all the target operating systems supported

TARGETFOLDERS = (
    "windows", "msdos",
    "mac", "macosx", "linux", "beos",
    "ps2", "ps3", "ps4", "ps5", "psvita",
    "gamecube", "wii", "wiiu", "dsi", "switch",
    "xbox", "xbox360", "xboxone",
    "stadia",
    "ios", "android", "shield", "ouya"

)

SPECIALHEADERS = (
    "brstartup.h",
    "brgl.h",
    "brglext.h",
    "brglut.h",
    "brglxext.h",
    "brstdint.h"
)

# Folders needed to be created to ensure proper builds
GENERATED_FOLDERS = (
    "source/generated",
    "source/graphics/shadersdx9/generated",
    "source/graphics/shadersopengl/generated",
    "source/graphics/shadersvita/generated",
    "source/graphics/shadersxbox360/generated",
    "source/platforms/windows/generated",
    "bin"
)

# List of super header templates and output files
SUPER_HEADERS = (
    ("templateburgerbase.h", "burger.h", "burger_stripped.h"),
    ("platforms" + os.sep + "mac" + os.sep + "templatemac.h",
     "burgermac.h", "burgermac_stripped.h"),
    ("platforms" + os.sep + "macosx" + os.sep + "templatemacosx.h",
     "burgermacosx.h", "burgermacosx_stripped.h"),
    ("platforms" + os.sep + "darwin" + os.sep + "templatedarwin.h",
     "burgerdarwin.h", "burgerdarwin_stripped.h"),
    ("platforms" + os.sep + "ps4" + os.sep + "templateps4.h",
     "burgerps4.h", "burgerps4_stripped.h"),
    ("platforms" + os.sep + "windows" + os.sep + "templatewindows.h",
     "burgerwindows.h", "burgerwindows_stripped.h"),
    ("platforms" + os.sep + "switch" + os.sep + "templateswitch.h",
     "burgerswitch.h", "burgerswitch_stripped.h"),
    ("platforms" + os.sep + "unix" + os.sep + "templateunix.h",
     "burgerunix.h", "burgerunix_stripped.h"),
    ("platforms" + os.sep + "linux" + os.sep + "templatelinux.h",
     "burgerlinux.h", "burgerlinux_stripped.h"),
    ("platforms" + os.sep + "xbox360" + os.sep + "templatexbox360.h",
     "burgerxbox360.h", "burgerxbox360_stripped.h"),
    ("platforms" + os.sep + "msdos" + os.sep + "templatemsdos.h",
     "burgermsdos.h", "burgermsdos_stripped.h")
)

# Copy headers that are unique to specific platforms
# Source is from this directory, destination is BURGER_SDKS
UNIQUE_HEADERS = (
    # Windows
    ("bin" + os.sep + "burgerwindows_stripped.h",
        "windows" + os.sep + "burgerlib" + os.sep + "burgerwindows.h"),

    # MSDos
    ("bin" + os.sep + "burgermsdos_stripped.h",
        "msdos" + os.sep + "burgerlib" + os.sep + "burgermsdos.h"),

    # Xbox 360
    ("bin" + os.sep + "burgerxbox360_stripped.h",
        "xbox360" + os.sep + "burgerlib" + os.sep + "burgerxbox360.h"),

    # MacOS Classic
    ("bin" + os.sep + "burgermac_stripped.h",
        "mac" + os.sep + "burgerlib" + os.sep + "burgermac.h"),

    # MacOSX
    ("bin" + os.sep + "burgermacosx_stripped.h",
        "macosx" + os.sep + "burgerlib" + os.sep + "burgermacosx.h"),

    # PS4
    ("bin" + os.sep + "burgerps4_stripped.h",
        "ps4" + os.sep + "burgerlib" + os.sep + "burgerps4.h"),
    ("bin" + os.sep + "burgerps4_stripped.h",
        "ps5" + os.sep + "burgerlib" + os.sep + "burgerps4.h"),

    # Switch
    ("bin" + os.sep + "burgerswitch_stripped.h",
        "switch" + os.sep + "burgerlib" + os.sep + "burgerswitch.h"),

    # Linux
    ("bin" + os.sep + "burgerlinux_stripped.h",
        "linux" + os.sep + "burgerlib" + os.sep + "burgerlinux.h"),

    # Darwin
    ("bin" + os.sep + "burgerdarwin_stripped.h",
        "macosx" + os.sep + "burgerlib" + os.sep + "burgerdarwin.h"),
    ("bin" + os.sep + "burgerdarwin_stripped.h",
        "ios" + os.sep + "burgerlib" + os.sep + "burgerdarwin.h"),

    # Unix
    ("bin" + os.sep + "burgerunix_stripped.h",
        "android" + os.sep + "burgerlib" + os.sep + "burgerunix.h"),
    ("bin" + os.sep + "burgerunix_stripped.h",
        "shield" + os.sep + "burgerlib" + os.sep + "burgerunix.h"),
    ("bin" + os.sep + "burgerunix_stripped.h",
        "stadia" + os.sep + "burgerlib" + os.sep + "burgerunix.h"),
    ("bin" + os.sep + "burgerunix_stripped.h",
        "linux" + os.sep + "burgerlib" + os.sep + "burgerunix.h"),
    ("bin" + os.sep + "burgerunix_stripped.h",
        "ios" + os.sep + "burgerlib" + os.sep + "burgerunix.h"),
    ("bin" + os.sep + "burgerunix_stripped.h",
        "macosx" + os.sep + "burgerlib" + os.sep + "burgerunix.h")
)

# Was Burgerlib already installed?
SDKS_FOLDER = get_sdks_folder()

# Build folder
BUILD_FOLDER = os.path.dirname(os.path.abspath(__file__))

########################################


def is_git():
    """
    Detect if perforce or git is source control

    Check if the file AUTHORS is write protected. If it is, it's likely under
    perforce control.

    Returns:
        True if building under git
    """

    # pylint: disable=global-statement
    global _GIT_FOUND

    if _GIT_FOUND is None:

        # Check if write protected, which is true under perforce
        _GIT_FOUND = not is_write_protected(
            os.path.join(BUILD_FOLDER, "AUTHORS"))

    return _GIT_FOUND

########################################


def find_makeheader():
    """
    Look for the tool, "makeheader"

    If building at Olde Skuul, the latest version is on the system path.
    If it's on github, the file is found in the folder "tools"

    """

    # Read the docs has it in the docs folder
    if _ON_RTD:
        return "docs/makeheader"

    if not is_git():
        # At Olde Skuul, it's on the path
        return "makeheader"

    # Select the right binary for the host OS and use the local copy
    return get_tool_path(os.path.join(BUILD_FOLDER, "tools"), "makeheader")

########################################


def create_generated_folders(working_directory=None):
    """
    Create folders for storing shader headers

    Args:
        working_directory: Directory to create directories in.
    """

    if working_directory is None:
        working_directory = BUILD_FOLDER

    for item in GENERATED_FOLDERS:
        create_folder_if_needed(os.path.join(working_directory, item))

########################################


def create_version_header(working_directory=None):
    """
    Update the changelist header

    Only pull the version from Perforce. If under git, assume the file has been
    pulled from the Olde Skuul buildbot

    Args:
        working_directory: Directory to store version.h in.
    """

    if not is_git():
        if working_directory is None:
            working_directory = BUILD_FOLDER

        dest_folder = os.path.join(working_directory, "source", "generated")
        make_version_header(working_directory,
                        os.path.join(dest_folder, "version.h"),
                        verbose=False)

########################################


def update_burger_h(working_directory=None):
    """
    Update burger.h in all platform folders

    Args:
        working_directory: Directory this script resides in.
    Returns:
        Zero if no error, non zero on error
    """

    if is_git():
        return 0

    if working_directory is None:
        working_directory = BUILD_FOLDER

    # Get the location of the super header
    dest_folder = os.path.join(working_directory, "bin")
    headerfilepath = os.path.join(dest_folder, "burger_stripped.h")

    # Was there a change in the output?
    error = 0
    for item in TARGETFOLDERS:
        destfolder = os.path.join(SDKS_FOLDER, item, "burgerlib")
        create_folder_if_needed(destfolder)
        testfile = os.path.join(destfolder, "burger.h")

        # Copy only if changed or doesn't exist
        if not os.path.isfile(testfile) or not compare_files(
                headerfilepath, testfile):
            error = copy_file_if_needed(
                headerfilepath, testfile, perforce=True)
            if error:
                break

    return error

########################################


def update_special_headers(working_directory=None):
    """
    Update all the special headers for each platform

    Args:
        working_directory: Directory this script resides in.
    Returns:
        Zero if no error, non zero on error
    """

    if is_git():
        return 0

    if working_directory is None:
        working_directory = BUILD_FOLDER

    error = 0
    for dest in TARGETFOLDERS:

        # Get the platform folder
        sourcefolder = os.path.join(
            working_directory, "source", "platforms", dest)

        # Is this platform's source code present?
        if not os.path.isdir(sourcefolder):
            sourcefolder = None

        # Process the special headers
        for item in SPECIALHEADERS:

            # Assume the generic header
            headerfilepath = os.path.join(working_directory, "source", item)
            if sourcefolder:
                # Is there an override?
                headerfiletest = os.path.join(sourcefolder, item)
                if os.path.isfile(headerfiletest):
                    # Use the override
                    headerfilepath = headerfiletest

            destfile = os.path.join(SDKS_FOLDER, dest, "burgerlib", item)

            # Copy if the destination doesn't exist or it's different
            # from the header
            if not os.path.isfile(destfile) or not compare_files(
                    headerfilepath, destfile):
                error = copy_file_if_needed(
                    headerfilepath, destfile, perforce=True)
                if error:
                    break
    return error


########################################


def update_unique_headers(working_directory=None):
    """
    Update all the unique headers for each platform

    Args:
        working_directory: Directory this script resides in.
    Returns:
        Zero if no error, non zero on error
    """

    if is_git():
        return 0

    if working_directory is None:
        working_directory = BUILD_FOLDER

    error = 0
    for item in UNIQUE_HEADERS:

        # Get the platform folder
        sourcefile = os.path.join(working_directory, item[0])
        destfile = os.path.join(SDKS_FOLDER, item[1])

        # Copy if the destination doesn't exist or it's different
        # from the header
        if not os.path.isfile(destfile) or \
                not compare_files(sourcefile, destfile):
            error = copy_file_if_needed(
                sourcefile, destfile, perforce=True)
            if error:
                break

    return error

########################################


def update_mac_resources(working_directory=None):
    """
    Did any of the Mac Carbon/Classic resource files change?

    Args:
        working_directory: Directory this script resides in.
    Returns:
        Zero if no error, non zero on error
    """

    if is_git():
        return 0

    if working_directory is None:
        working_directory = BUILD_FOLDER

    error = 0
    sourcefolder = os.path.join(
        working_directory, "source", "platforms", "mac")
    destfolder = os.path.join(SDKS_FOLDER, "mac", "burgerlib")
    filedata = os.listdir(sourcefolder)
    for item in filedata:
        if item.lower().endswith(".r"):
            destfile = os.path.join(destfolder, item)
            sourcefile = os.path.join(sourcefolder, item)
            if not os.path.isfile(destfile) or not compare_files(
                    sourcefile, destfile):
                error = copy_file_if_needed(
                    sourcefile, destfile, perforce=True)
                if error:
                    break
    return error

########################################


def prebuild(working_directory, configuration):
    """
    Perform actions before building any IDE based projects.

    This function is called before any IDE or other script is invoked. This is
    perfect for creating headers or other data that the other build projects
    need before being invoked.

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

    # Generate folders and create version.h

    # Create folders for storing shader headers
    create_generated_folders(working_directory)

    # Update the changelist header
    create_version_header(working_directory)

    # Create the super header using the makeheader tool
    dest_folder = os.path.join(working_directory, "bin")

    # makeheader is a local copy or one from the path?
    makeheader = find_makeheader()

    for item in SUPER_HEADERS:
        templatepath = os.path.join(working_directory, "source", item[0])

        # Only process files that exist
        if os.path.isfile(templatepath):
            if item[1]:
                headerfilepath = os.path.join(dest_folder, item[1])
                cmd = (makeheader, templatepath, headerfilepath)
                print(" ".join(cmd))
                sys.stdout.flush()
                error, _, _ = run_command(cmd, working_dir=working_directory)
                # if error:
                #    break

            if item[2]:
                headerfilepath = os.path.join(dest_folder, item[2])
                cmd = (makeheader, "-r", templatepath, headerfilepath)
                print(" ".join(cmd))
                sys.stdout.flush()
                error, _, _ = run_command(cmd, working_dir=working_directory)
                # if error:
                #    break

    return error

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

    # Copy the headers of burgerlib into their proper folder
    error = update_burger_h(working_directory)

    # Was there a change in special headers?
    if not error:
        error = update_special_headers(working_directory)

    # Was there a change in unique headers?
    if not error:
        error = update_unique_headers(working_directory)

    if not error:
        # Did any of the Mac Carbon/Classic resource files change?
        error = update_mac_resources(working_directory)

    return error


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

    clean_directories(working_directory, (".vscode", "appfolder", "temp",
                                          "ipch", "bin", ".vs", "*_Data",
                                          "* Data", "__pycache__"))

    clean_files(working_directory, (".DS_Store", "*.suo", "*.user", "*.ncb",
                                    "*.err", "*.sdf", "*.layout.cbTemp",
                                    "*.VC.db", "*.pyc", "*.pyo"))

########################################


def do_build(working_directory):
    """
    Build all the headers if invoked as a command

    Args:
        working_directory: Directory this file resides in.
    Returns:
        Zero on no error, non-zero on error.
    """

    # Perform the first build
    error = prebuild(working_directory, "all")
    if not error:

        # Perform the latter build
        error = postbuild(working_directory, "all")
    return error

########################################


# If called as a command line and not a class, perform the build
if __name__ == "__main__":
    sys.exit(do_build(BUILD_FOLDER))
