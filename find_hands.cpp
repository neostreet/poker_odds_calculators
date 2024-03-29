#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: find_hands (-debug_levellevel) hands_and_types (count)\n";
static char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  int debug_level;
  int retval;
  int count;
  hand work_hand;
  time_t start_time;
  time_t end_time;
  struct hand_and_type *found;
  double avg_compares;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  debug_level = 0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-debug_level",12))
      sscanf(&argv[curr_arg][12],"%d",&debug_level);
    else
      break;
  }

  if ((argc - curr_arg != 1) && (argc - curr_arg != 2)) {
    printf(usage);
    return 2;
  }

  retval = read_hands_and_types(argv[curr_arg],&hands_and_types);

  if (retval) {
    printf("read_hands_and_types() failed: %d\n",retval);
    return 3;
  }

  if (argc - curr_arg != 2)
    count = POKER_52_5_PERMUTATIONS;
  else
    sscanf(argv[curr_arg+1],"%d",&count);

  if (count > POKER_52_5_PERMUTATIONS) {
    printf("illegal count\n");
    return 4;
  }

  time(&start_time);

  for (n = 0; n < count; n++) {
    get_permutation_instance_five(
      NUM_CARDS_IN_DECK,
      &work_hand.cards[0],&work_hand.cards[1],&work_hand.cards[2],&work_hand.cards[3],&work_hand.cards[4],n);

    retval = find_hand(&work_hand,hands_and_types,debug_level,&found);

    if (!retval) {
      printf("find_hand() failed: %d\n",retval);
      return 5;
    }
  }

  time(&end_time);
  free(hands_and_types);

  avg_compares = (double)compare_key_called / (double)count;

  printf("computation time: %d seconds\n",end_time - start_time);
  printf("compare_key() called %d times\n",compare_key_called);
  printf("average number of compares: %lf\n",avg_compares);

  return 0;
}
