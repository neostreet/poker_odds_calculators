#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: best_holdem_hand (-unevaluate) (-verbose) (-plain) (-no_stream)\n"
"  hands_and_types_filename";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bUnEvaluate;
  bool bVerbose;
  bool bPlain;
  bool bNoStream;
  int n;
  int retval;
  struct hand_and_type *hands_and_types;
  int cards[NUM_CARDS_IN_DECK];
  HoldemPokerHand board_poker_hand;
  PokerHand best_poker_hand;

  if ((argc < 2) || (argc > 6)) {
    cout << usage << endl;
    return 1;
  }

  bUnEvaluate = false;
  bVerbose = false;
  bPlain = false;
  bNoStream = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-unevaluate"))
      bUnEvaluate = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-plain"))
      bPlain = true;
    else if (!strcmp(argv[curr_arg],"-no_stream"))
      bNoStream = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    cout << usage << endl;
    return 2;
  }

  retval = read_hands_and_types(argv[curr_arg],&hands_and_types);

  if (retval) {
    printf("read_hands_and_types() failed: %d\n",retval);
    return 3;
  }

  for (n = 0; n < NUM_CARDS_IN_DECK; n++)
    cards[n] = n;

  for (n = 0; n < NUM_CARDS_IN_DECK - 7 + 1; n++) {
    board_poker_hand.NewCards(cards[n],cards[n+1],cards[n+2],
      cards[n+3],cards[n+4],cards[n+5],cards[n+6]);

    best_poker_hand = board_poker_hand.BestPokerHandQuick(hands_and_types);

    if (bUnEvaluate)
      best_poker_hand.UnEvaluate();
    else {
      if (bVerbose)
        best_poker_hand.Verbose();

      if (bPlain)
        best_poker_hand.Plain();
    }

    if (!bNoStream)
      cout << best_poker_hand << endl;
    else
      cout << best_poker_hand.GetHand() << endl;
  }

  return 0;
}
