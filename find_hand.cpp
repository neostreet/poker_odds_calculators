#include <iostream>
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
"usage: find_hand (-debug_levellevel) (-verbose) hands_and_types\n"
"  card1 card2 card3 card4 card5\n";
static char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  int debug_level;
  bool bVerbose;
  int retval;
  struct hand_and_type *hands_and_types;
  hand sorted_hand;
  struct hand_and_type *found;

  if ((argc < 7) || (argc > 9)) {
    printf(usage);
    return 1;
  }

  debug_level = 0;
  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-debug_level",12))
      sscanf(&argv[curr_arg][12],"%d",&debug_level);
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (argc - curr_arg != 6) {
    printf(usage);
    return 2;
  }

  retval = read_hands_and_types(argv[curr_arg],&hands_and_types);

  if (retval) {
    printf("read_hands_and_types() failed: %d\n",retval);
    return 3;
  }

  curr_arg++;

  for (n = 0; n < NUM_CARDS_IN_HAND; n++)
    sscanf(argv[curr_arg+n],"%d",&sorted_hand.cards[n]);

  retval = find_hand(&sorted_hand,hands_and_types,debug_level,&found);

  if (retval)
    printf("hand_ix = %d, quick_ix = %d\n",found->hand_ix,found->quick_ix);

  return 0;
}
