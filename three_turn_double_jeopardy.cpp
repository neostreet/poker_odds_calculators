#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define NUM_PLAYERS 3
#define NUM_THREE_TURN_CARDS 10
#define NUM_REMAINING_CARDS (NUM_CARDS_IN_DECK - NUM_THREE_TURN_CARDS)

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: three_turn_double_jeopardy (-debug) filename";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  int m;
  int n;
  int o;
  int p;
  int s;
  int t;
  int u;
  int retval;
  FILE *fptr;
  int line_no;
  int line_len;
  int cards[NUM_THREE_TURN_CARDS];
  int remaining_cards[NUM_REMAINING_CARDS];
  HoldemPokerHand holdem_hand[NUM_PLAYERS];
  PokerHand hand[NUM_PLAYERS];
  int double_jeopardy_losses;
  int total;
  int comparisons[POKER_3_2_PERMUTATIONS];
  double pct;
  time_t start_time;
  time_t end_time;

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

  time(&start_time);

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    printf("%s\n",line);

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

    for (n = 0; n < NUM_THREE_TURN_CARDS; n++) {
      retval = card_value_from_card_string(&line[m],&cards[n]);

      if (retval) {
        printf(parse_error,line_no,n,5);
        return 5;
      }

      m += 2;

      if (n < NUM_THREE_TURN_CARDS - 1) {
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
      for (o = 0; o < NUM_THREE_TURN_CARDS; o++) {
        if (n == cards[o])
          break;
      }

      if (o == NUM_THREE_TURN_CARDS)
        remaining_cards[m++] = n;
    }

    double_jeopardy_losses = 0;

    for (m = 0; m < NUM_REMAINING_CARDS; m++) {
      holdem_hand[0].NewCards(cards[0],cards[1],
        cards[6],cards[7],cards[8],cards[9],
        remaining_cards[m]);

      holdem_hand[1].NewCards(cards[2],cards[3],
        cards[6],cards[7],cards[8],cards[9],
        remaining_cards[m]);

      holdem_hand[2].NewCards(cards[4],cards[5],
        cards[6],cards[7],cards[8],cards[9],
        remaining_cards[m]);

      for (p = 0; p < NUM_PLAYERS; p++)
        hand[p] = holdem_hand[p].BestPokerHand();

      for (u = 0; u < POKER_3_2_PERMUTATIONS; u++) {
        get_permutation_instance_two(
          NUM_PLAYERS,
          &s,&t,u);

        comparisons[u] = hand[s].Compare(hand[t],0);
      }

      // player 1

      if ((comparisons[0] == -1) && (comparisons[1] == -1))
        double_jeopardy_losses++;
    }
  }

  fclose(fptr);
  time(&end_time);

  total = NUM_REMAINING_CARDS;

  pct = (double)double_jeopardy_losses * (double)100 / (double)total;

  printf("%d (%5.2lf)\n",double_jeopardy_losses,pct);

  printf("\ncomputation time: %d seconds\n",end_time - start_time);

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
