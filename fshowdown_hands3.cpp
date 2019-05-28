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

#define MAX_PLAYER_NAME_LEN 30
static char player_name[MAX_PLAYER_NAME_LEN+1];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char buf[MAX_LINE_LEN];
static char buf2[MAX_LINE_LEN];

static char usage[] =
"usage: fshowdown_hands3 (-verbose) (-debug) (-abbrev) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char pokerstars[] = "PokerStars";
#define POKERSTARS_LEN (sizeof (pokerstars) - 1)
static char show_down[] = "*** SHOW DOWN ***";
#define SHOW_DOWN_LEN (sizeof (show_down) - 1)
static char summary[] = "*** SUMMARY ***";
#define SUMMARY_LEN (sizeof (summary) - 1)

static char space[] = " ";
static char indent[] = "  ";
static char linefeed[] = "\n";

static char hole_cards_abbrev[4];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);
static char *get_bracketed_string(char *line,int line_len);
void get_player_name(char *line,int line_len);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  bool bDebug;
  bool bAbbrev;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  int dbg_line_no;
  int ix;
  int file_no;
  int dbg_file_no;
  int num_hands;
  int num_qualifying_hands;
  int dbg;
  char *cpt;
  char *cpt2;
  int showdown_hands;
  int cards[NUM_CARDS_IN_HOLDEM_POOL];
  PokerHand poker_hand;
  HoldemPokerHand holdem_hand;
  char card_string[3];
  int retval;
  bool bHaveShowdown;
  int opened_count;
  int closed_count;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bDebug = false;
  bAbbrev = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-abbrev"))
      bAbbrev = true;
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

  hole_cards_abbrev[3] = 0;

  file_no = 0;
  dbg_file_no = -1;

  if (bDebug) {
    opened_count = 0;
    closed_count = 0;
  }

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

    if (bDebug) {
      opened_count++;
      printf("opened_count = %d\n",opened_count);
      printf("closed_count = %d\n",closed_count);
    }

    line_no = 0;
    num_hands = 0;
    num_qualifying_hands = 0;
    bHaveShowdown = false;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if (line_no == dbg_line_no)
        dbg = 1;

      if (bDebug)
        printf("line %d of %s read in outer GetLine() loop\n",line_no,filename);

      if (Contains(true,
        line,line_len,
        pokerstars,POKERSTARS_LEN,
        &ix)) {
        num_hands++;
        bHaveShowdown = false;

        if (bDebug) {
          printf("new hand %d started at line %d of %s\n",
            num_hands,line_no,filename);
        }
      }
      else if (!strncmp(line,show_down,SHOW_DOWN_LEN))
        bHaveShowdown = true;
      else if (bHaveShowdown && !strncmp(line,summary,SUMMARY_LEN)) {
        num_qualifying_hands++;
        showdown_hands = 0;
        buf[0] = 0;

        for ( ; ; ) {
          GetLine(fptr,line,&line_len,MAX_LINE_LEN);

          if (feof(fptr))
            break;

          line_no++;

          if (line_no == dbg_line_no)
            dbg = 1;

          if (bDebug)
            printf("line %d of %s read in inner GetLine() loop\n",line_no,filename);

          if (!line_len || ((line_len == 1) && (line[0] == '\r'))) {
            if (showdown_hands)
              printf("%s",buf);

            break;
          }

          if (!strncmp(line,"Board ",6)) {
            cpt = get_bracketed_string(line,line_len);

            if (cpt != NULL) {
              strcat(buf,cpt);

              if (bVerbose) {
                sprintf(buf2," %s %3d (%d)",filename,num_hands,file_no);
                strcat(buf,buf2);
              }

              strcat(buf,linefeed);

              for (n = 0; n < NUM_COMMUNITY_CARDS; n++) {
                card_string[0] = cpt[1 + n*3 + 0];
                card_string[1] = cpt[1 + n*3 + 1];

                retval = card_value_from_card_string(
                  card_string,&cards[n]);

                if (retval) {
                  printf("invalid card string %s on line %d of file %s\n",
                    card_string,line_no,filename);
                  return 4;
                }
              }
            }
          }
          else if (!strncmp(line,"Seat ",5)) {
            get_player_name(line,line_len);
            cpt = get_bracketed_string(line,line_len);

            if (!bAbbrev)
              cpt2 = cpt;
            else {
              if (Contains(true,
                line,line_len,
                "[",1,
                &ix)) {
                get_abbrev(&line[ix+1],&hole_cards_abbrev[0]);
                cpt2 = hole_cards_abbrev;
              }
              else
                cpt2 = cpt;
            }

            if (cpt2 != NULL) {
              strcat(buf,indent);
              strcat(buf,cpt2);

              showdown_hands++;

              for (n = 0; n < NUM_HOLE_CARDS_IN_HOLDEM_HAND; n++) {
                card_string[0] = cpt[1 + n*3 + 0];
                card_string[1] = cpt[1 + n*3 + 1];

                retval = card_value_from_card_string(
                  card_string,&cards[NUM_COMMUNITY_CARDS+n]);

                if (retval) {
                  printf("invalid card string %s on line %d\n",
                    card_string,line_no);
                  printf("opened_count = %d\n",opened_count);
                  printf("closed_count = %d\n",closed_count);
                  return 4;
                }
              }

              holdem_hand.NewCards(cards[0],cards[1],cards[2],
                cards[3],cards[4],cards[5],cards[6]);

              poker_hand = holdem_hand.BestPokerHand();

              strcat(buf,space);
              strcat(buf,plain_hand_types[poker_hand.GetHandType()]);
              strcat(buf,space);
              strcat(buf,poker_hand.GetHand());
              strcat(buf,space);
              strcat(buf,player_name);
              strcat(buf,linefeed);
            }
          }
        }
      }
    }

    fclose(fptr);

    if (bDebug) {
      closed_count++;
      printf("opened_count = %d\n",opened_count);
      printf("closed_count = %d\n",closed_count);
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

static char *get_bracketed_string(char *line,int line_len)
{
  int m;
  int n;

  for (n = 0; n < line_len; n++) {
    if (line[n] == '[')
      break;
  }

  if (n == line_len)
    return NULL;

  for (m = n + 1; m < line_len; m++) {
    if (line[m] == ']')
      break;
  }

  if (m == line_len)
    return NULL;

  line[m+1] = 0;

  return &line[n];
}

void get_player_name(char *line,int line_len)
{
  int m;
  int n;

  for (n = 0; n < line_len; n++) {
    if (line[n] == ':')
      break;
  }

  if (n == line_len) {
    player_name[0] = 0;
    return;
  }

  n += 2;

  for (m = n; m < line_len; m++) {
    if (line[m] == ' ')
      break;

    if (m - n < MAX_PLAYER_NAME_LEN)
      player_name[m - n] = line[m];
    else
      break;
  }

  if (n == line_len) {
    player_name[0] = 0;
    return;
  }

  player_name[m - n] = 0;
}
