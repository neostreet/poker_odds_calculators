#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: permute_starting_3card_hands0 (-card_strings) (-print_offset)\n"
"  (-unique_first_cards)\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bCardStrings;
  bool bPrintOffset;
  bool bUniqueFirstCards;
  int cards[3];
  char card_string[3];
  int prev_first_card;

  if (argc > 4) {
    printf(usage);
    return 1;
  }

  bCardStrings = false;
  bPrintOffset = false;
  bUniqueFirstCards = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-card_strings"))
      bCardStrings = true;
    else if (!strcmp(argv[curr_arg],"-print_offset"))
      bPrintOffset = true;
    else if (!strcmp(argv[curr_arg],"-unique_first_cards"))
      bUniqueFirstCards = true;
    else
      break;
  }

  card_string[2] = 0;

  for (m = 0; m < POKER_52_3_PERMUTATIONS; m++) {
    get_permutation_instance_three(
      NUM_CARDS_IN_DECK,
      &cards[0],&cards[1],&cards[2],m);

    if (bUniqueFirstCards && (m > 0) && (cards[0] == prev_first_card))
      continue;

    if (bPrintOffset)
      printf("%4d ",m);

    if (!bCardStrings)
      printf("%2d %2d %d\n",cards[0],cards[1],cards[2]);
    else {
      for (n = 0; n < 3; n++) {
        card_string_from_card_value(cards[n],card_string);
        printf("%s",card_string);

        if (n < 2)
          putchar(' ');
        else
          putchar(0x0a);
      }
    }

    prev_first_card = cards[0];
  }

  return 0;
}
