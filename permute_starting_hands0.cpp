#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static int hand_counts[NUM_HAND_ABBREVS];
static int premium_hand_counts[NUM_PREMIUM_HAND_ABBREVS];

static char usage[] =
"usage: permute_starting_hands0 (-usage) (-card_strings) (-print_offset)\n"
"  (-unique_first_cards) (-abbrev) (-premium) (-aggreg) (-aggreg2)\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bUsage;
  bool bCardStrings;
  bool bPrintOffset;
  bool bUniqueFirstCards;
  bool bAbbrev;
  bool bPremium;
  bool bAggreg;
  bool bAggreg2;
  int premium_ix;
  int abbrev_ix;
  int cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND];
  char hole_cards[6];
  char hole_cards_abbrev[4];
  int prev_first_card;
  int num_pairs;
  int num_suited_nonpairs;
  int num_nonsuited_nonpairs;

  bUsage = false;
  bCardStrings = false;
  bPrintOffset = false;
  bUniqueFirstCards = false;
  bAbbrev = false;
  bPremium = false;
  bAggreg = false;
  bAggreg2 = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-usage"))
      bUsage = true;
    else if (!strcmp(argv[curr_arg],"-card_strings"))
      bCardStrings = true;
    else if (!strcmp(argv[curr_arg],"-print_offset"))
      bPrintOffset = true;
    else if (!strcmp(argv[curr_arg],"-unique_first_cards"))
      bUniqueFirstCards = true;
    else if (!strcmp(argv[curr_arg],"-abbrev"))
      bAbbrev = true;
    else if (!strcmp(argv[curr_arg],"-premium"))
      bPremium = true;
    else if (!strcmp(argv[curr_arg],"-aggreg")) {
      bAggreg = true;
      bAbbrev = true;
    }
    else if (!strcmp(argv[curr_arg],"-aggreg2")) {
      bAggreg2 = true;
      bCardStrings = true;
    }
    else
      break;
  }

  if (bUsage) {
    printf(usage);
    return 0;
  }

  if (bAggreg && bAggreg2) {
    printf("can't specify both -aggreg and -aggreg2\n");
    return 1;
  }

  if (bAggreg) {
    if (bPremium) {
      for (n = 0; n < NUM_PREMIUM_HAND_ABBREVS; n++)
        premium_hand_counts[n] = 0;
    }
    else {
      for (n = 0; n < NUM_HAND_ABBREVS; n++)
        hand_counts[n] = 0;
    }
  }
  else if (bAggreg2) {
    num_pairs = 0;
    num_suited_nonpairs = 0;
    num_nonsuited_nonpairs = 0;
  }

  hole_cards[2] = ' ';
  hole_cards[5] = 0;
  hole_cards_abbrev[3] = 0;

  for (m = 0; ; ) {
    get_permutation_instance_two(
      NUM_CARDS_IN_DECK,
      &cards[0],&cards[1],m);

    m++;

    if (bUniqueFirstCards && (m > 0) && (cards[0] == prev_first_card))
      goto end_loop;

    if (bPrintOffset)
      printf("%4d ",m);

    if (bCardStrings || bAbbrev || bPremium) {
      for (n = 0; n < NUM_HOLE_CARDS_IN_HOLDEM_HAND; n++)
        card_string_from_card_value(cards[n],&hole_cards[n * 3]);

      if (bAbbrev || bPremium)
        get_abbrev(hole_cards,hole_cards_abbrev);

      if (bPremium) {
        if (!is_premium_hand(hole_cards_abbrev,&premium_ix)) {
          goto end_loop;
        }
        else if (bAggreg) {
          premium_hand_counts[premium_ix]++;
          goto end_loop;
        }
      }
      else if (bAggreg) {
        index_of_hand_abbrev(hole_cards_abbrev,&abbrev_ix);

        hand_counts[abbrev_ix]++;
        goto end_loop;
      }
      else if (bAggreg2) {
        if (hole_cards[0] == hole_cards[3])
          num_pairs++;
        else if (hole_cards[1] == hole_cards[4])
          num_suited_nonpairs++;
        else
          num_nonsuited_nonpairs++;

        goto end_loop;
      }

      if (bCardStrings) {
        if (!bAbbrev)
          printf("%s\n",hole_cards);
        else
          printf("%s %s\n",hole_cards,hole_cards_abbrev);
      }
      else if (bAbbrev)
        printf("%s\n",hole_cards_abbrev);
      else
        printf("%2d %2d\n",cards[0],cards[1]);
    }
    else
      printf("%2d %2d\n",cards[0],cards[1]);

    prev_first_card = cards[0];

end_loop:

    if (m == POKER_52_2_PERMUTATIONS)
      break;
  }

  if (bAggreg) {
    if (bPremium) {
      for (n = 0; n < NUM_PREMIUM_HAND_ABBREVS; n++)
        printf("%2d %s\n",premium_hand_counts[n],premium_hand_abbrevs[n]);
    }
    else {
      for (n = 0; n < NUM_HAND_ABBREVS; n++) {
        printf("%2d %s\n",hand_counts[n],hand_abbrevs[n]);
      }
    }
  }
  else if (bAggreg2) {
    printf("num_pairs = %d\n",num_pairs);
    printf("num_suited_nonpairs = %d\n",num_suited_nonpairs);
    printf("num_nonsuited_nonpairs = %d\n",num_nonsuited_nonpairs);
  }

  return 0;
}
