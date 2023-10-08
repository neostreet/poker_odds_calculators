#include <stdio.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: agg_card_ixs filename\n";
static char couldnt_open[] = "couldn't open %s\n";

#define NUM_CARDS_IN_DECK 52
static int card_counts[NUM_CARDS_IN_DECK];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  FILE *fptr;
  int line_len;
  int line_no;
  int card_ixs[5];

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(line,"%d %d %d %d %d",&card_ixs[0],&card_ixs[1],&card_ixs[2],&card_ixs[3],&card_ixs[4]);

    for (n = 0; n < 5; n++) {
      if (card_ixs[n] >= NUM_CARDS_IN_DECK) {
        printf("invalid card_ix on line %d: %s\n",line_no,line);
        return 3;
      }

      card_counts[card_ixs[n]]++;
    }
  }

  fclose(fptr);

  for (n = 0; n < NUM_CARDS_IN_DECK; n++) {
    if (card_counts[n])
      printf("%2d: %4d\n",n,card_counts[n]);
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
