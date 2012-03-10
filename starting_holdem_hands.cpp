#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static void get_permutation_instance(
  int set_size,int subset_size,
  int *m,int *n,
  int instance_ix
);

int main(int argc,char **argv)
{
  int m;
  int n;
  int o;
  char card_string[3];

  card_string[2] = 0;

  for (o = 0; o < POKER_52_2_PERMUTATIONS; o++) {
    get_permutation_instance(
      NUM_CARDS_IN_DECK,NUM_HOLE_CARDS_IN_HOLDEM_HAND,
      &m,&n,o);
    card_string_from_card_value(m,card_string);
    printf("%s ",card_string);
    card_string_from_card_value(n,card_string);
    printf("%s\n",card_string);
  }

  return 0;
}

static void get_permutation_instance(
  int set_size,int subset_size,
  int *m,int *n,
  int instance_ix
)
{
  if (instance_ix)
    goto after_return_point;

  for (*m = 0; *m < set_size - subset_size + 1; (*m)++) {
    for (*n = *m + 1; *n < set_size - subset_size + 2; (*n)++) {
      return;

      after_return_point:
      ;
    }
  }
}
