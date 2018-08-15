#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: get_52_2_index_of_hand card card\n";

int main(int argc,char **argv)
{
  int cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND];
  int ix;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&cards[0]);
  sscanf(argv[2],"%d",&cards[1]);

  ix = get_52_2_index_of_hand(cards);

  printf("%d\n",ix);

  return 0;
}
