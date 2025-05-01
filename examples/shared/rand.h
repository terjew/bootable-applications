// https://gist.github.com/tommyettinger/46a874533244883189143505d203312c

unsigned int currentRand;

void srand(unsigned int seed)
{
  currentRand = seed;
}

unsigned int rand(void)
{
  unsigned int z = (currentRand += 0x6D2B79F5UL);
  z = (z ^ (z >> 15)) * (z | 1UL);
  z ^= z + (z ^ (z >> 7)) * (z | 61UL);
  return z ^ (z >> 14);
}

int randRange(int min, int max) {
  return min + ((int)rand()) % (max - min + 1);
}

