# rpi-periphals static library makefile
# Copyright (c) Dibase Limited 2012
# Author: Ralph E. McArdell

# Build configuration (debug | release), default to release:
BUILD_CONFIG = release
ROOT_DIR = $(realpath $(CURDIR)/..)

include $(ROOT_DIR)/makeinclude.mak

# Files and directories
SRC_FILES = phymem_ptr.cpp
TGT_FILE = $(LIB_DIR)/$(LIB_FILE)
OBJ_FILES = $(SRC_FILES:%.cpp=$(OBJ_DIR)/%.o)
OBJ_FILENAMES = $(SRC_FILES:%.cpp=%.o)
DEP_FILES = $(SRC_FILES:%.cpp=%.d)

# File delete command
RM = rm

.PHONY: all clean tidy

all: $(DEP_FILES) $(TGT_FILE)

tidy:
	-$(RM) $(OBJ_FILENAMES:%=$(BUILD_DIR)/release/%)
	-$(RM) $(OBJ_FILENAMES:%=$(BUILD_DIR)/debug/%)

clean: tidy
	-$(RM) $(LIB_DIR)/*.a
	-$(RM) $(DEP_FILES)

$(TGT_FILE): $(OBJ_FILES)
	$(AR) $(AR_FLAGS) $(TGT_FILE) $(OBJ_FILES)

$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(COMPILE_FLAGS) -o $@ $<

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(COMPILE_FLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(DEP_FILES)
