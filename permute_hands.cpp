#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

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
  time_t start_time;
  time_t end_time;

  time(&start_time);

  for (n = 0; n < NUM_HAND_TYPES; n++)
    hand_counts[n] = 0;

  for (r = 0; r < POKER_52_5_PERMUTATIONS; r++) {
    get_permutation_instance_five(
      NUM_CARDS_IN_DECK,
      &m,&n,&o,&p,&q,r);

    hand.NewCards(m,n,o,p,q);
    hand.Evaluate();
    hand_counts[hand.GetHandType()]++;
  }

  time(&end_time);

  for (n = NUM_HAND_TYPES - 1; (n >= 0); n--) {
    pct = (double)hand_counts[n] / (double)POKER_52_5_PERMUTATIONS;
    printf("%s %9d %9.6lf\n",hand_type_abbrevs[n],hand_counts[n],pct);
  }

  printf("============\n");
  printf("   %9d\n",POKER_52_5_PERMUTATIONS);

  printf("\ncomputation time: %d seconds\n",end_time - start_time);

  return 0;
}
