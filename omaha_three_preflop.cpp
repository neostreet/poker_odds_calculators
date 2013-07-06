#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static void get_permutation_instance(
  int set_size,int subset_size,int *m,int *n,int *o,int *p,int *q,int instance_ix
);

#define NUM_PLAYERS 3
#define NUM_OMAHA_PREFLOP_CARDS (NUM_PLAYERS * NUM_HOLE_CARDS_IN_OMAHA_HAND)
#define NUM_REMAINING_CARDS (NUM_CARDS_IN_DECK - NUM_OMAHA_PREFLOP_CARDS)

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: omaha_three_preflop (-debug) filename";
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
  int q;
  int r;
  int s;
  int retval;
  FILE *fptr;
  int line_no;
  int line_len;
  int cards[NUM_OMAHA_PREFLOP_CARDS];
  int remaining_cards[NUM_REMAINING_CARDS];
  OmahaPokerHand omaha_hand[NUM_PLAYERS];
  PokerHand hand[NUM_PLAYERS];
  int ret_compare;
  int wins;
  int losses;
  int ties;
  int total;
  double pct;
  int work_wins;
  int work_losses;
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

    for (n = 0; n < NUM_OMAHA_PREFLOP_CARDS; n++) {
      retval = card_value_from_card_string(&line[m],&cards[n]);

      if (retval) {
        printf(parse_error,line_no,n,5);
        return 5;
      }

      m += 2;

      if (n < NUM_OMAHA_PREFLOP_CARDS - 1) {
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
      for (o = 0; o < NUM_OMAHA_PREFLOP_CARDS; o++) {
        if (n == cards[o])
          break;
      }

      if (o == NUM_OMAHA_PREFLOP_CARDS)
        remaining_cards[m++] = n;
    }

    wins = 0;
    losses = 0;
    ties = 0;
    total = 0;

    for (r = 0; r < POKER_40_5_PERMUTATIONS; r++) {
      get_permutation_instance(NUM_REMAINING_CARDS,NUM_CARDS_AFTER_DEAL,&m,&n,&o,&p,&q,r);

      omaha_hand[0].NewCards(cards[0],cards[1],cards[2],cards[3],
        remaining_cards[m],remaining_cards[n],
        remaining_cards[o],remaining_cards[p],
        remaining_cards[q]);

      omaha_hand[1].NewCards(cards[4],cards[5],cards[6],cards[7],
        remaining_cards[m],remaining_cards[n],
        remaining_cards[o],remaining_cards[p],
        remaining_cards[q]);

      omaha_hand[2].NewCards(cards[8],cards[9],cards[10],cards[11],
        remaining_cards[m],remaining_cards[n],
        remaining_cards[o],remaining_cards[p],
        remaining_cards[q]);

      for (s = 0; s < NUM_PLAYERS; s++)
        hand[s] = omaha_hand[s].BestPokerHand(false);

      work_wins = 0;
      work_losses = 0;

      for (s = 0; s < NUM_PLAYERS - 1; s++) {
        ret_compare = hand[0].Compare(hand[1+s],0);

        if (ret_compare == 1)
          work_wins++;
        else if (ret_compare == -1) {
          work_losses++;
          break;
        }
      }

      if (work_wins == NUM_PLAYERS - 1)
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

    pct = (double)ties * (double)100 / (double)total;
    printf("  ties      %7d (%5.2lf)\n",ties,pct);

    printf("  total     %7d\n",total);

    if (bDebug) {
      printf("  num_evaluations        %10d\n",num_evaluations);
      printf("  num_unique_evaluations %10d\n",num_unique_evaluations);
      printf("  num_comparisons        %10d\n",num_comparisons);
    }
  }

  fclose(fptr);
  time(&end_time);

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

static void get_permutation_instance(
  int set_size,int subset_size,int *m,int *n,int *o,int *p,int *q,int instance_ix
)
{
  if (instance_ix)
    goto after_return_point;

  for (*m = 0; *m < set_size - subset_size + 1; (*m)++) {
    for (*n = *m + 1; *n < set_size - subset_size + 2; (*n)++) {
      for (*o = *n + 1; *o < set_size - subset_size + 3; (*o)++) {
        for (*p = *o + 1; *p < set_size - subset_size + 4; (*p)++) {
          for (*q = *p + 1; *q < set_size - subset_size + 5; (*q)++) {
            return;

            after_return_point:
            ;
          }
        }
      }
    }
  }
}
