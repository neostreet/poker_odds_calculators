#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static char usage[] =
"usage: card_indexes0 (-sort) card_string (card_string ...)\n";
static char couldnt_open[] = "couldn't open %s\n";

int compare(const void *elem1,const void *elem2);

#define MAX_CARDS NUM_CARDS_IN_DECK

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bSort;
  int num_cards;
  int retval;
  int cards[MAX_CARDS];

  if (argc < 2) {
    cout << usage << endl;
    return 1;
  }

  bSort = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-sort"))
      bSort = true;
    else
      break;
  }

  if (argc - curr_arg < 1) {
    cout << usage << endl;
    return 2;
  }

  if (argc - curr_arg > MAX_CARDS) {
    printf("only %d card strings allowed\n",MAX_CARDS);
    return 3;
  }

  num_cards = 0;

  for ( ; curr_arg < argc; curr_arg++) {
    retval = card_value_from_card_string(argv[curr_arg],&cards[num_cards]);

    if (retval) {
      printf("invalid card string: %s (%d)\n",argv[curr_arg],retval);
      return 4;
    }

    num_cards++;
  }

  if (bSort)
    qsort(cards,num_cards,sizeof (int),compare);

  for (n = 0; n < num_cards; n++) {
    printf("%d",cards[n]);

    if (n < num_cards - 1)
      putchar(' ');
    else
      putchar(0x0a);
  }

  return 0;
}

int compare(const void *elem1,const void *elem2)
{
  int int1;
  int int2;

  int1 = *(int *)elem1;
  int2 = *(int *)elem2;

  return int1 - int2;
}
