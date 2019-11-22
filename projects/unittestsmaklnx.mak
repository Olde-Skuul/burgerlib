#
# Build unittests with make
#

#
# Default configuration
#

ifndef $(CONFIG)
CONFIG = Release
endif

#
# Default target
#

ifndef $(TARGET)
TARGET = lnx
endif

#
# Directory name fragments
#

TARGET_SUFFIX_lnx = lnx

CONFIG_SUFFIX_Debug = dbg
CONFIG_SUFFIX_Internal = int
CONFIG_SUFFIX_Release = rel

#
# Set the set of known files supported
# Note: They are in the reverse order of building. .c is built first, then .x86
# until the .exe or .lib files are built
#

.SUFFIXES:
.SUFFIXES: .cpp .x86 .c .i86 .a .o .h

#
# SOURCE_DIRS = Work directories for the source code
#

SOURCE_DIRS =../source
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
SOURCE_DIRS +=../unittest

#
# Name of the output library
#

PROJECT_NAME = unittests

#
# Base name of the temp directory
#

BASE_TEMP_DIR = temp/$(PROJECT_NAME)
BASE_SUFFIX = mak$(TARGET_SUFFIX_$(TARGET))$(CONFIG_SUFFIX_$(CONFIG))
TEMP_DIR = $(BASE_TEMP_DIR)$(BASE_SUFFIX)

#
# Binary directory
#

DESTINATION_DIR = bin

#
# INCLUDE_DIRS = Header includes
#

INCLUDE_DIRS = $(SOURCE_DIRS) $(BURGER_SDKS)/linux/burgerlib

#
# Tell WMAKE where to find the files to work with
#

vpath %.c $(SOURCE_DIRS)
vpath %.cpp $(SOURCE_DIRS)
vpath %.x86 $(SOURCE_DIRS)
vpath %.i86 $(SOURCE_DIRS)
vpath %.o $(TEMP_DIR)

#
# Set the compiler flags for each of the build types
#

CFlagsDebug=-D_DEBUG -g -Og
CFlagsInternal=-D_DEBUG -g -O3
CFlagsRelease=-DNDEBUG -O3

#
# Set the flags for each target operating system
#

CFlagslnx= -D__LINUX__

#
# Set the WASM flags for each of the build types
#

AFlagsDebug=-D_DEBUG -g
AFlagsInternal=-D_DEBUG -g
AFlagsRelease=-DNDEBUG

#
# Set the as flags for each operating system
#

AFlagslnx=-D__LINUX__=1

LFlagsDebug=-g -lburgerlibmaklnxdbg
LFlagsInternal=-g -lburgerlibmaklnxint
LFlagsRelease=-lburgerlibmaklnxrel

LFlagslnx=-lGL -L$(BURGER_SDKS)/linux/burgerlib

# Now, set the compiler flags

C_INCLUDES=$(addprefix -I,$(INCLUDE_DIRS))
CL=$(CXX) -c -Wall -x c++ $(C_INCLUDES)
CP=$(CXX) -c -Wall -x c++ $(C_INCLUDES)
ASM=$(AS)
LINK=$(CXX)

# Set the default build rules
# Requires ASM, CP to be set

# Macro expansion is GNU make User's Guide
# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html

%.o: %.i86
	@echo $(*F).i86 / $(CONFIG) / $(TARGET)
	@$(ASM) $(AFlags$(CONFIG)) $(AFlags$(TARGET)) $< -o $(TEMP_DIR)/$@ -MMD -MF $(TEMP_DIR)/$(*F).d

%.o: %.x86
	@echo $(*F).x86 / $(CONFIG) / $(TARGET)
	@$(ASM) $(AFlags$(CONFIG)) $(AFlags$(TARGET)) $< -o $(TEMP_DIR)/$@ -MMD -MF $(TEMP_DIR)/$(*F).d

%.o: %.c
	@echo $(*F).c / $(CONFIG) / $(TARGET)
	@$(CP) $(CFlags$(CONFIG)) $(CFlags$(TARGET)) $< -o $(TEMP_DIR)/$@ -MMD -MF $(TEMP_DIR)/$(*F).d
	@sed -i "s:$(TEMP_DIR)/$@:$@:g" $(TEMP_DIR)/$(*F).d

%.o: %.cpp
	@echo $(*F).cpp / $(CONFIG) / $(TARGET)
	@$(CP) $(CFlags$(CONFIG)) $(CFlags$(TARGET)) $< -o $(TEMP_DIR)/$@ -MMD -MF $(TEMP_DIR)/$(*F).d
	@sed -i "s:$(TEMP_DIR)/$@:$@:g" $(TEMP_DIR)/$(*F).d

#
# Object files to work with for the library
#

OBJS= bradler16.o \
	bradler32.o \
	bralaw.o \
	bralgorithm.o \
	brarray.o \
	brasciito.o \
	brassert.o \
	bratomic.o \
	brautorepeat.o \
	brbase.o \
	brcapturestdout.o \
	brcodelibrary.o \
	brcommandparameter.o \
	brcommandparameterbooltrue.o \
	brcommandparameterstring.o \
	brcommandparameterwordptr.o \
	brcompress.o \
	brcompressdeflate.o \
	brcompresslbmrle.o \
	brcompresslzss.o \
	brconsolemanager.o \
	brcrc16.o \
	brcrc32.o \
	brcriticalsection.o \
	brdebug.o \
	brdecompress.o \
	brdecompressdeflate.o \
	brdecompresslbmrle.o \
	brdecompresslzss.o \
	brdetectmultilaunch.o \
	brdirectorysearch.o \
	brdirectorysearchlinux.o \
	brdisplay.o \
	brdisplayobject.o \
	brdisplayopengl.o \
	brdisplayopenglsoftware8.o \
	brdjb2hash.o \
	brdoublylinkedlist.o \
	brdxt1.o \
	brdxt3.o \
	brdxt5.o \
	brearcliptriangulate.o \
	breffect.o \
	breffect2d.o \
	breffectpositiontexturecolor.o \
	breffectstaticpositiontexture.o \
	brendian.o \
	brerror.o \
	brfile.o \
	brfileansihelpers.o \
	brfileapf.o \
	brfilebmp.o \
	brfiledds.o \
	brfilegif.o \
	brfileini.o \
	brfilelbm.o \
	brfilemanager.o \
	brfilemp3.o \
	brfilename.o \
	brfilepcx.o \
	brfilepng.o \
	brfiletga.o \
	brfilexml.o \
	brfixedmatrix3d.o \
	brfixedmatrix4d.o \
	brfixedpoint.o \
	brfixedvector2d.o \
	brfixedvector3d.o \
	brfixedvector4d.o \
	brflashaction.o \
	brflashactionvalue.o \
	brflashavm2.o \
	brflashcolortransform.o \
	brflashdisasmactionscript.o \
	brflashmanager.o \
	brflashmatrix23.o \
	brflashrect.o \
	brflashrgba.o \
	brflashstream.o \
	brflashutils.o \
	brfloatingpoint.o \
	brfont.o \
	brfont4bit.o \
	brfpinfo.o \
	brgameapp.o \
	brglobalmemorymanager.o \
	brglobals.o \
	brgost.o \
	brgridindex.o \
	brguid.o \
	brhashmap.o \
	brimage.o \
	brimportit.o \
	brimports3m.o \
	brimportxm.o \
	brinputmemorystream.o \
	brintrinsics.o \
	brisolatin1.o \
	brjoypad.o \
	brkeyboard.o \
	brlastinfirstout.o \
	brlinkedlistobject.o \
	brlinkedlistpointer.o \
	brlocalization.o \
	brmace.o \
	brmacromanus.o \
	brmatrix3d.o \
	brmatrix4d.o \
	brmd2.o \
	brmd4.o \
	brmd5.o \
	brmemoryansi.o \
	brmemoryfunctions.o \
	brmemoryhandle.o \
	brmemorymanager.o \
	brmersennetwist.o \
	brmicrosoftadpcm.o \
	brmouse.o \
	brmp3.o \
	brmp3math.o \
	brnetendpoint.o \
	brnetendpointtcp.o \
	brnetmanager.o \
	brnetmodule.o \
	brnetmoduletcp.o \
	brnetplay.o \
	brnumberstring.o \
	brnumberstringhex.o \
	brnumberto.o \
	broscursor.o \
	brosstringfunctions.o \
	broutputmemorystream.o \
	brpackfloat.o \
	brpair.o \
	brpalette.o \
	brperforce.o \
	brpoint2d.o \
	brprintf.o \
	brqueue.o \
	brrandom.o \
	brrandombase.o \
	brrect.o \
	brrenderer.o \
	brrenderersoftware16.o \
	brrenderersoftware8.o \
	brrezfile.o \
	brrunqueue.o \
	brsdbmhash.o \
	brsequencer.o \
	brsha1.o \
	brshader2dccolor.o \
	brshader2dcolorvertex.o \
	brshaders.o \
	brshape8bit.o \
	brsimplearray.o \
	brsimplestring.o \
	brsmartpointer.o \
	brsound.o \
	brsoundbufferrez.o \
	brsounddecompress.o \
	brstaticrtti.o \
	brstdouthelpers.o \
	brstring.o \
	brstring16.o \
	brstringfunctions.o \
	brtexture.o \
	brtextureopengl.o \
	brtick.o \
	brtimedate.o \
	brtypes.o \
	brulaw.o \
	brunpackbytes.o \
	brutf16.o \
	brutf32.o \
	brutf8.o \
	brvector2d.o \
	brvector3d.o \
	brvector4d.o \
	brvertexbuffer.o \
	brvertexbufferopengl.o \
	brwin1252.o \
	common.o \
	createtables.o \
	testbralgorithm.o \
	testbrcompression.o \
	testbrdisplay.o \
	testbrendian.o \
	testbrfileloaders.o \
	testbrfilemanager.o \
	testbrfixedpoint.o \
	testbrfloatingpoint.o \
	testbrhashes.o \
	testbrmatrix3d.o \
	testbrmatrix4d.o \
	testbrnetwork.o \
	testbrprintf.o \
	testbrstaticrtti.o \
	testbrstrings.o \
	testbrtimedate.o \
	testbrtypes.o \
	testcharset.o \
	testmacros.o

TRUE_OBJS = $(addprefix $(TEMP_DIR)/,$(OBJS))
DEPS = $(addprefix $(TEMP_DIR)/,$(OBJS:.o=.d))

#
# List the names of all of the final binaries to build
#

.PHONY: all
all: Debug Internal Release
	@

#
# Configurations
#

.PHONY: Debug
Debug: Debuglnx
	@

.PHONY: Internal
Internal: Internallnx
	@

.PHONY: Release
Release: Releaselnx
	@

.PHONY: lnx
lnx: Debuglnx Internallnx Releaselnx
	@

#
# List the names of all of the final binaries to build
#

.PHONY: Debuglnx
Debuglnx:
	@-mkdir -p "$(DESTINATION_DIR)"
	@-mkdir -p "$(BASE_TEMP_DIR)maklnxdbg"
	@$(MAKE) -e CONFIG=Debug TARGET=lnx -f unittestsmaklnx.mak $(DESTINATION_DIR)/$(PROJECT_NAME)maklnxdbg

.PHONY: Internallnx
Internallnx:
	@-mkdir -p "$(DESTINATION_DIR)"
	@-mkdir -p "$(BASE_TEMP_DIR)maklnxint"
	@$(MAKE) -e CONFIG=Internal TARGET=lnx -f unittestsmaklnx.mak $(DESTINATION_DIR)/$(PROJECT_NAME)maklnxint

.PHONY: Releaselnx
Releaselnx:
	@-mkdir -p "$(DESTINATION_DIR)"
	@-mkdir -p "$(BASE_TEMP_DIR)maklnxrel"
	@$(MAKE) -e CONFIG=Release TARGET=lnx -f unittestsmaklnx.mak $(DESTINATION_DIR)/$(PROJECT_NAME)maklnxrel

#
# Disable building this make file
#

unittestsmaklnx.mak:
	@

#
# A = The object file temp folder
#

$(DESTINATION_DIR)/$(PROJECT_NAME)maklnxdbg: $(OBJS) unittestsmaklnx.mak
	@$(LINK) -o $@ $(TRUE_OBJS) $(LFlags$(TARGET)) $(LFlags$(CONFIG))

$(DESTINATION_DIR)/$(PROJECT_NAME)maklnxint: $(OBJS) unittestsmaklnx.mak
	@$(LINK) -o $@ $(TRUE_OBJS) $(LFlags$(TARGET)) $(LFlags$(CONFIG))

$(DESTINATION_DIR)/$(PROJECT_NAME)maklnxrel: $(OBJS) unittestsmaklnx.mak
	@$(LINK) -o $@ $(TRUE_OBJS) $(LFlags$(TARGET)) $(LFlags$(CONFIG))

%.d:
	@

%: %,v

%: RCS/%,v

%: RCS/%

%: s.%

%: SCCS/s.%

%.h:
	@

# Include the generated dependencies
-include $(DEPS)
