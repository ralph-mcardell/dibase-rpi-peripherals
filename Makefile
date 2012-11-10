# rpi-periphals toplevel makefile
# Copyright (c) Dibase Limited 2012
# Author: Ralph E. McArdell
#
# Targets:
# ---------
# all (default): build all libraries and examples using 
#                release and debug configurations
# release: as all but only  builds using release configuarion
# debug: as all but only builds using debug configuration
# test: builds test executables using debug configuration
# check: as test but also executes tests
# clean: removes all executables, libraries and object files built by make
# tidy: removes only intermediate files such as object files
# help: shows this text on console

ROOT_DIR = $(CURDIR)

# Set BUILD_CONFIG to valid value, but not used by this make file.
BUILD_CONFIG = release

include makeinclude.mak

.PHONY : all debug release

all: debug release

debug:
	$(MAKE) -C ./src BUILD_CONFIG=debug

release:
	$(MAKE) -C ./src BUILD_CONFIG=release
