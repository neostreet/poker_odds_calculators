#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#ifdef FREEBSD
#define O_BINARY 0
#endif
#endif
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: reverse_hands_and_types infile outfile\n"
"  offset count\n";
static char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int retval;
  struct hand_and_type *hands_and_types;
  int fhndl;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  if (!strcmp(argv[1],argv[2])) {
    printf("infile and outfile must differ\n");
    return 2;
  }

  retval = read_hands_and_types(argv[1],&hands_and_types);

  if (retval) {
    printf("read_hands_and_types() failed: %d\n",retval);
    return 3;
  }

  for (n = 0; n < POKER_52_5_PERMUTATIONS; n++)
    hands_and_types[n].quick_ix = 7461 - hands_and_types[n].quick_ix;

  if ((fhndl = open(argv[2],
    O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
    S_IREAD | S_IWRITE)) == -1) {

    printf(couldnt_open,argv[2]);
    return 4;
  }

  write(fhndl,hands_and_types,sizeof (struct hand_and_type) * POKER_52_5_PERMUTATIONS);

  close(fhndl);

  return 0;
}
