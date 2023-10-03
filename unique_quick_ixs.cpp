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
"usage: unique_quick_ixs (-verbose) hands_and_types_filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static int quick_ixs[POKER_52_5_PERMUTATIONS];

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  int retval;
  struct hand_and_type *hands_and_types;
  int unique_count;
  int last_ix;

  if ((argc < 2) || (argc > 3)) {
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

    quick_ixs[hands_and_types[n].quick_ix] += 1;
  }

  if (n < POKER_52_5_PERMUTATIONS) {
    printf("invalid %s\n",argv[curr_arg]);
    return 4;
  }

  unique_count = 0;

  for (n = 0; n < POKER_52_5_PERMUTATIONS; n++) {
    if (quick_ixs[n]) {
      unique_count++;
      last_ix = n;
    }
  }

  printf("%d (%d)\n",unique_count,last_ix);

  return 0;
}
