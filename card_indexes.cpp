#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: card_indexes (-sort) filename num_cards\n";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int compare(const void *elem1,const void *elem2);

#define MAX_CARDS NUM_CARDS_IN_DECK

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bSort;
  FILE *fptr;
  int line_no;
  int line_len;
  int num_cards;
  int retval;
  int cards[MAX_CARDS];

  if ((argc < 3) || (argc > 4)) {
    cout << usage << endl;
    return 1;
  }

  bSort = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-sort"))
      bSort = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    cout << usage << endl;
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  sscanf(argv[curr_arg+1],"%d",&num_cards);

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
      return 4;
    }

    for (n = 0; n < num_cards; n++) {
      retval = card_value_from_card_string(&line[m],&cards[n]);

      if (retval) {
        printf(parse_error,line_no,n,2);
        return 5;
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
          return 6;
        }
      }
    }
  }

  fclose(fptr);

  if (bSort)
    qsort(cards,num_cards,sizeof (int),compare);

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

int compare(const void *elem1,const void *elem2)
{
  int int1;
  int int2;

  int1 = *(int *)elem1;
  int2 = *(int *)elem2;

  return int1 - int2;
}
