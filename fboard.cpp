#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
using namespace std;

static char save_dir[_MAX_PATH];

#define MAX_FILENAME_LEN 1024
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: fboard (-debug) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char board[] = "Board [";
#define BOARD_LEN (sizeof (board) - 1)

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static bool Contains(int bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  int ix;
  int file_no;
  int dbg_file_no;
  int dbg;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bDebug = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug")) {
      bDebug = true;
      getcwd(save_dir,_MAX_PATH);
    }
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

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if (Contains(true,
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

        if (!bDebug)
          printf("%s\n",&line[ix]);
        else
          printf("%s %s\\%s\n",&line[ix],save_dir,filename);

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
