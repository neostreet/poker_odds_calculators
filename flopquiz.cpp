#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] = "usage: flopquiz seed\n";
static char correct[] = "correct\n";
static char incorrect[] = "incorrect\n";

static Flop myflop;

void new_flop();

int main(int argc,char **argv)
{
  int n;
  int seed;
  int num_quizzes;
  int num_dry_flops;
  int num_wet_flops;
  FlopType flop_type;
  int correct_answers;
  int incorrect_answers;
  double dwork;
  char ans[10];

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&seed);
  srand(seed);

  num_quizzes = 0;
  num_dry_flops = 0;
  num_wet_flops = 0;
  correct_answers = 0;
  incorrect_answers = 0;

  for ( ; ; ) {
    new_flop();

    for ( ; ; ) {
      cout << myflop << endl;
      cout << "d(ry), w(et), or q(uit)? ";

      cin >> ans;

      if ((ans[0] == 'w') || (ans[0] == 'd') || (ans[0] == 'q'))
        break;
    }

    if (ans[0] == 'q')
      break;

    flop_type = myflop.GetFlopType();

    if (flop_type == DRY) {
      num_dry_flops++;

      if (ans[0] == 'd') {
        correct_answers++;
        printf(correct);
      }
      else {
        incorrect_answers++;
        printf(incorrect);
      }
    }
    else {
      num_wet_flops++;

      if (ans[0] == 'w') {
        correct_answers++;
        printf(correct);
      }
      else {
        incorrect_answers++;
        printf(incorrect);
      }
    }

    num_quizzes++;
  }

  printf("seed = %d\n",seed);
  printf("num_quizzes       = %6d\n",num_quizzes);
  printf("num_dry_flops     = %6d\n",num_dry_flops);
  printf("num_wet_flops     = %6d\n",num_wet_flops);
  printf("correct_answers   = %6d\n",correct_answers);
  printf("incorrect_answers = %6d\n",incorrect_answers);

  if (num_quizzes) {
    dwork = (double)correct_answers / (double)num_quizzes * (double)100;
    printf("percent_correct   = %6.2lf\n",dwork);
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

void new_flop()
{
  int m;
  int n;
  int work;
  int cards[NUM_CARDS_IN_FLOP];

  for (n = 0; n < NUM_CARDS_IN_FLOP; n++) {
    for ( ; ; ) {
      work = rand() % NUM_CARDS_IN_DECK;

      for (m = 0; m < n; m++) {
        if (cards[m] == work)
          break;
      }

      if (m == n)
        break;
    }

    cards[n] = work;
  }

  myflop.NewCards(cards[0],cards[1],cards[2]);
}
