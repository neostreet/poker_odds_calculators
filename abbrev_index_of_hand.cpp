#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] = "usage: abbrev index_of_hand hand\n";

int main(int argc,char **argv)
{
  int retval;
  int ix;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  retval = abbrev_index_of_hand(argv[1],&ix);

  if (retval)
    printf("abbrev_index_of_hand() failed: %d\n",retval);
  else
    printf("%d %s\n",ix,argv[1]);

  return 0;
}
