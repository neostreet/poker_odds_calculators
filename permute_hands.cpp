#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static void get_permutation_instance(
  int set_size,int subset_size,
  int *m,int *n,int *o,int *p,int *q,
  int instance_ix
);

int main(int argc,char **argv)
{
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  PokerHand hand;
  int hand_counts[NUM_HAND_TYPES];
  double pct;

  for (n = 0; n < NUM_HAND_TYPES; n++)
    hand_counts[n] = 0;

  for (r = 0; r < POKER_52_5_PERMUTATIONS; r++) {
    get_permutation_instance(
      NUM_CARDS_IN_DECK,NUM_CARDS_IN_HAND,
      &m,&n,&o,&p,&q,r);

    hand.NewCards(m,n,o,p,q);
    hand.Evaluate();
    hand_counts[hand.GetHandType()]++;
  }

  for (n = NUM_HAND_TYPES - 1; (n >= 0); n--) {
    pct = (double)hand_counts[n] / (double)POKER_52_5_PERMUTATIONS;
    printf("%s %7d %9.6lf\n",hand_type_abbrevs[n],hand_counts[n],pct);
  }

  printf("==========\n");
  printf("   %7d\n",POKER_52_5_PERMUTATIONS);

  return 0;
}

static void get_permutation_instance(
  int set_size,int subset_size,
  int *m,int *n,int *o,int *p,int *q,
  int instance_ix
)
{
  if (instance_ix)
    goto after_return_point;

  for (*m = 0; *m < set_size - subset_size + 1; (*m)++) {
    for (*n = *m + 1; *n < set_size - subset_size + 2; (*n)++) {
      for (*o = *n + 1; *o < set_size - subset_size + 3; (*o)++) {
        for (*p = *o + 1; *p < set_size - subset_size + 4; (*p)++) {
          for (*q = *p + 1; *q < set_size - subset_size + 5; (*q)++) {
            return;

            after_return_point:
            ;
          }
        }
      }
    }
  }
}
