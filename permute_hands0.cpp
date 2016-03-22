#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

int main(int argc,char **argv)
{
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  int s;
  char card_string[3];

  for (r = 0; r < POKER_52_5_PERMUTATIONS; r++) {
    get_permutation_instance_five(
      NUM_CARDS_IN_DECK,
      &m,&n,&o,&p,&q,r);

    if (argc == 1)
      printf("%d %d %d %d %d\n",m,n,o,p,q);
    else {
      card_string[2] = 0;
      card_string_from_card_value(m,card_string);
      printf("%s ",card_string);
      card_string_from_card_value(n,card_string);
      printf("%s ",card_string);
      card_string_from_card_value(o,card_string);
      printf("%s ",card_string);
      card_string_from_card_value(p,card_string);
      printf("%s ",card_string);
      card_string_from_card_value(q,card_string);
      printf("%s\n",card_string);
    }
  }

  return 0;
}
