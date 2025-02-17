#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: print_hand hand filename\n";

static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  FILE *fptr;
  int line_len;
  int hands;
  char hole_cards[6];
  char hole_cards_abbrev[4];

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[2],"r")) == NULL) {
    printf(couldnt_open,argv[2]);
    return 2;
  }

  if ((argv[1][0] >= 'a') && (argv[1][0] <= 'z'))
    argv[1][0] -= 'a' - 'A';

  hands = 0;

  hole_cards[2] = ' ';
  hole_cards[5] = 0;
  hole_cards_abbrev[3] = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    hands++;

    if ((line[2] != ' ') || (line[5] && (line[5] != ' ') && (line[5] != ','))) {
      printf("invalid hole card delimiters in line %d\n",hands);
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

    if ((strlen(argv[1]) == 3) && (argv[1][2] == 'x')) {
      // disregard whether or not the hand is suited

      if (!strncmp(hole_cards_abbrev,argv[1],2))
        printf("%s hand %d\n",line,hands);
    }
    else {
      if (!strcmp(hole_cards_abbrev,argv[1]))
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
