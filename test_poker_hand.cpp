#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define FALSE 0
#define TRUE  1

#define MAIN_MODULE
#include "poker_hand.h"

using namespace std;

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: test_poker_hand (-debug) filename";
static char couldnt_open[] = "couldn't open %s\n";
static char parse_error[] = "couldn't parse line %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  int retval;
  int curr_arg;
  int bDebug;
  FILE *fptr;
  int line_len;
  int cards[NUM_CARDS_IN_HAND];
  int hand_type;
  HandType ret_hand_type;
  PokerHand hand;
  int num_tests;
  int num_tests_passed;

  if ((argc != 2) && (argc != 3)) {
    cout << usage << endl;
    return 1;
  }

  bDebug = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = TRUE;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    cout << usage << endl;
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  num_tests = 0;
  num_tests_passed = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    num_tests++;

    m = 0;

    // skip whitespace

    for ( ; m < line_len; m++) {
      if (line[m] != ' ')
        break;
    }

    if (m == line_len) {
      printf(parse_error,num_tests,4);
      return 4;
    }

    for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
      retval = card_value_from_card_string(&line[m],&cards[n]);

      if (retval) {
        printf(parse_error,num_tests,5);
        return 5;
      }

      m += 2;

      // skip whitespace

      for ( ; m < line_len; m++) {
        if (line[m] != ' ')
          break;
      }

      if (m == line_len) {
        printf(parse_error,num_tests,6);
        return 6;
      }
    }

    for (hand_type = 0; hand_type < NUM_HAND_TYPES; hand_type++) {
      for (p = 0; p < 2; p++) {
        if (line[m+p] != hand_type_abbrevs[hand_type][p])
          break;
      }

      if (p == 2)
        break;
    }

    if (hand_type == NUM_HAND_TYPES) {
      printf(parse_error,num_tests,7);
      return 7;
    }

    hand.NewCards(cards[0],cards[1],cards[2],cards[3],cards[4]);

    ret_hand_type = hand.Evaluate();

    if (bDebug)
      cout << hand << endl;

    if (ret_hand_type == (HandType)hand_type)
      num_tests_passed++;
  }

  fclose(fptr);

  printf("num_tests:        %5d\n",num_tests);
  printf("num_tests_passed: %5d\n",num_tests_passed);

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
