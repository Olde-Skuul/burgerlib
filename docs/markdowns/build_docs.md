# How to build the documentation

The documentation requires several steps which are all automated using the makeprojects tool chain. Makeprojects is a python module that uses generic build rules stored in ``build_rules.py`` files. Invoking ``buildme`` will build the libraries, but because documentation is usually optional, ``buildme`` has a command switch of ``-docs`` which enables it to locate Doxygen files and starts Doxygen.

## Build commands

Note, these commands are the same on Linux, MacOSX and Windows.

```bash
# Move to the documentation folder
cd docs
# Invoke doxygen with buildme from makeprojects
buildme -docs
```

HTML documentation will be in ``temp/burgerlibdoxygenraw`` with the file ``index.htm`` being the root file.

PDF file will be at ``bin/burgerlib.pdf`` from the root folder.

## Prerequisites

Burgerlib's documentation is built using Doxygen, latex, python, makeprojects, pandoc, and makeheader.

Makeheader is included in the tools folder from the root, built for Windows, MacOSX and Linux for x64.

**Make sure all the tools below are available in the system path.**

## Python 3

Python is used for all the build scripts. While they do run on Python 2.7, Python 3.12 or higher is strongly recommended.

[Python can be downloaded here.](https://www.python.org/downloads/)

After Python is installed, run ``pip install -r requirements.txt`` from the root folder. This will ensure ``buildme`` and other python scripts needed by the build system are present.

## Doxygen

Doxygen is the primary tool used to generate the documentation.

Install Doxygen 1.12 or later from [Doxygen download page](https://www.doxygen.nl/download.html). This page has installers for all current platforms.

## Graphviz

Graphviz is used by Doxygen to generate the graphs in the documentation. [Download Graphviz from here.](https://graphviz.org/download/)

## Pandoc

Pandoc is used by Sphinx when generating documentation for ReadTheDocs.org. It's not used for pure Doxygen documentation generation, so this is an optional install. [Download Pandoc from here.](https://pandoc.org/installing.html)

## Linux apt-get

``sudo apt-get install pandoc doxygen graphviz texlive-latex-recommended``

``sudo python -m pip install -U burger makeprojects``

## Older MacOSX

If you're running MacOSX Snow Leopard/Leopard/Tiger, use [MacPorts for your version of MacOSX](https://www.macports.org/install.php) and use ``sudo port install doxygen graphviz pandoc texlive`` to obtain and build doxygen for PowerPC or i386 based Macs. Pandoc, graphviz, and python312 are all installed with MacPorts.

Afterwards, install the python tools with ``sudo python -m pip install -U burger makeprojects``
