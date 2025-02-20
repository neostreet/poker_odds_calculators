#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: win_pct (-saw_flop) (-premium) (-not_premium) filename\n";

static char sf_str[] = ", sf";
#define SF_STR_LEN (sizeof (sf_str) - 1)

static char ws_str[] = ", ws";
#define WS_STR_LEN (sizeof (ws_str) - 1)

static char wws_str[] = ", wws";
#define WWS_STR_LEN (sizeof (wws_str) - 1)

static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static bool Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bSawFlop;
  bool bPremium;
  bool bNotPremium;
  FILE *fptr;
  int line_len;
  int line_no;
  int hands;
  int ix;
  int premium_ix;
  int pots_won;
  char hole_cards[6];
  char hole_cards_abbrev[4];
  double dwork;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bSawFlop = false;
  bPremium = false;
  bNotPremium = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-saw_flop"))
      bSawFlop = true;
    else if (!strcmp(argv[curr_arg],"-premium"))
      bPremium = true;
    else if (!strcmp(argv[curr_arg],"-not_premium"))
      bNotPremium = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bPremium && bNotPremium) {
    printf("can't specify both -premium and -not_premium\n");
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  line_no = 0;
  hands = 0;
  pots_won = 0;

  if (bPremium || bNotPremium) {
    hole_cards[2] = ' ';
    hole_cards[5] = 0;
    hole_cards_abbrev[3] = 0;
  }

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (bSawFlop) {
      if (!Contains(true,
        line,line_len,
        sf_str,SF_STR_LEN,
        &ix)) {

        continue;
      }
    }

    if (bPremium || bNotPremium) {
      if ((line[2] != ' ') || (line[5] && (line[5] != ' ') && (line[5] != ','))) {
        printf("invalid hole card delimiters in line %d\n",line_no);
        return 5;
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

      if (bPremium) {
        if (!is_premium_hand(hole_cards_abbrev,&premium_ix))
          continue;
      }
      else if (bNotPremium) {
        if (is_premium_hand(hole_cards_abbrev,&premium_ix))
          continue;
      }
    }

    hands++;

    if (Contains(true,
      line,line_len,
      ws_str,WS_STR_LEN,
      &ix) || Contains(true,
      line,line_len,
      wws_str,WWS_STR_LEN,
      &ix)) {

      pots_won++;
    }
  }

  fclose(fptr);

  if (!hands)
    dwork = (double)0;
  else
    dwork = (double)pots_won / (double)hands * (double)100;

  printf("%6.2lf%% (won %d of %d hands)\n",dwork,pots_won,hands);

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

static bool Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index)
{
  int m;
  int n;
  int tries;
  char chara;

  tries = line_len - string_len + 1;

  if (tries <= 0)
    return false;

  for (m = 0; m < tries; m++) {
    for (n = 0; n < string_len; n++) {
      chara = line[m + n];

      if (!bCaseSens) {
        if ((chara >= 'A') && (chara <= 'Z'))
          chara += 'a' - 'A';
      }

      if (chara != string[n])
        break;
    }

    if (n == string_len) {
      *index = m;
      return true;
    }
  }

  return false;
}
