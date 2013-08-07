#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: all_rivers (-indexes)\n";
static char *fmt = "%s %s %s %s %s\n";

int main(int argc,char **argv)
{
  int curr_arg;
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  char card_string[5][3];
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

  for (n = 0; n < 5; n++)
    card_string[n][2] = 0;

  for (r = 0; r < POKER_52_5_PERMUTATIONS; r++) {
    get_permutation_instance_five(
      NUM_CARDS_IN_DECK,
      &m,&n,&o,&p,&q,r);

    if (!bIndexes) {
      card_string_from_card_value(m,card_string[0]);
      card_string_from_card_value(n,card_string[1]);
      card_string_from_card_value(o,card_string[2]);
      card_string_from_card_value(p,card_string[3]);
      card_string_from_card_value(q,card_string[4]);

      printf(fmt,
        card_string[0],card_string[1],card_string[2],card_string[3],card_string[4]);
    }
    else
      printf("%02d %02d %02d %02d %02d\n",m,n,o,p,q);
  }

  return 0;
}
