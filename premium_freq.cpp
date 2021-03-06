#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <direct.h>
#else
#define _MAX_PATH 4096
#include <unistd.h>
#endif
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define TOTAL_PREMIUM_HANDS 58

static char save_dir[_MAX_PATH];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: premium_freq (-only_zero) (-total_hands_first) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char dealt_to[] = "Dealt to ";
#define DEALT_TO_LEN (sizeof (dealt_to) - 1)

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bOnlyZero;
  bool bTotalHandsFirst;
  FILE *fptr;
  int line_len;
  int line_no;
  char hole_cards_abbrev[4];
  int total_hands;
  int total_premium_hands;
  double dwork;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bOnlyZero = false;
  bTotalHandsFirst = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-only_zero"))
      bOnlyZero = true;
    else if (!strcmp(argv[curr_arg],"-total_hands_first"))
      bTotalHandsFirst = true;
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

  getcwd(save_dir,_MAX_PATH);

  line_no = 0;
  total_hands = 0;
  total_premium_hands = 0;
  hole_cards_abbrev[3] = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (!strncmp(line,dealt_to,DEALT_TO_LEN)) {
      total_hands++;
      get_abbrev(&line[20],hole_cards_abbrev);

      if (is_premium_hand(hole_cards_abbrev))
        total_premium_hands++;
    }
  }

  dwork = (double)total_hands / (double)POKER_52_2_PERMUTATIONS;
  dwork = (double)total_premium_hands / dwork;
  dwork /= (double)TOTAL_PREMIUM_HANDS;

  if (!bOnlyZero || !total_premium_hands) {
    if (!bTotalHandsFirst)
      printf("%lf (%d %d) %s\n",dwork,total_hands,total_premium_hands,save_dir);
    else
      printf("%d %d %lf %s\n",total_hands,total_premium_hands,dwork,save_dir);
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
