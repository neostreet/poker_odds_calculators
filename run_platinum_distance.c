#include <stdio.h>

static char usage[] = "usage: run_platinum_distance filename\n";
static char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  FILE *fptr;
  int nobs;
  int wins;
  int losses;
  int work;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  nobs = 0;
  wins = 0;
  losses = 0;

  for ( ; ; ) {
    fscanf(fptr,"%d",&work);

    if (feof(fptr))
      break;

    nobs++;

    if (work > 0)
      wins++;
    else if (work < 0)
      losses++;

    work = losses * 3 - wins;

    printf("%3d\n",work);
  }

  fclose(fptr);

  return 0;
}
