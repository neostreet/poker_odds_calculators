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
  int s;
  int t;
  HoldemPokerHand holdem_hand;
  PokerHand hand;
  int hand_counts[NUM_HAND_TYPES];
  double pct;
  time_t start_time;
  time_t end_time;

  time(&start_time);

  for (n = 0; n < NUM_HAND_TYPES; n++)
    hand_counts[n] = 0;

  for (t = 0; t < POKER_52_7_PERMUTATIONS; t++) {
    get_permutation_instance_seven(
      NUM_CARDS_IN_DECK,
      &m,&n,&o,&p,&q,&r,&s,t);

    holdem_hand.NewCards(m,n,o,p,q,r,s);
    hand = holdem_hand.BestPokerHand();
    hand_counts[hand.GetHandType()]++;
  }

  time(&end_time);

  for (n = NUM_HAND_TYPES - 1; (n >= 0); n--) {
    pct = (double)hand_counts[n] / (double)POKER_52_7_PERMUTATIONS;
    printf("%s %9d %9.6lf\n",hand_type_abbrevs[n],hand_counts[n],pct);
  }

  printf("============\n");
  printf("   %9d\n",POKER_52_7_PERMUTATIONS);

  printf("\ncomputation time: %d seconds\n",end_time - start_time);

  return 0;
}
