#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: compare_poker_hand (-card_ixs) filename";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d, card %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bCardIxs;
  int retval;
  FILE *fptr;
  int line_no;
  int mod_line_no;
  int line_len;
  hand work_hand;
  PokerHand hands[2];
  int ret_compare;
  char card_string[3];

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  card_string[2] = 0;

  bCardIxs = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-card_ixs"))
      bCardIxs = true;
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
  mod_line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if (bCardIxs) {
      sscanf(line,"%d %d %d %d %d",
        &work_hand.cards[0],&work_hand.cards[1],&work_hand.cards[2],&work_hand.cards[3],&work_hand.cards[4]);
    }
    else {
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
    }

    hands[mod_line_no].NewCards(work_hand.cards[0],work_hand.cards[1],work_hand.cards[2],work_hand.cards[3],work_hand.cards[4]);

    if (!mod_line_no) {
      if (!bCardIxs)
        printf("%s ",line);
      else {
        for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
          card_string_from_card_value(work_hand.cards[n],card_string);
          printf("%s ",card_string);
        }
      }

      printf("- ");
    }
    else {
      if (!bCardIxs)
        printf("%s ",line);
      else {
        for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
          card_string_from_card_value(work_hand.cards[n],card_string);
          printf("%s ",card_string);
        }
      }

      ret_compare = hands[0].Compare(hands[1],0);

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
