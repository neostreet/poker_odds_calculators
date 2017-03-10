#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define NUM_PCT_AT_FLOP_CARDS 5
#define NUM_REMAINING_CARDS1 (NUM_CARDS_IN_DECK - NUM_PCT_AT_FLOP_CARDS)
#define NUM_REMAINING_CARDS2 (NUM_CARDS_IN_DECK - NUM_PCT_AT_FLOP_CARDS - 2)

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: pct_at_flop (-debug) (-deep_debug) (-show_losses) filename";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  int deep_debug;
  bool bShowLosses;
  int deep_debug_counter;
  int outer_m;
  int outer_n;
  int outer_o;
  int m;
  int n;
  int o;
  int p;
  int q;
  int retval;
  FILE *fptr;
  int line_no;
  int line_len;
  int cards[NUM_PCT_AT_FLOP_CARDS];
  int remaining_cards1[NUM_REMAINING_CARDS1];
  int remaining_cards2[NUM_REMAINING_CARDS2];
  HoldemPokerHand holdem_hand1;
  HoldemPokerHand holdem_hand2;
  PokerHand hand1;
  PokerHand hand2;
  int ret_compare;
  int wins;
  int losses;
  int ties;
  int total;
  double pct;
  int hand1_winning_hand_counts[NUM_HAND_TYPES];
  int hand2_winning_hand_counts[NUM_HAND_TYPES];
  int tie_hand_counts[NUM_HAND_TYPES];
  int total_hand_counts[NUM_HAND_TYPES];
  time_t start_time;
  time_t end_time;

  if ((argc < 2) || (argc > 4)) {
    cout << usage << endl;
    return 1;
  }

  bDebug = false;
  deep_debug = 0;
  bShowLosses = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strncmp(argv[curr_arg],"-deep_debug",11))
      sscanf(&argv[curr_arg][11],"%d",&deep_debug);
    else if (!strcmp(argv[curr_arg],"-show_losses"))
      bShowLosses = true;
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

    for (n = 0; n < NUM_PCT_AT_FLOP_CARDS; n++) {
      retval = card_value_from_card_string(&line[m],&cards[n]);

      if (retval) {
        printf(parse_error,line_no,n,5);
        return 5;
      }

      m += 2;

      if (n < NUM_PCT_AT_FLOP_CARDS - 1) {
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
      for (o = 0; o < NUM_PCT_AT_FLOP_CARDS; o++) {
        if (n == cards[o])
          break;
      }

      if (o == NUM_PCT_AT_FLOP_CARDS)
        remaining_cards1[m++] = n;
    }

    wins = 0;
    losses = 0;
    ties = 0;
    total = 0;

    if (bDebug) {
      for (o = 0; o < NUM_HAND_TYPES; o++) {
        hand1_winning_hand_counts[o] = 0;
        hand2_winning_hand_counts[o] = 0;
        tie_hand_counts[o] = 0;
        total_hand_counts[o] = 0;
      }
    }

    deep_debug_counter = 0;

    for (outer_o = 0; outer_o < POKER_47_2_PERMUTATIONS; outer_o++) {
      get_permutation_instance_two(
        NUM_REMAINING_CARDS1,
        &outer_m,&outer_n,outer_o);

      q = 0;

      for (p = 0; p < NUM_REMAINING_CARDS1; p++) {
        if ((p == outer_m) || (p == outer_n))
          continue;

        remaining_cards2[q++] = remaining_cards1[p];
      }

      for (o = 0; o < POKER_45_2_PERMUTATIONS; o++, deep_debug_counter++) {
        get_permutation_instance_two(
          NUM_REMAINING_CARDS2,
          &m,&n,o);

        if (deep_debug > 1) {
          if (deep_debug_counter != deep_debug)
            continue;
        }

        holdem_hand1.NewCards(cards[0],cards[1],
          cards[2],cards[3],cards[4],
          remaining_cards2[m],remaining_cards2[n]);

        holdem_hand2.NewCards(
          remaining_cards1[outer_m],remaining_cards1[outer_n],
          cards[2],cards[3],cards[4],
          remaining_cards2[m],remaining_cards2[n]);

        hand1 = holdem_hand1.BestPokerHand(false);
        hand2 = holdem_hand2.BestPokerHand(false);

        ret_compare = hand1.Compare(hand2,0,false);

        if (deep_debug) {
          cout << o << " " << deep_debug_counter << " " << endl;
          holdem_hand1.print(cout);
          cout << endl;
          holdem_hand2.print(cout);
          cout << endl;
          hand1.print(cout);
          cout << endl;
          hand2.print(cout);
          cout << endl;
          cout << ret_compare << endl;
        }

        if (ret_compare == 1) {
          wins++;

          if (bDebug)
            hand1_winning_hand_counts[hand1.GetHandType()]++;
        }
        else if (ret_compare == -1) {
          losses++;

          if (bDebug)
            hand2_winning_hand_counts[hand2.GetHandType()]++;

          if (bShowLosses) {
            hand1.print(cout);
            cout << endl;
            hand2.print(cout);
            cout << endl;
          }
        }
        else {
          ties++;

          if (bDebug)
            tie_hand_counts[hand1.GetHandType()]++;
        }

        total++;
      }
    }

    pct = (double)wins * (double)100 / (double)total;
    printf("  wins      %7d (%5.2lf)\n",wins,pct);

    if (bDebug) {
      for (o = 0; o < NUM_HAND_TYPES; o++) {
        if (hand1_winning_hand_counts[o]) {
          printf("    %s      %7d\n",
            hand_type_abbrevs[o],
            hand1_winning_hand_counts[o]);

          total_hand_counts[o] += hand1_winning_hand_counts[o];
        }
      }
    }

    pct = (double)losses * (double)100 / (double)total;
    printf("  losses    %7d (%5.2lf)\n",losses,pct);

    if (bDebug) {
      for (o = 0; o < NUM_HAND_TYPES; o++) {
        if (hand2_winning_hand_counts[o]) {
          printf("    %s      %7d\n",
            hand_type_abbrevs[o],
            hand2_winning_hand_counts[o]);

          total_hand_counts[o] += hand2_winning_hand_counts[o];
        }
      }
    }

    if (ties) {
      pct = (double)ties * (double)100 / (double)total;
      printf("  ties      %7d (%5.2lf)\n",ties,pct);

      if (bDebug) {
        for (o = 0; o < NUM_HAND_TYPES; o++) {
          if (tie_hand_counts[o]) {
            printf("    %s      %7d\n",
              hand_type_abbrevs[o],
              tie_hand_counts[o]);

            total_hand_counts[o] += tie_hand_counts[o];
          }
        }
      }
    }

    printf("  total     %7d\n",total);

    if (bDebug) {
      for (o = 0; o < NUM_HAND_TYPES; o++) {
        if (total_hand_counts[o]) {
          printf("    %s      %7d\n",
            hand_type_abbrevs[o],
            total_hand_counts[o]);
        }
      }
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
