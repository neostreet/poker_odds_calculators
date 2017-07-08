#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] = "usage: index_of_hand_abbrev hand_abbrev\n";
static char bad_rank[] = "bad rank in card%d: %s\n";
static char bad_suit[] = "bad suit in card%d: %s\n";

int main(int argc,char **argv)
{
  int retval;
  int ix;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  retval = index_of_hand_abbrev(argv[1],&ix);

  if (retval)
    printf("index_of_hand() failed: %d\n",retval);
  else
    printf("%d %s\n",ix,argv[1]);

  return 0;
}
