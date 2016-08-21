#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: get_hand_ixs_by_quick_ix (-verbose) hands_and_types_filename quick_ix";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bVerbose;
  int retval;
  struct hand_and_type *hands_and_types;
  int quick_ix;

  if ((argc < 3) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
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
        printf("%d ",n);

        for (m = 0; m < NUM_CARDS_IN_HAND; m++) {
          printf("%d",hands_and_types[n].cards[m]);

          if (m < NUM_CARDS_IN_HAND - 1)
            putchar(' ');
          else
            putchar(0x0a);
        }
      }
    }
  }

  return 0;
}
