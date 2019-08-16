#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Configuration file on how to build and clean projects in a specific folder.

This file is parsed by the cleanme, buildme, rebuildme and makeprojects
command line tools to clean, build and generate project files.
"""

from __future__ import absolute_import, print_function, unicode_literals

import sys
import os

from burger import make_version_header, get_sdks_folder, \
    create_folder_if_needed, run_command, compare_files, \
    copy_file_if_needed


# Folders for all the target operating systems supported

TARGETFOLDERS = (
    'windows',
    'dos',
    'mac',
    'macosx',
    'linux',
    'beos',
    'ps2',
    'ps3',
    'ps4',
    'vita',
    'gamecube',
    'wii',
    'dsi',
    'xbox',
    'xbox360',
    'xboxone',
    'ios',
    'android',
    'shield',
    'ouya',
    'switch'
)

SPECIALHEADERS = (
    'brstartup.h',
    'brgl.h',
    'brglext.h',
    'brglut.h',
    'brglxext.h',
    'brstdint.h'
)

GENERATED_FOLDERS = (
    'source/generated',
    'source/graphics/shadersdx9/generated',
    'source/graphics/shadersopengl/generated',
    'source/graphics/shadersvita/generated',
    'source/graphics/shadersxbox360/generated',
    'source/windows/generated'
)

########################################


def do_prebuild(working_directory):
    """ Copy the headers of burgerlib into their proper folder

    :Args:
        working_dir: Directory where the headers to copy are located

    Returns:
        Zero on no error, non-zero on error

    """

    # Too many branches
    # pylint: disable=R0912

    # Update the changelist header
    dest_folder = os.path.join(working_directory, 'source', 'generated')
    create_folder_if_needed(dest_folder)
    make_version_header(
        working_directory,
        os.path.join(dest_folder, 'version.h'),
        verbose=False)

    # Was Burgerlib already installed?
    sdks_folder = get_sdks_folder()

    # Ensure the destination directories exist for headers
    for item in TARGETFOLDERS:
        create_folder_if_needed(os.path.join(sdks_folder, item, 'burgerlib'))

    for item in GENERATED_FOLDERS:
        create_folder_if_needed(os.path.join(working_directory, item))

    # Ensure the output folder exists
    dest_folder = os.path.join(working_directory, 'bin')
    create_folder_if_needed(dest_folder)

    # Create the super header using the makeheader tool
    headerfilepath = os.path.join(dest_folder, 'burger.h')
    cmd = (
        'makeheader',
        os.path.join(working_directory, 'source', 'templateburgerbase.h'),
        headerfilepath)
    print(' '.join(cmd))
    error = run_command(cmd, working_dir=working_directory)[0]

    if not error:

        # Was there a change in the output?

        windowsburgerlib = os.path.join(sdks_folder, 'windows', 'burgerlib')
        testfile = os.path.join(windowsburgerlib, 'burger.h')
        if not os.path.isfile(testfile) or not compare_files(
                headerfilepath, testfile):

            # Copy only if changed

            for item in TARGETFOLDERS:
                destfolder = os.path.join(sdks_folder, item, 'burgerlib')
                error = copy_file_if_needed(
                    headerfilepath, os.path.join(
                        destfolder, 'burger.h'), perforce=True)
                if error:
                    break

    # Was there a change in special headers?
    if not error:

        for dest in TARGETFOLDERS:

            # Special case, burgerlib has the folder msdos and SDKS
            # has dos. Perform the override here

            if dest == 'dos':
                sourcefolder = os.path.join(
                    working_directory, 'source', 'msdos')
            else:
                sourcefolder = os.path.join(working_directory, 'source', dest)

            # Is this platform's source code present?
            if not os.path.isdir(sourcefolder):
                sourcefolder = None

            # Process the headers
            for item in SPECIALHEADERS:

                # Assume the generic header
                headerfilepath = os.path.join(working_directory, 'source', item)
                if sourcefolder:
                    # Is there an override?
                    headerfiletest = os.path.join(sourcefolder, item)
                    if os.path.isfile(headerfiletest):
                        # Use the override
                        headerfilepath = headerfiletest

                destfile = os.path.join(sdks_folder, dest, 'burgerlib', item)

                # Copy if the destination doesn't exist or it's different
                # from the header
                if not os.path.isfile(destfile) or not compare_files(
                        headerfilepath, destfile):
                    error = copy_file_if_needed(
                        headerfilepath, destfile, perforce=True)
                    if error:
                        break

    # Did any of the Mac Carbon/Classic resource files change?
    if not error:
        sourcefolder = os.path.join(working_directory, 'source', 'mac')
        destfolder = os.path.join(sdks_folder, 'mac', 'burgerlib')
        filedata = os.listdir(sourcefolder)
        for item in filedata:
            if item.lower().endswith('.r'):
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


def rules(command, working_directory, **kargs):
    """
    Main entry point for build_rules.py.

    When ``makeprojects``, ``cleanme``, or ``buildme`` is executed, they will
    call this function to perform the actions required for build customization.

    The parameter ``working_directory`` is required, and if it has no default
    parameter, this function will only be called with the folder that this
    file resides in. If there is a default parameter of ``None``, it will be
    called with any folder that it is invoked on. If the default parameter is a
    directory, this function will only be called if that directory is desired.

    The optional parameter of ``root``` alerts the tool if subsequent processing
    of other ``build_rules.py`` files are needed or if set to have a default
    parameter of ``True``, processing will end once the calls to this
    ``rules()`` function are completed.

    Commands are 'build', 'clean', 'prebuild', 'postbuild', 'project',
    'configurations'

    Arg:
        command: Command to execute.
        working_directory: Directory for this function to clean
        root: If set to True, exit cleaning upon completion of this function
    Return:
        Zero on success, non-zero on failure, and a list for 'configurations'

    """

    # Unused arguments
    # pylint: disable=W0613

    # Perform the action based on the command.

    if command == 'prebuild':
        # Perform actions before building any IDE based projects
        # Return non zero integer on error.
        return do_prebuild(working_directory)

    return 0

########################################


# If called as a command line and not a class, perform the build
if __name__ == "__main__":
    sys.exit(do_prebuild(os.path.dirname(os.path.abspath(__file__))))
