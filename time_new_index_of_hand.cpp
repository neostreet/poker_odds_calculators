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

static int offsets[NUM_CARDS_IN_DECK] = {
  0,51,101,150,198,245,291,336,380,423,465,506,546,585,623,660,696,731,765,798,830,861,891,920,948,975,1001,1026,1050,1073,1095,1116,1136,1155,1173,1190,1206,1221,1235,1248,1260,1271,1281,1290,1298,1305,1311,1316,1320,1323,1325,1326
};

int main(int argc,char **argv)
{
  int m;
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
    for (m = 0; m < POKER_52_2_PERMUTATIONS; m++) {
      get_permutation_instance_two(
        NUM_CARDS_IN_DECK,
        &cards[0],&cards[1],m);

      ix = index_of_hand(cards);

      if (ix != m) {
        printf("bad index for %d: %d\n",m,ix);
        return 2;
      }
    }
  }

  time(&stop_sec);

  printf("elapsed seconds: %d\n",stop_sec - start_sec);

  return 0;
}

static int index_of_hand(int *cards)
{
  return offsets[cards[0]] + cards[1] - cards[0] - 1;
}
