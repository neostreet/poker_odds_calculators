#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] = "usage: permute_hands (-verbose)\n";

int card_values[NUM_CARDS_IN_DECK];

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  PokerHand hand(NUM_CARDS_IN_DECK);
  int hand_counts[NUM_HAND_TYPES];
  double pct;
  time_t start_time;
  time_t end_time;

  if (argc > 2) {
    printf(usage);
    return 1;
  }

  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (argc - curr_arg != 0) {
    printf(usage);
    return 2;
  }

  for (n = 0; n < NUM_CARDS_IN_DECK; n++)
    card_values[n] = n;

  time(&start_time);

  for (n = 0; n < NUM_HAND_TYPES; n++)
    hand_counts[n] = 0;

  for (r = 0; r < POKER_52_5_PERMUTATIONS; r++) {
    get_permutation_instance_five(
      NUM_CARDS_IN_DECK,
      &m,&n,&o,&p,&q,r);

    hand.NewCards(card_values[m],card_values[n],card_values[o],card_values[p],card_values[q]);
    hand.Evaluate();
    hand_counts[hand.GetHandType()]++;

    if (bVerbose)
      cout << hand << endl;
  }

  if (bVerbose)
    cout << endl;

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
