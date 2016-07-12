#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: find_hands hands_and_types count\n";
static char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int retval;
  int count;
  PokerHand myhand;
  time_t start_time;
  time_t end_time;
  struct hand_and_type *found;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  retval = myhand.read_quick_hands(argv[1]);

  if (retval) {
    printf("read_quick_hands() failed: %d\n",retval);
    return 2;
  }

  sscanf(argv[2],"%d",&count);

  if (count > POKER_52_5_PERMUTATIONS) {
    printf("illegal count\n");
    return 3;
  }

  time(&start_time);

  for (n = 0; n < count; n++) {
    myhand.get_permutation_instance_five(
      NUM_CARDS_IN_DECK,
      n);

    retval = myhand.find_quick_hand(&found);

    if (!retval) {
      printf("find_quick_hand() failed: %d\n",retval);
      return 4;
    }

    if (found->quick_ix != n) {
      printf("linear search failed: %d != %d\n",found->quick_ix,n);
      return 5;
    }
  }

  time(&end_time);

  printf("computation time: %d seconds\n",end_time - start_time);

  return 0;
}
