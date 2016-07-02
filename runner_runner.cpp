#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define NUM_PLAYERS 2
#define NUM_RUNNER_RUNNER_CARDS 9
#define NUM_REMAINING_CARDS (NUM_CARDS_IN_DECK - NUM_RUNNER_RUNNER_CARDS + 2)

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: runner_runner (-debug) filename";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bRunnerRunner;
  bool bCheckRiver;
  int m;
  int n;
  int o;
  int p;
  int retval;
  FILE *fptr;
  int line_no;
  int line_len;
  int cards[NUM_RUNNER_RUNNER_CARDS];
  int remaining_cards[NUM_REMAINING_CARDS];
  PokerHand hand[NUM_PLAYERS];
  HoldemTurnHand turn_hand[NUM_PLAYERS];
  HoldemPokerHand holdem_hand[NUM_PLAYERS];
  int ret_compare_flop;
  struct outcomes outcomes[NUM_PLAYERS];
  int ret_compare_turn;
  int ret_compare_river;

  if ((argc < 2) || (argc > 3)) {
    cout << usage << endl;
    return 1;
  }

  bDebug = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
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

  bRunnerRunner = false;
  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    m = 0;

    // skip whitespace

    for ( ; m < line_len; m++) {
      if (line[m] != ' ')
        break;
    }

    if (m == line_len) {
      printf(parse_error,line_no,-1,4);
      return 4;
    }

    for (n = 0; n < NUM_RUNNER_RUNNER_CARDS; n++) {
      retval = card_value_from_card_string(&line[m],&cards[n]);

      if (retval) {
        printf(parse_error,line_no,n,5);
        return 5;
      }

      m += 2;

      if (n < NUM_RUNNER_RUNNER_CARDS - 1) {
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

    m = 0;

    for (n = 0; n < NUM_CARDS_IN_DECK; n++) {
      for (o = 0; o < NUM_RUNNER_RUNNER_CARDS - 2; o++) {
        if (n == cards[o])
          break;
      }

      if (o == NUM_RUNNER_RUNNER_CARDS - 2)
        remaining_cards[m++] = n;
    }

    hand[0].NewCards(cards[0],cards[1],
      cards[4],cards[5],cards[6]);
    hand[1].NewCards(cards[2],cards[3],
      cards[4],cards[5],cards[6]);

    hand[0].Evaluate();
    hand[1].Evaluate();

    ret_compare_flop = hand[0].Compare(hand[1],0);

    for (n = 0; n < NUM_PLAYERS; n++) {
      outcomes[n].wins = 0;
      outcomes[n].losses = 0;
      outcomes[n].ties = 0;
    }

    for (o = 0; o < NUM_REMAINING_CARDS; o++) {
      turn_hand[0].NewCards(cards[0],cards[1],
        cards[4],cards[5],cards[6],
        remaining_cards[o]);

      turn_hand[1].NewCards(cards[2],cards[3],
        cards[4],cards[5],cards[6],
        remaining_cards[o]);

      for (p = 0; p < NUM_PLAYERS; p++)
        hand[p] = turn_hand[p].BestPokerHand();

      ret_compare_turn = hand[0].Compare(hand[1],0);

      if (ret_compare_turn == 1) {
        outcomes[0].wins++;
        outcomes[1].losses++;
      }
      else if (ret_compare_turn == -1) {
        outcomes[0].losses++;
        outcomes[1].wins++;
      }
      else {
        outcomes[0].ties++;
        outcomes[1].ties++;
      }
    }

    bCheckRiver = false;

    if (ret_compare_flop == 1) {
      if (outcomes[0].wins == 45)
        bCheckRiver = true;
    }
    else if (ret_compare_flop == -1) {
      if (outcomes[1].wins == 45)
        bCheckRiver = true;
    }

    if (bCheckRiver) {
      holdem_hand[0].NewCards(cards[0],cards[1],
        cards[4],cards[5],cards[6],cards[7],cards[8]);

      holdem_hand[1].NewCards(cards[2],cards[3],
        cards[4],cards[5],cards[6],cards[7],cards[8]);

      for (p = 0; p < NUM_PLAYERS; p++)
        hand[p] = holdem_hand[p].BestPokerHand();

      ret_compare_river = hand[0].Compare(hand[1],0);

      if (ret_compare_flop * ret_compare_river == -1)
        printf("%2d %s\n",ret_compare_river,line);
    }
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
