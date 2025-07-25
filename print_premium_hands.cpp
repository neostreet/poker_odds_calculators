#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: print_premium_hands (-verbose) (-hole_cards) (-not) (-saw_flop)\n"
"  (-won_pot) (-lost_pot) (-offsetval) filename\n";

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
  bool bVerbose;
  bool bHoleCards;
  bool bNot;
  bool bSawFlop;
  bool bWonPot;
  bool bLostPot;
  int offset;
  bool bPrint;
  FILE *fptr;
  int line_len;
  int premium_ix;
  int hands;
  char hole_cards[6];
  char hole_cards_abbrev[4];
  int ix;

  if ((argc < 2) || (argc > 9)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bHoleCards = false;
  bNot = false;
  bSawFlop = false;
  bWonPot = false;
  bLostPot = false;
  offset = 0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-hole_cards"))
      bHoleCards = true;
    else if (!strcmp(argv[curr_arg],"-not"))
      bNot = true;
    else if (!strcmp(argv[curr_arg],"-saw_flop"))
      bSawFlop = true;
    else if (!strcmp(argv[curr_arg],"-won_pot"))
      bWonPot = true;
    else if (!strcmp(argv[curr_arg],"-lost_pot"))
      bLostPot = true;
    else if (!strncmp(argv[curr_arg],"-offset",7))
      sscanf(&argv[curr_arg][7],"%d",&offset);
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bWonPot && bLostPot) {
    printf("can't specify both -won_pot and -lost_pot\n");
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  hands = 0;

  if (bHoleCards) {
    hole_cards[2] = ' ';
    hole_cards[5] = 0;
    hole_cards_abbrev[3] = 0;
  }

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    hands++;

    if (bSawFlop) {
      if (!Contains(true,
        line,line_len,
        sf_str,SF_STR_LEN,
        &ix)) {

        continue;
      }
    }

    if (bWonPot) {
      if (!Contains(true,
        line,line_len,
        ws_str,WS_STR_LEN,
        &ix) && !Contains(true,
        line,line_len,
        wws_str,WWS_STR_LEN,
        &ix)) {

        continue;
      }
    }
    else if (bLostPot) {
      if (Contains(true,
        line,line_len,
        ws_str,WS_STR_LEN,
        &ix) || Contains(true,
        line,line_len,
        wws_str,WWS_STR_LEN,
        &ix)) {

        continue;
      }
    }

    if (!bHoleCards) {
      if (!bNot)
        bPrint = is_premium_hand(line,&premium_ix);
      else
        bPrint = !is_premium_hand(line,&premium_ix);
    }
    else {
      if ((line[offset+2] != ' ') || (line[offset+5] && (line[offset+5] != ' ') && (line[offset+5] != ','))) {
        printf("invalid hole card delimiters in line %d\n",hands);
        return 5;
      }

      if ((line[0] >= 'a') && (line[0] <= 'z'))
        line[0] -= 'a' - 'A';

      if ((line[3] >= 'a') && (line[3] <= 'z'))
        line[3] -= 'a' - 'A';

      hole_cards[0] = line[offset+0];
      hole_cards[1] = line[offset+1];
      hole_cards[3] = line[offset+3];
      hole_cards[4] = line[offset+4];

      get_abbrev(hole_cards,hole_cards_abbrev);

      if (!bNot)
        bPrint = is_premium_hand(hole_cards_abbrev,&premium_ix);
      else
        bPrint = !is_premium_hand(hole_cards_abbrev,&premium_ix);
    }

    if (bPrint) {
      if (!bVerbose)
        printf("%s\n",line);
      else
        printf("%s hand %d\n",line,hands);
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
