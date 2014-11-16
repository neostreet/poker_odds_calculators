#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: starting_holdem_hands (-indexes) (-with_zero_deltas) (-dont_ignore_order)\n";
static char *fmt[] = {
  "%s %s\n",
  "         0 %s %s\n",
};

int main(int argc,char **argv)
{
  int curr_arg;
  int m;
  int n;
  int o;
  char card_string[2][3];
  bool bIndexes;
  bool bWithZeroDeltas;
  bool bDontIgnoreOrder;

  bIndexes = false;
  bWithZeroDeltas = false;
  bDontIgnoreOrder = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-indexes"))
      bIndexes = true;
    else if (!strcmp(argv[curr_arg],"-with_zero_deltas"))
      bWithZeroDeltas = true;
    else if (!strcmp(argv[curr_arg],"-dont_ignore_order"))
      bDontIgnoreOrder = true;
    else
      break;
  }

  if (argc != curr_arg) {
    printf(usage);
    return 1;
  }

  for (n = 0; n < 2; n++)
    card_string[n][2] = 0;

  if (!bDontIgnoreOrder) {
    for (o = 0; o < POKER_52_2_PERMUTATIONS; o++) {
      get_permutation_instance_two(
        NUM_CARDS_IN_DECK,
        &m,&n,o);

      if (!bIndexes) {
        card_string_from_card_value(m,card_string[0]);
        card_string_from_card_value(n,card_string[1]);

        if (rank_of(m) >= rank_of(n))
          printf(fmt[bWithZeroDeltas],card_string[0],card_string[1]);
        else
          printf(fmt[bWithZeroDeltas],card_string[1],card_string[0]);
      }
      else
        printf("%02d %02d\n",m,n);
    }
  }
  else {
    for (m = 0; m < NUM_CARDS_IN_DECK; m++) {
      for (n = 0; n < NUM_CARDS_IN_DECK; n++) {
        if (m != n) {
          if (!bIndexes) {
            card_string_from_card_value(m,card_string[0]);
            card_string_from_card_value(n,card_string[1]);

            printf(fmt[bWithZeroDeltas],card_string[0],card_string[1]);
          }
          else
            printf("%02d %02d\n",m,n);
        }
      }
    }
  }

  return 0;
}
