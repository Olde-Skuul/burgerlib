#
# Build burger with make
#
# Generated with makeprojects
#

#
# Project to build without a goal
#

.DEFAULT_GOAL := all

#
# List the names of all of the final binaries to build
#

.PHONY: all
all: Debug Internal Release ;

#
# Configurations
#

.PHONY: Debug
Debug: Debuglnx ;

.PHONY: Internal
Internal: Internallnx ;

.PHONY: Release
Release: Releaselnx ;

#
# Platforms
#

.PHONY: lnx
lnx: Debuglnx Internallnx Releaselnx ;

#
# List the names of all of the final binaries to build
#

.PHONY: Debuglnx
Debuglnx:
	@$(MAKE) -e --no-print-directory CONFIG=Debug TARGET=lnx -f burgermaklnx.mak bin/libburgermaklnxdbg.a

.PHONY: Internallnx
Internallnx:
	@$(MAKE) -e --no-print-directory CONFIG=Internal TARGET=lnx -f burgermaklnx.mak bin/libburgermaklnxint.a

.PHONY: Releaselnx
Releaselnx:
	@$(MAKE) -e --no-print-directory CONFIG=Release TARGET=lnx -f burgermaklnx.mak bin/libburgermaklnxrel.a

#
# Clean all binaries
#

.PHONY: clean
clean:
	@-rm -rf temp/burgermaklnxdbg temp/burgermaklnxint temp/burgermaklnxrel
	@-rm -f bin/libburgermaklnxdbg.a bin/libburgermaklnxint.a bin/libburgermaklnxrel.a
	@if [ -d bin ] && files=$$(ls -qAL -- bin) && [ -z "$$files" ]; then rm -fd bin; fi

#
# Create the folder for the binary output
#

bin:
	@-mkdir -p bin

#
# Disable building this make file
#

burgermaklnx.mak: ;

#
# Code below can only be invoked indirectly
#

ifneq (0,$(MAKELEVEL))

#
# Required environment variables
#

ifndef BURGER_SDKS
$(error the environment variable BURGER_SDKS was not declared)
endif

#
# Default configuration
#

CONFIG ?= Release

#
# Default target
#

TARGET ?= lnx

#
# Directory name fragments
#

TARGET_SUFFIX_lnx := lnx

CONFIG_SUFFIX_Debug := dbg
CONFIG_SUFFIX_Internal := int
CONFIG_SUFFIX_Release := rel

#
# Base name of the temp directory
#

BASE_SUFFIX := mak$(TARGET_SUFFIX_$(TARGET))$(CONFIG_SUFFIX_$(CONFIG))
TEMP_DIR := temp/burger$(BASE_SUFFIX)

#
# SOURCE_DIRS = Work directories for the source code
#

SOURCE_DIRS :=../source
SOURCE_DIRS +=../source/ansi
SOURCE_DIRS +=../source/audio
SOURCE_DIRS +=../source/commandline
SOURCE_DIRS +=../source/compression
SOURCE_DIRS +=../source/file
SOURCE_DIRS +=../source/flashplayer
SOURCE_DIRS +=../source/generated
SOURCE_DIRS +=../source/graphics
SOURCE_DIRS +=../source/graphics/effects
SOURCE_DIRS +=../source/graphics/shaders
SOURCE_DIRS +=../source/graphics/shadersopengl/generated
SOURCE_DIRS +=../source/input
SOURCE_DIRS +=../source/linux
SOURCE_DIRS +=../source/lowlevel
SOURCE_DIRS +=../source/math
SOURCE_DIRS +=../source/memory
SOURCE_DIRS +=../source/network
SOURCE_DIRS +=../source/random
SOURCE_DIRS +=../source/text

#
# INCLUDE_DIRS = Header includes
#

INCLUDE_DIRS = $(SOURCE_DIRS) $(BURGER_SDKS)/linux/burgerlib

#
# Set the compiler flags for each of the build types
#

CFlagsDebuglnx:= -g -Og -D_DEBUG -D__LINUX__
CFlagsInternallnx:= -g -O3 -D_DEBUG -D__LINUX__
CFlagsReleaselnx:= -O3 -DNDEBUG -D__LINUX__

#
# Set the WASM flags for each of the build types
#

AFlagsDebuglnx:= -g -D_DEBUG -D__LINUX__
AFlagsInternallnx:= -g -D_DEBUG -D__LINUX__
AFlagsReleaselnx:= -DNDEBUG -D__LINUX__

#
# Set the Linker flags for each of the build types
#

LFlagsDebuglnx:= -g
LFlagsInternallnx:= -g
LFlagsReleaselnx:=

# Now, set the compiler flags

C_INCLUDES:=$(addprefix -I,$(INCLUDE_DIRS))
CL:=$(CXX) -c -Wall -x c++ $(C_INCLUDES)
CP:=$(CXX) -c -Wall -x c++ $(C_INCLUDES)
ASM:=$(AS)
LINK:=$(CXX)

#
# Default build recipes
#

define BUILD_CPP=
@echo $(<F) / $(CONFIG) / $(TARGET); \
$(CP) $(CFlags$(CONFIG)$(TARGET)) $< -o $@ -MT '$@' -MMD -MF '$*.d'
endef

#
# Object files to work with for the library
#

OBJS:= $(TEMP_DIR)/bradler16.o \
	$(TEMP_DIR)/bradler32.o \
	$(TEMP_DIR)/bralaw.o \
	$(TEMP_DIR)/bralgorithm.o \
	$(TEMP_DIR)/brarray.o \
	$(TEMP_DIR)/brasciito.o \
	$(TEMP_DIR)/brassert.o \
	$(TEMP_DIR)/bratomic.o \
	$(TEMP_DIR)/brautorepeat.o \
	$(TEMP_DIR)/brbase.o \
	$(TEMP_DIR)/brcapturestdout.o \
	$(TEMP_DIR)/brcodelibrary.o \
	$(TEMP_DIR)/brcommandparameter.o \
	$(TEMP_DIR)/brcommandparameterbooltrue.o \
	$(TEMP_DIR)/brcommandparameterstring.o \
	$(TEMP_DIR)/brcommandparameterwordptr.o \
	$(TEMP_DIR)/brcompress.o \
	$(TEMP_DIR)/brcompressdeflate.o \
	$(TEMP_DIR)/brcompresslbmrle.o \
	$(TEMP_DIR)/brcompresslzss.o \
	$(TEMP_DIR)/brconsolemanager.o \
	$(TEMP_DIR)/brcrc16.o \
	$(TEMP_DIR)/brcrc32.o \
	$(TEMP_DIR)/brcriticalsection.o \
	$(TEMP_DIR)/brdebug.o \
	$(TEMP_DIR)/brdecompress.o \
	$(TEMP_DIR)/brdecompressdeflate.o \
	$(TEMP_DIR)/brdecompresslbmrle.o \
	$(TEMP_DIR)/brdecompresslzss.o \
	$(TEMP_DIR)/brdetectmultilaunch.o \
	$(TEMP_DIR)/brdirectorysearch.o \
	$(TEMP_DIR)/brdirectorysearchlinux.o \
	$(TEMP_DIR)/brdisplay.o \
	$(TEMP_DIR)/brdisplayobject.o \
	$(TEMP_DIR)/brdisplayopengl.o \
	$(TEMP_DIR)/brdisplayopenglsoftware8.o \
	$(TEMP_DIR)/brdjb2hash.o \
	$(TEMP_DIR)/brdoublylinkedlist.o \
	$(TEMP_DIR)/brdxt1.o \
	$(TEMP_DIR)/brdxt3.o \
	$(TEMP_DIR)/brdxt5.o \
	$(TEMP_DIR)/brearcliptriangulate.o \
	$(TEMP_DIR)/breffect.o \
	$(TEMP_DIR)/breffect2d.o \
	$(TEMP_DIR)/breffectpositiontexturecolor.o \
	$(TEMP_DIR)/breffectstaticpositiontexture.o \
	$(TEMP_DIR)/brendian.o \
	$(TEMP_DIR)/brerror.o \
	$(TEMP_DIR)/brfile.o \
	$(TEMP_DIR)/brfileansihelpers.o \
	$(TEMP_DIR)/brfileapf.o \
	$(TEMP_DIR)/brfilebmp.o \
	$(TEMP_DIR)/brfiledds.o \
	$(TEMP_DIR)/brfilegif.o \
	$(TEMP_DIR)/brfileini.o \
	$(TEMP_DIR)/brfilelbm.o \
	$(TEMP_DIR)/brfilemanager.o \
	$(TEMP_DIR)/brfilemp3.o \
	$(TEMP_DIR)/brfilename.o \
	$(TEMP_DIR)/brfilepcx.o \
	$(TEMP_DIR)/brfilepng.o \
	$(TEMP_DIR)/brfiletga.o \
	$(TEMP_DIR)/brfilexml.o \
	$(TEMP_DIR)/brfixedmatrix3d.o \
	$(TEMP_DIR)/brfixedmatrix4d.o \
	$(TEMP_DIR)/brfixedpoint.o \
	$(TEMP_DIR)/brfixedvector2d.o \
	$(TEMP_DIR)/brfixedvector3d.o \
	$(TEMP_DIR)/brfixedvector4d.o \
	$(TEMP_DIR)/brflashaction.o \
	$(TEMP_DIR)/brflashactionvalue.o \
	$(TEMP_DIR)/brflashavm2.o \
	$(TEMP_DIR)/brflashcolortransform.o \
	$(TEMP_DIR)/brflashdisasmactionscript.o \
	$(TEMP_DIR)/brflashmanager.o \
	$(TEMP_DIR)/brflashmatrix23.o \
	$(TEMP_DIR)/brflashrect.o \
	$(TEMP_DIR)/brflashrgba.o \
	$(TEMP_DIR)/brflashstream.o \
	$(TEMP_DIR)/brflashutils.o \
	$(TEMP_DIR)/brfloatingpoint.o \
	$(TEMP_DIR)/brfont.o \
	$(TEMP_DIR)/brfont4bit.o \
	$(TEMP_DIR)/brfpinfo.o \
	$(TEMP_DIR)/brgameapp.o \
	$(TEMP_DIR)/brglobalmemorymanager.o \
	$(TEMP_DIR)/brglobals.o \
	$(TEMP_DIR)/brglobalslinux.o \
	$(TEMP_DIR)/brgost.o \
	$(TEMP_DIR)/brgridindex.o \
	$(TEMP_DIR)/brguid.o \
	$(TEMP_DIR)/brhashmap.o \
	$(TEMP_DIR)/brimage.o \
	$(TEMP_DIR)/brimportit.o \
	$(TEMP_DIR)/brimports3m.o \
	$(TEMP_DIR)/brimportxm.o \
	$(TEMP_DIR)/brinputmemorystream.o \
	$(TEMP_DIR)/brintrinsics.o \
	$(TEMP_DIR)/brisolatin1.o \
	$(TEMP_DIR)/brjoypad.o \
	$(TEMP_DIR)/brkeyboard.o \
	$(TEMP_DIR)/brlastinfirstout.o \
	$(TEMP_DIR)/brlinkedlistobject.o \
	$(TEMP_DIR)/brlinkedlistpointer.o \
	$(TEMP_DIR)/brlocalization.o \
	$(TEMP_DIR)/brmace.o \
	$(TEMP_DIR)/brmacromanus.o \
	$(TEMP_DIR)/brmatrix3d.o \
	$(TEMP_DIR)/brmatrix4d.o \
	$(TEMP_DIR)/brmd2.o \
	$(TEMP_DIR)/brmd4.o \
	$(TEMP_DIR)/brmd5.o \
	$(TEMP_DIR)/brmemoryansi.o \
	$(TEMP_DIR)/brmemoryfunctions.o \
	$(TEMP_DIR)/brmemoryhandle.o \
	$(TEMP_DIR)/brmemorymanager.o \
	$(TEMP_DIR)/brmersennetwist.o \
	$(TEMP_DIR)/brmicrosoftadpcm.o \
	$(TEMP_DIR)/brmouse.o \
	$(TEMP_DIR)/brmp3.o \
	$(TEMP_DIR)/brmp3math.o \
	$(TEMP_DIR)/brnetendpoint.o \
	$(TEMP_DIR)/brnetendpointtcp.o \
	$(TEMP_DIR)/brnetmanager.o \
	$(TEMP_DIR)/brnetmodule.o \
	$(TEMP_DIR)/brnetmoduletcp.o \
	$(TEMP_DIR)/brnetplay.o \
	$(TEMP_DIR)/brnumberstring.o \
	$(TEMP_DIR)/brnumberstringhex.o \
	$(TEMP_DIR)/brnumberto.o \
	$(TEMP_DIR)/broscursor.o \
	$(TEMP_DIR)/brosstringfunctions.o \
	$(TEMP_DIR)/brosstringfunctionslinux.o \
	$(TEMP_DIR)/broutputmemorystream.o \
	$(TEMP_DIR)/brpackfloat.o \
	$(TEMP_DIR)/brpair.o \
	$(TEMP_DIR)/brpalette.o \
	$(TEMP_DIR)/brperforce.o \
	$(TEMP_DIR)/brpoint2d.o \
	$(TEMP_DIR)/brprintf.o \
	$(TEMP_DIR)/brqueue.o \
	$(TEMP_DIR)/brrandom.o \
	$(TEMP_DIR)/brrandombase.o \
	$(TEMP_DIR)/brrect.o \
	$(TEMP_DIR)/brrenderer.o \
	$(TEMP_DIR)/brrenderersoftware16.o \
	$(TEMP_DIR)/brrenderersoftware8.o \
	$(TEMP_DIR)/brrezfile.o \
	$(TEMP_DIR)/brrunqueue.o \
	$(TEMP_DIR)/brsdbmhash.o \
	$(TEMP_DIR)/brsequencer.o \
	$(TEMP_DIR)/brsha1.o \
	$(TEMP_DIR)/brshader2dccolor.o \
	$(TEMP_DIR)/brshader2dcolorvertex.o \
	$(TEMP_DIR)/brshaders.o \
	$(TEMP_DIR)/brshape8bit.o \
	$(TEMP_DIR)/brsimplearray.o \
	$(TEMP_DIR)/brsimplestring.o \
	$(TEMP_DIR)/brsinecosine.o \
	$(TEMP_DIR)/brsmartpointer.o \
	$(TEMP_DIR)/brsound.o \
	$(TEMP_DIR)/brsoundbufferrez.o \
	$(TEMP_DIR)/brsounddecompress.o \
	$(TEMP_DIR)/brstaticrtti.o \
	$(TEMP_DIR)/brstdouthelpers.o \
	$(TEMP_DIR)/brstring.o \
	$(TEMP_DIR)/brstring16.o \
	$(TEMP_DIR)/brstringfunctions.o \
	$(TEMP_DIR)/brtexture.o \
	$(TEMP_DIR)/brtextureopengl.o \
	$(TEMP_DIR)/brtick.o \
	$(TEMP_DIR)/brtimedate.o \
	$(TEMP_DIR)/brtypes.o \
	$(TEMP_DIR)/brulaw.o \
	$(TEMP_DIR)/brunpackbytes.o \
	$(TEMP_DIR)/brutf16.o \
	$(TEMP_DIR)/brutf32.o \
	$(TEMP_DIR)/brutf8.o \
	$(TEMP_DIR)/brvector2d.o \
	$(TEMP_DIR)/brvector3d.o \
	$(TEMP_DIR)/brvector4d.o \
	$(TEMP_DIR)/brvertexbuffer.o \
	$(TEMP_DIR)/brvertexbufferopengl.o \
	$(TEMP_DIR)/brwin1252.o

DEPS:= $(TEMP_DIR)/bradler16.d \
	$(TEMP_DIR)/bradler32.d \
	$(TEMP_DIR)/bralaw.d \
	$(TEMP_DIR)/bralgorithm.d \
	$(TEMP_DIR)/brarray.d \
	$(TEMP_DIR)/brasciito.d \
	$(TEMP_DIR)/brassert.d \
	$(TEMP_DIR)/bratomic.d \
	$(TEMP_DIR)/brautorepeat.d \
	$(TEMP_DIR)/brbase.d \
	$(TEMP_DIR)/brcapturestdout.d \
	$(TEMP_DIR)/brcodelibrary.d \
	$(TEMP_DIR)/brcommandparameter.d \
	$(TEMP_DIR)/brcommandparameterbooltrue.d \
	$(TEMP_DIR)/brcommandparameterstring.d \
	$(TEMP_DIR)/brcommandparameterwordptr.d \
	$(TEMP_DIR)/brcompress.d \
	$(TEMP_DIR)/brcompressdeflate.d \
	$(TEMP_DIR)/brcompresslbmrle.d \
	$(TEMP_DIR)/brcompresslzss.d \
	$(TEMP_DIR)/brconsolemanager.d \
	$(TEMP_DIR)/brcrc16.d \
	$(TEMP_DIR)/brcrc32.d \
	$(TEMP_DIR)/brcriticalsection.d \
	$(TEMP_DIR)/brdebug.d \
	$(TEMP_DIR)/brdecompress.d \
	$(TEMP_DIR)/brdecompressdeflate.d \
	$(TEMP_DIR)/brdecompresslbmrle.d \
	$(TEMP_DIR)/brdecompresslzss.d \
	$(TEMP_DIR)/brdetectmultilaunch.d \
	$(TEMP_DIR)/brdirectorysearch.d \
	$(TEMP_DIR)/brdirectorysearchlinux.d \
	$(TEMP_DIR)/brdisplay.d \
	$(TEMP_DIR)/brdisplayobject.d \
	$(TEMP_DIR)/brdisplayopengl.d \
	$(TEMP_DIR)/brdisplayopenglsoftware8.d \
	$(TEMP_DIR)/brdjb2hash.d \
	$(TEMP_DIR)/brdoublylinkedlist.d \
	$(TEMP_DIR)/brdxt1.d \
	$(TEMP_DIR)/brdxt3.d \
	$(TEMP_DIR)/brdxt5.d \
	$(TEMP_DIR)/brearcliptriangulate.d \
	$(TEMP_DIR)/breffect.d \
	$(TEMP_DIR)/breffect2d.d \
	$(TEMP_DIR)/breffectpositiontexturecolor.d \
	$(TEMP_DIR)/breffectstaticpositiontexture.d \
	$(TEMP_DIR)/brendian.d \
	$(TEMP_DIR)/brerror.d \
	$(TEMP_DIR)/brfile.d \
	$(TEMP_DIR)/brfileansihelpers.d \
	$(TEMP_DIR)/brfileapf.d \
	$(TEMP_DIR)/brfilebmp.d \
	$(TEMP_DIR)/brfiledds.d \
	$(TEMP_DIR)/brfilegif.d \
	$(TEMP_DIR)/brfileini.d \
	$(TEMP_DIR)/brfilelbm.d \
	$(TEMP_DIR)/brfilemanager.d \
	$(TEMP_DIR)/brfilemp3.d \
	$(TEMP_DIR)/brfilename.d \
	$(TEMP_DIR)/brfilepcx.d \
	$(TEMP_DIR)/brfilepng.d \
	$(TEMP_DIR)/brfiletga.d \
	$(TEMP_DIR)/brfilexml.d \
	$(TEMP_DIR)/brfixedmatrix3d.d \
	$(TEMP_DIR)/brfixedmatrix4d.d \
	$(TEMP_DIR)/brfixedpoint.d \
	$(TEMP_DIR)/brfixedvector2d.d \
	$(TEMP_DIR)/brfixedvector3d.d \
	$(TEMP_DIR)/brfixedvector4d.d \
	$(TEMP_DIR)/brflashaction.d \
	$(TEMP_DIR)/brflashactionvalue.d \
	$(TEMP_DIR)/brflashavm2.d \
	$(TEMP_DIR)/brflashcolortransform.d \
	$(TEMP_DIR)/brflashdisasmactionscript.d \
	$(TEMP_DIR)/brflashmanager.d \
	$(TEMP_DIR)/brflashmatrix23.d \
	$(TEMP_DIR)/brflashrect.d \
	$(TEMP_DIR)/brflashrgba.d \
	$(TEMP_DIR)/brflashstream.d \
	$(TEMP_DIR)/brflashutils.d \
	$(TEMP_DIR)/brfloatingpoint.d \
	$(TEMP_DIR)/brfont.d \
	$(TEMP_DIR)/brfont4bit.d \
	$(TEMP_DIR)/brfpinfo.d \
	$(TEMP_DIR)/brgameapp.d \
	$(TEMP_DIR)/brglobalmemorymanager.d \
	$(TEMP_DIR)/brglobals.d \
	$(TEMP_DIR)/brglobalslinux.d \
	$(TEMP_DIR)/brgost.d \
	$(TEMP_DIR)/brgridindex.d \
	$(TEMP_DIR)/brguid.d \
	$(TEMP_DIR)/brhashmap.d \
	$(TEMP_DIR)/brimage.d \
	$(TEMP_DIR)/brimportit.d \
	$(TEMP_DIR)/brimports3m.d \
	$(TEMP_DIR)/brimportxm.d \
	$(TEMP_DIR)/brinputmemorystream.d \
	$(TEMP_DIR)/brintrinsics.d \
	$(TEMP_DIR)/brisolatin1.d \
	$(TEMP_DIR)/brjoypad.d \
	$(TEMP_DIR)/brkeyboard.d \
	$(TEMP_DIR)/brlastinfirstout.d \
	$(TEMP_DIR)/brlinkedlistobject.d \
	$(TEMP_DIR)/brlinkedlistpointer.d \
	$(TEMP_DIR)/brlocalization.d \
	$(TEMP_DIR)/brmace.d \
	$(TEMP_DIR)/brmacromanus.d \
	$(TEMP_DIR)/brmatrix3d.d \
	$(TEMP_DIR)/brmatrix4d.d \
	$(TEMP_DIR)/brmd2.d \
	$(TEMP_DIR)/brmd4.d \
	$(TEMP_DIR)/brmd5.d \
	$(TEMP_DIR)/brmemoryansi.d \
	$(TEMP_DIR)/brmemoryfunctions.d \
	$(TEMP_DIR)/brmemoryhandle.d \
	$(TEMP_DIR)/brmemorymanager.d \
	$(TEMP_DIR)/brmersennetwist.d \
	$(TEMP_DIR)/brmicrosoftadpcm.d \
	$(TEMP_DIR)/brmouse.d \
	$(TEMP_DIR)/brmp3.d \
	$(TEMP_DIR)/brmp3math.d \
	$(TEMP_DIR)/brnetendpoint.d \
	$(TEMP_DIR)/brnetendpointtcp.d \
	$(TEMP_DIR)/brnetmanager.d \
	$(TEMP_DIR)/brnetmodule.d \
	$(TEMP_DIR)/brnetmoduletcp.d \
	$(TEMP_DIR)/brnetplay.d \
	$(TEMP_DIR)/brnumberstring.d \
	$(TEMP_DIR)/brnumberstringhex.d \
	$(TEMP_DIR)/brnumberto.d \
	$(TEMP_DIR)/broscursor.d \
	$(TEMP_DIR)/brosstringfunctions.d \
	$(TEMP_DIR)/brosstringfunctionslinux.d \
	$(TEMP_DIR)/broutputmemorystream.d \
	$(TEMP_DIR)/brpackfloat.d \
	$(TEMP_DIR)/brpair.d \
	$(TEMP_DIR)/brpalette.d \
	$(TEMP_DIR)/brperforce.d \
	$(TEMP_DIR)/brpoint2d.d \
	$(TEMP_DIR)/brprintf.d \
	$(TEMP_DIR)/brqueue.d \
	$(TEMP_DIR)/brrandom.d \
	$(TEMP_DIR)/brrandombase.d \
	$(TEMP_DIR)/brrect.d \
	$(TEMP_DIR)/brrenderer.d \
	$(TEMP_DIR)/brrenderersoftware16.d \
	$(TEMP_DIR)/brrenderersoftware8.d \
	$(TEMP_DIR)/brrezfile.d \
	$(TEMP_DIR)/brrunqueue.d \
	$(TEMP_DIR)/brsdbmhash.d \
	$(TEMP_DIR)/brsequencer.d \
	$(TEMP_DIR)/brsha1.d \
	$(TEMP_DIR)/brshader2dccolor.d \
	$(TEMP_DIR)/brshader2dcolorvertex.d \
	$(TEMP_DIR)/brshaders.d \
	$(TEMP_DIR)/brshape8bit.d \
	$(TEMP_DIR)/brsimplearray.d \
	$(TEMP_DIR)/brsimplestring.d \
	$(TEMP_DIR)/brsinecosine.d \
	$(TEMP_DIR)/brsmartpointer.d \
	$(TEMP_DIR)/brsound.d \
	$(TEMP_DIR)/brsoundbufferrez.d \
	$(TEMP_DIR)/brsounddecompress.d \
	$(TEMP_DIR)/brstaticrtti.d \
	$(TEMP_DIR)/brstdouthelpers.d \
	$(TEMP_DIR)/brstring.d \
	$(TEMP_DIR)/brstring16.d \
	$(TEMP_DIR)/brstringfunctions.d \
	$(TEMP_DIR)/brtexture.d \
	$(TEMP_DIR)/brtextureopengl.d \
	$(TEMP_DIR)/brtick.d \
	$(TEMP_DIR)/brtimedate.d \
	$(TEMP_DIR)/brtypes.d \
	$(TEMP_DIR)/brulaw.d \
	$(TEMP_DIR)/brunpackbytes.d \
	$(TEMP_DIR)/brutf16.d \
	$(TEMP_DIR)/brutf32.d \
	$(TEMP_DIR)/brutf8.d \
	$(TEMP_DIR)/brvector2d.d \
	$(TEMP_DIR)/brvector3d.d \
	$(TEMP_DIR)/brvector4d.d \
	$(TEMP_DIR)/brvertexbuffer.d \
	$(TEMP_DIR)/brvertexbufferopengl.d \
	$(TEMP_DIR)/brwin1252.d

#
# Disable building the source files
#

../source/ansi/brcapturestdout.cpp ../source/ansi/brfileansihelpers.cpp ../source/ansi/brmemoryansi.cpp ../source/ansi/brstdouthelpers.cpp ../source/audio/bralaw.cpp ../source/audio/brimportit.cpp ../source/audio/brimports3m.cpp ../source/audio/brimportxm.cpp ../source/audio/brmace.cpp ../source/audio/brmicrosoftadpcm.cpp ../source/audio/brmp3.cpp ../source/audio/brsequencer.cpp ../source/audio/brsound.cpp ../source/audio/brsoundbufferrez.cpp ../source/audio/brsounddecompress.cpp ../source/audio/brulaw.cpp ../source/commandline/brcommandparameter.cpp ../source/commandline/brcommandparameterbooltrue.cpp ../source/commandline/brcommandparameterstring.cpp ../source/commandline/brcommandparameterwordptr.cpp ../source/commandline/brconsolemanager.cpp ../source/commandline/brgameapp.cpp ../source/compression/bradler16.cpp ../source/compression/bradler32.cpp ../source/compression/brcompress.cpp ../source/compression/brcompressdeflate.cpp ../source/compression/brcompresslbmrle.cpp ../source/compression/brcompresslzss.cpp ../source/compression/brcrc16.cpp ../source/compression/brcrc32.cpp ../source/compression/brdecompress.cpp ../source/compression/brdecompressdeflate.cpp ../source/compression/brdecompresslbmrle.cpp ../source/compression/brdecompresslzss.cpp ../source/compression/brdjb2hash.cpp ../source/compression/brdxt1.cpp ../source/compression/brdxt3.cpp ../source/compression/brdxt5.cpp ../source/compression/brgost.cpp ../source/compression/brhashmap.cpp ../source/compression/brmd2.cpp ../source/compression/brmd4.cpp ../source/compression/brmd5.cpp ../source/compression/brpackfloat.cpp ../source/compression/brsdbmhash.cpp ../source/compression/brsha1.cpp ../source/compression/brunpackbytes.cpp ../source/file/brdirectorysearch.cpp ../source/file/brfile.cpp ../source/file/brfileapf.cpp ../source/file/brfilebmp.cpp ../source/file/brfiledds.cpp ../source/file/brfilegif.cpp ../source/file/brfileini.cpp ../source/file/brfilelbm.cpp ../source/file/brfilemanager.cpp ../source/file/brfilemp3.cpp ../source/file/brfilename.cpp ../source/file/brfilepcx.cpp ../source/file/brfilepng.cpp ../source/file/brfiletga.cpp ../source/file/brfilexml.cpp ../source/file/brinputmemorystream.cpp ../source/file/broutputmemorystream.cpp ../source/file/brrezfile.cpp ../source/flashplayer/brflashaction.cpp ../source/flashplayer/brflashactionvalue.cpp ../source/flashplayer/brflashavm2.cpp ../source/flashplayer/brflashcolortransform.cpp ../source/flashplayer/brflashdisasmactionscript.cpp ../source/flashplayer/brflashmanager.cpp ../source/flashplayer/brflashmatrix23.cpp ../source/flashplayer/brflashrect.cpp ../source/flashplayer/brflashrgba.cpp ../source/flashplayer/brflashstream.cpp ../source/flashplayer/brflashutils.cpp ../source/graphics/brdisplay.cpp ../source/graphics/brdisplayobject.cpp ../source/graphics/brdisplayopengl.cpp ../source/graphics/brdisplayopenglsoftware8.cpp ../source/graphics/brfont.cpp ../source/graphics/brfont4bit.cpp ../source/graphics/brimage.cpp ../source/graphics/brpalette.cpp ../source/graphics/brpoint2d.cpp ../source/graphics/brrect.cpp ../source/graphics/brrenderer.cpp ../source/graphics/brrenderersoftware16.cpp ../source/graphics/brrenderersoftware8.cpp ../source/graphics/brshape8bit.cpp ../source/graphics/brtexture.cpp ../source/graphics/brtextureopengl.cpp ../source/graphics/brvertexbuffer.cpp ../source/graphics/brvertexbufferopengl.cpp ../source/graphics/effects/breffect.cpp ../source/graphics/effects/breffect2d.cpp ../source/graphics/effects/breffectpositiontexturecolor.cpp ../source/graphics/effects/breffectstaticpositiontexture.cpp ../source/graphics/effects/brshaders.cpp ../source/graphics/shaders/brshader2dccolor.cpp ../source/graphics/shaders/brshader2dcolorvertex.cpp ../source/input/brjoypad.cpp ../source/input/brkeyboard.cpp ../source/input/brmouse.cpp ../source/linux/brdirectorysearchlinux.cpp ../source/linux/brglobalslinux.cpp ../source/linux/brosstringfunctionslinux.cpp ../source/lowlevel/bralgorithm.cpp ../source/lowlevel/brarray.cpp ../source/lowlevel/brassert.cpp ../source/lowlevel/bratomic.cpp ../source/lowlevel/brautorepeat.cpp ../source/lowlevel/brbase.cpp ../source/lowlevel/brcodelibrary.cpp ../source/lowlevel/brcriticalsection.cpp ../source/lowlevel/brdebug.cpp ../source/lowlevel/brdetectmultilaunch.cpp ../source/lowlevel/brdoublylinkedlist.cpp ../source/lowlevel/brendian.cpp ../source/lowlevel/brerror.cpp ../source/lowlevel/brglobals.cpp ../source/lowlevel/brgridindex.cpp ../source/lowlevel/brguid.cpp ../source/lowlevel/brintrinsics.cpp ../source/lowlevel/brlastinfirstout.cpp ../source/lowlevel/brlinkedlistobject.cpp ../source/lowlevel/brlinkedlistpointer.cpp ../source/lowlevel/broscursor.cpp ../source/lowlevel/brpair.cpp ../source/lowlevel/brperforce.cpp ../source/lowlevel/brqueue.cpp ../source/lowlevel/brrunqueue.cpp ../source/lowlevel/brsimplearray.cpp ../source/lowlevel/brsmartpointer.cpp ../source/lowlevel/brstaticrtti.cpp ../source/lowlevel/brtick.cpp ../source/lowlevel/brtimedate.cpp ../source/lowlevel/brtypes.cpp ../source/math/brearcliptriangulate.cpp ../source/math/brfixedmatrix3d.cpp ../source/math/brfixedmatrix4d.cpp ../source/math/brfixedpoint.cpp ../source/math/brfixedvector2d.cpp ../source/math/brfixedvector3d.cpp ../source/math/brfixedvector4d.cpp ../source/math/brfloatingpoint.cpp ../source/math/brfpinfo.cpp ../source/math/brmatrix3d.cpp ../source/math/brmatrix4d.cpp ../source/math/brmp3math.cpp ../source/math/brsinecosine.cpp ../source/math/brvector2d.cpp ../source/math/brvector3d.cpp ../source/math/brvector4d.cpp ../source/memory/brglobalmemorymanager.cpp ../source/memory/brmemoryhandle.cpp ../source/memory/brmemorymanager.cpp ../source/network/brnetendpoint.cpp ../source/network/brnetendpointtcp.cpp ../source/network/brnetmanager.cpp ../source/network/brnetmodule.cpp ../source/network/brnetmoduletcp.cpp ../source/network/brnetplay.cpp ../source/random/brmersennetwist.cpp ../source/random/brrandom.cpp ../source/random/brrandombase.cpp ../source/text/brasciito.cpp ../source/text/brisolatin1.cpp ../source/text/brlocalization.cpp ../source/text/brmacromanus.cpp ../source/text/brmemoryfunctions.cpp ../source/text/brnumberstring.cpp ../source/text/brnumberstringhex.cpp ../source/text/brnumberto.cpp ../source/text/brosstringfunctions.cpp ../source/text/brprintf.cpp ../source/text/brsimplestring.cpp ../source/text/brstring.cpp ../source/text/brstring16.cpp ../source/text/brstringfunctions.cpp ../source/text/brutf16.cpp ../source/text/brutf32.cpp ../source/text/brutf8.cpp ../source/text/brwin1252.cpp: ;

#
# Build the object file folder
#

$(OBJS): | $(TEMP_DIR)

$(TEMP_DIR):
	@-mkdir -p $(TEMP_DIR)

#
# Build the object files
#

$(TEMP_DIR)/brcapturestdout.o: ../source/ansi/brcapturestdout.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfileansihelpers.o: ../source/ansi/brfileansihelpers.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brmemoryansi.o: ../source/ansi/brmemoryansi.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brstdouthelpers.o: ../source/ansi/brstdouthelpers.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/bralaw.o: ../source/audio/bralaw.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brimportit.o: ../source/audio/brimportit.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brimports3m.o: ../source/audio/brimports3m.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brimportxm.o: ../source/audio/brimportxm.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brmace.o: ../source/audio/brmace.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brmicrosoftadpcm.o: ../source/audio/brmicrosoftadpcm.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brmp3.o: ../source/audio/brmp3.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brsequencer.o: ../source/audio/brsequencer.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brsound.o: ../source/audio/brsound.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brsoundbufferrez.o: ../source/audio/brsoundbufferrez.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brsounddecompress.o: ../source/audio/brsounddecompress.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brulaw.o: ../source/audio/brulaw.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brcommandparameter.o: ../source/commandline/brcommandparameter.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brcommandparameterbooltrue.o: ../source/commandline/brcommandparameterbooltrue.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brcommandparameterstring.o: ../source/commandline/brcommandparameterstring.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brcommandparameterwordptr.o: ../source/commandline/brcommandparameterwordptr.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brconsolemanager.o: ../source/commandline/brconsolemanager.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brgameapp.o: ../source/commandline/brgameapp.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/bradler16.o: ../source/compression/bradler16.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/bradler32.o: ../source/compression/bradler32.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brcompress.o: ../source/compression/brcompress.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brcompressdeflate.o: ../source/compression/brcompressdeflate.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brcompresslbmrle.o: ../source/compression/brcompresslbmrle.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brcompresslzss.o: ../source/compression/brcompresslzss.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brcrc16.o: ../source/compression/brcrc16.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brcrc32.o: ../source/compression/brcrc32.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdecompress.o: ../source/compression/brdecompress.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdecompressdeflate.o: ../source/compression/brdecompressdeflate.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdecompresslbmrle.o: ../source/compression/brdecompresslbmrle.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdecompresslzss.o: ../source/compression/brdecompresslzss.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdjb2hash.o: ../source/compression/brdjb2hash.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdxt1.o: ../source/compression/brdxt1.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdxt3.o: ../source/compression/brdxt3.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdxt5.o: ../source/compression/brdxt5.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brgost.o: ../source/compression/brgost.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brhashmap.o: ../source/compression/brhashmap.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brmd2.o: ../source/compression/brmd2.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brmd4.o: ../source/compression/brmd4.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brmd5.o: ../source/compression/brmd5.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brpackfloat.o: ../source/compression/brpackfloat.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brsdbmhash.o: ../source/compression/brsdbmhash.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brsha1.o: ../source/compression/brsha1.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brunpackbytes.o: ../source/compression/brunpackbytes.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdirectorysearch.o: ../source/file/brdirectorysearch.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfile.o: ../source/file/brfile.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfileapf.o: ../source/file/brfileapf.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfilebmp.o: ../source/file/brfilebmp.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfiledds.o: ../source/file/brfiledds.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfilegif.o: ../source/file/brfilegif.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfileini.o: ../source/file/brfileini.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfilelbm.o: ../source/file/brfilelbm.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfilemanager.o: ../source/file/brfilemanager.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfilemp3.o: ../source/file/brfilemp3.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfilename.o: ../source/file/brfilename.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfilepcx.o: ../source/file/brfilepcx.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfilepng.o: ../source/file/brfilepng.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfiletga.o: ../source/file/brfiletga.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfilexml.o: ../source/file/brfilexml.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brinputmemorystream.o: ../source/file/brinputmemorystream.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/broutputmemorystream.o: ../source/file/broutputmemorystream.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brrezfile.o: ../source/file/brrezfile.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brflashaction.o: ../source/flashplayer/brflashaction.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brflashactionvalue.o: ../source/flashplayer/brflashactionvalue.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brflashavm2.o: ../source/flashplayer/brflashavm2.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brflashcolortransform.o: ../source/flashplayer/brflashcolortransform.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brflashdisasmactionscript.o: ../source/flashplayer/brflashdisasmactionscript.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brflashmanager.o: ../source/flashplayer/brflashmanager.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brflashmatrix23.o: ../source/flashplayer/brflashmatrix23.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brflashrect.o: ../source/flashplayer/brflashrect.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brflashrgba.o: ../source/flashplayer/brflashrgba.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brflashstream.o: ../source/flashplayer/brflashstream.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brflashutils.o: ../source/flashplayer/brflashutils.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdisplay.o: ../source/graphics/brdisplay.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdisplayobject.o: ../source/graphics/brdisplayobject.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdisplayopengl.o: ../source/graphics/brdisplayopengl.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdisplayopenglsoftware8.o: ../source/graphics/brdisplayopenglsoftware8.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfont.o: ../source/graphics/brfont.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfont4bit.o: ../source/graphics/brfont4bit.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brimage.o: ../source/graphics/brimage.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brpalette.o: ../source/graphics/brpalette.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brpoint2d.o: ../source/graphics/brpoint2d.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brrect.o: ../source/graphics/brrect.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brrenderer.o: ../source/graphics/brrenderer.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brrenderersoftware16.o: ../source/graphics/brrenderersoftware16.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brrenderersoftware8.o: ../source/graphics/brrenderersoftware8.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brshape8bit.o: ../source/graphics/brshape8bit.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brtexture.o: ../source/graphics/brtexture.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brtextureopengl.o: ../source/graphics/brtextureopengl.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brvertexbuffer.o: ../source/graphics/brvertexbuffer.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brvertexbufferopengl.o: ../source/graphics/brvertexbufferopengl.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/breffect.o: ../source/graphics/effects/breffect.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/breffect2d.o: ../source/graphics/effects/breffect2d.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/breffectpositiontexturecolor.o: ../source/graphics/effects/breffectpositiontexturecolor.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/breffectstaticpositiontexture.o: ../source/graphics/effects/breffectstaticpositiontexture.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brshaders.o: ../source/graphics/effects/brshaders.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brshader2dccolor.o: ../source/graphics/shaders/brshader2dccolor.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brshader2dcolorvertex.o: ../source/graphics/shaders/brshader2dcolorvertex.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brjoypad.o: ../source/input/brjoypad.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brkeyboard.o: ../source/input/brkeyboard.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brmouse.o: ../source/input/brmouse.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdirectorysearchlinux.o: ../source/linux/brdirectorysearchlinux.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brglobalslinux.o: ../source/linux/brglobalslinux.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brosstringfunctionslinux.o: ../source/linux/brosstringfunctionslinux.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/bralgorithm.o: ../source/lowlevel/bralgorithm.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brarray.o: ../source/lowlevel/brarray.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brassert.o: ../source/lowlevel/brassert.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/bratomic.o: ../source/lowlevel/bratomic.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brautorepeat.o: ../source/lowlevel/brautorepeat.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brbase.o: ../source/lowlevel/brbase.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brcodelibrary.o: ../source/lowlevel/brcodelibrary.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brcriticalsection.o: ../source/lowlevel/brcriticalsection.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdebug.o: ../source/lowlevel/brdebug.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdetectmultilaunch.o: ../source/lowlevel/brdetectmultilaunch.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brdoublylinkedlist.o: ../source/lowlevel/brdoublylinkedlist.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brendian.o: ../source/lowlevel/brendian.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brerror.o: ../source/lowlevel/brerror.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brglobals.o: ../source/lowlevel/brglobals.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brgridindex.o: ../source/lowlevel/brgridindex.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brguid.o: ../source/lowlevel/brguid.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brintrinsics.o: ../source/lowlevel/brintrinsics.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brlastinfirstout.o: ../source/lowlevel/brlastinfirstout.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brlinkedlistobject.o: ../source/lowlevel/brlinkedlistobject.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brlinkedlistpointer.o: ../source/lowlevel/brlinkedlistpointer.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/broscursor.o: ../source/lowlevel/broscursor.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brpair.o: ../source/lowlevel/brpair.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brperforce.o: ../source/lowlevel/brperforce.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brqueue.o: ../source/lowlevel/brqueue.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brrunqueue.o: ../source/lowlevel/brrunqueue.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brsimplearray.o: ../source/lowlevel/brsimplearray.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brsmartpointer.o: ../source/lowlevel/brsmartpointer.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brstaticrtti.o: ../source/lowlevel/brstaticrtti.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brtick.o: ../source/lowlevel/brtick.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brtimedate.o: ../source/lowlevel/brtimedate.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brtypes.o: ../source/lowlevel/brtypes.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brearcliptriangulate.o: ../source/math/brearcliptriangulate.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfixedmatrix3d.o: ../source/math/brfixedmatrix3d.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfixedmatrix4d.o: ../source/math/brfixedmatrix4d.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfixedpoint.o: ../source/math/brfixedpoint.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfixedvector2d.o: ../source/math/brfixedvector2d.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfixedvector3d.o: ../source/math/brfixedvector3d.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfixedvector4d.o: ../source/math/brfixedvector4d.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfloatingpoint.o: ../source/math/brfloatingpoint.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brfpinfo.o: ../source/math/brfpinfo.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brmatrix3d.o: ../source/math/brmatrix3d.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brmatrix4d.o: ../source/math/brmatrix4d.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brmp3math.o: ../source/math/brmp3math.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brsinecosine.o: ../source/math/brsinecosine.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brvector2d.o: ../source/math/brvector2d.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brvector3d.o: ../source/math/brvector3d.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brvector4d.o: ../source/math/brvector4d.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brglobalmemorymanager.o: ../source/memory/brglobalmemorymanager.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brmemoryhandle.o: ../source/memory/brmemoryhandle.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brmemorymanager.o: ../source/memory/brmemorymanager.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brnetendpoint.o: ../source/network/brnetendpoint.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brnetendpointtcp.o: ../source/network/brnetendpointtcp.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brnetmanager.o: ../source/network/brnetmanager.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brnetmodule.o: ../source/network/brnetmodule.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brnetmoduletcp.o: ../source/network/brnetmoduletcp.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brnetplay.o: ../source/network/brnetplay.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brmersennetwist.o: ../source/random/brmersennetwist.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brrandom.o: ../source/random/brrandom.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brrandombase.o: ../source/random/brrandombase.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brasciito.o: ../source/text/brasciito.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brisolatin1.o: ../source/text/brisolatin1.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brlocalization.o: ../source/text/brlocalization.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brmacromanus.o: ../source/text/brmacromanus.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brmemoryfunctions.o: ../source/text/brmemoryfunctions.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brnumberstring.o: ../source/text/brnumberstring.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brnumberstringhex.o: ../source/text/brnumberstringhex.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brnumberto.o: ../source/text/brnumberto.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brosstringfunctions.o: ../source/text/brosstringfunctions.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brprintf.o: ../source/text/brprintf.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brsimplestring.o: ../source/text/brsimplestring.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brstring.o: ../source/text/brstring.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brstring16.o: ../source/text/brstring16.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brstringfunctions.o: ../source/text/brstringfunctions.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brutf16.o: ../source/text/brutf16.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brutf32.o: ../source/text/brutf32.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brutf8.o: ../source/text/brutf8.cpp ; $(BUILD_CPP)

$(TEMP_DIR)/brwin1252.o: ../source/text/brwin1252.cpp ; $(BUILD_CPP)

#
# Create final binaries
#

bin/libburgermaklnxdbg.a: $(OBJS) burgermaklnx.mak | bin
	@ar -rcs $@ $(OBJS)
	@if [ -f /bin/wslpath ]; then \
	p4.exe edit $$(wslpath -a -w '$(BURGER_SDKS)/linux/burgerlib/$(@F)'); \
	cp -T "$@" "$(BURGER_SDKS)/linux/burgerlib/$(@F)"; \
	p4.exe revert -a $$(wslpath -a -w '$(BURGER_SDKS)/linux/burgerlib/$(@F)'); \
	else \
	p4 edit "$(BURGER_SDKS)/linux/burgerlib/$(@F)"; \
	cp -T "$@" "$(BURGER_SDKS)/linux/burgerlib/$(@F)"; \
	p4 revert -a "$(BURGER_SDKS)/linux/burgerlib/$(@F)"; \
	fi

bin/libburgermaklnxint.a: $(OBJS) burgermaklnx.mak | bin
	@ar -rcs $@ $(OBJS)
	@if [ -f /bin/wslpath ]; then \
	p4.exe edit $$(wslpath -a -w '$(BURGER_SDKS)/linux/burgerlib/$(@F)'); \
	cp -T "$@" "$(BURGER_SDKS)/linux/burgerlib/$(@F)"; \
	p4.exe revert -a $$(wslpath -a -w '$(BURGER_SDKS)/linux/burgerlib/$(@F)'); \
	else \
	p4 edit "$(BURGER_SDKS)/linux/burgerlib/$(@F)"; \
	cp -T "$@" "$(BURGER_SDKS)/linux/burgerlib/$(@F)"; \
	p4 revert -a "$(BURGER_SDKS)/linux/burgerlib/$(@F)"; \
	fi

bin/libburgermaklnxrel.a: $(OBJS) burgermaklnx.mak | bin
	@ar -rcs $@ $(OBJS)
	@if [ -f /bin/wslpath ]; then \
	p4.exe edit $$(wslpath -a -w '$(BURGER_SDKS)/linux/burgerlib/$(@F)'); \
	cp -T "$@" "$(BURGER_SDKS)/linux/burgerlib/$(@F)"; \
	p4.exe revert -a $$(wslpath -a -w '$(BURGER_SDKS)/linux/burgerlib/$(@F)'); \
	else \
	p4 edit "$(BURGER_SDKS)/linux/burgerlib/$(@F)"; \
	cp -T "$@" "$(BURGER_SDKS)/linux/burgerlib/$(@F)"; \
	p4 revert -a "$(BURGER_SDKS)/linux/burgerlib/$(@F)"; \
	fi

%.d: ;

%: %,v

%: RCS/%,v

%: RCS/%

%: s.%

%: SCCS/s.%

%.h: ;

#
# Include the generated dependencies
#

-include $(DEPS)

endif
