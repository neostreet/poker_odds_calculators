#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

int main(int argc,char **argv)
{
  int cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND];
  int ix;

  for (cards[0] = 0; cards[0] < NUM_CARDS_IN_DECK; cards[0]++) {
    for (cards[1] = 0; cards[1] < NUM_CARDS_IN_DECK; cards[1]++) {
      if (cards[0] != cards[1]) {
        ix = get_52_2_index_of_hand2(cards);
        printf("%d\n",ix);
      }
    }
  }

  return 0;
}
