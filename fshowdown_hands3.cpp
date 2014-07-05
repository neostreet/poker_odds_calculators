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
static char buf[MAX_LINE_LEN];
static char buf2[MAX_LINE_LEN];

static char usage[] =
"usage: fshowdown_hands3 (-verbose) (-debug) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char summary[] = "*** SUMMARY ***";
#define SUMMARY_LEN (sizeof (summary) - 1)

static char indent[] = "  ";
static char linefeed[] = "\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);
static char *get_bracketed_string(char *line,int line_len);

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  int curr_arg;
  bool bVerbose;
  bool bDebug;
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
  int dbg;
  char *cpt;
  int showdown_hands;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bDebug = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
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

  file_no = 0;
  dbg_file_no = -1;

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
    num_hands = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if (line_no == dbg_line_no)
        dbg = 1;

      if (bDebug)
        printf("line %d %s\n",line_no,line);

      if (!strncmp(line,summary,SUMMARY_LEN)) {
        num_hands++;
        showdown_hands = 0;
        buf[0] = 0;

        for ( ; ; ) {
          GetLine(fptr,line,&line_len,MAX_LINE_LEN);

          if (feof(fptr))
            break;

          line_no++;

          if (line_no == dbg_line_no)
            dbg = 1;

          if ((line_len == 1) && (line[0] == '\r')) {
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
            }
          }
          else if (!strncmp(line,"Seat ",5)) {
            cpt = get_bracketed_string(line,line_len);

            if (cpt != NULL) {
              strcat(buf,indent);
              strcat(buf,cpt);
              strcat(buf,linefeed);

              showdown_hands++;
            }
          }
        }
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
