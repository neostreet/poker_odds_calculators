#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

int main(int argc,char **argv)
{
  int m;
  int n;
  int cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND];
  char card_string[3];

  for (m = 0; m < POKER_52_2_PERMUTATIONS; m++) {
    get_permutation_instance_two(
      NUM_CARDS_IN_DECK,
      &cards[0],&cards[1],m);

    if (argc == 1)
      printf("%2d %2d\n",cards[0],cards[1]);
    else {
      card_string[2] = 0;

      for (n = 0; n < NUM_HOLE_CARDS_IN_HOLDEM_HAND; n++) {
        card_string_from_card_value(cards[n],card_string);
        printf("%s",card_string);

        if (n < NUM_HOLE_CARDS_IN_HOLDEM_HAND - 1)
          putchar(' ');
        else
          putchar(0x0a);
      }
    }
  }

  return 0;
}
