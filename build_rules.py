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
    copy_file_if_needed, clean_directories, clean_files

# ``cleanme`` will assume only the function ``clean()`` is used if False.
# Overrides PROCESS_PROJECT_FILES
CLEANME_PROCESS_PROJECT_FILES = False

# Folders for all the target operating systems supported

TARGETFOLDERS = (
    "windows",
    "msdos",
    "mac",
    "macosx",
    "linux",
    "beos",
    "ps2",
    "ps3",
    "ps4",
    "ps5",
    "psvita",
    "gamecube",
    "wii",
    "dsi",
    "xbox",
    "xbox360",
    "xboxone",
    "ios",
    "android",
    "shield",
    "ouya",
    "switch"
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
    "source/windows/generated",
    "bin"
)

# Was Burgerlib already installed?
SDKS_FOLDER = get_sdks_folder()

########################################


def create_generated_folders(working_directory):
    """
    Create folders for storing shader headers

    Args:
        working_directory: Directory to create directories in.
    """

    for item in GENERATED_FOLDERS:
        create_folder_if_needed(os.path.join(working_directory, item))

########################################


def create_version_header(working_directory):
    """
    Update the changelist header

     Args:
        working_directory: Directory to store version.h in.
    """

    dest_folder = os.path.join(working_directory, "source", "generated")
    make_version_header(working_directory, os.path.join(dest_folder, "version.h"),
        verbose=False)

########################################


def update_burger_h(working_directory):
    """
    Update burger.h in all platform folders

    Args:
        working_directory: Directory this script resides in.
    Returns:
        Zero if no error, non zero on error
    """

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


def update_special_headers(working_directory):
    """
    Update all the special headers for each platform

    Args:
        working_directory: Directory this script resides in.
    Returns:
        Zero if no error, non zero on error
    """

    error = 0
    for dest in TARGETFOLDERS:

        # Get the platform folder
        sourcefolder = os.path.join(working_directory, "source", "platforms", dest)

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

def update_mac_resources(working_directory):
    """
    Did any of the Mac Carbon/Classic resource files change?

    Args:
        working_directory: Directory this script resides in.
    Returns:
        Zero if no error, non zero on error
    """

    error = 0
    sourcefolder = os.path.join(working_directory, "source", "platforms", "mac")
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
    headerfilepath = os.path.join(dest_folder, "burger.h")
    templatepath = os.path.join(working_directory, "source", "templateburgerbase.h")
    cmd = ("makeheader", templatepath, headerfilepath)
    print(" ".join(cmd))
    error, _, _ = run_command(cmd, working_dir=working_directory)
    if not error:
        headerfilepath = os.path.join(dest_folder, "burger_stripped.h")
        cmd = ("makeheader", "-r", templatepath, headerfilepath)
        print(" ".join(cmd))
        error, _, _ = run_command(cmd, working_dir=working_directory)
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

    clean_directories(working_directory, (".vscode", "appfolder", "temp", "ipch",
                                          "bin", ".vs", "*_Data", "* Data",
                                          "__pycache__"))

    clean_files(working_directory, (".DS_Store", "*.suo", "*.user", "*.ncb",
                                    "*.err", "*.sdf", "*.layout.cbTemp", "*.VC.db",
                                    "*.pyc", "*.pyo"))

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
    sys.exit(do_build(os.path.dirname(os.path.abspath(__file__))))
