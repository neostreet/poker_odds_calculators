#include <stdio.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: agg_hand_type_ixs filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  FILE *fptr;
  int line_len;
  int line_no;
  int hand_type_ix;
  int prev_hand_type_ix;
  int hand_count;
  int total;
  int grand_total;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  line_no = 0;
  grand_total = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(line,"%d %d",&hand_type_ix,&hand_count);

    if (line_no == 1) {
      total = hand_count;
    }
    else if (hand_type_ix == prev_hand_type_ix)
      total += hand_count;
    else {
      printf("%d %7d\n",prev_hand_type_ix,total);
      grand_total += total;
      total = hand_count;
    }

    prev_hand_type_ix = hand_type_ix;
  }

   printf("%d %7d\n",prev_hand_type_ix,total);
   grand_total += total;
   printf("\n  %7d\n",grand_total);

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
