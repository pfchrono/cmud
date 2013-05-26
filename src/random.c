/* file          : random.c
 * date          : May 5th, 2003
 * version       : 0.1
 *
 * This file contains a simple algorithm to create pseudo-random
 * numbers, and a few functions to roll dices of specific sizes.
 */
#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/resource.h>

/* include main header file */
#include "mud.h"

void init_mm()
{
    srandom(time(NULL) ^ getpid());
    return;
}
long number_mm(void)
{
    return random() >> 6;
}

/* local seed variable, used to create pseudo random numbers */
static unsigned int iSeed;

double rand_seed   ( void );

/* returns a random number between a and b */
int random_range(int a, int b)
{
  return ((int)(rand_seed() * ((double) (b) - (a) + 1))) + (a);
}

/* return a random number between 1 and 100 */
int random_percent()
{
  return random_range(1, 100);
}

/* Roll some amount of dices of a specific size */
int roll_dice(int amount, int size)
{
  int i;
  int result = 0;

  for (i = 1; i <= amount; i++)
  {
    result += random_range(1, size);
  }

  return result;
}

/* this function initializes the randomize functions */
void init_dice()
{
  iSeed = (unsigned int) time(NULL);
}

/* Local function that returns a random double between 0 and 1 */
double rand_seed()
{
  static unsigned int a = 1588635695, m = 4294967291U, q = 2, r = 1117695901;

  iSeed = a * (iSeed % q) - r * (iSeed / q);

  return ((double) iSeed / (double) m);
}

/*
 * Generate a random number.
 */
int number_range(int from, int to)
{
    unsigned int power;
    int number;
    if (from == 0 && to == 0)
        return 0;
    if ((to = to - from + 1) <= 1)
        return from;
    for (power = 2; power < to; power <<= 1);
    while ((number = number_mm() & (power - 1)) >= to);
    return from + number;
}
