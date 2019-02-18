# ------------------------------------------------------------------
#
# TDD makefile Words lib
#
# ---------------------------------------------------------------------

# Base name of the test file.
TARGET_BASE = lineedit
# Name of the source folder.
TARGET_BASE_DIR = ui

# Defs common to the utils.
include ../ui_common_pre.mak

# The complete files list
SRC_FILES := $(SRC_FILES) \
								$(HARNESS_TESTS_SRC) $(HARNESS_MAIN_SRC) $(LIBS)

# Clean and build
include ../ui_common_build.mak

# ------------------------------- eof ------------------------------------

