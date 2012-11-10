# rpi-periphals static library makefile
# Copyright (c) Dibase Limited 2012
# Author: Ralph E. McArdell

# Build configuration (debug | release):
BUILD_CONFIG = release
ROOT_DIR = $(realpath $(CURDIR)/..)

include ../makeinclude.mak

# Files and directories
SRC_FILES = phymem_ptr.cpp
ifeq ($(BUILD_CONFIG),release)
	TGT_FILE = $(LIB_DIR)/librpi-periphals.a
else ifeq ($(BUILD_CONFIG),debug)
	TGT_FILE = $(LIB_DIR)/librpi-periphalsd.a
endif
	OBJ_FILES = $(SRC_FILES:%.cpp=$(OBJ_DIR)/%.o)

# File delete command
RM = rm

.PHONY: all clean very-clean

all: $(TGT_FILE)

clean:
	-$(RM) $(BUILD_DIR)/release/*.o
	-$(RM) $(BUILD_DIR)/debug/*.o

very-clean: clean
	-$(RM) $(LIB_DIR)/*.a

$(TGT_FILE): $(OBJ_FILES)
	echo $(OBJ_FILES)
	$(AR) $(AR_FLAGS) $(TGT_FILE) $(OBJ_FILES)

$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(COMPILE_FLAGS) -o $@ $<

$(OBJ_DIR)/%d.o: %.cpp
	$(CC) $(COMPILE_FLAGS) -o $@ $<
	
$(OBJ_DIR)/phymem_ptr.o: phymem_ptr.cpp phymem_ptr.h
