#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: time_new_52_2_index_of_hand (-debug) (-swap) loop_count\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bDebug;
  bool bSwap;
  int loop_count;
  int cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND];
  time_t start_sec;
  time_t stop_sec;
  int ix;
  int bad_count;
  int temp;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bSwap = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug =  true;
    else if (!strcmp(argv[curr_arg],"-swap"))
      bSwap =  true;
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

    for (m = 0; ; ) {
      get_permutation_instance_two(
        NUM_CARDS_IN_DECK,
        &cards[0],&cards[1],m);

      if (bSwap) {
        temp = cards[0];
        cards[0] = cards[1];
        cards[1] = temp;
      }

      ix = get_52_2_index_of_hand(cards);

      if (ix != m) {
        if (bDebug)
          printf("bad index for %d: %d (%d %d)\n",m,ix,cards[0],cards[1]);

        bad_count++;
        // return 2;
      }

      m++;

      if (!bSwap) {
        if (cards[0] == NUM_CARDS_IN_DECK - 2)
          break;
      }
      else {
        if (cards[1] == NUM_CARDS_IN_DECK - 2)
          break;
      }
    }

    if (bad_count)
      printf("\nbad_count = %d\n",bad_count);
  }

  time(&stop_sec);

  printf("elapsed seconds: %d\n",stop_sec - start_sec);

  return 0;
}
