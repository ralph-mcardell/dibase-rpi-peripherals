# rpi-periphals common makefile variable definitions
# Copyright (c) Dibase Limited 2012
# Author: Ralph E. McArdell

# Major project directories:
# --------------------------
ifndef ROOT_DIR
$(error ROOT_DIR, which defines the project root directory path, is not defined)
endif

ifndef BUILD_CONFIG
$(error BUILD_CONFIG is not defined, it should have a value of 'release' or 'debug')
else ifeq ($(BUILD_CONFIG),release)
else ifeq ($(BUILD_CONFIG),debug)
else
$(error BUILD_CONFIG variable should be 'release' or 'debug' not '$(BUILD_CONFIG)' )
endif

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

# Linker
LD = ld


# Tool flags:
# -----------
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

# Preprocessor flags
CPP_FLAGS_COMMON = -I. -I$(INC_DIR)

# Archiver flags
AR_FLAGS = -crs
