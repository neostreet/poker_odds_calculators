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
"usage: find_hand (-debug_levellevel) hands_and_types\n"
"  card_str card_str card_str card_str card_str\n";
static char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  int debug_level;
  int retval;
  struct hand_and_type *hands_and_types;
  hand work_hand;
  struct hand_and_type *found;

  if ((argc < 7) || (argc > 8)) {
    printf(usage);
    return 1;
  }

  debug_level = 0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-debug_level",12))
      sscanf(&argv[curr_arg][12],"%d",&debug_level);
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

  for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
    retval = card_value_from_card_string(argv[curr_arg+n],&work_hand.cards[n]);

    if (retval) {
      printf("card_value_from_cards_string() failed on card %s: %d\n",argv[curr_arg+n],retval);
      return 4;
    }
  }

  retval = find_hand(&work_hand,hands_and_types,debug_level,&found);

  if (retval)
    printf("quick_ix = %d\n",found->quick_ix);

  return 0;
}
