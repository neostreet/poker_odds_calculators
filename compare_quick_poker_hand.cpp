#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: compare_poker_hand filename hands_and_types_filename";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int m;
  int n;
  int retval;
  FILE *fptr;
  int line_no;
  int mod_line_no;
  int line_len;
  struct hand_and_type *hands_and_types;
  hand work_hand;
  PokerHand hands[2];
  int ret_compare;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  retval = read_hands_and_types(argv[2],&hands_and_types);

  if (retval) {
    printf("read_hands_and_types() failed: %d\n",retval);
    return 3;
  }

  line_no = 0;
  mod_line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    m = 0;

    // skip whitespace

    for ( ; m < line_len; m++) {
      if (line[m] != ' ')
        break;
    }

    if (m == line_len) {
      printf(parse_error,line_no,-1,4);
      return 4;
    }

    for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
      retval = card_value_from_card_string(&line[m],&work_hand.cards[n]);

      if (retval) {
        printf(parse_error,line_no,n,5);
        return 5;
      }

      m += 2;

      if (n < NUM_CARDS_IN_HAND - 1) {
        // skip whitespace

        for ( ; m < line_len; m++) {
          if (line[m] != ' ')
            break;
        }

        if (m == line_len) {
          printf(parse_error,line_no,n,6);
          return 6;
        }
      }
    }

    hands[mod_line_no].NewCards(work_hand.cards[0],work_hand.cards[1],work_hand.cards[2],work_hand.cards[3],work_hand.cards[4]);

    if (!mod_line_no)
      printf("%s - ",line);
    else {
      printf("%s ",line);

      ret_compare = hands[0].CompareQuick(hands[1],hands_and_types);

      switch(ret_compare) {
        case -1:
          printf("less than\n");
          break;
        case 0:
          printf("equal\n");
          break;
        case 1:
          printf("greater than\n");
          break;
      }
    }

    line_no++;
    mod_line_no = line_no % 2;
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
