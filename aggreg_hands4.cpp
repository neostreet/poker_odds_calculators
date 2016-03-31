#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: aggreg_hands4 (-debug) (-verbose) filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

struct aggreg_info {
  int hand_count;
  int sum_delta;
  int sum_wins;
  int sum_losses;
  int num_wins;
  int num_losses;
  int num_washes;
};

static struct aggreg_info *aggreg;

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int index_of_hand(int *cards);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  int dbg_ix;
  int dbg;
  int m;
  int n;
  int retval;
  int cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND];
  int work;
  int ordered_cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND];
  FILE *fptr;
  int line_no;
  int line_len;
  int delta;
  int ix;
  char card_string[3];
  double freq_factor;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  aggreg = (struct aggreg_info *)malloc(POKER_52_2_PERMUTATIONS * sizeof (struct aggreg_info));

  for (n = 0; n < POKER_52_2_PERMUTATIONS; n++) {
    aggreg[n].hand_count = 0;
    aggreg[n].sum_delta = 0;
    aggreg[n].sum_wins = 0;
    aggreg[n].sum_losses = 0;
    aggreg[n].num_wins = 0;
    aggreg[n].num_losses = 0;
    aggreg[n].num_washes = 0;
  }

  line_no = 0;
  card_string[2] = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;
    m = 11;

    for (n = 0; n < NUM_HOLE_CARDS_IN_HOLDEM_HAND; n++) {
      retval = card_value_from_card_string(&line[m],&cards[n]);

      if (retval) {
        printf(parse_error,line_no,n,5);
        return 4;
      }

      m += 3;
    }

    if (cards[0] > cards[1]) {
      work = cards[0];
      cards[0] = cards[1];
      cards[1] = work;
    }

    sscanf(&line[0],"%d",&delta);

    ix = index_of_hand(cards);

    if (ix == dbg_ix)
      dbg = 1;

    if (ix >= POKER_52_2_PERMUTATIONS) {
      printf("error on line %d: ix = %d\n",line_no,ix);
      ix = 0;
    }

    aggreg[ix].hand_count++;
    aggreg[ix].sum_delta += delta;

    if (delta > 0) {
      aggreg[ix].sum_wins += delta;
      aggreg[ix].num_wins++;
    }
    else if (delta < 0) {
      aggreg[ix].sum_losses += delta;
      aggreg[ix].num_losses++;
    }
    else
      aggreg[ix].num_washes++;
  }

  fclose(fptr);

  for (m = 0; m < POKER_52_2_PERMUTATIONS; m++) {
    get_permutation_instance_two(
      NUM_CARDS_IN_DECK,
      &cards[0],&cards[1],m);

    if (cards[0] < cards[1]) {
      ordered_cards[0] = cards[1];
      ordered_cards[1] = cards[0];
    }
    else {
      ordered_cards[0] = cards[0];
      ordered_cards[1] = cards[1];
    }

    for (n = 0; n < NUM_HOLE_CARDS_IN_HOLDEM_HAND; n++) {
      card_string_from_card_value(ordered_cards[n],card_string);
      printf("%s ",card_string);
    }

    freq_factor = (double)aggreg[m].hand_count * (double)POKER_52_2_PERMUTATIONS /
      (double)line_no;

    if (bVerbose) {
      printf("%10d %10d %10d %6d %6d %6d %6d %6d %9.2lf\n",
        aggreg[m].sum_delta,
        aggreg[m].sum_wins,
        aggreg[m].sum_losses,
        aggreg[m].num_wins,
        aggreg[m].num_losses,
        aggreg[m].num_washes,
        aggreg[m].hand_count,
        line_no,
        freq_factor);
    }
    else {
      printf("%10d %10d %10d %6d %6d %6d %6d %9.2lf\n",
        aggreg[m].sum_delta,
        aggreg[m].sum_wins,
        aggreg[m].sum_losses,
        aggreg[m].num_wins,
        aggreg[m].num_losses,
        aggreg[m].num_washes,
        aggreg[m].hand_count,
        freq_factor);
    }
  }

  free(aggreg);

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

static int index_of_hand(int *cards)
{
  int n;
  int index;
  int num_other_cards;

  index = 0;
  num_other_cards = NUM_CARDS_IN_DECK - 1;

  for (n = 0; n < cards[0]; n++) {
    index += num_other_cards;
    num_other_cards--;
  }

  index += cards[1] - cards[0] - 1;

  return index;
}
