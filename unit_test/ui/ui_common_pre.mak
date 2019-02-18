# --------------------------------------------------------------------------
#
# Arithmetic Library: TDD Unit Tests, common defs.
#
# ---------------------------------------------------------------------------

TDDDIR = ../../../../unity_tdd/

# Import TDD harness defs
include ../../tiny2_common_pre.mak

# (Additional) compiler flags
CFLAGS := $(CFLAGS) -DGNRC_UNIT_TEST -D__COMPILER_IS_GENERIC__ -D__SYSTEM_IS_ANY__

# File of tests is here.
SRCDIR = ../../../src/$(TARGET_BASE_DIR)/

SRC_FILES := $(SRC_FILES) ../ui_test_support.c  $(UNITYDIR)unity.c $(SRCDIR)$(DUT_SRC)

OUT_FILE = -o $(TARGET)
SYMBOLS=-DTEST

# All arith tests reference the same includes
INC_DIRS := $(INC_DIRS) -I. -I../../../src -I../../../private -I../../../public -I../../../GenericTypes -I../../../arith_incl

# --------------------------------- eof ------------------------------------


