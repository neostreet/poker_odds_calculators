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
"usage: fshowdown_count (-verbose) (-not) (-show_board) (-show_best_hand)\n"
"  (-show_all_hands) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char show_down[] = "*** SHOW DOWN ***";
#define SHOW_DOWN_LEN (sizeof (show_down) - 1)

static char board[] = "Board [";
#define BOARD_LEN (sizeof (board) - 1)

static char and_won[] = " and won ";
#define AND_WON_LEN (sizeof (and_won) - 1)

#define MAX_PLAYER_NAME_LEN 32

static char button[] = " (button) ";
#define BUTTON_LEN (sizeof (button) - 1)

static char small_blind[] = " (small blind) ";
#define SMALL_BLIND_LEN (sizeof (small_blind) - 1)

static char big_blind[] = " (big blind) ";
#define BIG_BLIND_LEN (sizeof (big_blind) - 1)

static char mucked[] = " mucked ";
#define MUCKED_LEN (sizeof (mucked) - 1)

static char showed[] = " showed ";
#define SHOWED_LEN (sizeof (showed) - 1)

#define MAX_SHOWDOWN_HANDS 9

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);
static int get_player_name(char *line,int line_len,char *player_name,int max_player_name_len);

int main(int argc,char **argv)
{
  int m;
  int n;
  int q;
  int curr_arg;
  bool bVerbose;
  bool bNot;
  bool bShowBoard;
  bool bShowBestHand;
  bool bShowAllHands;
  bool bHaveShowdown;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  int dbg_line_no;
  int dbg;
  int file_no;
  int dbg_file_no;
  int showdown_count;
  int local_showdown_count;
  int ix;
  int retval;
  char card_string[2];
  int cards[NUM_CARDS_IN_HOLDEM_POOL];
  char player_name[MAX_PLAYER_NAME_LEN+1];
  HoldemPokerHand holdem_hand;
  PokerHand poker_hand;

  if ((argc < 2) || (argc > 7)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bNot = false;
  bShowBoard = false;
  bShowBestHand = false;
  bShowAllHands = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-not"))
      bNot = true;
    else if (!strcmp(argv[curr_arg],"-show_board")) {
      bShowBoard = true;
      bVerbose = true;
    }
    else if (!strcmp(argv[curr_arg],"-show_best_hand")) {
      bShowBestHand = true;
      bVerbose = true;
    }
    else if (!strcmp(argv[curr_arg],"-show_all_hands")) {
      bShowAllHands = true;
      bVerbose = true;
      bShowBoard = true;
    }
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bShowBestHand && bShowAllHands) {
    printf("can't specify both -show_best_hand and -show_all_hands\n");
    return 3;
  }


  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  file_no = 0;
  dbg_file_no = 1;
  showdown_count = 0;

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    file_no++;

    if (file_no == dbg_file_no)
      dbg = 1;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    line_no = 0;
    dbg_line_no = -1;
    bHaveShowdown = false;
    local_showdown_count = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if (line_no == dbg_line_no)
        dbg = 1;

      if (!strncmp(line,show_down,SHOW_DOWN_LEN)) {
        bHaveShowdown = true;

        if (!bNot) {
          if (!bVerbose)
            showdown_count++;
          else if (!bShowBoard && !bShowBestHand && !bShowAllHands)
            printf("%s %d\n",filename,file_no);
        }

        if (!bShowBoard && !bShowBestHand && !bShowAllHands)
          break;
      }
      else if (bHaveShowdown && bShowBoard && !strncmp(line,board,BOARD_LEN)) {
        if (!bNot) {
          for (q = 0; q < NUM_COMMUNITY_CARDS; q++) {
            card_string[0] = line[BOARD_LEN + q*3 + 0];
            card_string[1] = line[BOARD_LEN + q*3 + 1];

            retval = card_value_from_card_string(
              card_string,&cards[q+2]);

            if (retval) {
              printf("invalid card string %s on line %d\n",
                card_string,line_no);
              return 5;
            }
          }

          if (!bShowAllHands) {
            poker_hand.NewCards(cards[2],cards[3],cards[4],cards[5],cards[6]);
            poker_hand.Evaluate();
            cout << poker_hand << endl;
          }
        }
      }
      else if (bHaveShowdown && bShowBestHand && Contains(true,line,line_len,and_won,AND_WON_LEN,&ix)) {
        if (!bNot) {
          line[ix-1] = 0;
          printf("%s %s\n",filename,&line[ix-6]);
        }
      }
      else if (bHaveShowdown && bShowAllHands && !strncmp(line,"Seat ",5) && Contains(true,line,line_len,"[",1,&ix)) {
        if (!bNot) {
          retval = get_player_name(line,line_len,player_name,MAX_PLAYER_NAME_LEN);

          if (retval) {
            printf("get_player_name() failed on line %d: %d\n",
              line_no,retval);
            return 6;
          }

          line[ix+6] = 0;

          for (q = 0; q < NUM_HOLE_CARDS_IN_HOLDEM_HAND; q++) {
            card_string[0] = line[ix + 1 + q*3 + 0];
            card_string[1] = line[ix + 1 + q*3 + 1];

            retval = card_value_from_card_string(
              card_string,&cards[q]);

            if (retval) {
              printf("invalid card string %s on line %d: %d\n",
                card_string,line_no,retval);
              return 7;
            }
          }

          holdem_hand.NewCards(cards[0],cards[1],cards[2],cards[3],cards[4],cards[5],cards[6]);
          poker_hand = holdem_hand.BestPokerHand(false);
          cout << poker_hand << " " << &line[ix+1] << " " << player_name << " " << filename << endl;
        }
      }
    }

    fclose(fptr);

    if (bNot && !bHaveShowdown) {
      if (!bVerbose)
        showdown_count++;
      else
        printf("%s %d\n",filename,file_no);
    }
  }

  fclose(fptr0);

  if (!bVerbose)
    printf("%d\n",showdown_count);

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

static int get_player_name(char *line,int line_len,char *player_name,int max_player_name_len)
{
  int n;
  int ix;
  bool bFound;

  bFound = false;

  if (Contains(true,line,line_len,button,BUTTON_LEN,&ix))
    bFound = true;
  else if (Contains(true,line,line_len,small_blind,SMALL_BLIND_LEN,&ix))
    bFound = true;
  else if (Contains(true,line,line_len,big_blind,BIG_BLIND_LEN,&ix))
    bFound = true;
  else if (Contains(true,line,line_len,mucked,MUCKED_LEN,&ix))
    bFound = true;
  else if (Contains(true,line,line_len,showed,SHOWED_LEN,&ix))
    bFound = true;

  if (!bFound)
    return 1;

  if (ix - 8 > MAX_PLAYER_NAME_LEN)
    return 2;

  for (n = 8; n < ix; n++)
    player_name[n - 8] = line[n];

  player_name[n - 8] = 0;

  return 0;
}
