# rpi-periphals test executables makefile
# Copyright (c) Dibase Limited 2012
# Author: Ralph E. McArdell

# Build configuration (debug | release), default to debug:

BUILD_CONFIG = debug

ROOT_DIR = $(realpath $(CURDIR)/../..)

include $(ROOT_DIR)/makeinclude.mak

# Files and directories
COMMON_SRC_FILES = test_main.cpp
INTERACTIVETEST_SRC_FILES = pin_interactivetests.cpp\
                            sysfs_interactivetests.cpp\
                            pin_edge_event_interactivetests.cpp\
                            clock_pin_interactivetests.cpp\
                            pwm_pin_interactivetests.cpp
PLATTEST_SRC_FILES = phymem_ptr_platformtests.cpp\
                    rpi_info_platformtests.cpp \
                    sysfs_platformtests.cpp\
                    pin_alloc_platformtests.cpp\
                    pin_platformtests.cpp\
                    pin_edge_event_platformtests.cpp\
                    clock_pin_platformtests.cpp\
                    pwm_pin_platformtests.cpp\
                    spi0_pins_platformtests.cpp
UNITTEST_SRC_FILE = gpio_registers_unittests.cpp\
                    clock_registers_unittests.cpp\
                    pwm_registers_unittests.cpp\
                    spi0_registers_unittests.cpp\
                    pin_id_unittests.cpp\
                    rpi_info_unittests.cpp\
                    pin_alloc_unittests.cpp\
                    gpio_alt_fn_unittests.cpp\
                    clockdefs_unittests.cpp\
                    clock_parameters_unittests.cpp\
                    simple_allocator_unittests.cpp\
                    pwm_pin_unittests.cpp\
                    spi0_pins_unittests.cpp
ALL_SRC_FILES = $(COMMON_SRC_FILES) $(INTERACTIVETEST_SRC_FILES) \
          $(PLATTEST_SRC_FILES) $(UNITTEST_SRC_FILE)

COMMON_OBJ_FILES = $(COMMON_SRC_FILES:%.cpp=$(OBJ_DIR)/%.o)
INTERACTIVETEST_OBJ_FILES = $(INTERACTIVETEST_SRC_FILES:%.cpp=$(OBJ_DIR)/%.o)
PLATTEST_OBJ_FILES = $(PLATTEST_SRC_FILES:%.cpp=$(OBJ_DIR)/%.o)
UNITTEST_OBJ_FILES = $(UNITTEST_SRC_FILE:%.cpp=$(OBJ_DIR)/%.o)

ALL_OBJ_FILES = $(ALL_SRC_FILES:%.cpp=$(OBJ_DIR)/%.o)
ALL_OBJ_FILENAMES = $(ALL_SRC_FILES:%.cpp=%.o)

DEP_FILES = $(ALL_SRC_FILES:%.cpp=%.d)

INTERACTIVETEST_TGT_FILE_RELEASE = $(TEST_DIR)/interactivetests-$(LIB_NAME_BASE)$(RELEASE_FILE_SUFFIX)
PLATTEST_TGT_FILE_RELEASE = $(TEST_DIR)/platformtests-$(LIB_NAME_BASE)$(RELEASE_FILE_SUFFIX)
UNITTEST_TGT_FILE_RELEASE = $(TEST_DIR)/unittests-$(LIB_NAME_BASE)$(RELEASE_FILE_SUFFIX)

INTERACTIVETEST_TGT_FILE_DEBUG = $(TEST_DIR)/interactivetests-$(LIB_NAME_BASE)$(DEBUG_FILE_SUFFIX)
PLATTEST_TGT_FILE_DEBUG = $(TEST_DIR)/platformtests-$(LIB_NAME_BASE)$(DEBUG_FILE_SUFFIX)
UNITTEST_TGT_FILE_DEBUG = $(TEST_DIR)/unittests-$(LIB_NAME_BASE)$(DEBUG_FILE_SUFFIX)

INTERACTIVETEST_TGT_FILE = $(TEST_DIR)/interactivetests-$(LIB_NAME)
PLATTEST_TGT_FILE = $(TEST_DIR)/platformtests-$(LIB_NAME)
UNITTEST_TGT_FILE = $(TEST_DIR)/unittests-$(LIB_NAME)

TGT_FILES_RELEASE = $(INTERACTIVETEST_TGT_FILE_RELEASE) $(PLATTEST_TGT_FILE_RELEASE) $(UNITTEST_TGT_FILE_RELEASE)
TGT_FILES_DEBUG = $(INTERACTIVETEST_TGT_FILE_DEBUG) $(PLATTEST_TGT_FILE_DEBUG) $(UNITTEST_TGT_FILE_DEBUG)

# But only one list of target paths is used when building:
ifeq ($(BUILD_CONFIG),release)
	TGT_FILES = $(TGT_FILES_RELEASE)
else ifeq ($(BUILD_CONFIG),debug)
	TGT_FILES = $(TGT_FILES_DEBUG)
endif

.PHONY: lib all clean tidy

all: lib $(DEP_FILES) $(TGT_FILES)

tidy: 
	-$(RM) $(ALL_OBJ_FILENAMES:%=$(BUILD_DIR)/release/%)
	-$(RM) $(ALL_OBJ_FILENAMES:%=$(BUILD_DIR)/debug/%)

clean: tidy
	-$(RM) $(TGT_FILES_RELEASE)
	-$(RM) $(TGT_FILES_DEBUG)
	-$(RM) $(DEP_FILES)
	-$(RM) *.d.*

lib:
	$(MAKE) -C $(SRC_DIR) BUILD_CONFIG=$(BUILD_CONFIG) 

$(INTERACTIVETEST_TGT_FILE): $(COMMON_OBJ_FILES) $(INTERACTIVETEST_OBJ_FILES) $(LIB_DIR)/$(LIB_FILE)
	$(LD) $(LD_FLAGS) -o $@ $(COMMON_OBJ_FILES) $(INTERACTIVETEST_OBJ_FILES) $(LD_LIBS)

$(PLATTEST_TGT_FILE): $(COMMON_OBJ_FILES) $(PLATTEST_OBJ_FILES) $(LIB_DIR)/$(LIB_FILE)
	$(LD) $(LD_FLAGS) -o $@ $(COMMON_OBJ_FILES) $(PLATTEST_OBJ_FILES) $(LD_LIBS)

$(UNITTEST_TGT_FILE): $(COMMON_OBJ_FILES) $(UNITTEST_OBJ_FILES) $(LIB_DIR)/$(LIB_FILE)
	$(LD) $(LD_FLAGS) -o $@ $(COMMON_OBJ_FILES) $(UNITTEST_OBJ_FILES) $(LD_LIBS)

$(OBJ_DIR)/%.o: %.cpp
	$(CC) $(COMPILE_FLAGS) -I./.. -I$(EXT_PACKAGES_DIR)/Catch/single_include -o $@ $<

$(LIB_DIR)/$(LIB_FILE):
	$(MAKE) -C $(SRC_DIR) BUILD_CONFIG=$(BUILD_CONFIG) 

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(COMPILE_FLAGS) -MT'$$(OBJ_DIR)/$(<:%.cpp=%.o)' \
	-I./.. -I$(EXT_PACKAGES_DIR)/Catch/single_include $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(DEP_FILES)
