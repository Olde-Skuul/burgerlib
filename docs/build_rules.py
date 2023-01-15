#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Build rules for the makeprojects suite of build tools.

This file is parsed by the cleanme, buildme, rebuildme and makeprojects
command line tools to clean, build and generate project files.

When any of these tools are invoked, this file is loaded and parsed to
determine special rules on how to handle building the code and / or data.
"""

# pylint: disable=consider-using-f-string
# pylint: disable=unused-argument

from __future__ import absolute_import, print_function, unicode_literals

import sys
import os
import stat
from shutil import copyfile
import filecmp

from burger import import_py_script, clean_directories, clean_files, \
    create_folder_if_needed

# If set to True, ``buildme -r``` will not parse directories in this folder.
NO_RECURSE = True

########################################


def remove_missing(dest_dir, source_dir):
    """
    Given two folders, check if files exist in the destination
    directory but not in the source directory, delete the file
    in the destination directory

    Args:
        dest_dir
            Directory to remove files from
        source_dir
            Directory to check if files are missing

    """
    source_files = os.listdir(source_dir)
    dest_files = os.listdir(dest_dir)
    for item in dest_files:
        if item == "search":
            continue
        if item == ".pyftpsync-meta.json":
            continue
        if item not in source_files:
            os.remove(os.path.join(dest_dir, item))

########################################


def copy_if_changed(dest_dir, source_dir):
    """
    Given two folders, check if files exist in the destination
    directory but not in the source directory, delete the file
    in the destination directory

    Args:
        dest_dir
            Directory to remove files from
        source_dir
            Directory to check if files are missing

    """
    source_files = os.listdir(source_dir)
    dest_files = os.listdir(dest_dir)

    # Copy all the files that have changed
    for item in source_files:

        # Skip the directory
        if item == "search":
            continue

        # Create the paths
        source = os.path.join(source_dir, item)
        destination = os.path.join(dest_dir, item)

        # Easy, if it's a new file, just copy it.
        if item not in dest_files:
            print("Copying {0} -> {1}".format(source, destination))
            try:
                copyfile(source, destination)
            except IOError as error:
                print(error)
                return error.errno
            continue

        if not filecmp.cmp(source, destination):
            print("Refreshing {0} -> {1}".format(source, destination))
            try:
                copyfile(source, destination)
            except IOError as error:
                print(error)
                return error.errno

    return 0

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
    script = import_py_script(os.path.join(root_folder, "build_rules.py"))
    if not script:
        error = 10
    else:
        error = script.prebuild(root_folder, "all")

    if not error:
        # Generate the charsets
        script = import_py_script(os.path.join(working_directory,
            "generate_charsets.py"))
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
    raw_dir = os.path.join(working_directory, "temp", "burgerlibdoxygenraw")

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
        filename = os.path.join(raw_dir, item)
        try:
            os.chmod(filename, stat.S_IMODE(os.stat(filename).st_mode) |
                stat.S_IWRITE | stat.S_IREAD | stat.S_IRGRP | stat.S_IWOTH)
        except FileNotFoundError:
            break

    # Get all the paths for the docs folder and the cache
    html_dir = os.path.join(working_directory, "temp", "burgerlibdoxygen")
    html_source = os.path.join(html_dir, "search")
    raw_source = os.path.join(raw_dir, "search")

    # Ensure the output folders exist
    create_folder_if_needed(html_dir)
    create_folder_if_needed(html_source)

    # Check if files are to be deleted
    remove_missing(html_dir, raw_dir)
    remove_missing(html_source, raw_source)

    # Check if files are to be deleted
    copy_if_changed(html_dir, raw_dir)
    copy_if_changed(html_source, raw_source)
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

    clean_directories(working_directory, (".vscode",
                                          "appfolder",
                                          "temp",
                                          "ipch",
                                          "bin",
                                          ".vs",
                                          "*_Data",
                                          "* Data",
                                          "__pycache__"))

    clean_files(working_directory, (".DS_Store",
                                    "*.suo",
                                    "*.user",
                                    "*.ncb",
                                    "*.err",
                                    "*.sdf",
                                    "*.layout.cbTemp",
                                    "*.VC.db",
                                    "*.pyc",
                                    "*.pyo"))


########################################


# If called as a command line and not a class, perform the build
if __name__ == "__main__":
    WORKING_DIRECOTORY = os.path.dirname(os.path.abspath(__file__))
    sys.exit(prebuild(WORKING_DIRECOTORY, "all"))
