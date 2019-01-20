#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <direct.h>
#else
#define _MAX_PATH 4096
#include <unistd.h>
#endif
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: card_values_from_card_strings card_string (card_string ...)\n";

int main(int argc,char **argv)
{
  int n;
  int val;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  for (n = 1; n < argc; n++) {
    card_value_from_card_string(argv[n],&val);
    printf("%s %2d\n",argv[n],val);
  }

  return 0;
}
