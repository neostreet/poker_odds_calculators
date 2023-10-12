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
"usage: get_quick_ix3 (-verbose) (-card_ixs) (-no_ixs) hands_and_types_filename\n"
"  card_str card_str card_str card_str card_str\n";
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
    retval = card_value_from_card_string(argv[curr_arg+n],&work_hand.cards[n]);

    if (retval) {
      printf("card_value_from_cards_string() failed on card %s: %d\n",argv[curr_arg+n],retval);
      return 4;
    }
  }

  retval = find_hand(&work_hand,hands_and_types,0,&found);

  if (!retval) {
    printf("find_hand() failed\n");
    return 5;
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
