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
LIB_NAME = rpi-periphals$(FILE_SUFFIX)
LIB_FILE = lib$(LIB_NAME).a

# Directories of interest:
SRC_DIR = $(ROOT_DIR)/src
INC_DIR = $(ROOT_DIR)/include
LIB_DIR = $(ROOT_DIR)/lib
BUILD_DIR = $(ROOT_DIR)/build
OBJ_DIR = $(BUILD_DIR)/$(BUILD_CONFIG)
EXEC_DIR = $(ROOT_DIR)/bin
DOC_DIR = $(ROOT_DIR)/doc
EXT_PACKAGES_DIR = $(ROOT_DIR)/thirdparty

# Tools:
# ------
# C++ compiler
CXX = g++
CC = $(CXX)

# Library archiver
AR = ar

# Linker (use the g++ C++ driver so C++ libraries are included automatically)
LD = g++

# File delete command
RM = rm

# Tool flags:
# -----------
# Preprocessor flags
CPP_FLAGS = -I. -I$(INC_DIR)

# C++ compiler flags
CXX_FLAGS_COMMON = -std=c++0x -Wall -Wextra -pedantic -c
CXX_DEBUG_FLAGS = -O0 -g
CXX_RELEASE_FLAGS = -O2

# Full compile flags
ifeq ($(BUILD_CONFIG),release)
	COMPILE_FLAGS = $(CXX_FLAGS_COMMON) $(CXX_RELEASE_FLAGS) $(CPP_FLAGS)
else ifeq ($(BUILD_CONFIG),debug)
	COMPILE_FLAGS = $(CXX_FLAGS_COMMON) $(CXX_DEBUG_FLAGS) $(CPP_FLAGS)
endif

# Archiver flags
AR_FLAGS = -crs

# Linker flags
LD_FLAGS = -L $(LIB_DIR)
