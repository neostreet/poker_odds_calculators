#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define NUM_PLAYERS 2
#define NUM_PREFLOP_CARDS (NUM_PLAYERS * NUM_HOLE_CARDS_IN_HOLDEM_HAND)
#define NUM_REMAINING_CARDS (NUM_CARDS_IN_DECK - NUM_PREFLOP_CARDS)

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: heads_up (-debug) (-compare_low) filename";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bCompareLow;
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  int retval;
  FILE *fptr;
  int line_no;
  int line_len;
  int cards[NUM_PREFLOP_CARDS];
  int remaining_cards[NUM_REMAINING_CARDS];
  HoldemPokerHand holdem_hand[NUM_PLAYERS];
  PokerHand hand[NUM_PLAYERS];
  int ret_compare;
  struct outcomes outcomes[NUM_PLAYERS];
  int total;
  double pct;
  time_t start_time;
  time_t end_time;

  if ((argc < 2) || (argc > 4)) {
    cout << usage << endl;
    return 1;
  }

  bDebug = false;
  bCompareLow = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-compare_low"))
      bCompareLow = true;
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

    for (n = 0; n < NUM_PREFLOP_CARDS; n++) {
      retval = card_value_from_card_string(&line[m],&cards[n]);

      if (retval) {
        printf(parse_error,line_no,n,5);
        return 5;
      }

      m += 2;

      if (n < NUM_PREFLOP_CARDS - 1) {
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
      for (o = 0; o < NUM_PREFLOP_CARDS; o++) {
        if (n == cards[o])
          break;
      }

      if (o == NUM_PREFLOP_CARDS)
        remaining_cards[m++] = n;
    }

    for (n = 0; n < NUM_PLAYERS; n++) {
      outcomes[n].wins = 0;
      outcomes[n].losses = 0;
      outcomes[n].ties = 0;

      if (bDebug) {
        for (m = 0; m < NUM_HAND_TYPES; m++) {
          outcomes[n].wins_hand_counts[m] = 0;
          outcomes[n].losses_hand_counts[m] = 0;
          outcomes[n].ties_hand_counts[m] = 0;
        }
      }
    }

    for (r = 0; r < POKER_48_5_PERMUTATIONS; r++) {
      get_permutation_instance_five(NUM_REMAINING_CARDS,&m,&n,&o,&p,&q,r);

      holdem_hand[0].NewCards(cards[0],cards[1],
        remaining_cards[m],remaining_cards[n],
        remaining_cards[o],remaining_cards[p],
        remaining_cards[q]);

      holdem_hand[1].NewCards(cards[2],cards[3],
        remaining_cards[m],remaining_cards[n],
        remaining_cards[o],remaining_cards[p],
        remaining_cards[q]);

      hand[0] = holdem_hand[0].BestPokerHand();
      hand[1] = holdem_hand[1].BestPokerHand();

      if (!bCompareLow)
        ret_compare = hand[0].Compare(hand[1],0);
      else
        ret_compare = hand[0].CompareLow(hand[1],0);

      if (ret_compare == 1) {
        outcomes[0].wins++;
        outcomes[1].losses++;

        if (bDebug) {
          outcomes[0].wins_hand_counts[hand[0].GetHandType()]++;
          outcomes[1].losses_hand_counts[hand[1].GetHandType()]++;
        }
      }
      else if (ret_compare == -1) {
        outcomes[0].losses++;
        outcomes[1].wins++;

        if (bDebug) {
          outcomes[0].losses_hand_counts[hand[0].GetHandType()]++;
          outcomes[1].wins_hand_counts[hand[1].GetHandType()]++;
        }
      }
      else {
        outcomes[0].ties++;
        outcomes[1].ties++;

        if (bDebug) {
          outcomes[0].ties_hand_counts[hand[0].GetHandType()]++;
          outcomes[1].ties_hand_counts[hand[1].GetHandType()]++;
        }
      }
    }

    putchar(0x0a);

    for (n = 0; n < NUM_PLAYERS; n++) {
      printf("player %d\n",n+1);
      total = outcomes[n].wins + outcomes[n].losses + outcomes[n].ties;

      pct = (double)outcomes[n].wins * (double)100 / (double)total;
      printf("  wins      %7d (%5.2lf)\n",outcomes[n].wins,pct);

      if (bDebug) {
        for (m = 0; m < NUM_HAND_TYPES; m++) {
          if (outcomes[n].wins_hand_counts[m]) {
            printf("    %s      %7d\n",
              hand_type_abbrevs[m],
              outcomes[n].wins_hand_counts[m]);
          }
        }
      }

      pct = (double)outcomes[n].losses * (double)100 / (double)total;
      printf("  losses    %7d (%5.2lf)\n",outcomes[n].losses,pct);

      if (bDebug) {
        for (m = 0; m < NUM_HAND_TYPES; m++) {
          if (outcomes[n].losses_hand_counts[m]) {
            printf("    %s      %7d\n",
              hand_type_abbrevs[m],
              outcomes[n].losses_hand_counts[m]);
          }
        }
      }

      pct = (double)outcomes[n].ties * (double)100 / (double)total;
      printf("  ties      %7d (%5.2lf)\n",outcomes[n].ties,pct);

      if (bDebug) {
        for (m = 0; m < NUM_HAND_TYPES; m++) {
          if (outcomes[n].ties_hand_counts[m]) {
            printf("    %s      %7d\n",
              hand_type_abbrevs[m],
              outcomes[n].ties_hand_counts[m]);
          }
        }
      }

      printf("  total     %7d\n",total);
    }

    if (bDebug) {
      putchar(0x0a);
      printf("num_evaluations                        %10d\n",num_evaluations);
      printf("num_unique_evaluations                 %10d\n",num_unique_evaluations);
      printf("num_comparisons                        %10d\n",num_comparisons);
      printf("num_holdem_best_poker_hand_comparisons %10d\n",num_holdem_best_poker_hand_comparisons);
      printf("total_comparisons                      %10d\n",
        num_comparisons + num_holdem_best_poker_hand_comparisons);
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
