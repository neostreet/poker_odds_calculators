#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] = "usage: time_new_index_of_hand loop_count\n";

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
  int n;
  int index;
  int num_other_cards;

  index = 0;
  num_other_cards = NUM_CARDS_IN_DECK - 1;

  for (n = 0; n < cards[0]; n++) {
    index += num_other_cards;
    num_other_cards--;
  }

  index += cards[1] - cards[0] - 1;

  return index;
}
