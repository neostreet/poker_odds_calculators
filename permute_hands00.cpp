#include <iostream>
#include <vector>
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
"usage: permute_hands00 (-terse) (-skip_sort) (-count_ties) (-countcount)\n";
static char couldnt_open[] = "couldn't open %s\n";

static int *ixs;
static int *ixs2;
static vector<PokerHand> hands;

int elem_compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  int curr_arg;
  bool bTerse;
  bool bSkipSort;
  bool bCountTies;
  int count;
  char *binfile_name;
  int cards[NUM_CARDS_IN_HAND];
  char card_string[3];
  PokerHand hand;
  time_t start_time;
  time_t end_time;
  int ties;

  if (argc > 4) {
    printf(usage);
    return 1;
  }

  bTerse = false;
  bSkipSort = false;
  bCountTies = false;
  count = POKER_52_5_PERMUTATIONS;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-skip_sort"))
      bSkipSort = true;
    else if (!strcmp(argv[curr_arg],"-count_ties")) {
      bSkipSort = false;
      bCountTies = true;
    }
    else if (!strncmp(argv[curr_arg],"-count",6))
      sscanf(&argv[curr_arg][6],"%d",&count);
    else
      break;
  }

  if (argc != curr_arg) {
    printf(usage);
    return 2;
  }

  ixs = (int *)malloc(count * sizeof (int));

  if (ixs == NULL) {
    printf("couldn't malloc %d ints for ixs\n",count);
    return 3;
  }

  ixs2 = (int *)malloc(count * sizeof (int));

  if (ixs2 == NULL) {
    free(ixs);
    printf("couldn't malloc %d ints for ixs2\n",count);
    return 4;
  }

  card_string[2] = 0;

  time(&start_time);

  for (m = 0; m < count; m++) {
    get_permutation_instance_five(
      NUM_CARDS_IN_DECK,
      &cards[0],&cards[1],&cards[2],&cards[3],&cards[4],m);

    hand.NewCards(cards[0],cards[1],cards[2],cards[3],cards[4]);
    hand.Evaluate();
    hands.push_back(hand);
    ixs[m] = m;
  }

  if (!bSkipSort)
    qsort(ixs,count,sizeof (int),elem_compare);

  if (bCountTies) {
    ties = 0;

    for (m = 0; m < count - 1; m++) {
      if (!hands[ixs[m]].Compare(hands[ixs[m+1]],0))
        ties++;
    }
  }

  /* normalize for ties */
  p = 0;

  for (m = 0; m < count - 1; ) {
    for (n = m + 1; n < count; n++) {
      if (hands[ixs[m]].Compare(hands[ixs[n]],0))
        break;
    }

    for ( ; m < n; m++)
      ixs2[m] = p;

    p++;
    m = n;
  }

  time(&end_time);

  printf("computation time: %d seconds\n",end_time - start_time);

  if (bCountTies)
    printf("%d ties\n",ties);

  if (!bTerse) {
    for (m = 0; m < count; m++) {
      cout << hands[ixs[m]] << " " << ixs2[m] << endl;
    }
  }

  free(ixs2);
  free(ixs);

  return 0;
}

int elem_compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  return hands[ix1].Compare(hands[ix2],0);
}
