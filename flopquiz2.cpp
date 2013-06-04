#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] = "usage: flopquiz2 (-debug) (-verbose) seed iterations\n";

static Flop myflop;

void new_flop();

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  int seed;
  int iterations;
  int num_quizzes;
  int num_dry_flops;
  int num_wet_flops;
  FlopType flop_type;

  if ((argc < 3) || (argc > 5)) {
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

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if (!bVerbose)
    myflop.Plain();

  sscanf(argv[curr_arg],"%d",&seed);
  srand(seed);

  sscanf(argv[curr_arg+1],"%d",&iterations);

  num_quizzes = 0;
  num_dry_flops = 0;
  num_wet_flops = 0;

  for (n = 0; n < iterations; n++) {
    new_flop();

    if (bDebug)
      cout << myflop;

    flop_type = myflop.GetFlopType();

    if (flop_type == DRY) {
      num_dry_flops++;

      if (bDebug) {
        if (bVerbose)
          cout << " dry" << endl;
        else
          cout << " 0" << endl;
      }
    }
    else {
      num_wet_flops++;

      if (bDebug) {
        if (bVerbose)
          cout << " wet" << endl;
        else
          cout << " 1" << endl;
      }
    }

    num_quizzes++;
  }

  printf("seed = %d\n",seed);
  printf("num_quizzes   = %3d\n",num_quizzes);
  printf("num_dry_flops = %3d\n",num_dry_flops);
  printf("num_wet_flops = %3d\n",num_wet_flops);

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
