# rpi-periphals toplevel makefile
# Copyright (c) Dibase Limited 2012
# Author: Ralph E. McArdell
#
# Targets:
# --------
# all (default): build all libraries and examples using 
#                release and debug configurations
# release: as all but only  builds using release configuration
# debug: as all but only builds using debug configuration
# test: builds test executables using debug configuration
# check: as test but also executes tests
# clean: removes all executables, libraries and object files built by make
# tidy: removes only intermediate files such as object files
# help: shows similar help text on console

ROOT_DIR = $(CURDIR)

# Set BUILD_CONFIG to valid value, but not used by this make file.
BUILD_CONFIG = release

include makeinclude.mak

.PHONY: all dirs debug release test testcompilefail check tidy clean help

export COMPILE_OPTS
export CROSS_COMPILE

all: debug release

# We expect these directories to be present:
dirs: $(BUILD_DIR)/debug $(BUILD_DIR)/release $(LIB_DIR) $(EXEC_DIR) $(TEST_DIR)

debug: dirs
	$(MAKE) -C $(SRC_DIR) BUILD_CONFIG=debug
	$(MAKE) -C $(SRC_DIR)/examples BUILD_CONFIG=debug

release: dirs
	$(MAKE) -C $(SRC_DIR) BUILD_CONFIG=release
	$(MAKE) -C $(SRC_DIR)/examples BUILD_CONFIG=release

test: dirs
	$(MAKE) -C $(SRC_DIR)/tests BUILD_CONFIG=debug
	$(MAKE) -C $(SRC_DIR)/examples/led-string-display/tests BUILD_CONFIG=debug

testcompilefail: dirs
	$(MAKE) -C $(SRC_DIR)/tests BUILD_CONFIG=debug COMPILE_OPTS=-DCOMPILE_FAIL_TESTS
	$(MAKE) -C $(SRC_DIR)/examples/led-string-display/tests BUILD_CONFIG=debug COMPILE_OPTS=-DCOMPILE_FAIL_TESTS

check: dirs test
	@echo 'To be done...'

tidy: dirs
	$(MAKE) -C $(SRC_DIR) tidy
	$(MAKE) -C $(SRC_DIR)/examples tidy
	$(MAKE) -C $(SRC_DIR)/tests tidy
	-$(RM) $(BUILD_DIR)/debug/*
	-$(RM) $(BUILD_DIR)/release/*

clean: dirs
	$(MAKE) -C $(SRC_DIR) clean
	$(MAKE) -C $(SRC_DIR)/examples clean
	$(MAKE) -C $(SRC_DIR)/tests clean
	-$(RM) $(BUILD_DIR)/debug/*
	-$(RM) $(BUILD_DIR)/release/*

help:
	@echo 'Usage: make [COMPILE_OPTS=extra_opts] [CROSS_COMPILE=prefix] [target]'
	@echo 'Targets:'
	@echo '  all (default)   Build release and debug targets.'
	@echo '  release         Build release library and example executables.'
	@echo '  debug           Build debug library and example executables.'
	@echo '  test            Build tests.'
	@echo '  testcompilefail Build tests with COMPILE_FAIL_TESTS defined.'
	@echo '  check           Build and execute tests.'
	@echo '  clean           Remove final and intermediate targets.'
	@echo '  tidy            Remove only intermediate targets such as .o files.'
	@echo '  help            Print this help.'
	@echo 'Variables:'
	@echo '  COMPILE_OPTS    Value gives additional g++ compilation options.'
	@echo '  CROSS_COMPILE   Value is build tools prefix for cross compilation.'

# The following rules ensure we have the various end
# and intermediate target directories available:
$(EXEC_DIR):
	mkdir $@

$(TEST_DIR):
	mkdir $@

$(LIB_DIR):
	mkdir $@

$(BUILD_DIR):
	mkdir $@

$(BUILD_DIR)/debug: $(BUILD_DIR)
	mkdir $@

$(BUILD_DIR)/release: $(BUILD_DIR)
	mkdir $@
	