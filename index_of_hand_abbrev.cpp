#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define NUM_SUITED_NONPAIRS ((NUM_CARDS_IN_SUIT * (NUM_CARDS_IN_SUIT - 1)) / 2)
#define NUM_NONSUITED_NONPAIRS NUM_SUITED_NONPAIRS

static char usage[] = "usage: index_of_hand_abbrev hand_abbrev\n";
static char bad_rank[] = "bad rank in card%d: %s\n";
static char bad_suit[] = "bad suit in card%d: %s\n";
static char illegal_hand_abbrev[] = "illegal hand_abbrev\n";

static int index_of_hand(int rank_ix1,int suit_ix1,int rank_ix2,int suit_ix2);

int main(int argc,char **argv)
{
  int n;
  int retval;
  int rank_ix[2];
  int suit_ix[2];
  int len;
  int ix;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  for (n = 0; n < 2; n++) {
    retval = get_rank_index(argv[1][n],&rank_ix[n]);

    if (retval) {
      printf("illegal rank in hand_abbrev\n");
      return 2;
    }
  }

  len = strlen(argv[1]);

  if (len == 2) {
    if (rank_ix[0] != rank_ix[1]) {
      printf(illegal_hand_abbrev);
      return 3;
    }

    suit_ix[0] = 0;
    suit_ix[1] = 1;
  }
  else if (len == 3) {
    if (argv[1][2] == 's') {
      suit_ix[0] = 0;
      suit_ix[1] = 0;
    }
    else if (argv[1][2] == 'o') {
      suit_ix[0] = 0;
      suit_ix[1] = 1;
    }
    else {
      printf(illegal_hand_abbrev);
      return 4;
    }
  }
  else {
    printf(illegal_hand_abbrev);
    return 5;
  }

  ix = index_of_hand(rank_ix[0],suit_ix[0],rank_ix[1],suit_ix[1]);

  printf("%d %s\n",ix,argv[1]);

  return 0;
}

static int index_of_hand(int rank_ix1,int suit_ix1,int rank_ix2,int suit_ix2)
{
  int n;
  int work;
  int index;
  int num_other_cards;

  if (rank_ix1 == rank_ix2)
    return rank_ix1;

  if (suit_ix1 == suit_ix2)
    index = NUM_CARDS_IN_SUIT;
  else
    index = NUM_CARDS_IN_SUIT + NUM_SUITED_NONPAIRS;

  if (rank_ix1 > rank_ix2) {
    work = rank_ix1;
    rank_ix1 = rank_ix2;
    rank_ix2 = work;
  }

  num_other_cards = NUM_CARDS_IN_SUIT - 1;

  for (n = 0; n < rank_ix1; n++) {
    index += num_other_cards;
    num_other_cards--;
  }

  index += rank_ix2 - rank_ix1 - 1;

  return index;
}
