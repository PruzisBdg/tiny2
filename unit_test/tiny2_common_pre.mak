# --------------------------------------------------------------------------
#
# Arithmetic Library: TDD Unit Tests, common defs.
#
# ---------------------------------------------------------------------------

# Import TDD harness defs
include $(TDDDIR)tdd_common_pre_build.mak

# (Additional) compiler flags
CFLAGS := $(CFLAGS) -DGNRC_UNIT_TEST -D__COMPILER_IS_GENERIC__ -D__SYSTEM_IS_ANY__ -D__TOOL_IS_GCC_ARM__

# Most test harnesses need arith support.
INTEGER_BASIC_FULLPATH = $(SRCDIR)arith_integer_basic$(CEXT)

OUT_FILE = -o $(TARGET)
SYMBOLS=-DTEST

# --------------------------------- eof ------------------------------------


