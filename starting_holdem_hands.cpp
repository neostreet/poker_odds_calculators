#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: starting_holdem_hands (-indexes) (-reverse) (-with_zero_deltas\n";
static char *fmt[] = {
  "%s %s\n",
  "%s %s          0\n",
};

static void get_permutation_instance(
  int set_size,
  int *m,int *n,
  int instance_ix
);

static void get_permutation_instance_reverse(
  int set_size,
  int *m,int *n,
  int instance_ix
);

int main(int argc,char **argv)
{
  int curr_arg;
  int m;
  int n;
  int o;
  char card_string[2][3];
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

  for (n = 0; n < 2; n++)
    card_string[n][2] = 0;

  for (o = 0; o < POKER_52_2_PERMUTATIONS; o++) {
    if (!bReverse) {
      get_permutation_instance(
        NUM_CARDS_IN_DECK,
        &m,&n,o);
    }
    else {
      get_permutation_instance_reverse(
        NUM_CARDS_IN_DECK,
        &m,&n,o);
    }

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

  return 0;
}

static void get_permutation_instance(
  int set_size,
  int *m,int *n,
  int instance_ix
)
{
  if (instance_ix)
    goto after_return_point;

  for (*m = 0; *m < set_size - 1; (*m)++) {
    for (*n = *m + 1; *n < set_size; (*n)++) {
      return;

      after_return_point:
      ;
    }
  }
}

static void get_permutation_instance_reverse(
  int set_size,
  int *m,int *n,
  int instance_ix
)
{
  if (instance_ix)
    goto after_return_point;

  for (*m = set_size - 1; *m > 0; (*m)--) {
    for (*n = *m - 1; *n >= 0; (*n)--) {
      return;

      after_return_point:
      ;
    }
  }
}
