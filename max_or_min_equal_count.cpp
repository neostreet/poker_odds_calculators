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
"usage: max_or_min_equal_count (-verbose) (-max) hands_and_types_filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bVerbose;
  bool bMax;
  int retval;
  struct hand_and_type *hands_and_types;
  int max_or_min_count;
  int max_or_min_count_start_ix;
  int curr_count;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bMax = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-max"))
      bMax = true;
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

  if (bMax)
    max_or_min_count = 0;
  else
    max_or_min_count = POKER_52_5_PERMUTATIONS;

  n = 0;

  for ( ; ; ) {
    for (m = n + 1; m < POKER_52_5_PERMUTATIONS; m++) {
      if (hands_and_types[m].quick_ix != hands_and_types[n].quick_ix)
        break;
    }

    curr_count = m - n;

    if (bMax) {
      if (curr_count > max_or_min_count) {
        max_or_min_count = curr_count;
        max_or_min_count_start_ix = n;
      }
    }
    else {
      if (curr_count < max_or_min_count) {
        max_or_min_count = curr_count;
        max_or_min_count_start_ix = n;
      }
    }

    if (m == POKER_52_5_PERMUTATIONS)
      break;

    n = m;
  }

  printf("max_or_min_count = %d, max_or_min_count_start_ix = %d\n",max_or_min_count,max_or_min_count_start_ix);

  return 0;
}
