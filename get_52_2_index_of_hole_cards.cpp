#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: get_52_2_index_of_hole_cards hole_cards\n";

int main(int argc,char **argv)
{
  int retval;
  int ix;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  retval = get_52_2_index_of_hole_cards(argv[1],&ix);

  if (retval) {
    printf("get_52_2_index_of_hole_cards() failed: %d\n",retval);
    return 2;
  }

  printf("%d\n",ix);

  return 0;
}
