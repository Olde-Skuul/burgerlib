#
# Build test_dll with make
#
# Generated with makeprojects.makefile
#

#
# Project to build without a goal
#

.DEFAULT_GOAL := all

#
# List the names of all of the final binaries to build and clean
#

.PHONY: all
all: Release ;

.PHONY: clean
clean: clean_Release ;

#
# Configurations
#

.PHONY: Release
Release: Releaselnx ;

.PHONY: clean_Release
clean_Release: clean_Releaselnx ;

#
# Platforms
#

.PHONY: lnx
lnx: Releaselnx ;

.PHONY: clean_lnx
clean_lnx: clean_Releaselnx ;

#
# List of binaries to build or clean
#

.PHONY: Releaselnx
Releaselnx:
	@$(MAKE) -e --no-print-directory CONFIG=Release TARGET=lnx -f test_dllmaklnx.mak bin/libtest_dllmaklnxrel.so

.PHONY: clean_Releaselnx
clean_Releaselnx:
	@-rm -rf temp/test_dllmaklnxrel
	@-rm -f bin/test_dllmaklnxrel
	@if [ -d bin ] && files=$$(ls -qAL -- bin) && [ -z "$$files" ]; then rm -fd bin; fi
	@if [ -d temp ] && files=$$(ls -qAL -- temp) && [ -z "$$files" ]; then rm -fd temp; fi

#
# Create the folder for the binary output
#

bin:
	@-mkdir -p bin

temp:
	@-mkdir -p temp

#
# Disable building this make file
#

test_dllmaklnx.mak: ;

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

CONFIG_SUFFIX_Release := rel

#
# Base name of the temp directory
#

BASE_SUFFIX := mak$(TARGET_SUFFIX_$(TARGET))$(CONFIG_SUFFIX_$(CONFIG))
TEMP_DIR := temp/test_dll$(BASE_SUFFIX)

#
# SOURCE_DIRS = Work directories for the source code
#

SOURCE_DIRS :=source

#
# INCLUDE_DIRS = Header includes
#

INCLUDE_DIRS = $(SOURCE_DIRS)
INCLUDE_DIRS +=../../source/lowlevel

#
# Set the compiler flags for each of the build types
#

CFlagsReleaselnx:= -O3 -fPIC -DNDEBUG -D__LINUX__

#
# Set the assembler flags for each of the build types
#

AFlagsReleaselnx:= -DNDEBUG -D__LINUX__

#
# Set the Linker flags for each of the build types
#

LFlagsReleaselnx:= -shared

# Now, set the compiler flags

C_INCLUDES:=$(addprefix -I,$(INCLUDE_DIRS))
CL:=$(CC) -c -Wall -x c $(C_INCLUDES)
CP:=$(CXX) -c -Wall -x c++ $(C_INCLUDES)
ASM:=$(AS)
LINK:=$(CXX)

#
# Default build recipes
#

define BUILD_C=
@echo $(<F) / $(CONFIG) / $(TARGET); \
$(CL) $(CFlags$(CONFIG)$(TARGET)) $< -o $@ -MT '$@' -MMD -MF '$*.d'
endef

define BUILD_CPP=
@echo $(<F) / $(CONFIG) / $(TARGET); \
$(CP) $(CFlags$(CONFIG)$(TARGET)) $< -o $@ -MT '$@' -MMD -MF '$*.d'
endef

#
# Object files to work with for the library
#

OBJS:= $(TEMP_DIR)/test_dll.o

DEPS:= $(TEMP_DIR)/test_dll.d

#
# Disable building the source files
#

source/test_dll.cpp: ;

#
# Build the object file folder
#

$(OBJS): | $(TEMP_DIR)

$(TEMP_DIR):
	@-mkdir -p $(TEMP_DIR)

#
# Build the object files
#

$(TEMP_DIR)/test_dll.o: source/test_dll.cpp ; $(BUILD_CPP)

#
# Create final binaries
#

bin/libtest_dllmaklnxrel.so: $(OBJS) test_dllmaklnx.mak | bin
	@$(LINK) -o $@ $(OBJS) $(LFlags$(CONFIG)$(TARGET))
	@if [ -f /bin/wslpath ]; then \
	p4.exe edit $$(wslpath -a -w '../../data/test_dlls/linux/libtest_dllmaklnxrel.so'); \
	cp -T "$@" "../../data/test_dlls/linux/libtest_dllmaklnxrel.so"; \
	p4.exe revert -a $$(wslpath -a -w '../../data/test_dlls/linux/libtest_dllmaklnxrel.so'); \
	else \
	p4 edit "../../data/test_dlls/linux/libtest_dllmaklnxrel.so"; \
	cp -T "$@" "../../data/test_dlls/linux/libtest_dllmaklnxrel.so"; \
	p4 revert -a "../../data/test_dlls/linux/libtest_dllmaklnxrel.so"; \
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
