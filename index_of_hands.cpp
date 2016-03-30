#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define NUM_SUITED_NONPAIRS ((NUM_CARDS_IN_SUIT * (NUM_CARDS_IN_SUIT - 1)) / 2)
#define NUM_NONSUITED_NONPAIRS NUM_SUITED_NONPAIRS

static char usage[] =
"usage: index_of_hands (-card_strings)\n";

static char bad_rank[] = "bad rank in card%d: %s\n";
static char bad_suit[] = "bad suit in card%d: %s\n";

static int index_of_hand(int rank_ix1,int suit_ix1,int rank_ix2,int suit_ix2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bCardStrings;
  int cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND];
  int rank_ix[2];
  int suit_ix[2];
  int ix;
  char card_string[3];

  if (argc > 2) {
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

  for (cards[0] = 0; cards[0] < NUM_CARDS_IN_DECK - 1; cards[0]++) {
    rank_ix[0] = rank_of(cards[0]);
    suit_ix[0] = suit_of(cards[0]);

    for (cards[1] = cards[0] + 1; cards[1] < NUM_CARDS_IN_DECK; cards[1]++) {
      rank_ix[1] = rank_of(cards[1]);
      suit_ix[1] = suit_of(cards[1]);

      ix = index_of_hand(rank_ix[0],suit_ix[0],rank_ix[1],suit_ix[1]);

      printf("%d ",ix);

      if (!bCardStrings)
        printf("%d %d\n",cards[0],cards[1]);
      else {
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
  }

  return 0;
}

static int index_of_hand(int rank_ix1,int suit_ix1,int rank_ix2,int suit_ix2)
{
  int n;
  int work;
  int index;
  int num_other_cards;

  if (rank_ix1 == rank_ix2)
    return rank_ix1;

  if (suit_ix1 == suit_ix2)
    index = NUM_CARDS_IN_SUIT;
  else
    index = NUM_CARDS_IN_SUIT + NUM_SUITED_NONPAIRS;

  if (rank_ix1 > rank_ix2) {
    work = rank_ix1;
    rank_ix1 = rank_ix2;
    rank_ix2 = work;
  }

  num_other_cards = NUM_CARDS_IN_SUIT - 1;

  for (n = 0; n < rank_ix1; n++) {
    index += num_other_cards;
    num_other_cards--;
  }

  index += rank_ix2 - rank_ix1 - 1;

  return index;
}
