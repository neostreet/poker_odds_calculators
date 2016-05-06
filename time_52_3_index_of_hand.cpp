#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: time_52_3_index_of_hand (-debug) index_algorithm loop_count\n";

static int index_of_hand0(int *cards);
static int index_of_hand1(int *cards);
static int index_of_hand2(int *cards);
static int index_of_hand3(int *cards);
static int index_of_hand4(int *cards);
static int index_of_hand5(int *cards);

static int ixs[NUM_CARDS_IN_DECK][NUM_CARDS_IN_DECK][NUM_CARDS_IN_DECK];
static int ixs2[NUM_CARDS_IN_DECK-2][NUM_CARDS_IN_DECK-2][NUM_CARDS_IN_DECK-2];

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bDebug;
  int index_algorithm;
  int loop_count;
  int cards[NUM_CARDS_IN_FLOP];
  time_t start_sec;
  time_t stop_sec;
  int ix;

  if ((argc < 3) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bDebug = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  sscanf(argv[curr_arg],"%d",&index_algorithm);

  if ((index_algorithm < 0) || (index_algorithm > 5)) {
    printf("invalid index_algorithm\n");
    return 3;
  }

  if (index_algorithm == 4) {
    for (m = 0; m < POKER_52_2_PERMUTATIONS; m++) {
      get_permutation_instance_three(
        NUM_CARDS_IN_DECK,
        &cards[0],&cards[1],&cards[2],m);

      ixs[cards[0]][cards[1]][cards[2]] = m;
    }
  }
  else if (index_algorithm == 5) {
    for (m = 0; m < POKER_52_2_PERMUTATIONS; m++) {
      get_permutation_instance_three(
        NUM_CARDS_IN_DECK,
        &cards[0],&cards[1],&cards[2],m);

      ixs2[cards[0]][cards[1]-1][cards[2]-2] = m;
    }
  }

  sscanf(argv[curr_arg+1],"%d",&loop_count);

  if (bDebug)
    loop_count = 1;

  if (!bDebug)
    time(&start_sec);

  for (n = 0; n < loop_count; n++) {
    for (m = 0; m < POKER_52_2_PERMUTATIONS; m++) {
      get_permutation_instance_three(
        NUM_CARDS_IN_DECK,
        &cards[0],&cards[1],&cards[2],m);

      switch (index_algorithm) {
        case 0:
          ix = index_of_hand0(cards);
          break;
        case 1:
          ix = index_of_hand1(cards);
          break;
        case 2:
          ix = index_of_hand2(cards);
          break;
        case 3:
          ix = index_of_hand3(cards);
          break;
        case 4:
          ix = index_of_hand4(cards);
          break;
        case 5:
          ix = index_of_hand5(cards);
          break;
      }

      if (ix != m) {
        printf("bad index for %d: %d\n",m,ix);
        return 4;
      }

      if (bDebug)
        printf("%d\n",ix);
    }
  }

  if (!bDebug) {
    time(&stop_sec);

    printf("elapsed seconds: %d\n",stop_sec - start_sec);
  }

  return 0;
}

static int index_of_hand0(int *cards)
{
  int m;
  int n;
  int local_cards[NUM_CARDS_IN_FLOP];

  for (m = 0; m < POKER_52_3_PERMUTATIONS; m++) {
    get_permutation_instance_three(
      NUM_CARDS_IN_DECK,
      &local_cards[0],&local_cards[1],&local_cards[2],m);

    for (n = 0; n < NUM_CARDS_IN_FLOP; n++) {
      if (cards[n] != local_cards[n])
        break;
    }

    if (n == NUM_CARDS_IN_FLOP)
      return m;
  }

  return 0;
}

static int index_of_hand1(int *cards)
{
  int m;
  int n;
  int o;
  int p;

  p = 0;

  for (m = 0; m < NUM_CARDS_IN_DECK - 2; m++) {
    for (n = m + 1; n < NUM_CARDS_IN_DECK - 1; n++) {
      for (o = n + 1; o < NUM_CARDS_IN_DECK; o++) {
        if ((m == cards[0]) && (n == cards[1]) & (o == cards[2]))
          return p;

        p++;
      }
    }
  }
}

static int index_of_hand2(int *cards)
{
  return 0;
}

static int index_of_hand3(int *cards)
{
  return 0;
}

static int index_of_hand4(int *cards)
{
  return ixs[cards[0]][cards[1]][cards[2]];
}

static int index_of_hand5(int *cards)
{
  return ixs2[cards[0]][cards[1]-1][cards[2]-2];
}
