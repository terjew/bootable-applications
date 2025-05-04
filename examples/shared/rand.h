#ifndef RAND_H
#define RAND_H
/*  
Splitmix64

Written in 2015 by Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

/* This is a fixed-increment version of Java 8's SplittableRandom generator
   See http://dx.doi.org/10.1145/2714064.2660195 and
   http://docs.oracle.com/javase/8/docs/api/java/util/SplittableRandom.html

   It is a very fast generator passing BigCrush, and it can be useful if
   for some reason you absolutely want 64 bits of state. */

static EFI_UINT64 rand_state; /* The state can be seeded with any value. */

EFI_UINT64 rand_long()
{
  EFI_UINT64 z = (rand_state += 0x9e3779b97f4a7c15);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
  z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
  return z ^ (z >> 31);
}

EFI_UINT32 rand()
{
  return (EFI_UINT32)(rand_long() & 0xffffffff);
}

void srand(EFI_UINT64 seed)
{
  rand_state = seed;
}

EFI_GRAPHICS_OUTPUT_BLT_PIXEL randomColor()
{
    EFI_UINT32 color = rand() & 0xffffff;
    return *((EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)&color);
}

EFI_UINT32 randRange(EFI_UINT32 min, EFI_UINT32 max) {
  return min + ((EFI_UINT32)rand_long()) % (max - min + 1);
}

#endif
