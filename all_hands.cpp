#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: all_hands (-countcount) (-sort) (-hand_type_ix) hands_and_types_filename\n";

static char couldnt_open[] = "couldn't open %s\n";

int elem_compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  int count;
  bool bSort;
  bool bHandTypeIx;
  int retval;
  char card_string[3];
  int *ixs;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  count = POKER_52_5_PERMUTATIONS;
  bSort = false;
  bHandTypeIx = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-count",6)) {
      sscanf(&argv[curr_arg][6],"%d",&count);

      if (count > POKER_52_5_PERMUTATIONS)
        count = POKER_52_5_PERMUTATIONS;
    }
    else if (!strcmp(argv[curr_arg],"-sort"))
      bSort = true;
    else if (!strcmp(argv[curr_arg],"-hand_type_ix"))
      bHandTypeIx = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  retval = read_hands_and_types(argv[curr_arg],&hands_and_types);

  if (retval) {
    printf("read_hands_and_types() failed: %d\n",retval);
    return 3;
  }

  card_string[2] = 0;

  ixs = (int *)malloc(count * sizeof (int));

  if (ixs == NULL) {
    printf("couldn't malloc %d ints for ixs\n",count);
    return 4;
  }

  for (n = 0; n < count; n++)
    ixs[n] = n;

  if (bSort)
    qsort(ixs,count,sizeof (int),elem_compare);

  for (n = 0; n < count; n++) {
    if (bHandTypeIx)
      printf("%d ",hands_and_types[ixs[n]].hand_type);

    printf("%4d ",hands_and_types[ixs[n]].quick_ix);

    for (m = 0; m < NUM_CARDS_IN_HAND; m++) {
      card_string_from_card_value(
        hands_and_types[ixs[n]].cards[m],
        card_string);
      printf("%s ",card_string);
    }

    printf("%s\n",plain_hand_types[hands_and_types[ixs[n]].hand_type]);
  }

  free(ixs);

  return 0;
}

int elem_compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  return hands_and_types[ix1].quick_ix - hands_and_types[ix2].quick_ix;
}
