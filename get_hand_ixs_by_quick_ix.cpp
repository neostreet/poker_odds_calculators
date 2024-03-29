#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: get_hand_ixs_by_quick_ix (-verbose) (-card_ixs) (-no_ixs)\n"
"  hands_and_types_filename quick_ix";
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
  int quick_ix;
  char card_string[3];

  if ((argc < 3) || (argc > 6)) {
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

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  retval = read_hands_and_types(argv[curr_arg],&hands_and_types);

  if (retval) {
    printf("read_hands_and_types() failed: %d\n",retval);
    return 3;
  }

  sscanf(argv[curr_arg+1],"%d",&quick_ix);

  for (n = 0; n < POKER_52_5_PERMUTATIONS; n++) {
    if (hands_and_types[n].quick_ix == quick_ix) {
      if (!bVerbose)
        printf("%d\n",n);
      else {
        if (!bNoIxs)
          printf("%7d ",n);

        for (m = 0; m < NUM_CARDS_IN_HAND; m++) {
          if (!bCardIxs) {
            card_string_from_card_value(
              hands_and_types[n].cards[m],
              card_string);
            printf("%s ",card_string);
          }
          else
            printf("%d ",hands_and_types[n].cards[m]);
        }

        printf("%s\n",plain_hand_types[hands_and_types[n].hand_type]);
      }
    }
  }

  return 0;
}
