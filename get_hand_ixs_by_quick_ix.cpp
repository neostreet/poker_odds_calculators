#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: get_hand_ixs_by_quick_ix hands_and_types_filename quick_ix";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

int main(int argc,char **argv)
{
  int n;
  int retval;
  struct hand_and_type *hands_and_types;
  int quick_ix;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  retval = read_hands_and_types(argv[1],&hands_and_types);

  if (retval) {
    printf("read_hands_and_types() failed: %d\n",retval);
    return 2;
  }

  sscanf(argv[2],"%d",&quick_ix);

  for (n = 0; n < POKER_52_5_PERMUTATIONS; n++) {
    if (hands_and_types[n].quick_ix == quick_ix)
      printf("%d\n",n);
  }

  return 0;
}
