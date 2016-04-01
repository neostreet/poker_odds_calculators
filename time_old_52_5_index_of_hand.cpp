#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: time_old_52_5_index_of_hand (-skip_index_step) exit_count\n";

static int index_of_hand(int *cards);

int main(int argc,char **argv)
{
  int m;
  int curr_arg;
  bool bSkipIndexStep;
  int exit_count;
  int cards[NUM_CARDS_IN_HAND];
  time_t start_sec;
  time_t stop_sec;
  int ix;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bSkipIndexStep = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-skip_index_step"))
      bSkipIndexStep = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  sscanf(argv[curr_arg],"%d",&exit_count);

  time(&start_sec);

  for (m = 0; m < POKER_52_5_PERMUTATIONS; m++) {
    if (m == exit_count)
      break;

    get_permutation_instance_five(
      NUM_CARDS_IN_DECK,
      &cards[0],&cards[1],&cards[2],&cards[3],&cards[4],m);

    if (!bSkipIndexStep)
      ix = index_of_hand(cards);
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
  int p;
  int q;
  int r;

  r = 0;

  for (m = 0; m < NUM_CARDS_IN_DECK - 4; m++) {
    for (n = m + 1; n < NUM_CARDS_IN_DECK - 3; n++) {
      for (o = n + 1; o < NUM_CARDS_IN_DECK - 2; o++) {
        for (p = o + 1; p < NUM_CARDS_IN_DECK - 1; p++) {
          for (q = p + 1; q < NUM_CARDS_IN_DECK; q++) {
            if ((m == cards[0]) && (n == cards[1]) && (o == cards[2]) &&
                (p == cards[3]) && (q == cards[4])) {
              return r;
            }

            r++;
          }
        }
      }
    }
  }
}
