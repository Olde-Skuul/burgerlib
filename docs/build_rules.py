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
import stat

from burger import import_py_script, clean_directories, clean_files

# If set to True, ``buildme -r``` will not parse directories in this folder.
BUILDME_NO_RECURSE = True

# ``buildme``` will build these files and folders first.
BUILDME_DEPENDENCIES = []

# If set to True, ``cleanme -r``` will not parse directories in this folder.
CLEANME_NO_RECURSE = True

# ``cleanme`` will clean the listed folders using their rules before cleaning.
# this folder.
CLEANME_DEPENDENCIES = []

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

    # Create the super header for extracting documentation.
    # Update the changelist header
    root_folder = os.path.dirname(working_directory)
    script = import_py_script(os.path.join(root_folder, 'build_rules.py'))
    if not script:
        error = 10
    else:
        error = script.prebuild(root_folder, 'all')

    if not error:
        # Generate the charsets
        script = import_py_script(os.path.join(working_directory,
            'generate_charsets.py'))
        if not script:
            error = 10
        else:
            error = script.main(working_directory)

    return error

########################################


def postbuild(working_directory, configuration):
    """
    Perform actions after building doxygen.

    This function is called after doxygen and will clean up files there retained
    their ``read only`` attribute. There is a bug in doxygen that once it
    copies such a file, it can't overwrite it on subsequent executions of
    doxgyen.

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

    # Get the directory where the cleanup must occur
    html_dir = os.path.join(working_directory, 'temp', 'burgerlibdoxygen')

    file_list = (
        "docs.css",
        "burger.png",
        "oldeskuul.png",
        "sourceforge.jpg",
        "twitter.jpg",
        "facebook.png",
        "linkedin.png",
        "github.png",
        "burgerbackground.png",
        "spec-gif89a.txt",
        "swf-file-format-spec.pdf",
        "avm2overview.pdf",
        "qtff-2001.pdf",
        "mpeg-2_audio_is.pdf",
        "11172-3.pdf",
        "aiff-1.3.pdf",
        "aiff-c.9.26.91.pdf",
        "creative voice file format.txt",
        "3dnow.pdf",
        "avx.pdf",
        "m68000prm.pdf",
        "powerpc-cwg.pdf",
        "qt6apiref.pdf",
        "qt4reference-extract.pdf",
        "MacintoshToolboxEssentials.pdf",
        "Sound_Manager.pdf",
        "macos_sound-extract.pdf",
        "mp3_theory.pdf",
        "lfsr04.pdf",
        "is138181.pdf",
        "is138182.pdf"
    )

    # Allow writing on these files.
    for item in file_list:
        try:
            os.chmod(os.path.join(html_dir, item), stat.S_IWRITE)
        except FileNotFoundError:
            break
    return 0

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

    clean_directories(working_directory, ('.vscode',
                                          'appfolder',
                                          'temp',
                                          'ipch',
                                          'bin',
                                          '.vs',
                                          '*_Data',
                                          '* Data',
                                          '__pycache__'))

    clean_files(working_directory, ('.DS_Store',
                                    '*.suo',
                                    '*.user',
                                    '*.ncb',
                                    '*.err',
                                    '*.sdf',
                                    '*.layout.cbTemp',
                                    '*.VC.db',
                                    '*.pyc',
                                    '*.pyo'))


########################################


# If called as a command line and not a class, perform the build
if __name__ == "__main__":
    WORKING_DIRECOTORY = os.path.dirname(os.path.abspath(__file__))
    sys.exit(prebuild(WORKING_DIRECOTORY, 'all'))
