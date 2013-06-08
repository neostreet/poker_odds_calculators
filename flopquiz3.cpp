#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] = "usage: flopquiz3 filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static Flop myflop;

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int get_flop(char *line,int line_len);

int main(int argc,char **argv)
{
  int n;
  FILE *fptr;
  int line_len;
  int line_no;
  int iterations;
  int num_quizzes;
  int num_dry_flops;
  int num_wet_flops;
  int retval;
  FlopType flop_type;
  double dwork;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  num_quizzes = 0;
  num_dry_flops = 0;
  num_wet_flops = 0;
  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    retval = get_flop(line,line_len);

    if (retval) {
      printf("get_flop() failed on line %d: %d\n",line_no,retval);
      return 3;
    }

    flop_type = myflop.GetFlopType();

    if (flop_type == DRY)
      num_dry_flops++;
    else
      num_wet_flops++;

    num_quizzes++;
  }

  dwork = (double)num_wet_flops / (double)num_quizzes * (double)100;

  printf("num_quizzes   = %5d\n",num_quizzes);
  printf("num_dry_flops = %5d\n",num_dry_flops);
  printf("num_wet_flops = %5d\n",num_wet_flops);
  printf("wet_pct       = %5.2lf\n",dwork);

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

int get_flop(char *line,int line_len)
{
  int n;
  int ix;
  int rank;
  int suit;
  int cards[NUM_CARDS_IN_FLOP];

  if (line_len != 8)
    return 1;

  for (n = 0; n < NUM_CARDS_IN_FLOP; n++) {
    ix = n * 3;
    // get the rank index

    for (rank = 0; rank < NUM_CARDS_IN_SUIT; rank++) {
      if (line[ix] == rank_chars[rank])
        break;
    }

    if (rank == NUM_CARDS_IN_SUIT)
      return 2;

    // get the suit index

    for (suit = 0; suit < NUM_SUITS; suit++) {
      if (line[ix+1] == suit_chars[suit])
        break;
    }

    if (suit == NUM_SUITS)
      return 3;

    cards[n] = suit * NUM_CARDS_IN_SUIT + rank;
  }

  myflop.NewCards(cards[0],cards[1],cards[2]);

  return 0;
}
