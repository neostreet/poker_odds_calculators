#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: best_holdem_hand (-unevaluate) (-verbose) (-plain) (-no_stream)\n"
"  (-card_ixs) (-debug) filename";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bUnEvaluate;
  bool bVerbose;
  bool bPlain;
  bool bNoStream;
  bool bCardIxs;
  bool bDebug;
  int m;
  int n;
  int retval;
  FILE *fptr;
  int line_no;
  int line_len;
  int cards[NUM_CARDS_IN_HOLDEM_POOL];
  HoldemPokerHand board_poker_hand;
  PokerHand best_poker_hand;

  if ((argc < 2) || (argc > 8)) {
    cout << usage << endl;
    return 1;
  }

  bUnEvaluate = false;
  bVerbose = false;
  bPlain = false;
  bNoStream = false;
  bCardIxs = false;
  bDebug = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-unevaluate"))
      bUnEvaluate = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-plain"))
      bPlain = true;
    else if (!strcmp(argv[curr_arg],"-no_stream"))
      bNoStream = true;
    else if (!strcmp(argv[curr_arg],"-card_ixs"))
      bCardIxs = true;
    else if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    cout << usage << endl;
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (bCardIxs) {
      sscanf(line,"%d %d %d %d %d %d %d",
        &cards[0],&cards[1],&cards[2],&cards[3],&cards[4],&cards[5],&cards[6]);
    }
    else {
      m = 0;

      // skip whitespace

      for ( ; m < line_len; m++) {
        if (line[m] != ' ')
          break;
      }

      if (m == line_len) {
        printf(parse_error,line_no,-1,3);
        return 4;
      }

      for (n = 0; n < NUM_CARDS_IN_HOLDEM_POOL; n++) {
        retval = card_value_from_card_string(&line[m],&cards[n]);

        if (retval) {
          printf(parse_error,line_no,n,5);
          return 5;
        }

        m += 2;

        if (n < NUM_CARDS_IN_HOLDEM_POOL - 1) {
          // skip whitespace

          for ( ; m < line_len; m++) {
            if (line[m] != ' ')
              break;
          }

          if (m == line_len) {
            printf(parse_error,line_no,n,6);
            return 6;
          }
        }
      }
    }

    board_poker_hand.NewCards(cards[0],cards[1],cards[2],
      cards[3],cards[4],cards[5],cards[6]);

    cout << board_poker_hand << endl;

    best_poker_hand = board_poker_hand.BestPokerHand(bDebug);

    cout << "best hand: " << endl << endl;

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

  fclose(fptr);

  return 0;
}

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
{
  int chara;
  int local_line_len;

  local_line_len = 0;

  for ( ; ; ) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    if (chara == '\n')
      break;

    if (local_line_len < maxllen - 1)
      line[local_line_len++] = (char)chara;
  }

  line[local_line_len] = 0;
  *line_len = local_line_len;
}
