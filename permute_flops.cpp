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
  Flop flop;
  int flop_counts[NUM_FLOP_TYPES];
  double pct;
  time_t start_time;
  time_t end_time;

  time(&start_time);

  for (n = 0; n < NUM_FLOP_TYPES; n++)
    flop_counts[n] = 0;

  for (p = 0; p < POKER_52_3_PERMUTATIONS; p++) {
    get_permutation_instance_three(
      NUM_CARDS_IN_DECK,
      &m,&n,&o,p);

    flop.NewCards(m,n,o);
    flop.Evaluate();
    flop_counts[flop.GetFlopType()]++;
  }

  time(&end_time);

  for (n = NUM_FLOP_TYPES - 1; (n >= 0); n--) {
    pct = (double)flop_counts[n] / (double)POKER_52_3_PERMUTATIONS;
    printf("%s %5d %9.6lf\n",flop_type_abbrevs[n],flop_counts[n],pct);
  }

  printf("=========\n");
  printf("    %5d\n",POKER_52_3_PERMUTATIONS);

  printf("\ncomputation time: %d seconds\n",end_time - start_time);

  return 0;
}
