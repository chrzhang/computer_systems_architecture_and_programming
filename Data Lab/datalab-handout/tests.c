/* Testing Code */

#include <limits.h>
#include <math.h>

/* Routines used by floation point test code */

/* Convert from bit level representation to floating point number */
float u2f(unsigned u) {
  union {
    unsigned u;
    float f;
  } a;
  a.u = u;
  return a.f;
}

/* Convert from floating point number to bit-level representation */
unsigned f2u(float f) {
  union {
    unsigned u;
    float f;
  } a;
  a.f = f;
  return a.u;
}

int test_bitNand(int x, int y)
{
  return ~(x&y);
}
int test_isEqualThree(int x, int y, int z) {
  return ((x==y ) && (x==z));
}
int test_swap30(int x) {
  int zeroBits = (x & 0x11111111)<<3; //mask out the 0th bits
  int threeBits = (x & 0x88888888)>>3; //mask out the 3rd bits
  int remainingBits=x & 0x66666666; //mask out remaining bits
  if(x<0)
    x=x+0x20000000;
  return (threeBits) | (zeroBits) | (remainingBits);
}
int test_logicalByteAdd(int x, int y)
{
  return x+y;
}
int test_rotateNibbleLeft(int x, int n) {
  unsigned u = (unsigned) x;
  int i;
  for (i = 0; i < n; i++) {
    unsigned msb = (u >> 28)&0xf;
      unsigned rest = u << 4;
      u = rest | msb;
  }
  return (int) u;
}
int test_bitBurstSize(int x) {
  int result = 0;
  int i;
  for (i = 31; i >= 0; i--) {
      int bit = (x >> i) & 0x1;
      if (!bit)
   break;
      result ++;
  }
  return result;
}
int test_isNegatable(int x)
{
   return (x==0) || (x==0x80000000);
}
int test_sign(int x) {
    if ( !x ) return 0;
    return (x < 0) ? -1 : 1;
}
int test_maxOfThree(int x, int y, int z) {
  int maxOfxy = (x > y) ? x : y;
  return (maxOfxy > z) ? maxOfxy : z;
}
int test_isInRange(int x,int lower, int upper) {
  return (lower <= x) && (x <= upper);
}
int test_ezThreeSixteenths(int x)
{
  return (x*3)/16;
}
int test_sm2tc(int x) {
  int sign = x < 0;
  int mag = x & 0x7FFFFFFF;
  return sign ? -mag : mag;
}
int test_binarySize(int x) {
    unsigned int a, cnt;
    if(x<0) {
      x=-x-1;
    }
    a = (unsigned int)x;
    for (cnt=0; a; a>>=1, cnt++)
        ;
    return (int)(cnt + 1);
}
unsigned test_float_abs(unsigned uf) {
  float f = u2f(uf);
  unsigned unf = f2u(-f);
  if (isnan(f))
    return uf;
  /* An unfortunate hack to get around a limitation of the BDD Checker */
  if ((int) uf < 0)
      return unf;
  else
      return uf;
}
int test_float_exp(unsigned uf) {
float f = u2f(uf);
 int exp = (uf&0x7f800000) >> 23;
if(!isfinite(f))
  return 0x7fffffff;
if(exp == 0)
  exp++;
return exp - 127;
}
unsigned test_float_half_denorm(unsigned uf) {
  float f = u2f(uf);
  float hf = 0.5*f;
  if(uf&0x7f800000)
    return uf;
  else
    return f2u(hf);
}
