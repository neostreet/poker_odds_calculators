#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define NUM_PLAYERS 3

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: three_handed (-terse) (-verbose) filename";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bTerse;
  bool bVerbose;
  int m;
  int n;
  int retval;
  FILE *fptr;
  int line_no;
  int line_len;
  ThreeHanded th;
  int cards[NUM_THREE_HANDED_CARDS];
  struct outcomes *outcomes;
  int total;
  double pct;
  time_t start_time;
  time_t end_time;

  if ((argc < 2) || (argc > 4)) {
    cout << usage << endl;
    return 1;
  }

  bTerse = false;
  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    cout << usage << endl;
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  time(&start_time);

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if (line_no)
      putchar(0x0a);

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
      return 5;
    }

    for (n = 0; n < NUM_THREE_HANDED_CARDS; n++) {
      retval = card_value_from_card_string(&line[m],&cards[n]);

      if (retval) {
        printf(parse_error,line_no,n,5);
        return 6;
      }

      m += 2;

      if (n < NUM_THREE_HANDED_CARDS - 1) {
        // skip whitespace

        for ( ; m < line_len; m++) {
          if (line[m] != ' ')
            break;
        }

        if (m == line_len) {
          printf(parse_error,line_no,n,6);
          return 7;
        }
      }
    }

    th.NewCards(cards[0],cards[1],cards[2],cards[3],cards[4],cards[5]);
    th.Evaluate();
    outcomes = th.GetOutcomes();

    if (!bTerse)
      putchar(0x0a);

    for (n = 0; n < NUM_PLAYERS; n++) {
      if (!bTerse)
        printf("player %d\n",n+1);

      total = outcomes[n].wins + outcomes[n].losses + outcomes[n].ties;

      pct = (double)outcomes[n].wins * (double)100 / (double)total;

      if (!bTerse)
        printf("  wins      %7d (%5.2lf)\n",outcomes[n].wins,pct);
      else
        printf("%d ",outcomes[n].wins);

      pct = (double)outcomes[n].losses * (double)100 / (double)total;

      if (!bTerse)
        printf("  losses    %7d (%5.2lf)\n",outcomes[n].losses,pct);
      else
        printf("%d ",outcomes[n].losses);

      pct = (double)outcomes[n].ties * (double)100 / (double)total;

      if (!bTerse) {
        printf("  ties      %7d (%5.2lf)\n",outcomes[n].ties,pct);
        printf("  total     %7d\n",total);
      }
      else
        printf("%d\n",outcomes[n].ties);
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
