#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

static void get_permutation_instance(
  int set_size,int subset_size,
  int *m,int *n,int *o,int *p,int *q,int *r,int *s,
  int instance_ix
);

int main(int argc,char **argv)
{
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  int s;
  int t;
  HoldemPokerHand holdem_hand;
  PokerHand hand;
  int hand_counts[NUM_HAND_TYPES];
  double pct;
  time_t start_time;
  time_t end_time;

  time(&start_time);

  for (n = 0; n < NUM_HAND_TYPES; n++)
    hand_counts[n] = 0;

  for (t = 0; t < POKER_52_7_PERMUTATIONS; t++) {
    get_permutation_instance(
      NUM_CARDS_IN_DECK,NUM_CARDS_IN_HOLDEM_POOL,
      &m,&n,&o,&p,&q,&r,&s,t);

    holdem_hand.NewCards(m,n,o,p,q,r,s);
    hand = holdem_hand.BestPokerHand();
    hand_counts[hand.GetHandType()]++;
  }

  time(&end_time);

  for (n = NUM_HAND_TYPES - 1; (n >= 0); n--) {
    pct = (double)hand_counts[n] / (double)POKER_52_7_PERMUTATIONS;
    printf("%s %9d %9.6lf\n",hand_type_abbrevs[n],hand_counts[n],pct);
  }

  printf("============\n");
  printf("   %9d\n",POKER_52_7_PERMUTATIONS);

  printf("\ncomputation time: %d seconds\n",end_time - start_time);

  return 0;
}

static void get_permutation_instance(
  int set_size,int subset_size,
  int *m,int *n,int *o,int *p,int *q,int *r,int *s,
  int instance_ix
)
{
  if (instance_ix)
    goto after_return_point;

  for (*m = 0; *m < set_size - subset_size + 1; (*m)++) {
    for (*n = *m + 1; *n < set_size - subset_size + 2; (*n)++) {
      for (*o = *n + 1; *o < set_size - subset_size + 3; (*o)++) {
        for (*p = *o + 1; *p < set_size - subset_size + 4; (*p)++) {
          for (*q = *p + 1; *q < set_size - subset_size + 5; (*q)++) {
            for (*r = *q + 1; *r < set_size - subset_size + 6; (*r)++) {
              for (*s = *r + 1; *s < set_size - subset_size + 7; (*s)++) {
                return;

                after_return_point:
                ;
              }
            }
          }
        }
      }
    }
  }
}
