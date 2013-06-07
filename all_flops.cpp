#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: all_flops (-indexes) (-reverse) (-with_zero_deltas\n";
static char *fmt[] = {
  "%s %s %s\n",
  "%s %s %s          0\n",
};

static void get_permutation_instance(
  int set_size,
  int *m,int *n,int *o,
  int instance_ix
);

static void get_permutation_instance_reverse(
  int set_size,
  int *m,int *n,int *o,
  int instance_ix
);

int main(int argc,char **argv)
{
  int curr_arg;
  int m;
  int n;
  int o;
  int p;
  char card_string[3][3];
  bool bIndexes;
  bool bReverse;
  bool bWithZeroDeltas;

  bIndexes = false;
  bReverse = false;
  bWithZeroDeltas = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-indexes"))
      bIndexes = true;
    else if (!strcmp(argv[curr_arg],"-reverse"))
      bReverse = true;
    else if (!strcmp(argv[curr_arg],"-with_zero_deltas"))
      bWithZeroDeltas = true;
    else
      break;
  }

  if (argc != curr_arg) {
    printf(usage);
    return 1;
  }

  for (n = 0; n < 3; n++)
    card_string[n][2] = 0;

  for (p = 0; p < POKER_52_3_PERMUTATIONS; p++) {
    if (!bReverse) {
      get_permutation_instance(
        NUM_CARDS_IN_DECK,
        &m,&n,&o,p);
    }
    else {
      get_permutation_instance_reverse(
        NUM_CARDS_IN_DECK,
        &m,&n,&o,p);
    }

    if (!bIndexes) {
      card_string_from_card_value(m,card_string[0]);
      card_string_from_card_value(n,card_string[1]);
      card_string_from_card_value(o,card_string[2]);

      printf(fmt[bWithZeroDeltas],card_string[0],card_string[1],card_string[2]);
    }
    else
      printf("%02d %02d %02d\n",m,n,o);
  }

  return 0;
}

static void get_permutation_instance(
  int set_size,
  int *m,int *n,int *o,
  int instance_ix
)
{
  if (instance_ix)
    goto after_return_point;

  for (*m = 0; *m < set_size - 2; (*m)++) {
    for (*n = *m + 1; *n < set_size - 1; (*n)++) {
      for (*o = *n + 1; *o < set_size; (*o)++) {
        return;

        after_return_point:
        ;
      }
    }
  }
}

static void get_permutation_instance_reverse(
  int set_size,
  int *m,int *n,int *o,
  int instance_ix
)
{
  if (instance_ix)
    goto after_return_point;

  for (*m = set_size - 1; *m >= 2; (*m)--) {
    for (*n = *m - 1; *n >= 1; (*n)--) {
      for (*o = *n - 1; *o >= 0; (*o)--) {
        return;

        after_return_point:
        ;
      }
    }
  }
}
