#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: time_old_52_2_index_of_hand (-debug) loop_count\n";

static int index_of_hand(int *cards);

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bDebug;
  int loop_count;
  int cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND];
  time_t start_sec;
  time_t stop_sec;
  int ix;
  int bad_count;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bDebug = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug =  true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  sscanf(argv[curr_arg],"%d",&loop_count);
  time(&start_sec);

  for (n = 0; n < loop_count; n++) {
    bad_count = 0;

    for (m = 0; m < POKER_52_2_PERMUTATIONS; m++) {
      get_permutation_instance_two(
        NUM_CARDS_IN_DECK,
        &cards[0],&cards[1],m);

      ix = index_of_hand(cards);

      if (bDebug) {
        if (ix != m) {
          printf("bad index for %d: %d (%d %d)\n",m,ix,cards[0],cards[1]);
          bad_count++;
          // return 2;
        }
      }
    }

    if (bDebug)
      printf("\nbad_count = %d\n",bad_count);
  }

  time(&stop_sec);

  printf("elapsed seconds: %d\n",stop_sec - start_sec);

  return 0;
}

static int index_of_hand(int *cards)
{
  int m;
  int n;
  int o;

  o = 0;

  for (m = 0; m < NUM_CARDS_IN_DECK - 1; m++) {
    for (n = m + 1; n < NUM_CARDS_IN_DECK; n++) {
      if ((m == cards[0]) && (n == cards[1]))
        return o;

      o++;
    }
  }

  return -1;
}
