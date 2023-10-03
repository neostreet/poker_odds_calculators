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
"usage: all_hands hands_and_types_filename\n";

static char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int retval;
  struct hand_and_type *hands_and_types;
  char card_string[3];

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  retval = read_hands_and_types(argv[1],&hands_and_types);

  if (retval) {
    printf("read_hands_and_types() failed: %d\n",retval);
    return 2;
  }

  card_string[2] = 0;

  for (n = 0; n < POKER_52_5_PERMUTATIONS; n++) {
    printf("%7d %4d ",hands_and_types[n].hand_ix,hands_and_types[n].quick_ix);

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
