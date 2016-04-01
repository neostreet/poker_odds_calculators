#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] = "usage: time_old_index_of_hand loop_count\n";

static int index_of_hand(int *cards);

int main(int argc,char **argv)
{
  int n;
  int loop_count;
  int cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND];
  time_t start_sec;
  time_t stop_sec;
  int ix;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&loop_count);

  time(&start_sec);

  for (n = 0; n < loop_count; n++) {
    for (cards[0] = 0; cards[0] < NUM_CARDS_IN_DECK - 1; cards[0]++) {
      for (cards[1] = cards[0] + 1; cards[1] < NUM_CARDS_IN_DECK; cards[1]++) {
        ix = index_of_hand(cards);
      }
    }
  }

  time(&stop_sec);

  printf("elapsed seconds: %d\n",stop_sec - start_sec);

  return 0;
}

static int index_of_hand(int *cards)
{
  int m;
  int n;
  int p;

  p = 0;

  for (m = 0; m < NUM_CARDS_IN_DECK - 1; m++) {
    for (n = m + 1; n < NUM_CARDS_IN_DECK; n++) {
      if (((m == cards[0]) && (n == cards[1])) ||
          ((m == cards[1]) && (n == cards[0]))) {
        return p;
      }
    }

    p++;
  }
}
