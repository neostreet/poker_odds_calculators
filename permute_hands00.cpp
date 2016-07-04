#include <iostream>
#include <vector>
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
"usage: permute_hands00\n";
static char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  char *binfile_name;
  int cards[NUM_CARDS_IN_HAND];
  char card_string[3];
  PokerHand hand;
  vector<PokerHand> hands;
  int fhndl;

  card_string[2] = 0;

  for (m = 0; m < POKER_52_5_PERMUTATIONS; m++) {
    get_permutation_instance_five(
      NUM_CARDS_IN_DECK,
      &cards[0],&cards[1],&cards[2],&cards[3],&cards[4],m);

    hand.NewCards(cards[0],cards[1],cards[2],cards[3],cards[4]);
    hand.Evaluate();
    hands.push_back(hand);
  }

  for (m = 0; m < POKER_52_5_PERMUTATIONS; m++) {
    cout << hands[m] << endl;
  }

  return 0;
}
