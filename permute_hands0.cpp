#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: permute_hands0 (-card_strings)\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bCardStrings;
  int cards[NUM_CARDS_IN_HAND];
  char card_string[3];

  if (argc > 3) {
    printf(usage);
    return 1;
  }

  bCardStrings = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-card_strings"))
      bCardStrings = true;
    else
      break;
  }

  card_string[2] = 0;

  for (m = 0; m < POKER_52_5_PERMUTATIONS; m++) {
    get_permutation_instance_five(
      NUM_CARDS_IN_DECK,
      &cards[0],&cards[1],&cards[2],&cards[3],&cards[4],m);

    if (!bCardStrings)
      printf("%2d %2d %2d %2d %2d\n",cards[0],cards[1],cards[2],cards[3],cards[4]);
    else {
      for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
        card_string_from_card_value(cards[n],card_string);
        printf("%s",card_string);

        if (n < NUM_CARDS_IN_HAND - 1)
          putchar(' ');
        else
          putchar(0x0a);
      }
    }
  }

  return 0;
}
