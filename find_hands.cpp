#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: find_hands (-bsearch) hands_and_types count\n";
static char couldnt_open[] = "couldn't open %s\n";

int compare_key(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  int curr_arg;
  bool bBsearch;
  struct hand_and_type *hands_and_types;
  int retval;
  int count;
  struct hand work_hand;
  time_t start_time;
  time_t end_time;
  struct hand_and_type *found;

  if ((argc < 3) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bBsearch = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-bsearch"))
      bBsearch = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  retval = read_hands_and_types(argv[curr_arg],&hands_and_types);

  if (retval) {
    printf("read_hands_and_types() failed: %d\n",retval);
    return 3;
  }

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

    if (!bBsearch) {
      for (p = 0; p < POKER_52_5_PERMUTATIONS; p++) {
        for (m = 0; m < NUM_CARDS_IN_HAND; m++) {
          if (hands_and_types[p].cards[m] != (char)work_hand.cards[m])
            break;
        }

        if (m == NUM_CARDS_IN_HAND) {
          if (hands_and_types[p].quick_ix != n) {
            printf("linear search failed: %d != %d\n",hands_and_types[p].quick_ix,n);
            return 5;
          }

          break;
        }
      }
    }
    else {
      found = (struct hand_and_type *)bsearch(&work_hand,hands_and_types,POKER_52_5_PERMUTATIONS,
        sizeof (struct hand_and_type),compare_key);

      if (found == NULL) {
        printf("bsearch failed\n");
        return 6;
      }

      if (found->quick_ix != n) {
        printf("binary search failed: %d != %d\n",found->quick_ix,n);
        return 7;
      }
    }
  }

  time(&end_time);
  free(hands_and_types);

  printf("computation time: %d seconds\n",end_time - start_time);

  return 0;
}

int compare_key(const void *vkey,const void *velem)
{
  int n;
  struct hand *key;
  struct hand_and_type *elem;

  key = (struct hand *)vkey;
  elem = (struct hand_and_type *)velem;

  for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
    if (key->cards[n] < (int)elem->cards[n])
      return -1;

    if (key->cards[n] > (int)elem->cards[n])
      return 1;
  }

  return 0;
}
