# rpi-periphals library makefile
# Copyright (c) Dibase Limited 2012
# Author: Ralph E. McArdell

# Files and directories
SRC_FILES = phymem_ptr.cpp
LIB_DIR = ../lib
OBJ_DIR= ../build
TGT_FILE = $(LIB_DIR)/librpi-periphals.a
OBJ_FILES = $(SRC_FILES:%.cpp=$(OBJ_DIR)/%.o)

# C++ compiler
CXX = g++
CC = $(CXX)

# Library archiver
AR = ar

# C++ compiler flags
CXX_FLAGS_COMMON = -std=c++0x -Wall -Wextra -pedantic -c
CXX_DEBUG_FLAGS = -O0 -g
CXX_RELEASE_FLAGS = -O2

# Preprocessor flags
CPP_FLAGS = -I.

# Archiver flags
AR_FLAGS = -crs

# Full compile flags
COMPILE_FLAGS = $(CXX_FLAGS_COMMON) $(CXX_RELEASE_FLAGS) $(CPP_FLAGS)

# File delete command
RM = rm

all: $(TGT_FILE)

clean:
	-$(RM) $(OBJ_DIR)/*.o

very-clean: clean
	-$(RM) $(LIB_DIR)/*.a

$(TGT_FILE): $(OBJ_FILES)
	$(AR) $(AR_FLAGS) $(TGT_FILE) $(OBJ_FILES)

$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(COMPILE_FLAGS) -o $@ $<

$(OBJ_DIR)/phymem_ptr.o: phymem_ptr.cpp phymem_ptr.h
