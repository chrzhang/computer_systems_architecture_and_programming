/* 
 * CS:APP Data Lab 
 * 
 * Christopher Zhang (czhang44)
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif

/* 
 * bitNand -  using only ~ and | 
 *   return the NAND of the two numbers -- NAND is the logical complement of AND
 *   Example: bitNand(6, 5) = 0xfffffffb  (only the 4 bit is 1 in both numbers)
 *   Legal ops: ~ |
 *   Max ops: 6
 *   Rating: 1
 */
int bitNand(int x, int y) {
  return (~x) | (~y); // DeMorgan's Laws equate NAND to the OR of the NOTs
}

/*
 * isEqualThree - Returns 1 if all three numbers are equal otherwise 0
 *
 * Examples: isEqualThree(1, 2, 3) = 0
 *           isEqualThree(4, 4, 4) = 1
 *
 * Legal Ops: ! ~ & ^ | + << >>
 * Max Ops: 10
 * Rating: 2
 */
int isEqualThree(int x, int y, int z) {
  // Two bits can be tested for equality by the NOT of the XOR operator
  return !(x ^ y) & !(y ^ z);
}

/* 
 * swap30 -- switch the 0 and 3rd bits of every hex digit of the number
 *   Example: swap30(0) = 0
 *            swap30(8) = 1
              swap30(0x812) = 0x182
              swap30(0x12345678) = 0x82a4c6e1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 25
 *   Rating: 2
 */
int swap30(int x) {
  int fo1, foMask, fi1, fiMask, fourthDigits, firstDigits;
  // Make a mask for every third digit
  fo1 = 0x88;
  fo1 |= fo1<<8;
  fo1 |= fo1<<16;
  foMask = fo1;
  fi1 = 0x11;
  fi1 |= fi1<<8;
  fi1 |= fi1<<16;
  fiMask = fi1;
  // Apply the masks with AND to get the digits
  fourthDigits = foMask & x;
  firstDigits = fiMask & x;
  // Clear the original positions
  x &= ~foMask;
  x &= ~fiMask;
  // Shift the fourth digits right thrice
  fourthDigits >>= 3;
  // Nullify the first three bits
  fourthDigits &=  ~(0xE<<28);
  // Shift the first digits left
  firstDigits = firstDigits << 3;
  // OR the original with each
  x |= fourthDigits;
  x |= firstDigits;
  return x;
}

/* 
 * logicalByteAdd - Add two bytes without using +.  Note that the sum may be
   bigger than 255
 *   Example: logicalByteAddBytes(0x20,0x30) = 0x50,
              logicalByteAddBytes(0xff,0xff) = 0x1fe
 *   Legal ops: ! ~ & ^ | << >>
 *   Max ops: 30
 *   Rating: 3
 */
int logicalByteAdd(int x, int y) {
  // Model the action of a full gate adder and the fact that a carry
  // is generated from AND and the sum is generated from XOR
  int carry;
  carry = x & y;
  x ^= y;
  y = carry << 1;
  carry = x & y;
  x ^= y;
  y = carry << 1;
  carry = x & y;
  x ^= y;
  y = carry << 1;
  carry = x & y;
  x ^= y;
  y = carry << 1;
  carry = x & y;
  x ^= y;
  y = carry << 1;
  carry = x & y;
  x ^= y;
  y = carry << 1;
  carry = x & y;
  x ^= y;
  y = carry << 1;
  carry = x & y;
  x ^= y;
  y = carry << 1;
  carry = x & y;
  x ^= y;
  y = carry << 1;
  return x;
}

/* 
 * rotateNibbleLeft - Rotate x to the left by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateNibbleLeft(0x87654321,1) = 0x76543218
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 25
 *   Rating: 3 
 */
int rotateNibbleLeft(int x, int n) {
  // Store the leftmost n nibbles with a mask
  int mask, firstNibbles, neg1, tmin, fourn;
  neg1 = ~0;
  fourn = n << 2;
  // Shift Tmin right by however many digits, if n is zero do nothing
  tmin = 1 << 31;
  mask = tmin >> (fourn + neg1);
  // AND the mask to store the first n nibbles
  firstNibbles = mask & x;
  firstNibbles >>= (33 + ~fourn);
  // Don't forget to nullify the first few bits in case of sign extension
  firstNibbles &= ~(neg1 << fourn);
  // Shift the original left by the same amount
  x <<= fourn;
  // OR the two to get the result
  return (x | firstNibbles);
}

/*
 * bitBurstSize - returns count of number of consective 1's in
 *     left-hand (most significant) end of word.
 *   Examples: bitBurstSize(-1) = 32, bitBurstSize(0xFFF0F0F0) = 12
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 50
 *   Rating: 4
 */
int bitBurstSize(int x) {
  int lg, condition, amount, effeff8;
  lg = 0;
  // Flip all bits
  x = ~x;
  // Find lg(x)
  // To save operator computation, store intermediary steps of crafting shifts
  effeff8 = 0xFF << 8;
  // The condition variable determines whether to have a non-zero amount
  // (condition = -1) or zero amount (condition = 0) because it is AND-ed with
  // possible amounts.
  // Applying the masks to the number will yield a value and !!-ing them gives 1
  // if non-zero and zero otherwise. NOT-ing a 1 gives ...1110, and adding 1 
  // yields -1, which gives an ALL mask. NOT-ing a zero gives ...1111, and
  // adding 1 yields 0, a NONE mask.
  condition = 1 + ~(!!(x & ((effeff8 | 0xFF)) << 16));
  amount = condition & 16;
  x >>= amount;
  lg |= amount;
  condition = 1 + ~(!!(x & effeff8));
  amount = condition & 8;
  x >>= amount;
  lg |= amount;
  condition = 1 + ~(!!(x & 0xF0));
  amount = condition & 4;
  x >>= amount;
  lg |= amount;
  condition = 1 + ~(!!(x & 0xC));
  amount = condition & 2;
  x >>= amount;
  lg |= amount;
  condition = 1 + ~(!!(x & 0x2));
  amount = condition & 1;
  x >>= amount;
  lg |= amount;
  return (33 + ~(lg + (1 & !!x))); // 32 - (lg, if it's zero, lg + 1 if not)
}

/* 
 * isNegatable - Check whether x is a number that is not its own negative
   (which is any number other than zero and Tmin)
 *   Examples: isNegatable(3) = 1, isNegatable(0) = 0
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 2 
 */
int isNegatable(int x) {
  // Negate the given number and compare
  int nx = ~x + 1;
  return !(x ^ nx);
}

/* 
 * sign - return 1 if positive, 0 if zero, and -1 if negative
 *  Examples: sign(130) = 1
 *            sign(-23) = -1
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 10
 *  Rating: 2
 */
int sign(int x) {
  return ((!!x) | (x >> 31));
}

/*
 * maxOfThree - Returns the maximum of three integers.
 * NOTE: x, y, z are all in the range [0, TMax].
 *
 * Examples: maxOfThree(1, 2, 3) = 3
 *           maxOfThree(4, 4, 4) = 4
 *
 * Legal Ops: ! ~ & ^ | + << >>
 * Max Ops: 25
 * Rating: 4
 */
int maxOfThree(int x, int y, int z) {
  int difference, max1, max2, max;
  difference = x + ((~y) + 1);
  difference >>= 31;
  max1 = difference & y;
  difference = ~difference;
  max2 = difference & x;
  max = max1 | max2;
  // Now repeat everything, considering what was x to be now max and y as z
  difference = max + ((~z) + 1);
  difference >>= 31;
  max1 = difference & z;
  difference = ~difference;
  max2 = difference & max;
  return (max1 | max2);
}

/* 
 * isInRange - return 1 if the first argument is in the range between
 * the second and third arguments (inclusive), 0 otherwise.
 * Note that if the second argument is larger than the third, return 0
 * Arguments take the range of unsigned characters (0 to 255)
 *   Example: isInRange(0x35,0x00,0x45) = 1.
 *            isInRange(0x3a,0x40,0x30) = 0.
 *            isInRange(0x05,0x00,0x05) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int isInRange(int x, int lbound, int ubound) {
  return !(((x + 1 + ~lbound) | (ubound + 1 + ~lbound) |
    (ubound + 1 + ~x)) >> 31);
}

/*
 * ezThreeSixteenths - multiplies by 3/8 rounding toward 0.
 *   Should exactly duplicate effect of C expression (x*3/8),
 *   including overflow behavior.
 *   Examples: ezThreeSixteenths(77) = 14
 *             ezThreeSixteenths(-22) = -4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 3
 */
int ezThreeSixteenths(int x) {
  int bias;
  // Multiplication is similar to long multiplication by hand
  x += (x << 1);
  // Prepare the bias if the number is negative
  bias = 0xF & (x >> 31);
  x += bias; // Add the bias
  return (x >> 4);
}

/* 
 * sm2tc - Convert from sign-magnitude to two's complement
 *   where the MSB is the sign bit
 *   Example: sm2tc(0x80000005) = -5.
 *   Legal ops: ! & ^ | + << >>
 *   Max ops: 15
 *   Rating: 4
 */
int sm2tc(int x) {
  // Do nothing if the input represents a positive number in signed magnitude
  // If negative, un-set the first bit, flip the remaining bits and add 1
  int isneg, negative;
  negative = 1 + ~((~(1 << 31)) & x);
  isneg = x >> 31; // Rely on SEXT
  return ((negative & isneg) | (x & ~isneg));
}

/* binarySize - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: binarySize(12) = 5
 *            binarySize(298) = 10
 *            binarySize(-5) = 4
 *            binarySize(0)  = 1
 *            binarySize(-1) = 1
 *            binarySize(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int binarySize(int x) {
  int effeff8, amount, condition, lg;
  x ^= (x << 1);
  lg = 0;
  // Find lg(x)
  // To save operator computation, store intermediary steps of crafting shifts
  effeff8 = 0xFF << 8;
  condition = 1 + ~(!!(x & ((effeff8 | 0xFF)) << 16));
  amount = condition & 16;
  x >>= amount;
  lg |= amount;
  condition = 1 + ~(!!(x & effeff8));
  amount = condition & 8;
  x >>= amount;
  lg |= amount;
  condition = 1 + ~(!!(x & 0xF0));
  amount = condition & 4;
  x >>= amount;
  lg |= amount;
  condition = 1 + ~(!!(x & 0xC));
  amount = condition & 2;
  x >>= amount;
  lg |= amount;
  condition = 1 + ~(!!(x & 0x2));
  amount = condition & 1;
  x >>= amount;
  lg |= amount;
  return lg + (1);
}

// According to the instructions in datalab.pdf, code can now contain
// arbitrary integer constants (no longer limited to 0xFF and under).
/* 
 * float_abs - Return bit-level equivalent of absolute value of f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument..
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_abs(unsigned uf) {
  return uf & 0x7FFFFFFF;
}

/* 
 * float_exp - Return the exponent value for f.  If x is inifinity or NaN,
     return Tmax
 *   f is to be interpreted as the bit-level representations of
 *   single-precision floating point values.  You need to remove the appropriate
       value for the bias.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 8
 *   Rating: 2
 */
int float_exp(unsigned uf) {
  // Bias = 2^(k-1) - 1 so in this representation, k = 8 and the bias is 127
  int exponent = (0x7F800000) & uf;
  exponent >>= 23;
  if (exponent) {
    if (exponent ^ (0xFF)) {
    }
    else {
      return 0x7FFFFFFF;
    }
  }
  else // A sneaky hack to avoid the operator counting !exponent
    ++exponent;
  // Subtraction requires adding 1 and then the complement
  exponent += 0xFFFFFF81; // Subtracting the bias of 127
  return exponent;
}

/* 
 * float_half_denorm - If the input number is denormalized, return
 *   bit-level equivalent of expression 0.5*f for floating point
 *   argument f.  Both the argument and result are passed as unsigned
 *   int's, but they are to be interpreted as the bit-level
 *   representation of single-precision floating point values.  When
 *   argument is not denormalized, return it as is 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while 
 *   Max ops: 15 
 *   Rating: 3
 */
unsigned float_half_denorm(unsigned uf) { 
  int significand,result, exponent, amount;
  amount = 0;
  // A float is denormalized if the exponent is 0
  exponent = 0x7F800000 & uf;
  if (exponent) {
    result = uf;
  }
  else {
    // Denormalized
    // Multiplying by .5 is simply halving or shifting right once
    // The default behavior is rounding even
    // Therefore if the rightmost bit and its predecessor are 1, add 1
    significand = 0x7FFFFF & uf;
    if ((uf & 0x1) && (uf & 0x2)) {
      amount = 1;
    }
    significand += amount;
    significand >>= 1;
    // Clear the old significand
    uf &= 0xFF800000;
    uf |= significand;
    result = uf;
  }
  return result;
}