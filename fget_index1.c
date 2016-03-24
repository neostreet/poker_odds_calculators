#include <stdio.h>

#define NUM_CARDS 52

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: get_index1 file\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int get_index(int card1,int card2);

int main(int argc,char **argv)
{
  FILE *fptr;
  int line_len;
  int line_no;
  int cards[2];
  int ix;

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

    sscanf(line,"%d %d",&cards[0],&cards[1]);

    ix = get_index(cards[0],cards[1]);

    printf("%d\n",ix);
  }

  fclose(fptr);

  return 0;
}

int get_index(int card1,int card2)
{
  int m;
  int n;
  int p;

  p = 0;

  for (m = 0; m < NUM_CARDS - 1; m++) {
    for (n = m + 1; n < NUM_CARDS; n++) {
      if ((m == card1) && (n == card2))
        return p;

      p++;
    }
  }

  return -1;
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
