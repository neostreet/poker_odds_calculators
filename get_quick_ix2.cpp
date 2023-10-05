#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: get_quick_ix2 (-verbose) (-card_ixs) (-no_ixs) hands_and_types_filename\n"
"  card_ix card_ix card_ix card_ix card_ix\n";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bVerbose;
  bool bCardIxs;
  bool bNoIxs;
  int retval;
  struct hand_and_type *hands_and_types;
  hand work_hand;
  char card_string[3];
  struct hand_and_type *found;

  if ((argc < 7) || (argc > 10)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bCardIxs = false;
  bNoIxs = false;

  card_string[2] = 0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-card_ixs"))
      bCardIxs = true;
    else if (!strcmp(argv[curr_arg],"-no_ixs"))
      bNoIxs = true;
    else
      break;
  }

  if (argc - curr_arg != 6) {
    printf(usage);
    return 2;
  }

  retval = read_hands_and_types(argv[curr_arg],&hands_and_types);

  curr_arg++;

  if (retval) {
    printf("read_hands_and_types() failed: %d\n",retval);
    return 3;
  }

  for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
    sscanf(argv[curr_arg+n],"%d",&work_hand.cards[n]);
  }

  found = (struct hand_and_type *)bsearch(&work_hand,hands_and_types,
    POKER_52_5_PERMUTATIONS,
    sizeof (struct hand_and_type),compare_key);

  if (found == NULL) {
    printf("bsearch failed\n");
    return 4;
  }

  if (!bVerbose)
    printf("%4d\n",found->quick_ix);
  else {
    if (!bNoIxs)
      printf("%4d ",found->quick_ix);

    for (m = 0; m < NUM_CARDS_IN_HAND; m++) {
      if (!bCardIxs) {
        card_string_from_card_value(
          found->cards[m],
          card_string);
        printf("%s ",card_string);
      }
      else
        printf("%d ",found->cards[m]);
    }

    printf("%s\n",plain_hand_types[found->hand_type]);
  }

  return 0;
}
