#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] = "usage: index_of_hand card1 card2\n";
static char bad_rank[] = "bad rank in card%d: %s\n";
static char bad_suit[] = "bad suit in card%d: %s\n";

int main(int argc,char **argv)
{
  int n;
  int retval;
  int rank_ix[2];
  int suit_ix[2];
  int ix;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  for (n = 0; n < 2; n++) {
    retval = get_rank_index(argv[1+n][0],&rank_ix[n]);

    if (retval) {
      printf("illegal rank in card%d\n",n + 1);
      return 2;
    }

    retval = get_suit_index(argv[1+n][1],&suit_ix[n]);

    if (retval) {
      printf("illegal suit in card%d\n",n + 1);
      return 3;
    }
  }

  ix = index_of_hand(rank_ix[0],suit_ix[0],rank_ix[1],suit_ix[1]);

  printf("%d %s %s\n",ix,argv[1],argv[2]);

  return 0;
}
