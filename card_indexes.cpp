#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: card_indexes filename num_cards\n";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

#define MAX_CARDS NUM_CARDS_IN_DECK

int main(int argc,char **argv)
{
  int m;
  int n;
  FILE *fptr;
  int line_no;
  int line_len;
  int num_cards;
  int retval;
  int cards[MAX_CARDS];

  if (argc != 3) {
    cout << usage << endl;
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  sscanf(argv[2],"%d",&num_cards);

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    m = 0;

    // skip whitespace

    for ( ; m < line_len; m++) {
      if (line[m] != ' ')
        break;
    }

    if (m == line_len) {
      printf(parse_error,line_no,-1,1);
      return 3;
    }

    for (n = 0; n < num_cards; n++) {
      retval = card_value_from_card_string(&line[m],&cards[n]);

      if (retval) {
        printf(parse_error,line_no,n,2);
        return 4;
      }

      m += 2;

      if (n < num_cards - 1) {
        // skip whitespace

        for ( ; m < line_len; m++) {
          if (line[m] != ' ')
            break;
        }

        if (m == line_len) {
          printf(parse_error,line_no,n,3);
          return 5;
        }
      }
    }
  }

  fclose(fptr);

  for (n = 0; n < num_cards; n++) {
    printf("%d",cards[n]);

    if (n < num_cards - 1)
      putchar(' ');
    else
      putchar(0x0a);
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
