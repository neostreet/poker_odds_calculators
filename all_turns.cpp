#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: all_turns (-indexes)\n";
static char *fmt = "%s %s %s %s\n";

int main(int argc,char **argv)
{
  int curr_arg;
  int m;
  int n;
  int o;
  int p;
  int q;
  char card_string[4][3];
  bool bIndexes;

  bIndexes = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-indexes"))
      bIndexes = true;
    else
      break;
  }

  if (argc != curr_arg) {
    printf(usage);
    return 1;
  }

  for (n = 0; n < 4; n++)
    card_string[n][2] = 0;

  for (q = 0; q < POKER_52_4_PERMUTATIONS; q++) {
    get_permutation_instance_four(
      NUM_CARDS_IN_DECK,
      &m,&n,&o,&p,q);

    if (!bIndexes) {
      card_string_from_card_value(m,card_string[0]);
      card_string_from_card_value(n,card_string[1]);
      card_string_from_card_value(o,card_string[2]);
      card_string_from_card_value(p,card_string[3]);

      printf(fmt,
        card_string[0],card_string[1],card_string[2],card_string[3]);
    }
    else
      printf("%02d %02d %02d %02d\n",m,n,o,p);
  }

  return 0;
}
