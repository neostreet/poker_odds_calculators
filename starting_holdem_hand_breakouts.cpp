#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define NUM_SUIT_COMBOS (NUM_SUITS * NUM_SUITS)
#define NUM_SUIT_COMBOS_COLLAPSED ((NUM_SUIT_COMBOS - NUM_SUITS) / 2 + NUM_SUITS)

/* suit crosstab

  c c
  c d
  c h
  c s
  d c
  d d
  d h
  d s
  h c
  h d
  h h
  h s
  s c
  s d
  s h
  s s
*/

static char *suit_crosstab2[] = {
  "c c",
  "c d",
  "c h",
  "c s",
  "d d",
  "d h",
  "d s",
  "h h",
  "h s",
  "s s"
};

static int suit_combos_collapsed[NUM_SUIT_COMBOS_COLLAPSED];
static int suit_combos_map_to_collapsed[] = {
  0, /* c c */
  1, /* c d */
  2, /* c h */
  3, /* c s */
  1, /* d c */
  4, /* d d */
  5, /* d h */
  6, /* d s */
  2, /* h c */
  5, /* h d */
  7, /* h h */
  8, /* h s */
  3, /* s c */
  6, /* s d */
  8, /* s h */
  9 /* s s */
};

static int breakouts[NUM_SUIT_COMBOS_COLLAPSED];

int main(int argc,char **argv)
{
  int m;
  int n;
  int o;
  int suit_ix1;
  int suit_ix2;
  int map_ix;
  double pct;

  for (n = 0; n < NUM_SUIT_COMBOS_COLLAPSED; n++)
    breakouts[n] = 0;

  for (o = 0; ; ) {
    get_permutation_instance_two(
      NUM_CARDS_IN_DECK,
      &m,&n,o);

    o++;

    suit_ix1 = m / NUM_CARDS_IN_SUIT;
    suit_ix2 = n / NUM_CARDS_IN_SUIT;

    map_ix = suit_ix1 * NUM_SUITS + suit_ix2;

    breakouts[suit_combos_map_to_collapsed[map_ix]]++;

    if (m == NUM_CARDS_IN_DECK - 2)
      break;
  }

  for (n = 0; n < NUM_SUIT_COMBOS_COLLAPSED; n++) {
    pct = (double)breakouts[n] / (double)POKER_52_2_PERMUTATIONS;
    printf("%10d (%6.3lf) %s\n",breakouts[n],pct,suit_crosstab2[n]);
  }

  printf("\n%10d\n",POKER_52_2_PERMUTATIONS);

  return 0;
}
