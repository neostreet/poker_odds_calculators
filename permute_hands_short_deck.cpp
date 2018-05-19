#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: permute_hands_short_deck (-debug) (-verbose)\n";

#define POKER_36_5_PERMUTATIONS 376992
#define NUM_CARDS_IN_SHORT_DECK 36

int card_values[NUM_CARDS_IN_SHORT_DECK] = {
   4,  5,  6,  7,  8,  9, 10, 11, 12,
  17, 18, 19, 20, 21, 22, 23, 24, 25,
  30, 31, 32, 33, 34, 35, 36, 37, 38,
  43, 44, 45, 46, 47, 48, 49, 50, 51
};

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  PokerHand hand(NUM_CARDS_IN_SHORT_DECK);
  int hand_counts[NUM_HAND_TYPES];
  double pct;
  time_t start_time;
  time_t end_time;
  HandType hand_type;

  if (argc > 3) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (argc - curr_arg != 0) {
    printf(usage);
    return 2;
  }

  time(&start_time);

  for (n = 0; n < NUM_HAND_TYPES; n++)
    hand_counts[n] = 0;

  for (r = 0; r < POKER_36_5_PERMUTATIONS; r++) {
    get_permutation_instance_five(
      NUM_CARDS_IN_SHORT_DECK,
      &m,&n,&o,&p,&q,r);

    if (bDebug) {
      printf("%6d: %2d (%2d) %2d (%2d) %2d (%2d) %2d (%2d) %2d (%2d)\n",
        r,
        card_values[m],m,
        card_values[n],n,
        card_values[o],o,
        card_values[p],p,
        card_values[q],q);
    }

    hand.NewCards(card_values[m],card_values[n],card_values[o],card_values[p],card_values[q]);

    hand.Evaluate();

    hand_type = hand.GetHandType();

    if (bVerbose && (hand_type == ROYAL_FLUSH)) {
      cout << hand << endl;
      printf("%6d: %2d (%2d) %2d (%2d) %2d (%2d) %2d (%2d) %2d (%2d)\n",
        r,
        card_values[m],m,
        card_values[n],n,
        card_values[o],o,
        card_values[p],p,
        card_values[q],q);
    }

    hand_counts[hand_type]++;
  }

  if (bDebug || bVerbose)
    cout << endl;

  time(&end_time);

  for (n = NUM_HAND_TYPES - 1; (n >= 0); n--) {
    pct = (double)hand_counts[n] / (double)POKER_36_5_PERMUTATIONS;
    printf("%s %9d %9.6lf\n",hand_type_abbrevs[n],hand_counts[n],pct);
  }

  printf("============\n");
  printf("   %9d\n",POKER_36_5_PERMUTATIONS);

  printf("\ncomputation time: %d seconds\n",end_time - start_time);

  return 0;
}
