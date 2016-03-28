#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: index_of_hand5 (-debug) (-verbose) filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int elem_compare(const void *elem1,const void *elem2);
static int index_of_hand(int *cards);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  int dbg_ix;
  int dbg;
  int m;
  int n;
  int retval;
  int cards[NUM_CARDS_IN_HAND];
  int work;
  FILE *fptr;
  int line_no;
  int line_len;
  int ix;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
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

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;
    m = 17;

    for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
      retval = card_value_from_card_string(&line[m],&cards[n]);

      if (retval) {
        printf(parse_error,line_no,n,5);
        return 4;
      }

      m += 3;
    }

    qsort(cards,NUM_CARDS_IN_HAND,sizeof (int),elem_compare);

    ix = index_of_hand(cards);

    if (ix == dbg_ix)
      dbg = 1;

    if (ix >= POKER_52_5_PERMUTATIONS) {
      printf("error on line %d: ix = %d\n",line_no,ix);
      ix = 0;
    }

    printf("%7d %s\n",ix,line);
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

int elem_compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  return ix1 - ix2;
}

static int index_of_hand(int *cards)
{
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;

  r = 0;

  for (m = 0; m < NUM_CARDS_IN_DECK - 4; m++) {
    for (n = m + 1; n < NUM_CARDS_IN_DECK - 3; n++) {
      for (o = n + 1; o < NUM_CARDS_IN_DECK - 2; o++) {
        for (p = o + 1; p < NUM_CARDS_IN_DECK - 1; p++) {
          for (q = p + 1; q < NUM_CARDS_IN_DECK; q++) {
            if ((m == cards[0]) && (n == cards[1]) && (o == cards[2]) &&
                (p == cards[3]) && (q == cards[4])) {
              return r;
            }

            r++;
          }
        }
      }
    }
  }
}
