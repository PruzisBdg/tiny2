#include "unity.h"
#include <stdlib.h>
#include <time.h>
#include "tdd_common.h"
#include <string.h>
#include "lineedit.h"

// =============================== Tests start here ==================================


/* -------------------------------------- setUp ------------------------------------------- */

void setUp(void) {
    srand(time(NULL));     // Random seed for scrambling stimuli
}

/* -------------------------------------- tearDown ------------------------------------------- */

void tearDown(void) {
}

void test_Init(void)
{
   LineEdit_Init();
}

// ----------------------------------------- eof --------------------------------------------
