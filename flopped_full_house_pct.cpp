#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef WIN32
#include <direct.h>
#else
#define _MAX_PATH 4096
#include <unistd.h>
#endif
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: flopped_full_house_pct (-verbose) (-debug)\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  int curr_arg;
  bool bVerbose;
  bool bDebug;
  time_t start_time;
  time_t end_time;
  PokerHand poker_hand;
  int num_full_houses;
  double work;

  if ((argc < 1) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  time(&start_time);

  bVerbose = false;
  bDebug = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else
      break;
  }

  if (argc != curr_arg) {
    printf(usage);
    return 2;
  }

  num_full_houses = 0;

  for (r = 0; r < POKER_52_5_PERMUTATIONS; r++) {
    get_permutation_instance_five(
      NUM_CARDS_IN_DECK,
      &m,&n,&o,&p,&q,r);

    poker_hand.NewCards(m,n,o,p,q);
    poker_hand.Evaluate();

    if (poker_hand.GetHandType() == FULL_HOUSE)
      num_full_houses++;
  }

  work = (double)num_full_houses / (double)POKER_52_5_PERMUTATIONS;

  printf("%8.6lf %10d %10d\n",
    work,num_full_houses,POKER_52_5_PERMUTATIONS);

  time(&end_time);

  printf("\ncomputation time: %d seconds\n",end_time - start_time);

  return 0;
}
