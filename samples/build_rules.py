#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Configuration file on how to build and clean projects in a specific folder.

This file is parsed by the cleanme, buildme, rebuildme and makeprojects
command line tools to clean, build and generate project files.
"""

from __future__ import absolute_import, print_function, unicode_literals

from burger import clean_directories, clean_files, clean_codeblocks, \
    clean_xcode, clean_setup_py

from makeprojects import PlatformTypes, ProjectTypes, add_burgerlib

# Type of the project, default is ProjectTypes.tool
PROJECT_TYPE = ProjectTypes.tool

# Both ``cleanme`` and ``buildme`` will process any child directory with the
# clean(), prebuild(), build(), and postbuild() functions if True.
# Can be overridden above
GENERIC = True

# ``cleanme`` will assume only the function ``clean()`` is used if False.
# Overrides PROCESS_PROJECT_FILES
CLEANME_PROCESS_PROJECT_FILES = False

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

    clean_directories(working_directory, (
        "JSON", ".vscode", "appfolder", "temp",
        "ipch", "bin", ".vs", "*_Data",
        "* Data", "__pycache__"))

    clean_files(working_directory, (
        ".DS_Store", "*.suo", "*.user", "*.ncb",
        "*.err", "*.sdf", "*.layout.cbTemp", "*.VC.db",
        "*.pyc", "*.pyo"))

    # Check if the directory has a codeblocks project file and clean
    # codeblocks extra files
    clean_codeblocks(working_directory)

    # Allow purging user data in XCode project directories
    clean_xcode(working_directory)

    # Purge data for setup.py
    clean_setup_py(working_directory)

    return 0

########################################


def project_settings(project):
    """
    Set up defines and default libraries.

    Adjust the default settings for the project to generate. Usually it's
    setting the location of source code or perforce support.

    Args:
        project: Project record to update.

    Returns:
        None, to continue processing, zero is no error and stop processing,
        any other number is an error code.
    """

    if project.platform.is_windows():
        project.define_list.extend((
            "_CRT_NONSTDC_NO_WARNINGS",
            "_CRT_SECURE_NO_WARNINGS"))
    return 0

########################################


def configuration_settings(configuration):
    """
    Set up defines and libraries on a configuration basis.

    For each configation, set all configuration specific seting. Use
    configuration.name to determine which configuration is being processed.

    Args:
        configuration: Configuration class instance to update.

    Returns:
        None, to continue processing, zero is no error and stop processing,
        any other number is an error code.
    """

    # Use fastcall convention on windows 32 bit Intel
    if configuration.platform is PlatformTypes.win32:
        configuration.fastcall = True

    # Add burgerlib to any configuration
    add_burgerlib(configuration)

    # End processing here
    return 0
