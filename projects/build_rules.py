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

import os
from burger import copy_file_if_needed, get_windows_host_type, \
    is_codewarrior_mac_allowed, get_sdks_folder
from makeprojects import PlatformTypes, IDETypes, ProjectTypes, makeprojects, \
    _HLSL_MATCH, _GLSL_MATCH, _X360SL_MATCH, _VITACG_MATCH

# If set to True, ``buildme -r``` will not parse directories in this folder.
BUILDME_NO_RECURSE = True

# ``buildme``` will build these files and folders first.
BUILDME_DEPENDENCIES = []

# If set to True, ``cleanme -r``` will not parse directories in this folder.
CLEANME_NO_RECURSE = True

# ``cleanme`` will clean the listed folders using their rules before cleaning.
# this folder.
CLEANME_DEPENDENCIES = []

## List of Codewarrior library files to move to sdks/windows/burgerlib
WINDOWS_LIB_FILES = (
    'burgerc50w32rel.lib',
    'burgerc50w32dbg.lib',
    'burgerc50w32int.lib'
)

## List of Codewarrior library files to move to sdks/mac/burgerlib
MAC68K_LIB_FILES = (
    'burgerc58mac68krel.lib',
    'burgerc58mac68kdbg.lib',
    'burgerc58mac68kint.lib',
    'burgerc58mac68kfardbg.lib',
    'burgerc58mac68kfarint.lib',
    'burgerc58mac68kfarrel.lib',
    'burgerc58mac68kfpudbg.lib',
    'burgerc58mac68kfpuint.lib',
    'burgerc58mac68kfpurel.lib',
    'burgerc58mac68kfarfpudbg.lib',
    'burgerc58mac68kfarfpuint.lib',
    'burgerc58mac68kfarfpurel.lib',
    'burgerc58car68kfarfpudbg.lib',
    'burgerc58car68kfarfpuint.lib',
    'burgerc58car68kfarfpurel.lib'
)

## Directories for the cross platform source code
BURGER_LIB_SOURCE = (
    '../source',
    '../source/ansi',
    '../source/audio',
    '../source/commandline',
    '../source/compression',
    '../source/file',
    '../source/flashplayer',
    '../source/graphics',
    '../source/graphics/effects',
    '../source/graphics/shaders',
    '../source/input',
    '../source/lowlevel',
    '../source/math',
    '../source/memory',
    '../source/network',
    '../source/random',
    '../source/text'
)

## PS3 specific code
BURGER_LIB_PS3 = (
    '../source/ps3',
)

## PS4 specific code
BURGER_LIB_PS4 = (
    '../source/ps4',
)

## PS Vita specific code
BURGER_LIB_VITA = (
    '../source/vita',
    '../source/graphics/shadersvita'
)

## Nintendo WiiU specific code
BURGER_LIB_WIIU = (
    '../source/wiiu',
)

## Nintendo Switch specific code
BURGER_LIB_SWITCH = (
    '../source/switch',
    '../source/graphics/vulkan'
)

## Microsoft Xbox 360 specific code
BURGER_LIB_XBOX_360 = (
    '../source/xbox360',
    '../source/graphics/shadersxbox360'
)

## Microsoft Xbox ONE specific code
BURGER_LIB_XBOX_ONE = (
    '../source/xboxone',
    '../source/graphics/shadersxboxone'
)

## nVidia Shield specific code
BURGER_LIB_SHIELD = (
    '../source/shield',
    '../source/graphics/shadersopengl',
    '../source/graphics/vulkan'
)

## Apple macOS X specific code
BURGER_LIB_MACOSX = (
    '../source/macosx',
    '../source/graphics/shadersopengl'
)

## Apple macOS 7-9 and Carbon specific code
BURGER_LIB_MAC = (
    '../source/mac',
    '../source/graphics/shadersopengl'
)

## Apple iOS specific code
BURGER_LIB_IOS = (
    '../source/ios',
    '../source/graphics/shadersopengl'
)

## Microsoft MSDos specific code
BURGER_LIB_DOS = (
    '../source/msdos',
)

## Microsoft Windows specific code
BURGER_LIB_WINDOWS = (
    '../source/windows',
    '../source/graphics/shadersdx9',
    '../source/graphics/shadersopengl',
    '../source/graphics/vulkan'
)

## Linux specific code
BURGER_LIB_LINUX = (
    '../source/linux',
    '../source/graphics/shadersopengl',
    '../source/graphics/vulkan'
)

## Generated source file
BURGER_LIB_GENERATED = (
    '../source/generated/version.h',
)

## List of projects to build
ARG_LISTS = [
    ('windows', 'burger', 'library',
     ['vs2003', 'vs2005', 'vs2008', 'vs2010', 'vs2012',
      'vs2013', 'vs2015', 'vs2017', 'vs2019', 'watcom',
      'codewarrior50']),
    ('windows', 'unittests', 'console',
     ['vs2003', 'vs2005', 'vs2008', 'vs2010', 'vs2012',
      'vs2013', 'vs2015', 'vs2017', 'vs2019', 'watcom',
      'codewarrior50']),
    ('ps3', 'burger', 'library', ['vs2015']),
    ('ps4', 'burger', 'library', ['vs2015']),
    ('vita', 'burger', 'library', ['vs2015']),
    ('vita', 'unittests', 'console', ['vs2015']),
    ('xbox360', 'burger', 'library', ['vs2010']),
    ('xbox360', 'unittests', 'console', ['vs2010']),
    ('xboxone', 'burger', 'library', ['vs2017']),
    ('xboxone', 'unittests', 'console', ['vs2017']),    
    ('wiiu', 'burger', 'library', ['vs2013']),
    ('switch', 'burger', 'library', ['vs2017']),
    ('switch', 'unittests', 'app', ['vs2017']),
    ('shield', 'burger', 'library', ['vs2015']),
    ('msdos', 'burger', 'library', ['watcom']),
    ('msdos4gw', 'unittests', 'console', ['watcom']),
    ('macosx', 'burger', 'library', ['xcode3', 'xcode5']),
    ('macosx', 'unittests', 'console', ['xcode3', 'xcode5']),
    ('ios', 'burger', 'library', ['xcode5']),
    ('linux', 'burger', 'library', ['make']),
    ('linux', 'unittests', 'console', ['make']),
    ('msdos', 'burger', 'library', ['codeblocks']),
    ('windows', 'burger', 'library', ['codeblocks']),
    ('windows', 'unittests', 'console', ['codeblocks'])
]

########################################


def do_generate(working_directory):
    """
    Generate all the project files.

    Args:
        working_directory: Directory to place the project files.
    """

    #ARG_LISTS_TEST = [
    #    ('switch', 'burger', 'library', ['vs2017'])
    #]

    for item in ARG_LISTS:
        args = []
        args.extend(['-p', item[0]])
        args.extend(['-n', item[1]])
        args.extend(['-t', item[2]])
        for ide in item[3]:
            args.extend(['-g', ide])
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

    # Copy the windows version of the CodeWarrior libraries.
    error = 0

    # If the host can run CodeWarrior for Windows (Codewarrior 9.4 Mac or
    # Windows) then copy the generated windows library files

    if get_windows_host_type() or is_codewarrior_mac_allowed():

        # Update the codewarrior files

        sdks = get_sdks_folder()
        windowsburgerbase = os.path.join(sdks, 'windows', 'burgerlib')
        outputfolder = os.path.join(working_directory, 'bin')
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
        macburgerbase = os.path.join(sdks, 'mac', 'burgerlib')
        outputfolder = os.path.join(working_directory, 'bin')
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
            new_name = os.path.splitext(item)[0] + '.h'

            # Save in "generated" folder
            source_files.append(os.path.join(abs_path, 'generated', new_name))

########################################


def do_project(working_directory, project):
    """
    Set the custom attributes for each configuration.

    Args:
        configuration: Configuration to modify.
    """

    # Too many branches
    # Too many statements
    # pylint: disable=R0912,R0915

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
        platform_folder = 'windows'
        source_folders_list.extend(BURGER_LIB_WINDOWS)

        if not ide.is_codewarrior():
            include_folders_list.extend([
                '$(BURGER_SDKS)/windows/perforce',
                '$(BURGER_SDKS)/windows/directx9',
                '$(BURGER_SDKS)/windows/opengl'])

        if ide in (IDETypes.watcom, IDETypes.codeblocks):
            include_folders_list.append('$(BURGER_SDKS)/windows/windows5')

        # Visual Studio 2019 doesn't have the old Win XP headers
        # if ide >= IDETypes.vs2017:
        # include_folders_list.append(
        # '$(BURGER_SDKS)/windows/windows5')

        project.define_list = [
            '_CRT_NONSTDC_NO_WARNINGS',
            '_CRT_SECURE_NO_WARNINGS',
            'GLUT_NO_LIB_PRAGMA',
            '_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1']

        find_generated_source(
            source_files_list,
            working_directory,
            '../source/windows',
            _HLSL_MATCH)

    # MS/DOS
    if platform.is_msdos():
        platform_folder = 'dos'
        source_folders_list.extend(BURGER_LIB_DOS)
        include_folders_list.append('$(BURGER_SDKS)/dos/x32')

    # Linux
    if platform is PlatformTypes.linux:
        platform_folder = 'linux'
        source_folders_list.extend(BURGER_LIB_LINUX)

    # Apple platforms
    if platform.is_macosx():
        platform_folder = 'macosx'
        source_folders_list.extend(BURGER_LIB_MACOSX)

    if platform.is_ios():
        platform_folder = 'ios'
        source_folders_list.extend(BURGER_LIB_IOS)

    if platform.is_macos():
        platform_folder = 'mac'
        source_folders_list.extend(BURGER_LIB_MAC)

    # Sony platforms
    if platform is PlatformTypes.ps3:
        platform_folder = 'ps3'
        source_folders_list.extend(BURGER_LIB_PS3)

    if platform is PlatformTypes.ps4:
        platform_folder = 'ps4'
        source_folders_list.extend(BURGER_LIB_PS4)

    if platform is PlatformTypes.vita:
        platform_folder = 'vita'
        source_folders_list.extend(BURGER_LIB_VITA)

        vs_props.append('$(VCTargetsPath)\\BuildCustomizations\\vitacg.props')
        vs_targets.append(
            '$(VCTargetsPath)\\BuildCustomizations\\vitacg.targets')
        find_generated_source(
            source_files_list,
            working_directory,
            '../source/graphics/shadersvita',
            _VITACG_MATCH)

    # Microsoft platforms
    if platform is PlatformTypes.xbox360:
        platform_folder = 'xbox360'
        source_folders_list.extend(BURGER_LIB_XBOX_360)
        vs_props.append('$(VCTargetsPath)\\BuildCustomizations\\x360sl.props')
        vs_targets.append(
            '$(VCTargetsPath)\\BuildCustomizations\\x360sl.targets')
        find_generated_source(
            source_files_list, working_directory,
            '../source/graphics/shadersxbox360',
            _X360SL_MATCH)

    if platform is PlatformTypes.xboxone:
        platform_folder = 'xboxone'
        source_folders_list.extend(BURGER_LIB_XBOX_ONE)
        #vs_props.append('$(VCTargetsPath)\\BuildCustomizations\\x360sl.props')
        #vs_targets.append(
        #    '$(VCTargetsPath)\\BuildCustomizations\\x360sl.targets')
        #find_generated_source(
        #    source_files_list, working_directory,
        #    '../source/graphics/shadersxbox360',
        #    _X360SL_MATCH)

    # Android
    if platform is PlatformTypes.shield:
        platform_folder = 'shield'
        source_folders_list.extend(BURGER_LIB_SHIELD)

    # Nintendo
    if platform is PlatformTypes.wiiu:
        platform_folder = 'wiiu'
        source_folders_list.extend(BURGER_LIB_WIIU)

    if platform is PlatformTypes.switch:
        platform_folder = 'switch'
        source_folders_list.extend(BURGER_LIB_SWITCH)

    # Add property files for unittests or burgerlib
    if project.name == 'unittests':
        source_folders_list.append('../unittest')

        if platform is PlatformTypes.xboxone:
            source_files_list.append('../unittest/xboxone/unittestxboxone.appxmanifest')

        library_folders_list.append(
            '$(BURGER_SDKS)/{}/burgerlib'.format(platform_folder))
        if platform is PlatformTypes.linux:
            project.libraries_list.append('GL')

    else:
        project.deploy_folder = \
            '$(BURGER_SDKS)/{}/burgerlib'.format(platform_folder)

    if platform.is_windows():
        if ide.is_codewarrior() or project.name == 'unittests':
            library_folders_list.extend([
                '$(BURGER_SDKS)/windows/perforce',
                '$(BURGER_SDKS)/windows/opengl',
                '$(BURGER_SDKS)/windows/directx9'
            ])

    # Enable OpenGL extensions
    if platform.is_windows() or platform.is_android() or \
        platform is PlatformTypes.linux:
        vs_props.append(
            '$(VCTargetsPath)\\BuildCustomizations\\glsl.props')
        vs_targets.append(
            '$(VCTargetsPath)\\BuildCustomizations\\glsl.targets')
        vs_rules.append('glsl.rules')
        find_generated_source(
            source_files_list, working_directory,
            '../source/graphics/shadersopengl',
            _GLSL_MATCH)

    # Enable hlsl extensions
    if platform.is_windows():
        vs_props.append(
            '$(VCTargetsPath)\\BuildCustomizations\\hlsl.props')
        vs_targets.append('$(VCTargetsPath)\\BuildCustomizations\\hlsl.targets')
        vs_rules.append('hlsl.rules')
        find_generated_source(
            source_files_list,
            working_directory,
            '../source/graphics/shadersdx9',
            _HLSL_MATCH)

    #project.vs_SuppressStartupBanner = False
    #project.compiler_options = ['/nologo']
    #project.linker_options = ['/nologo']
    # Store the values into the project
    project.source_folders_list = source_folders_list
    project.include_folders_list = include_folders_list
    project.source_files_list = source_files_list
    project.library_folders_list = library_folders_list
    project.vs_props = vs_props
    project.vs_targets = vs_targets
    project.vs_rules = vs_rules
    if platform.is_windows():
        project.cw_environment_variables = ['BURGER_SDKS']

    project.custom_rules = {
        'ps*.hlsl':
        {'HeaderFileName': r'%(RootDir)%(Directory)Generated\%(FileName).h',
         'VariableName': 'g_%(FileName)',
         'TargetProfile': 'ps_2_0'},

        'vs*.hlsl':
        {'HeaderFileName': r'%(RootDir)%(Directory)Generated\%(FileName).h',
         'VariableName': 'g_%(FileName)',
         'TargetProfile': 'vs_2_0'},

        '*.glsl':
        {'ObjectFileName': r'%(RootDir)%(Directory)Generated\%(FileName).h'},

        'ps*.x360sl':
        {'HeaderFileName': r'%(RootDir)%(Directory)Generated\%(FileName).h',
         'VariableName': 'g_%(FileName)',
         'TargetProfile': 'ps_2_0'},

        'vs20*.x360sl':
        {'HeaderFileName': r'%(RootDir)%(Directory)Generated\%(FileName).h',
         'VariableName': 'g_%(FileName)',
         'TargetProfile': 'vs_2_0'},

        'ps*.vitacg':
        {'HeaderFileName': r'%(RootDir)%(Directory)Generated\%(FileName).h',
         'TargetProfile': 'sce_fp_psp2'},

        'vs*.vitacg':
        {'HeaderFileName': r'%(RootDir)%(Directory)Generated\%(FileName).h',
         'TargetProfile': 'sce_vp_psp2'}
    }


########################################

# Unused argument
# pylint: disable=W0613


def do_configuration(configuration):
    """
    Set the custom attributes for each configuration.

    Args:
        configuration: Configuration to modify.
    """
    # if configuration.name == 'Debug':
    # configuration.exclude_list = 'bralaw.cpp'
    return 1

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
        working_directory: Directory for this function to operate on.
        root: If True, stop execution upon completion of this function
        kargs: Extra arguments specific to each command.
    Return:
        Zero on no error or no action.
    """

    # Too many return statements
    # Unused arguments
    # pylint: disable=R0911,W0613

    # Commands for makeprojects.
    if command == 'default_project_name':
        # Return the default name of the project to create.
        return 'burger'

    if command == 'default_project_type':
        # Return the default type of project to create.
        if kargs['project_name'] == 'unittests':
            return ProjectTypes.tool
        return ProjectTypes.library

    if command == 'default_ide':
        # Return the default IDE to build for.
        return IDETypes.default()

    if command == 'default_platform':
        # Return the default platform to build for.
        return PlatformTypes.default()

    if command == 'project_settings':
        # Return the settings for a specific project
        do_project(working_directory, project=kargs['project'])

    elif command == 'configuration_settings':
        # Return the settings for a specific configuation
        return do_configuration(configuration=kargs['configuration'])

    return 0


# If called as a command line and not a class, perform the build
if __name__ == "__main__":
    import sys
    sys.exit(do_generate(os.path.dirname(os.path.abspath(__file__))))
