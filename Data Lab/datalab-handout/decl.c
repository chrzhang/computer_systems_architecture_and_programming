#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define TMin INT_MIN
#define TMax INT_MAX

#include "btest.h"
#include "bits.h"

test_rec test_set[] = {





 {"bitNand", (funct_t) bitNand, (funct_t) test_bitNand, 2, "| ~", 8, 1,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
{"isEqualThree", (funct_t) isEqualThree, (funct_t) test_isEqualThree, 3,
  "! ~ & ^ | + << >>", 10, 2,
  {{TMin, TMax}, {TMin, TMax}, {TMin, TMax}}},
 {"swap30", (funct_t) swap30, (funct_t) test_swap30, 1,
    "! ~ & ^ | + << >>", 25, 2,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"logicalByteAdd", (funct_t) logicalByteAdd, (funct_t) test_logicalByteAdd, 2,
    "! ~ & ^ | << >>", 30, 3,
     {{0, 255},{0,255},{TMin,TMax}}},
 {"rotateNibbleLeft", (funct_t) rotateNibbleLeft, (funct_t) test_rotateNibbleLeft,
   2, "! ~ & ^ | + << >>", 25, 3,
  {{TMin, TMax},{0,31},{TMin,TMax}}},
 {"bitBurstSize", (funct_t) bitBurstSize, (funct_t) test_bitBurstSize, 1, "! ~ & ^ | + << >>", 40, 4,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"isNegatable", (funct_t) isNegatable, (funct_t) test_isNegatable, 1,
    "! ~ & ^ | + << >>", 10, 2,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"sign", (funct_t) sign, (funct_t) test_sign, 1, "! ~ & ^ | + << >>", 10, 2,
     {{-TMax, TMax},{TMin,TMax},{TMin,TMax}}},
{"maxOfThree", (funct_t) maxOfThree, (funct_t) test_maxOfThree, 3,
  "! ~ & ^ | + << >>", 20, 3,
  {{0, TMax}, {0, TMax}, {0, TMax}}},
 {"isInRange", (funct_t) isInRange, (funct_t) test_isInRange, 3,
     "! ~ & ^ | + << >>", 20, 3,
  {{0, 255},{0,255},{0,255}}},
 {"ezThreeSixteenths", (funct_t) ezThreeSixteenths, (funct_t) test_ezThreeSixteenths, 1,
    "! ~ & ^ | + << >>", 14, 3,
  {{-(1<<28)-1, (1<<28)-1},{TMin,TMax},{TMin,TMax}}},
 {"sm2tc", (funct_t) sm2tc, (funct_t) test_sm2tc, 1, "! ~ & ^ | + << >>", 15, 4,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"binarySize", (funct_t) binarySize, (funct_t) test_binarySize, 1, "! ~ & ^ | + << >>", 90, 4,
  {{TMin, TMax},{TMin,TMax},{TMin,TMax}}},
 {"float_abs", (funct_t) float_abs, (funct_t) test_float_abs, 1,
    "$", 10, 2,
     {{1, 1},{1,1},{1,1}}},
 {"float_exp", (funct_t) float_exp, (funct_t) test_float_exp, 1,
    "$", 8, 2,
     {{1, 1},{1,1},{1,1}}},
 {"float_half_denorm", (funct_t) float_half_denorm, (funct_t) test_float_half_denorm, 1,
    "$", 15, 3,
     {{1, 1},{1,1},{1,1}}},
  {"", NULL, NULL, 0, "", 0, 0,
   {{0, 0},{0,0},{0,0}}}
};
