#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define MAX_FILENAME_LEN 1024
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: premium_pct2 (-not) filename\n";

static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bNot;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int hands;
  int premium_hands;
  int total_hands;
  int total_premium_hands;
  char hole_cards[6];
  char hole_cards_abbrev[4];
  int premium_ix;
  double dwork;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bNot = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-not"))
      bNot = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  total_hands = 0;
  total_premium_hands = 0;

  hole_cards[2] = ' ';
  hole_cards[5] = 0;
  hole_cards_abbrev[3] = 0;

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      return 4;
    }

    hands = 0;
    premium_hands = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      hands++;

      if ((line[2] != ' ') || (line[5] && (line[5] != ' ') && (line[5] != ','))) {
        printf("invalid hole card delimiters in line %d\n",hands);
        return 4;
      }

      if ((line[0] >= 'a') && (line[0] <= 'z'))
        line[0] -= 'a' - 'A';

      if ((line[3] >= 'a') && (line[3] <= 'z'))
        line[3] -= 'a' - 'A';

      hole_cards[0] = line[0];
      hole_cards[1] = line[1];
      hole_cards[3] = line[3];
      hole_cards[4] = line[4];

      get_abbrev(hole_cards,hole_cards_abbrev);

      if (!bNot) {
        if (is_premium_hand(hole_cards_abbrev,&premium_ix))
          premium_hands++;
      }
      else {
        if (!is_premium_hand(hole_cards_abbrev,&premium_ix))
          premium_hands++;
      }
    }

    fclose(fptr);

    total_hands += hands;
    total_premium_hands += premium_hands;

    if (!hands)
      dwork = (double)0;
    else
      dwork = (double)premium_hands / (double)hands * (double)100;

    printf("%6.2lf%% (%d %d) %s\n",dwork,premium_hands,hands,filename);
  }

  fclose(fptr0);

  if (!total_hands)
    dwork = (double)0;
  else
    dwork = (double)total_premium_hands / (double)total_hands * (double)100;

  printf("\n%6.2lf%% (%d %d)\n",dwork,total_premium_hands,total_hands);

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
