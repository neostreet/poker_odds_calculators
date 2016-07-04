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
"usage: permute_hands0 (-binfilefile)\n";
static char couldnt_open[] = "couldn't open %s\n";

struct hand_and_type {
  char cards[NUM_CARDS_IN_HAND];
  char hand_type;
  int ix;
};

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bBinFile;
  char *binfile_name;
  int cards[NUM_CARDS_IN_HAND];
  char card_string[3];
  PokerHand hand;
  struct hand_and_type *hands_and_types;
  int fhndl;

  if (argc > 2) {
    printf(usage);
    return 1;
  }

  bBinFile = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-binfile",8)) {
      bBinFile = true;
      binfile_name = &argv[curr_arg][8];
    }
    else
      break;
  }

  if (bBinFile) {
    hands_and_types = (struct hand_and_type *)malloc(sizeof (struct hand_and_type) * POKER_52_5_PERMUTATIONS);

    if (hands_and_types == NULL) {
      printf("malloc of hands_and_types failed\n");
      return 3;
    }
  }

  card_string[2] = 0;

  for (m = 0; m < POKER_52_5_PERMUTATIONS; m++) {
    get_permutation_instance_five(
      NUM_CARDS_IN_DECK,
      &cards[0],&cards[1],&cards[2],&cards[3],&cards[4],m);

    hand.NewCards(cards[0],cards[1],cards[2],cards[3],cards[4]);
    hand.Evaluate();

    if (bBinFile) {
      for (n = 0; n < NUM_CARDS_IN_HAND; n++)
        hands_and_types[m].cards[n] = (char)cards[n];

      hands_and_types[m].hand_type = (char)hand.GetHandType();
      hands_and_types[m].ix = m;
    }
    else
      cout << hand << endl;
  }

  if (bBinFile) {
    if ((fhndl = open(binfile_name,
      O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
      S_IREAD | S_IWRITE)) == -1) {

      printf(couldnt_open,binfile_name);
      return 4;
    }

    write(fhndl,hands_and_types,sizeof (struct hand_and_type) * POKER_52_5_PERMUTATIONS);
    close(fhndl);

    free(hands_and_types);
  }

  return 0;
}
