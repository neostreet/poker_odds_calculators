#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define NUM_PLAYERS 2

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: heads_up (-verbose) filename";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  int m;
  int n;
  int retval;
  FILE *fptr;
  int line_no;
  int line_len;
  HeadsUp hu;
  int cards[NUM_HEADS_UP_CARDS];
  struct outcomes *outcomes;
  int total;
  double pct;
  time_t start_time;
  time_t end_time;

  if ((argc < 2) || (argc > 3)) {
    cout << usage << endl;
    return 1;
  }

  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
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

    for (n = 0; n < NUM_HEADS_UP_CARDS; n++) {
      retval = card_value_from_card_string(&line[m],&cards[n]);

      if (retval) {
        printf(parse_error,line_no,n,5);
        return 5;
      }

      m += 2;

      if (n < NUM_HEADS_UP_CARDS - 1) {
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

    hu.NewCards(cards[0],cards[1],cards[2],cards[3]);
    hu.Evaluate();
    outcomes = hu.GetOutcomes();

    putchar(0x0a);

    for (n = 0; n < NUM_PLAYERS; n++) {
      printf("player %d\n",n+1);
      total = outcomes[n].wins + outcomes[n].losses + outcomes[n].ties;

      pct = (double)outcomes[n].wins * (double)100 / (double)total;
      printf("  wins      %7d (%5.2lf)\n",outcomes[n].wins,pct);

      if (bVerbose) {
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

      if (bVerbose) {
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

      if (bVerbose) {
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

    if (bVerbose) {
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
