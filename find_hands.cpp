#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#ifdef FREEBSD
#define O_BINARY 0
#endif
#endif
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: find_hands (-bsearch) hands_and_types count\n";
static char couldnt_open[] = "couldn't open %s\n";

struct hand {
  int cards[NUM_CARDS_IN_HAND];
};

struct hand_and_type {
  char cards[NUM_CARDS_IN_HAND];
  char hand_type;
  int ix;
};

int compare_key(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  int curr_arg;
  bool bBsearch;
  int fhndl;
  struct hand_and_type *hands_and_types;
  int bytes_read;
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

  hands_and_types = (struct hand_and_type *)malloc(sizeof (struct hand_and_type) * POKER_52_5_PERMUTATIONS);

  if (hands_and_types == NULL) {
    printf("malloc of hands_and_types failed\n");
    return 3;
  }

  if ((fhndl = open(argv[curr_arg],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  bytes_read = read(fhndl,hands_and_types,sizeof (struct hand_and_type) * POKER_52_5_PERMUTATIONS);

  if (bytes_read != sizeof (struct hand_and_type) * POKER_52_5_PERMUTATIONS) {
    printf("%s is the wrong size\n",argv[curr_arg]);
    return 5;
  }

  close(fhndl);

  sscanf(argv[curr_arg+1],"%d",&count);

  if (count > POKER_52_5_PERMUTATIONS) {
    printf("illegal count\n");
    return 6;
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
          if (hands_and_types[p].ix != n) {
            printf("linear search failed: %d != %d\n",hands_and_types[p].ix,n);
            return 7;
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
        return 8;
      }

      if (found->ix != n) {
        printf("binary search failed: %d != %d\n",found->ix,n);
        return 9;
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
