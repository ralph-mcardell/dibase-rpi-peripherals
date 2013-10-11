# rpi-peripherals example executables makefile
# Copyright (c) Dibase Limited 2012
# Author: Ralph E. McArdell

# Build configuration (debug | release), default to release:
BUILD_CONFIG = release
ROOT_DIR = $(realpath $(CURDIR)/../..)

include $(ROOT_DIR)/makeinclude.mak

# Files and directories
SRC_FILES = open-collector.cpp\
            leds_and_switches.cpp\
            led-segment-display.cpp\
            pwm-motor.cpp\
            spi0-adc-dac.cpp\
            pulse_counter.cpp
DEP_FILES = $(SRC_FILES:%.cpp=%.d)

# Executable target files have the name of the example source file less the 
# extension but might include a build configuration specific suffix.
# Both lists of paths are required for the clean target:
TGT_FILES_RELEASE = $(SRC_FILES:%.cpp=$(EXEC_DIR)/%$(RELEASE_FILE_SUFFIX))
TGT_FILES_DEBUG = $(SRC_FILES:%.cpp=$(EXEC_DIR)/%$(DEBUG_FILE_SUFFIX))

# But only one list of target paths is used when building:
ifeq ($(BUILD_CONFIG),release)
	TGT_FILES = $(TGT_FILES_RELEASE)
else ifeq ($(BUILD_CONFIG),debug)
	TGT_FILES = $(TGT_FILES_DEBUG)
endif

.PHONY: lib all clean tidy

all: lib $(DEP_FILES) $(TGT_FILES)

# Currently tidy does nothing 'cos gmake nicely removes the
# intermediate files after building that tidy would have removed.
tidy: 

clean: tidy
	-$(RM) $(TGT_FILES_RELEASE)
	-$(RM) $(TGT_FILES_DEBUG)
	-$(RM) $(DEP_FILES)
	-$(RM) *.d.*

lib:
	$(MAKE) -C $(SRC_DIR) BUILD_CONFIG=$(BUILD_CONFIG) 

$(EXEC_DIR)/%$(FILE_SUFFIX): $(OBJ_DIR)/%.o $(LIB_DIR)/$(LIB_FILE)
	$(LD) $(LD_FLAGS) -o $@ $<  $(LD_LIBS)


$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(COMPILE_FLAGS) -o $@ $< -pthread 

$(LIB_DIR)/$(LIB_FILE):
	$(MAKE) -C $(SRC_DIR) BUILD_CONFIG=$(BUILD_CONFIG)
%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(COMPILE_FLAGS) -MT'$$(OBJ_DIR)/$(<:%.cpp=%.o)' \
	-I./.. $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(DEP_FILES)
