#include <iostream>
#include <stdio.h>
using namespace std;

#include "poker_hand.h"

int main(int argc,char **argv)
{
  int n;
  int index;
  int num_other_cards;

  printf("static int offsets[][NUM_CARDS_IN_DECK] = {\n");
  printf("  {");

  index = 0;
  num_other_cards = NUM_CARDS_IN_DECK - 1;

  for (n = 0; n < NUM_CARDS_IN_DECK; n++) {
    printf("%d",index);

    if (n < NUM_CARDS_IN_DECK - 1)
      putchar(',');

    index += num_other_cards;
    num_other_cards--;
  }

  printf("},\n");

  printf("  {0,");

  for (n = 0; n < NUM_CARDS_IN_DECK - 1; n++) {
    printf("%d",n);

    if (n < NUM_CARDS_IN_DECK - 2)
      putchar(',');
  }

  printf("}\n");

  printf("};\n");

  return 0;
}
