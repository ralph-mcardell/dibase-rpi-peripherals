# rpi-periphals common makefile variable definitions
# Copyright (c) Dibase Limited 2012
# Author: Ralph E. McArdell

# Major project directories:
# --------------------------
ifndef ROOT_DIR
$(error ROOT_DIR, which defines the project root directory path, is not defined)
endif

# File name suffixes used for each build configuration
RELEASE_FILE_SUFFIX = 
DEBUG_FILE_SUFFIX = d

# BUILD_CONFIG dependent values:
ifndef BUILD_CONFIG
$(error BUILD_CONFIG not defined, its value should be 'release' or 'debug')
else ifeq ($(BUILD_CONFIG),release)
	FILE_SUFFIX = $(RELEASE_FILE_SUFFIX)
else ifeq ($(BUILD_CONFIG),debug)
	FILE_SUFFIX = $(DEBUG_FILE_SUFFIX)
else
$(error BUILD_CONFIG value should be 'release' or 'debug' not '$(BUILD_CONFIG)')
endif

# Name of the static library:
# LIB_NAME is used with tool -l options.
# LIB_FILE is the full file name _without path_.
LIB_NAME_BASE = rpi-peripherals
LIB_NAME = $(LIB_NAME_BASE)$(FILE_SUFFIX)
LIB_FILE = lib$(LIB_NAME).a

# Directories of interest:
SRC_DIR = $(ROOT_DIR)/src
INC_DIR = $(ROOT_DIR)/include
LIB_DIR = $(ROOT_DIR)/lib
BUILD_DIR = $(ROOT_DIR)/build
OBJ_DIR = $(BUILD_DIR)/$(BUILD_CONFIG)
EXEC_DIR = $(ROOT_DIR)/bin
TEST_DIR = $(ROOT_DIR)/test
DOC_DIR = $(ROOT_DIR)/doc
EXT_PACKAGES_DIR = $(ROOT_DIR)/thirdparty

# Tools:
# ------
# C++ compiler
CXX = $(CROSS_COMPILE)g++
CC = $(CXX)

# Library archiver
AR = $(CROSS_COMPILE)ar

# Linker (use the g++ C++ driver so C++ libraries are included automatically)
LD = $(CROSS_COMPILE)g++

# File delete command
RM = rm

# Tool flags:
# -----------
# Preprocessor flags
CPP_FLAGS = -I. -I$(INC_DIR)

# C++ compiler flags
CXX_FLAGS_COMMON = -std=c++0x -Wall -Wextra -pedantic -c
# Set additional compiler options on command line to override empty COMPILE_OPTS
COMPILE_OPTS=
CXX_DEBUG_FLAGS = -O0 -g $(COMPILE_OPTS)
CXX_RELEASE_FLAGS = -O2 $(COMPILE_OPTS)

# Full compile flags
ifeq ($(BUILD_CONFIG),release)
	COMPILE_FLAGS = $(CXX_FLAGS_COMMON) $(CXX_RELEASE_FLAGS) $(CPP_FLAGS)
else ifeq ($(BUILD_CONFIG),debug)
	COMPILE_FLAGS = $(CXX_FLAGS_COMMON) $(CXX_DEBUG_FLAGS) $(CPP_FLAGS)
endif

# Archiver flags
AR_FLAGS = -crs

# Linker flags
LD_FLAGS_COMMON = -L $(LIB_DIR)
# Override the currently empty flag lists variables below to modify
# link behaviour - e.g. adding -s to strip symbols
LD_DEBUG_FLAGS =
LD_RELEASE_FLAGS =

LD_LIBS = -l$(LIB_NAME) -lpthread

# Full link flags
ifeq ($(BUILD_CONFIG),release)
	LD_FLAGS = $(LD_FLAGS_COMMON) $(LD_RELEASE_FLAGS)
else ifeq ($(BUILD_CONFIG),debug)
	LD_FLAGS = $(LD_FLAGS_COMMON) $(LD_DEBUG_FLAGS)
endif
