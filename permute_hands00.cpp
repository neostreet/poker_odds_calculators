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
"usage: permute_hands00 (-terse) (-skip_sort) (-count_ties) (-countcount)\n"
"  (-binfilefile) (-write_sorted)\n";
static char couldnt_open[] = "couldn't open %s\n";

static int *ixs;
static int *ixs2;
static vector<PokerHand> hands;

static int hand_counts[NUM_HAND_TYPES];

int elem_compare(const void *elem1,const void *elem2);
int compare(const void *elem1,const void *elem2);

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
  bool bBinFile;
  bool bWriteSorted;
  char *binfile_name;
  int cards[NUM_CARDS_IN_HAND];
  char card_string[3];
  PokerHand myhand;
  time_t start_time;
  time_t end_time;
  int ties;
  struct hand_and_type *hands_and_types;
  struct hand_and_type *sorted_hands_and_types;
  hand work_hand;
  struct hand_and_type *found;
  int fhndl;
  size_t malloc_size;
  int hand_type_ixs[NUM_HAND_TYPES];
  int total;
  double pct;

  if (argc > 7) {
    printf(usage);
    return 1;
  }

  bTerse = false;
  bSkipSort = false;
  bCountTies = false;
  count = POKER_52_5_PERMUTATIONS;
  bBinFile = false;
  bWriteSorted = false;

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
    else if (!strncmp(argv[curr_arg],"-binfile",8)) {
      bBinFile = true;
      binfile_name = &argv[curr_arg][8];
    }
    else if (!strcmp(argv[curr_arg],"-write_sorted"))
      bWriteSorted = true;
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

  if (bBinFile) {
    malloc_size = sizeof (struct hand_and_type) * count;
    hands_and_types = (struct hand_and_type *)malloc(malloc_size);

    if (hands_and_types == NULL) {
      printf("malloc of hands_and_types failed\n");
      return 5;
    }

    if (bWriteSorted) {
      sorted_hands_and_types = (struct hand_and_type *)malloc(malloc_size);

      if (sorted_hands_and_types == NULL) {
        printf("malloc of hands_and_types failed\n");
        return 6;
      }
    }
  }

  card_string[2] = 0;

  time(&start_time);

  for (n = 0; n < NUM_HAND_TYPES; n++) {
    hand_counts[n] = 0;
    hand_type_ixs[n] = n;
  }

  for (m = 0; m < count; m++) {
    get_permutation_instance_five(
      NUM_CARDS_IN_DECK,
      &cards[0],&cards[1],&cards[2],&cards[3],&cards[4],m);

    myhand.NewCards(cards[0],cards[1],cards[2],cards[3],cards[4]);
    myhand.Evaluate();
    hand_counts[myhand.GetHandType()]++;
    hands.push_back(myhand);
    ixs[m] = m;

    if (bBinFile) {
      for (n = 0; n < NUM_CARDS_IN_HAND; n++)
        hands_and_types[m].cards[n] = (char)cards[n];

      hands_and_types[m].hand_type = (char)myhand.GetHandType();
      hands_and_types[m].hand_ix = m;
    }
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

  if (bBinFile) {
    /* calculate ixs */

    for (m = 0; m < count; m++)
      hands_and_types[hands_and_types[ixs[m]].hand_ix].quick_ix = ixs2[m];
  }

  time(&end_time);

  if (bCountTies)
    printf("%d ties\n",ties);

  if (!bTerse) {
    for (m = 0; m < count; m++) {
      cout << hands[ixs[m]] << " " << ixs2[m] << endl;
    }
  }

  if (bBinFile) {
    if ((fhndl = open(binfile_name,
      O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
      S_IREAD | S_IWRITE)) == -1) {

      printf(couldnt_open,binfile_name);
      return 7;
    }

    if (!bWriteSorted)
      write(fhndl,hands_and_types,sizeof (struct hand_and_type) * count);
    else {
      for (m = 0; m < count; m++)
        sorted_hands_and_types[m] = hands_and_types[ixs[m]];

      write(fhndl,sorted_hands_and_types,sizeof (struct hand_and_type) * count);
    }

    close(fhndl);

    free(hands_and_types);
  }

  qsort(hand_type_ixs,NUM_HAND_TYPES,sizeof (int),compare);

  total = 0;

  for (n = 0; n < NUM_HAND_TYPES; n++)
    total += hand_counts[n];

  for (n = 0; n < NUM_HAND_TYPES; n++) {
    pct = (double)hand_counts[hand_type_ixs[n]] / (double)total;
    printf("%s %9d %9.6lf\n",hand_type_abbrevs[hand_type_ixs[n]],hand_counts[hand_type_ixs[n]],pct);
  }

  printf("============\n");
  printf("   %9d\n",total);

  printf("\ncomputation time: %d seconds\n",end_time - start_time);

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

int compare(const void *elem1,const void *elem2)
{
  int int1;
  int int2;

  int1 = *(int *)elem1;
  int2 = *(int *)elem2;

  return hand_counts[int1] - hand_counts[int2];
}
