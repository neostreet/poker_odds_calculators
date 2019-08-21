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

#define MAX_FILENAME_LEN 1024
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: fpock_freq (-debug) (-verbose) (-detail) (-num_hands_genum)\n"
"  player_name filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char in_chips[] = " in chips";
#define IN_CHIPS_LEN (sizeof (in_chips) - 1)
static char summary[] = "*** SUMMARY ***";
#define SUMMARY_LEN (sizeof (summary) - 1)
static char dealt_to[] = "Dealt to ";
#define DEALT_TO_LEN (sizeof (dealt_to) - 1)

static char bad_rank_in_line[] = "bad rank in line %d of %s: %s\n";

static char pock_count[NUM_CARDS_IN_SUIT];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  bool bDetail;
  int num_hands_ge_num;
  int player_name_ix;
  int player_name_len;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  int dbg_line_no;
  int ix;
  int file_no;
  int dbg_file_no;
  int rank_ix;
  int any_pock_count;
  int total_hand_count;
  double pock_freq;
  int dbg;
  char hole_cards[6];
  bool bSkipping;

  if ((argc < 3) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bVerbose = false;
  bDetail = false;
  num_hands_ge_num = -1;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-detail"))
      bDetail = true;
    else if (!strncmp(argv[curr_arg],"-num_hands_ge",13))
      sscanf(&argv[curr_arg][13],"%d",&num_hands_ge_num);
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  player_name_ix = curr_arg++;
  player_name_len = strlen(argv[player_name_ix]);

  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  file_no = 0;
  dbg_file_no = -1;

  hole_cards[5] = 0;

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    file_no++;

    if (dbg_file_no == file_no)
      dbg = 1;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    line_no = 0;
    bSkipping = false;

    if (!bDetail)
      any_pock_count = 0;
    else {
      for (n = 0; n < NUM_CARDS_IN_SUIT; n++)
        pock_count[n] = 0;
    }

    total_hand_count = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if (line_no == dbg_line_no)
        dbg = 1;

      if (bDebug)
        printf("line %d %s\n",line_no,line);

      if (Contains(true,
        line,line_len,
        argv[player_name_ix],player_name_len,
        &ix)) {

        if (Contains(true,
          line,line_len,
          in_chips,IN_CHIPS_LEN,
          &ix)) {

          total_hand_count++;
          bSkipping = false;

          line[ix] = 0;

          for (ix--; (ix >= 0) && (line[ix] != '('); ix--)
            ;
        }
        else if (bSkipping)
          ;
        else if (!strncmp(line,dealt_to,DEALT_TO_LEN)) {
          for (n = 0; n < line_len; n++) {
            if (line[n] == '[')
              break;
          }

          if (n < line_len) {
            n++;

            for (m = n; m < line_len; m++) {
              if (line[m] == ']')
                break;
            }

            if (m < line_len) {
              for (p = 0; p < 5; p++) {
                if (line[n+p] == ']')
                  break;
                else
                  hole_cards[p] = line[n+p];
              }
            }
          }
        }
      }
      else if (bSkipping)
        ;
      else {
        if (!strncmp(line,summary,SUMMARY_LEN)) {
          if (bDebug)
            printf("line %d SUMMARY line detected; skipping\n",line_no);

          bSkipping = true;

          if (hole_cards[0] == hole_cards[3]) {
            if (!bDetail)
              any_pock_count++;
            else {
              for (rank_ix = 0; rank_ix < NUM_CARDS_IN_SUIT; rank_ix++) {
                if (hole_cards[0] == rank_chars[rank_ix])
                  break;
              }

              if (rank_ix == NUM_CARDS_IN_SUIT) {
                printf(bad_rank_in_line,line_no,filename,line);
                return 4;
              }

              pock_count[rank_ix]++;
            }
          }
        }
      }
    }

    fclose(fptr);

    if ((num_hands_ge_num == -1) || (total_hand_count >= num_hands_ge_num)) {
      if (!bDetail) {
        pock_freq = (double)any_pock_count / (double)total_hand_count * ANY_PAIR_PERIODICITY;

        if (!bVerbose)
          printf("%lf\n",pock_freq);
        else
          printf("%lf (%d %d) %s\n",pock_freq,total_hand_count,any_pock_count,filename);
      }
      else {
        for (n = 0; n < NUM_CARDS_IN_SUIT; n++) {
          if (pock_count[n]) {
            pock_freq = (double)pock_count[n] / (double)total_hand_count * PAIR_PERIODICITY;

            if (!bVerbose)
              printf("%c%c %lf\n",rank_chars[n],rank_chars[n],pock_freq);
            else {
              printf("%c%c %lf (%d %d) %s\n",rank_chars[n],rank_chars[n],pock_freq,
                total_hand_count,pock_count[n],filename);
            }
          }
        }
      }
    }
  }

  fclose(fptr0);

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

static int Contains(bool bCaseSens,char *line,int line_len,
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
