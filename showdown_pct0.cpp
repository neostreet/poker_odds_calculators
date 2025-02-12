#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: showdown_pct0 (-debug) filename\n";

static char ws_str[] = ", ws";
#define WS_STR_LEN (sizeof (ws_str) - 1)

static char ls_str[] = ", ls";
#define LS_STR_LEN (sizeof (ls_str) - 1)

static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static bool Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  FILE *fptr;
  int line_len;
  int ws_hands;
  int showdown_hands;
  int hands;
  int ix;
  double dwork1;
  double dwork2;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
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
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  ws_hands = 0;
  showdown_hands = 0;
  hands = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    hands++;

    if (Contains(true,
      line,line_len,
      ws_str,WS_STR_LEN,
      &ix)) {

      ws_hands++;
      showdown_hands++;
    }
    else if (Contains(true,
      line,line_len,
      ls_str,LS_STR_LEN,
      &ix)) {

      showdown_hands++;
    }
  }

  fclose(fptr);

  if (showdown_hands) {
    dwork1 = (double)ws_hands / (double) showdown_hands * (double)100;
    dwork2 = (double)showdown_hands / (double) hands * (double)100;

    printf("%6.2lf%% (won %3d of %3d showdowns) %6.2lf%% (went to showdown %3d times in %3d hands)\n",
      dwork1,ws_hands,showdown_hands,
      dwork2,showdown_hands,hands);
  }

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
