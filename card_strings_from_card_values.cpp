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
"usage: card_strings_from_card_values card_value (card_value ...)\n";

int main(int argc,char **argv)
{
  int n;
  int val;
  char card_string[3];

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  card_string[2] = 0;

  for (n = 1; n < argc; n++) {
    sscanf(argv[n],"%d",&val);
    card_string_from_card_value(val,card_string);
    printf("%2d %s\n",val,card_string);
  }

  return 0;
}
