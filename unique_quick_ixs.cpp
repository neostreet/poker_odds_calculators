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
"usage: unique_quick_ixs (-verbose) (-no_quick_ix) hands_and_types_filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

struct quick_ix_count_info {
  int quick_ix_count;
  int first_hand_ix;
};

static struct quick_ix_count_info quick_ix_counts[POKER_52_5_PERMUTATIONS];

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bVerbose;
  bool bNoQuickIx;
  int retval;
  struct hand_and_type *hands_and_types;
  char card_string[3];
  int unique_count;
  int total_count;
  int last_ix;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bNoQuickIx = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-no_quick_ix"))
      bNoQuickIx = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  retval = read_hands_and_types(argv[curr_arg],&hands_and_types);

  if (retval) {
    printf("read_hands_and_types() failed: %d\n",retval);
    return 3;
  }

  for (n = 0; n < POKER_52_5_PERMUTATIONS; n++) {
    if (hands_and_types[n].hand_ix != n)
      break;

    quick_ix_counts[hands_and_types[n].quick_ix].quick_ix_count += 1;

    if (quick_ix_counts[hands_and_types[n].quick_ix].quick_ix_count == 1)
      quick_ix_counts[hands_and_types[n].quick_ix].first_hand_ix = n;
  }

  if (n < POKER_52_5_PERMUTATIONS) {
    printf("invalid %s\n",argv[curr_arg]);
    return 4;
  }

  unique_count = 0;
  total_count = 0;

  card_string[2] = 0;

  for (n = 0; n < POKER_52_5_PERMUTATIONS; n++) {
    if (quick_ix_counts[n].quick_ix_count) {
      if (bVerbose) {
        if (!bNoQuickIx) {
          printf("%7d %4d %7d ",n,quick_ix_counts[n].quick_ix_count,
            quick_ix_counts[n].first_hand_ix);
        }
        else {
          printf("%4d %7d ",quick_ix_counts[n].quick_ix_count,
            quick_ix_counts[n].first_hand_ix);
        }

        for (m = 0; m < NUM_CARDS_IN_HAND; m++) {
          card_string_from_card_value(
            hands_and_types[quick_ix_counts[n].first_hand_ix].cards[m],
            card_string);
          printf("%s ",card_string);
        }

        printf("%s\n",
          plain_hand_types[hands_and_types[quick_ix_counts[n].first_hand_ix].hand_type]);
      }
      else {
        unique_count++;
        total_count += quick_ix_counts[n].quick_ix_count;
        last_ix = n;
      }
    }
  }

  if (!bVerbose)
    printf("%d (%d %d)\n",unique_count,total_count,last_ix);

  return 0;
}
