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
"usage: peek_hands_and_types (-verbose) (-card_ixs) hands_and_types_filename\n"
"  offset count\n";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bVerbose;
  bool bCardIxs;
  bool bNoIxs;
  int retval;
  struct hand_and_type *hands_and_types;
  int offset;
  int count;
  char card_string[3];

  if ((argc < 4) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bCardIxs = false;
  bNoIxs = false;

  card_string[2] = 0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-card_ixs"))
      bCardIxs = true;
    else if (!strcmp(argv[curr_arg],"-no_ixs"))
      bNoIxs = true;
    else
      break;
  }

  if (argc - curr_arg != 3) {
    printf(usage);
    return 2;
  }

  retval = read_hands_and_types(argv[curr_arg],&hands_and_types);

  curr_arg++;

  if (retval) {
    printf("read_hands_and_types() failed: %d\n",retval);
    return 3;
  }

  sscanf(argv[curr_arg],"%d",&offset);
  sscanf(argv[curr_arg+1],"%d",&count);

  for (n = 0; n < count; n++) {
    if (!bVerbose)
      printf("%d %d\n",hands_and_types[offset+n].hand_ix,hands_and_types[offset+n].quick_ix);
    else {
      if (!bNoIxs)
        printf("%d %d ",hands_and_types[offset+n].hand_ix,hands_and_types[offset+n].quick_ix);

      for (m = 0; m < NUM_CARDS_IN_HAND; m++) {
        if (!bCardIxs) {
          card_string_from_card_value(
            hands_and_types[offset+n].cards[m],
            card_string);
          printf("%s ",card_string);
        }
        else
          printf("%d ",hands_and_types[offset+n].cards[m]);
      }

      printf("%s\n",plain_hand_types[hands_and_types[offset+n].hand_type]);
    }
  }

  return 0;
}
