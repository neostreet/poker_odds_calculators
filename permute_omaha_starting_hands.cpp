#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: permute_omaha_starting_hands (-card_strings) (-print_offset) (-badugi)\n";

bool is_badugi(char *hole_cards);

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bCardStrings;
  bool bPrintOffset;
  bool bBadugi;
  int cards[NUM_HOLE_CARDS_IN_OMAHA_HAND];
  char hole_cards[12];

  if (argc > 4) {
    printf(usage);
    return 1;
  }

  bCardStrings = false;
  bPrintOffset = false;
  bBadugi = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-card_strings"))
      bCardStrings = true;
    else if (!strcmp(argv[curr_arg],"-print_offset"))
      bPrintOffset = true;
    else if (!strcmp(argv[curr_arg],"-badugi"))
      bBadugi = true;
    else
      break;
  }

  hole_cards[2] = ' ';
  hole_cards[5] = ' ';
  hole_cards[8] = ' ';
  hole_cards[11] = 0;

  for (m = 0; m < POKER_52_4_PERMUTATIONS; m++) {
    get_permutation_instance_four(
      NUM_CARDS_IN_DECK,
      &cards[0],&cards[1],&cards[2],&cards[3],m);

    for (n = 0; n < NUM_HOLE_CARDS_IN_OMAHA_HAND; n++)
      card_string_from_card_value(cards[n],&hole_cards[n * 3]);

    if (bBadugi) {
      if (!is_badugi(hole_cards))
        continue;
    }

    if (bPrintOffset)
      printf("%4d ",m);

    if (!bCardStrings)
      printf("%2d %2d %2d %2d\n",cards[0],cards[1],cards[2],cards[3]);
    else
      printf("%s\n",hole_cards);
  }

  return 0;
}

bool is_badugi(char *hole_cards)
{
  int m;
  int n;
  int p;

  for (n = 0; n < NUM_HOLE_CARDS_IN_OMAHA_HAND - 1; n++) {
    for (m = n + 1; m < NUM_HOLE_CARDS_IN_OMAHA_HAND; m++) {
      for (p = 0; p < 2; p++) {
        if (hole_cards[n * 3 + p] == hole_cards[m * 3 + p])
          return false;
      }
    }
  }

  return true;
}
