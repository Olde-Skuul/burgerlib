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
    create_folder_if_needed, where_is_pdflatex, where_is_makeindex, \
    run_command, load_text_file, save_text_file

# If set to True, ``buildme -r``` will not parse directories in this folder.
NO_RECURSE = True

# List of files to remove write protection
DOXYGEN_FILE_LIST = (
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

_ON_RTD = os.environ.get("READTHEDOCS", None) == "True"

########################################


def unlock_copied_files(working_directory):
    """
    Hack to unlock files copied by Doxygen

    When Doxygen copies files, the files checked into perforce are locked,
    and Doxygen retains the lock bit. Sadly, this means that on another
    pass, it can't copy the files because it can't overwrite a locked file.

    This function will iterate over these files and manually unlock them

    Args:
        working_directory
            Directory where the locked files reside
    """

    # Allow writing on these files.
    for item in DOXYGEN_FILE_LIST:

        # Full pathname
        filename = os.path.join(working_directory, item)
        try:
            # Unlock it
            os.chmod(filename, stat.S_IMODE(os.stat(filename).st_mode) |
                     stat.S_IWRITE | stat.S_IREAD | stat.S_IRGRP | stat.S_IWOTH)
        except FileNotFoundError:
            continue

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

    # Get the source and dest directories
    source_files = os.listdir(source_dir)
    dest_files = os.listdir(dest_dir)

    # Remove all files except the "blessed" ones
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

    # Get the source and dest directories
    source_files = os.listdir(source_dir)
    dest_files = os.listdir(dest_dir)

    # Copy all the files that have changed
    for item in source_files:

        # Skip the search directory
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

def create_pdf_docs(working_directory):
    """
    If latex is available, generate PDF docs

    This algorithm is a duplicate of the make file generated
    by Doxygen and duplicates its functionality so it's not
    necessary to have make installed on the host system.

    Args:
        working_directory
            Directory this script resides in.
    """
    # Check if the tools are present
    pdflatex = where_is_pdflatex()
    makeindex = where_is_makeindex()

    # Not installed? Just punt.
    if not pdflatex or not makeindex:
        return None

    # Where is the documentation directory?
    # Exit if there isn't any latex data
    latex_dir = os.path.join(
        working_directory, "temp", "burgerliblatex")
    if not os.path.isdir(latex_dir):
        return None

    # Start by cleaning off all of the cruft left over
    # from previous compilations
    clean_files(latex_dir, ("refman.pdf"
                            ".ps", "*.dvi", "*.aux", "*.toc",
                            "*.idx", "*.ind", "*.ilg", "*.log",
                            "*.out", "*.brf", "*.blg", "*.bbl"))

    # Commands to execute latex
    latex_cmd = [pdflatex, "refman"]
    index_cmd = [makeindex, "-q", "refman" + ".idx"]

    # First pass
    print("Compiling refman pass 1")
    error, std_lines, _ = run_command(
        latex_cmd, latex_dir, capture_stdout=True)
    if error:
        print(
            "Error in {}, look in refman.log for error".format(
                latex_dir))
        return None

    # Save the log for debugging
    if not _ON_RTD:
        save_text_file(os.path.join(latex_dir, "pass1.log"), std_lines)

    # Create the initial index
    print("Generating temp refman.idx")
    error, _, _ = run_command(
        index_cmd, latex_dir, capture_stdout=True, capture_stderr=True)

    # Try a maximum number of 8 times
    log_filename = os.path.join(latex_dir, "refman.log")
    for i in range(8):
        print("Compiling refman pass {}".format(i + 2))
        error, std_lines, _ = run_command(
            latex_cmd, latex_dir, capture_stdout=True)
        if error:
            print(
                "Error in {}, look in refman.log for error".format(
                    latex_dir))
        if not _ON_RTD:
            save_text_file(
                os.path.join(latex_dir, "pass{}.log".format(i + 2)), std_lines)

        # Check the log file for a need to rerun latex
        lines = load_text_file(log_filename)
        if not lines:
            print("Log file is empty")
            break

        do_loop = False
        for line in lines:
            # If any of these exist in the log, re-run
            if "Rerun LaTeX" in line or \
                "Rerun to get cross-references right" in line or \
                    "Rerun to get bibliographical references right" in line:
                do_loop = True
                break

        if not do_loop:
            break

    # Last pass
    # Create the final index
    print("Generating refman.idx")
    error, _, _ = run_command(
        index_cmd, latex_dir, capture_stdout=True)

    # Generate the final document
    print("Final pass on refman")
    error, std_lines, _ = run_command(
        latex_cmd, latex_dir, capture_stdout=True)
    if error:
        print(
            "Error in {}, look in refman.log for error".format(
                latex_dir))
        return None

    # Save the log for debugging
    if not _ON_RTD:
        save_text_file(os.path.join(latex_dir, "final.log"), std_lines)
    return None

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

    if not error:
        # Directory where the Doxygen data is stored
        raw_dir = os.path.join(
            working_directory, "temp", "burgerlibdoxygenraw")
        if os.path.isdir(raw_dir):
            # Allow writing on these files.
            unlock_copied_files(raw_dir)

        # Directory where the PDF is generated
        latex_dir = os.path.join(working_directory, "temp", "burgerliblatex")
        if os.path.isdir(latex_dir):
            # Allow writing on these files.
            unlock_copied_files(latex_dir)

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

    if not _ON_RTD:
        # Get the directory where the cleanup must occur
        raw_dir = os.path.join(
            working_directory, "temp", "burgerlibdoxygenraw")

        # Allow writing on these files.
        unlock_copied_files(raw_dir)

        # Get the directory where the latex cleanup must occur
        latex_dir = os.path.join(working_directory, "temp", "burgerliblatex")

        # Allow writing on these files.
        unlock_copied_files(latex_dir)

        # Get all the paths for the docs folder and the cache
        html_dir = os.path.join(
            working_directory, "temp", "burgerlibdoxygen")
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

    # Create the PDF version of the documentation
    create_pdf_docs(working_directory)

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
