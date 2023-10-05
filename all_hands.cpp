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
"usage: all_hands (-countcount) hands_and_types_filename\n";

static char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  int count;
  int retval;
  struct hand_and_type *hands_and_types;
  char card_string[3];

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  count = POKER_52_5_PERMUTATIONS;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-count",6)) {
      sscanf(&argv[curr_arg][6],"%d",&count);

      if (count > POKER_52_5_PERMUTATIONS)
        count = POKER_52_5_PERMUTATIONS;
    }
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

  card_string[2] = 0;

  for (n = 0; n < count; n++) {
    printf("%4d ",hands_and_types[n].quick_ix);

    for (m = 0; m < NUM_CARDS_IN_HAND; m++) {
      card_string_from_card_value(
        hands_and_types[n].cards[m],
        card_string);
      printf("%s ",card_string);
    }

    printf("%s\n",plain_hand_types[hands_and_types[n].hand_type]);
  }

  return 0;
}
