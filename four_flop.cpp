#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define FALSE 0
#define TRUE  1

#define MAIN_MODULE
#include "poker_hand.h"

static void get_permutation_instance(
  int *m,int *n,int instance_ix
);

#define FOUR 4
#define NUM_FOUR_FLOP_CARDS 11
#define NUM_REMAINING_CARDS (NUM_CARDS_IN_DECK - NUM_FOUR_FLOP_CARDS)

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: four_flop (-debug) filename";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  int bDebug;
  int m;
  int n;
  int o;
  int p;
  int retval;
  FILE *fptr;
  int line_no;
  int line_len;
  int cards[NUM_FOUR_FLOP_CARDS];
  int remaining_cards[NUM_REMAINING_CARDS];
  BoardPokerHand board_hand[FOUR];
  PokerHand hand[FOUR];
  int ret_compare;
  int wins;
  int losses;
  int ties;
  int total;
  double pct;
  int work_wins;
  int work_losses;

  if ((argc < 2) || (argc > 3)) {
    cout << usage << endl;
    return 1;
  }

  bDebug = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = TRUE;
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

    for (n = 0; n < NUM_FOUR_FLOP_CARDS; n++) {
      retval = card_value_from_card_string(&line[m],&cards[n]);

      if (retval) {
        printf(parse_error,line_no,n,5);
        return 5;
      }

      m += 2;

      if (n < NUM_FOUR_FLOP_CARDS - 1) {
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
      for (o = 0; o < NUM_FOUR_FLOP_CARDS; o++) {
        if (n == cards[o])
          break;
      }

      if (o == NUM_FOUR_FLOP_CARDS)
        remaining_cards[m++] = n;
    }

    wins = 0;
    losses = 0;
    ties = 0;
    total = 0;

    for (o = 0; o < POKER_41_2_PERMUTATIONS; o++) {
      get_permutation_instance(&m,&n,o);

      board_hand[0].NewCards(cards[0],cards[1],
        cards[8],cards[9],cards[10],
        remaining_cards[m],remaining_cards[n]);

      board_hand[1].NewCards(cards[2],cards[3],
        cards[8],cards[9],cards[10],
        remaining_cards[m],remaining_cards[n]);

      board_hand[2].NewCards(cards[4],cards[5],
        cards[8],cards[9],cards[10],
        remaining_cards[m],remaining_cards[n]);

      board_hand[3].NewCards(cards[6],cards[7],
        cards[8],cards[9],cards[10],
        remaining_cards[m],remaining_cards[n]);

      for (p = 0; p < FOUR; p++)
        hand[p] = board_hand[p].BestPokerHand();

      work_wins = 0;
      work_losses = 0;

      for (p = 0; p < FOUR - 1; p++) {
        ret_compare = hand[0].Compare(hand[1+p]);

        if (ret_compare == 1)
          work_wins++;
        else if (ret_compare == -1)
          work_losses++;
      }

      if (work_wins == FOUR - 1)
        wins++;
      else if (work_losses)
        losses++;
      else
        ties++;

      total++;
    }

    pct = (double)wins * (double)100 / (double)total;
    printf("  wins      %7d (%5.2lf)\n",wins,pct);

    pct = (double)losses * (double)100 / (double)total;
    printf("  losses    %7d (%5.2lf)\n",losses,pct);

    if (ties) {
      pct = (double)ties * (double)100 / (double)total;
      printf("  ties      %7d (%5.2lf)\n",ties,pct);
    }

    printf("  total     %7d\n",total);

    if (bDebug) {
      printf("  num_evaluations        %10d\n",num_evaluations);
      printf("  num_unique_evaluations %10d\n",num_unique_evaluations);
      printf("  num_comparisons        %10d\n",num_comparisons);
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

static void get_permutation_instance(
  int *m,int *n,int instance_ix
)
{
  if (instance_ix)
    goto after_return_point;

  for (*m = 0; *m < NUM_REMAINING_CARDS - NUM_CARDS_AFTER_FLOP + 1; (*m)++) {
    for (*n = *m + 1; *n < NUM_REMAINING_CARDS - NUM_CARDS_AFTER_FLOP + 2; (*n)++) {
      return;

      after_return_point:
      ;
    }
  }
}
