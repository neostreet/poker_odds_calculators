#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: best_holdem_hand (-unevaluate) (-verbose) (-plain) (-no_stream)";
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
  int cards[NUM_CARDS_IN_DECK];
  HoldemPokerHand board_poker_hand;
  PokerHand best_poker_hand;

  if ((argc < 1) || (argc > 5)) {
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

  if (argc != curr_arg) {
    cout << usage << endl;
    return 2;
  }

  for (n = 0; n < NUM_CARDS_IN_DECK; n++)
    cards[n] = n;

  for (n = 0; n < NUM_CARDS_IN_DECK - 7 + 1; n++) {
    board_poker_hand.NewCards(cards[n],cards[n+1],cards[n+2],
      cards[n+3],cards[n+4],cards[n+5],cards[n+6]);

    best_poker_hand = board_poker_hand.BestPokerHand();

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
