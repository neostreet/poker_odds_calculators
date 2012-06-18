#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char save_dir[_MAX_PATH];

#define MAX_FILENAME_LEN 1024
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: froyal_flush (-debug) (-fullpath) player_name filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char dealt_to[] = "Dealt to ";
#define DEALT_TO_LEN (sizeof (dealt_to) - 1)
static char board[] = "Board [";
#define BOARD_LEN (sizeof (board) - 1)

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static bool Contains(int bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  int q;
  int curr_arg;
  bool bDebug;
  bool bFullPath;
  int player_name_ix;
  int player_name_len;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  int ix;
  int file_no;
  int dbg_file_no;
  int dbg;
  char card_string[3];
  int retval;
  char hole_cards[6];
  int holdem_cards[NUM_CARDS_IN_HOLDEM_POOL];
  HoldemPokerHand holdem_hand;
  PokerHand hand;

  if ((argc < 3) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bFullPath = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-fullpath"))
      bFullPath = true;
    else
      break;
  }

  if (bDebug && !bFullPath)
    getcwd(save_dir,_MAX_PATH);

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  player_name_ix = curr_arg++;
  player_name_len = strlen(argv[player_name_ix]);

  hole_cards[5] = 0;

  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  file_no = 0;
  dbg_file_no = -1;
  card_string[2] = 0;

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

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if (Contains(true,
        line,line_len,
        argv[player_name_ix],player_name_len,
        &ix)) {

        if (!strncmp(line,dealt_to,DEALT_TO_LEN)) {
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
              for (p = 0; p < 5; p++)
                hole_cards[p] = line[n+p];
            }

            for (q = 0; q < NUM_HOLE_CARDS_IN_HOLDEM_HAND; q++) {
              card_string[0] = hole_cards[q*3 + 0];
              card_string[1] = hole_cards[q*3 + 1];

              retval = card_value_from_card_string(
                card_string,&holdem_cards[q]);

              if (retval) {
                printf("invalid card string %s on line %d\n",
                  card_string,line_no);
                return 4 + q;
              }
            }
          }
        }

        continue;
      }
      else if (Contains(true,
        line,line_len,
        board,BOARD_LEN,
        &ix)) {

        ix += BOARD_LEN;

        for (n = ix + 1; n < line_len; n++) {
          if (line[n] == ']') {
            line[n] = 0;
            break;
          }
        }

        if (n - ix == 14) {
          for (q = 0; q < NUM_CARDS_IN_HAND; q++) {
            card_string[0] = line[ix + q*3 + 0];
            card_string[1] = line[ix + q*3 + 1];

            retval = card_value_from_card_string(
              card_string,&holdem_cards[q+2]);

            if (retval) {
              printf("invalid card string %s on line %d\n",
                card_string,line_no);
              return 6 + q;
            }
          }

          holdem_hand.NewCards(holdem_cards[0],holdem_cards[1],holdem_cards[2],
            holdem_cards[3],holdem_cards[4],holdem_cards[5],holdem_cards[6]);

          hand = holdem_hand.BestPokerHand();

          if (hand.GetHandType() == ROYAL_FLUSH) {
            if (!bDebug)
              printf("%s %s\n",hole_cards,&line[ix]);
            else if (!bFullPath)
              printf("%s %s %s\\%s\n",hole_cards,&line[ix],save_dir,filename);
            else
              printf("%s %s %s\n",hole_cards,&line[ix],filename);
          }
        }

        break;
      }
    }

    fclose(fptr);
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

static bool Contains(int bCaseSens,char *line,int line_len,
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
