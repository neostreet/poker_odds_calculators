#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "poker_hand.h"

static void get_permutation_instance(
  int *m,int *n,int *o,int *p,int *q,
  int instance_ix,int *ixs
);

// default constructor

PokerHand::PokerHand()
{
  _have_cards = 0;
  _hand_sorted = 0;
  _hand_evaluated = 0;
  _no_wheel = 0;
}

// copy constructor

PokerHand::PokerHand(const PokerHand& hand)
{
  int n;

  for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
    _card[n] = hand._card[n];
    _suit[n] = hand._suit[n];
    _rank[n] = hand._rank[n];
    _sort_val[n] = hand._sort_val[n];
    _order[n] = hand._order[n];
  }

  _have_cards = hand._have_cards;
  _hand_sorted = hand._hand_sorted;
  _hand_evaluated = hand._hand_evaluated;
  _no_wheel = hand._no_wheel;

  _hand_type = hand._hand_type;
}

// assignment operator

PokerHand& PokerHand::operator=(const PokerHand& hand)
{
  int n;

  for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
    _card[n] = hand._card[n];
    _suit[n] = hand._suit[n];
    _rank[n] = hand._rank[n];
    _sort_val[n] = hand._sort_val[n];
    _order[n] = hand._order[n];
  }

  _have_cards = hand._have_cards;
  _hand_sorted = hand._hand_sorted;
  _hand_evaluated = hand._hand_evaluated;
  _no_wheel = hand._no_wheel;

  _hand_type = hand._hand_type;

  return *this;
}

// destructor

PokerHand::~PokerHand()
{
}

PokerHand::PokerHand(int card1,int card2,int card3,int card4, int card5)
{
  int n;

  _card[0] = card1 % NUM_CARDS_IN_DECK;
  _card[1] = card2 % NUM_CARDS_IN_DECK;
  _card[2] = card3 % NUM_CARDS_IN_DECK;
  _card[3] = card4 % NUM_CARDS_IN_DECK;
  _card[4] = card5 % NUM_CARDS_IN_DECK;

  for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
    _suit[n] = suit_of(_card[n]);
    _rank[n] = rank_of(_card[n]);
  }

  _have_cards = 1;
  _hand_sorted = 0;
  _hand_evaluated = 0;
  _no_wheel = 0;
}

void PokerHand::NewCards(int card1,int card2,int card3,int card4, int card5)
{
  int n;

  _card[0] = card1 % NUM_CARDS_IN_DECK;
  _card[1] = card2 % NUM_CARDS_IN_DECK;
  _card[2] = card3 % NUM_CARDS_IN_DECK;
  _card[3] = card4 % NUM_CARDS_IN_DECK;
  _card[4] = card5 % NUM_CARDS_IN_DECK;

  for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
    _suit[n] = suit_of(_card[n]);
    _rank[n] = rank_of(_card[n]);
  }

  _have_cards = 1;
  _hand_sorted = 0;
  _hand_evaluated = 0;
}

int PokerHand::GetRank(int card)
{
  if (!_have_cards)
    return -1;

  if (!_hand_sorted) {
    Sort();
    _hand_evaluated = 0;
  }

  return _rank[_order[card]];
}

void PokerHand::Sort()
{
  int m;
  int n;
  int num_cards_with_same_rank;
  int temp;

  if (!_have_cards)
    return;

  if (_hand_sorted)
    return;

  for (m = 0; m < NUM_CARDS_IN_HAND; m++) {
    num_cards_with_same_rank = 0;

    for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
      if (n == m)
        continue;

      if (_rank[m] == _rank[n])
        num_cards_with_same_rank++;
    }

    _sort_val[m] = _rank[m] + num_cards_with_same_rank * NUM_CARDS_IN_SUIT;
  }

  for (n = 0; n < NUM_CARDS_IN_HAND; n++)
    _order[n] = n;

  for (m = 0; m < NUM_CARDS_IN_HAND - 1; m++) {
    for (n = m + 1; n < NUM_CARDS_IN_HAND; n++) {
      if (_sort_val[_order[m]] < _sort_val[_order[n]]) {
        temp = _order[m];
        _order[m] = _order[n];
        _order[n] = temp;
      }
    }
  }

  _hand_sorted = 1;
}

HandType PokerHand::Evaluate()
{
  num_evaluations++;

  if (!_have_cards)
    return HIGH_CARD;

  if (!_hand_sorted) {
    Sort();

    if (!_hand_sorted)
      return HIGH_CARD;

    _hand_evaluated = 0;
  }

  if (_hand_evaluated)
    return _hand_type;

  num_unique_evaluations++;

  if (RoyalFlush())
    _hand_type = ROYAL_FLUSH;
  else if (StraightFlush())
    _hand_type = STRAIGHT_FLUSH;
  else if (FourOfAKind())
    _hand_type = FOUR_OF_A_KIND;
  else if (FullHouse())
    _hand_type = FULL_HOUSE;
  else if (Flush())
    _hand_type = FLUSH;
  else if (Straight())
    _hand_type = STRAIGHT;
  else if (ThreeOfAKind())
    _hand_type = THREE_OF_A_KIND;
  else if (TwoPairs())
    _hand_type = TWO_PAIRS;
  else if (OnePair())
    _hand_type = ONE_PAIR;
  else
    _hand_type = HIGH_CARD;

  _hand_evaluated = 1;

  return _hand_type;
}

int PokerHand::RoyalFlush()
{
  if (!StraightFlush())
    return 0;

  if ((_rank[_order[0]] != ACE) || (_rank[_order[1]] != KING))
    return 0;

  return 1;
}

int PokerHand::StraightFlush()
{
  if (!Straight())
    return 0;

  if (!Flush())
    return 0;

  return 1;
}

int PokerHand::FourOfAKind()
{
  int n;

  for (n = 1; n < 4; n++) {
    if (_rank[_order[n]] != _rank[_order[0]])
      break;
  }

  if (n < 4)
    return 0;

  return 1;
}

int PokerHand::FullHouse()
{
  int n;

  for (n = 1; n < 3; n++) {
    if (_rank[_order[n]] != _rank[_order[0]])
      break;
  }

  if (n < 3)
    return 0;

  if (_rank[_order[3]] != _rank[_order[4]])
    return 0;

  return 1;
}

int PokerHand::Flush()
{
  int n;

  for (n = 1; n < NUM_CARDS_IN_HAND; n++) {
    if (_suit[_order[n]] != _suit[_order[0]])
      break;
  }

  if (n < NUM_CARDS_IN_HAND)
    return 0;

  return 1;
}

int PokerHand::Straight()
{
  int n;

  if (!_no_wheel) {
    // first, handle the special case of a wheel (A, 2, 3, 4, 5)

    if ((_rank[_order[0]] == ACE) &&
        (_rank[_order[1]] == FIVE) &&
        (_rank[_order[2]] == FOUR) &&
        (_rank[_order[3]] == THREE) &&
        (_rank[_order[4]] == TWO))
      return 1;
  }

  for (n = 1; n < NUM_CARDS_IN_HAND; n++) {
    if (_rank[_order[n-1]] != _rank[_order[n]] + 1)
      break;
  }

  if (n < NUM_CARDS_IN_HAND)
    return 0;

  return 1;
}

int PokerHand::ThreeOfAKind()
{
  int n;

  for (n = 1; n < 3; n++) {
    if (_rank[_order[n]] != _rank[_order[0]])
      break;
  }

  if (n < 3)
    return 0;

  return 1;
}

int PokerHand::TwoPairs()
{
  if ((_rank[_order[0]] == _rank[_order[1]]) &&
      (_rank[_order[2]] == _rank[_order[3]]))
    return 1;

  return 0;
}

int PokerHand::OnePair()
{
  if (_rank[_order[0]] == _rank[_order[1]])
    return 1;

  return 0;
}

void PokerHand::SetNoWheel(int no_wheel)
{
  _no_wheel = no_wheel;
}

int PokerHand::GetNoWheel()
{
  return _no_wheel;
}

HandType PokerHand::GetHandType()
{
  if (!_hand_evaluated)
    Evaluate();

  return _hand_type;
}

int PokerHand::Compare(PokerHand& compare_hand)
{
  int n;
  HandType hand_type;
  HandType compare_hand_type;
  int rank;
  int compare_rank;

  num_comparisons++;

  hand_type = Evaluate();
  compare_hand_type = compare_hand.Evaluate();

  if (hand_type > compare_hand_type)
    return 1;

  if (hand_type < compare_hand_type)
    return -1;

  switch(hand_type) {
    case HIGH_CARD:
    case FLUSH:
      for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
        rank = GetRank(n);
        compare_rank = compare_hand.GetRank(n);

        if (rank > compare_rank)
          return 1;

        if (rank < compare_rank)
          return -1;
      }

      return 0;
    case ONE_PAIR:
      for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
        if (n == 1)
          continue;

        rank = GetRank(n);
        compare_rank = compare_hand.GetRank(n);

        if (rank > compare_rank)
          return 1;

        if (rank < compare_rank)
          return -1;
      }

      return 0;
    case TWO_PAIRS:
      for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
        if ((n == 1) || (n == 3))
          continue;

        rank = GetRank(n);
        compare_rank = compare_hand.GetRank(n);

        if (rank > compare_rank)
          return 1;

        if (rank < compare_rank)
          return -1;
      }

      return 0;
    case THREE_OF_A_KIND:
      for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
        if ((n == 1) || (n == 2))
          continue;

        rank = GetRank(n);
        compare_rank = compare_hand.GetRank(n);

        if (rank > compare_rank)
          return 1;

        if (rank < compare_rank)
          return -1;
      }

      return 0;
    case STRAIGHT:
    case STRAIGHT_FLUSH:
    case ROYAL_FLUSH:
      rank = GetRank(1);
      compare_rank = compare_hand.GetRank(1);

      if (rank > compare_rank)
        return 1;

      if (rank < compare_rank)
        return -1;

      return 0;
    case FULL_HOUSE:
      for (n = 0; n < NUM_CARDS_IN_HAND - 1; n++) {
        if ((n == 1) || (n == 2))
          continue;

        rank = GetRank(n);
        compare_rank = compare_hand.GetRank(n);

        if (rank > compare_rank)
          return 1;

        if (rank < compare_rank)
          return -1;
      }

      return 0;
    case FOUR_OF_A_KIND:
      for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
        if ((n == 1) || (n == 2) || (n == 3))
          continue;

        rank = GetRank(n);
        compare_rank = compare_hand.GetRank(n);

        if (rank > compare_rank)
          return 1;

        if (rank < compare_rank)
          return -1;
      }

      return 0;
  }

  return 0;
}

static char *suit_strings[] = {
  "clubs",
  "diamonds",
  "hearts",
  "spades"
};

static char *rank_strings[] = {
  "two",
  "three",
  "four",
  "five",
  "six",
  "seven",
  "eight",
  "nine",
  "ten",
  "jack",
  "queen",
  "king",
  "ace"
};

#define MAX_VAR_INFO_VARS 2

struct hand_type_info {
  char *hand_type_fmt;
  int num_var_info_vars;
  int var_info_vars[MAX_VAR_INFO_VARS];
};

static struct hand_type_info hand_types[] = {
  "high card %s", 1, { 0, 0 },
  "a pair of %ss", 1, { 0, 0 },
  "two pair, %ss and %ss", 2, { 0, 2},
  "three of a kind, %ss", 1, { 0, 0 },
  "a straight, %s to %s", 2, { 4, 0 },
  "a flush, %s high", 1, { 0, 0 },
  "a full house, %ss full of %ss", 2, { 0, 3 },
  "four of a kind, %ss", 1, { 0, 0 },
  "a straight flush, %s to %s", 2, { 4, 0 },
  "a royal flush", 0, { 0, 0 }
};

#define PRINT_BUF_LEN 256
static char print_buf[PRINT_BUF_LEN];

void PokerHand::print(ostream& out) const
{
  int n;
  char card_string[3];

  if (!_have_cards)
    return;

  card_string[2] = 0;

  if (!_hand_evaluated) {
    for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
      card_string_from_card_value(_card[n],card_string);

      out << card_string;

      if (n < NUM_CARDS_IN_HAND - 1)
        out << " ";
    }

    return;
  }

  for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
    card_string_from_card_value(
      _card[_order[NUM_CARDS_IN_HAND - 1 - n]],
      card_string);

    out << card_string;

    if (n < NUM_CARDS_IN_HAND - 1)
      out << " ";
    else
      out << endl;
  }

  switch (hand_types[_hand_type].num_var_info_vars) {
    case 0:
      sprintf(print_buf,hand_types[_hand_type].hand_type_fmt);

      break;
    case 1:
      sprintf(print_buf,hand_types[_hand_type].hand_type_fmt,
        rank_strings[_rank[_order[hand_types[_hand_type].var_info_vars[0]]]]);

      break;
    case 2:
      // first, handle the special case of a wheel (A, 2, 3, 4, 5)
      if (((_hand_type == STRAIGHT) || (_hand_type == STRAIGHT_FLUSH) ||
           (_hand_type == ROYAL_FLUSH)) && !_no_wheel &&
           (_rank[_order[0]] == ACE) && (_rank[_order[1]] == FIVE))
        sprintf(print_buf,hand_types[_hand_type].hand_type_fmt,
          rank_strings[ACE],
          rank_strings[FIVE]);
      else
        sprintf(print_buf,hand_types[_hand_type].hand_type_fmt,
          rank_strings[_rank[_order[hand_types[_hand_type].var_info_vars[0]]]],
          rank_strings[_rank[_order[hand_types[_hand_type].var_info_vars[1]]]]);

      break;
  }

  out << print_buf;
}

ostream& operator<<(ostream& out,const PokerHand& hand)
{
  hand.print(out);

  return out;
}

// default constructor

BoardPokerHand::BoardPokerHand()
{
  _have_cards = 0;
  _no_wheel = 0;
}

// copy constructor

BoardPokerHand::BoardPokerHand(const BoardPokerHand& hand)
{
  int n;

  for (n = 0; n < NUM_CARDS_IN_POOL; n++)
    _card[n] = hand._card[n];

  _have_cards = hand._have_cards;
  _no_wheel = hand._no_wheel;
}

// assignment operator

BoardPokerHand& BoardPokerHand::operator=(const BoardPokerHand& hand)
{
  int n;

  for (n = 0; n < NUM_CARDS_IN_POOL; n++)
    _card[n] = hand._card[n];

  _have_cards = hand._have_cards;
  _no_wheel = hand._no_wheel;

  return *this;
}

// destructor

BoardPokerHand::~BoardPokerHand()
{
}

BoardPokerHand::BoardPokerHand(int card1,int card2,int card3,int card4,int card5,int card6,int card7)
{
  _card[0] = card1 % NUM_CARDS_IN_DECK;
  _card[1] = card2 % NUM_CARDS_IN_DECK;
  _card[2] = card3 % NUM_CARDS_IN_DECK;
  _card[3] = card4 % NUM_CARDS_IN_DECK;
  _card[4] = card5 % NUM_CARDS_IN_DECK;
  _card[5] = card6 % NUM_CARDS_IN_DECK;
  _card[6] = card7 % NUM_CARDS_IN_DECK;

  _have_cards = 1;
}

void BoardPokerHand::NewCards(int card1,int card2,int card3,int card4,int card5,int card6,int card7)
{
  _card[0] = card1 % NUM_CARDS_IN_DECK;
  _card[1] = card2 % NUM_CARDS_IN_DECK;
  _card[2] = card3 % NUM_CARDS_IN_DECK;
  _card[3] = card4 % NUM_CARDS_IN_DECK;
  _card[4] = card5 % NUM_CARDS_IN_DECK;
  _card[5] = card6 % NUM_CARDS_IN_DECK;
  _card[6] = card7 % NUM_CARDS_IN_DECK;

  _have_cards = 1;
}

void BoardPokerHand::SetNoWheel(int no_wheel)
{
  _no_wheel = no_wheel;
}

int BoardPokerHand::GetNoWheel()
{
  return _no_wheel;
}

PokerHand& BoardPokerHand::BestPokerHand()
{
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  int ixs[NUM_CARDS_IN_HAND];
  PokerHand hand;
  int ret_compare;

  hand.SetNoWheel(GetNoWheel());

  for (r = 0; r < POKER_7_5_PERMUTATIONS; r++) {
    get_permutation_instance(&m,&n,&o,&p,&q,r,&ixs[0]);

    hand.NewCards(_card[ixs[0]],_card[ixs[1]],_card[ixs[2]],
      _card[ixs[3]],_card[ixs[4]]);

    if (!r)
      _best_poker_hand = hand;
    else {
      ret_compare = hand.Compare(_best_poker_hand);

      if (ret_compare == 1)
        _best_poker_hand = hand;
    }
  }

  return _best_poker_hand;
}

PokerHand& BoardPokerHand::BestPokerHandWheel()
{
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  int ixs[NUM_CARDS_IN_HAND];
  PokerHand hand;
  int ret_compare;

  hand.SetNoWheel(0);

  for (r = 0; r < POKER_7_5_PERMUTATIONS; r++) {
    get_permutation_instance(&m,&n,&o,&p,&q,r,&ixs[0]);

    hand.NewCards(_card[ixs[0]],_card[ixs[1]],_card[ixs[2]],
      _card[ixs[3]],_card[ixs[4]]);

    if (!r)
      _best_poker_hand = hand;
    else {
      ret_compare = hand.Compare(_best_poker_hand);

      if (ret_compare == 1)
        _best_poker_hand = hand;
    }
  }

  return _best_poker_hand;
}

PokerHand& BoardPokerHand::BestPokerHandNoWheel()
{
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  int ixs[NUM_CARDS_IN_HAND];
  PokerHand hand;
  int ret_compare;

  hand.SetNoWheel(1);

  for (r = 0; r < POKER_7_5_PERMUTATIONS; r++) {
    get_permutation_instance(&m,&n,&o,&p,&q,r,&ixs[0]);

    hand.NewCards(_card[ixs[0]],_card[ixs[1]],_card[ixs[2]],
      _card[ixs[3]],_card[ixs[4]]);

    if (!r)
      _best_poker_hand = hand;
    else {
      ret_compare = hand.Compare(_best_poker_hand);

      if (ret_compare == 1)
        _best_poker_hand = hand;
    }
  }

  return _best_poker_hand;
}

void BoardPokerHand::print(ostream& out) const
{
  int n;
  char card_string[3];

  if (!_have_cards)
    return;

  card_string[2] = 0;

  for (n = 0; n < NUM_CARDS_IN_POOL; n++) {
    card_string_from_card_value(_card[n],card_string);

    out << card_string;

    if (n < NUM_CARDS_IN_POOL - 1)
      out << " ";
    else
      out << endl;
  }
}

ostream& operator<<(ostream& out,const BoardPokerHand& board_hand)
{
  board_hand.print(out);

  return out;
}

static void get_permutation_instance(
  int *m,int *n,int *o,int *p,int *q,
  int instance_ix,int *ixs
)
{
  if (instance_ix)
    goto return_point;

  for (*m = 0; *m < NUM_CARDS_IN_POOL - NUM_CARDS_IN_HAND + 1; (*m)++) {
    ixs[0] = *m;

    for (*n = *m + 1; *n < NUM_CARDS_IN_POOL - NUM_CARDS_IN_HAND + 2; (*n)++) {
      ixs[1] = *n;

      for (*o = *n + 1; *o < NUM_CARDS_IN_POOL - NUM_CARDS_IN_HAND + 3; (*o)++) {
        ixs[2] = *o;

        for (*p = *o + 1; *p < NUM_CARDS_IN_POOL - NUM_CARDS_IN_HAND + 4; (*p)++) {
          ixs[3] = *p;

          for (*q = *p + 1; *q < NUM_CARDS_IN_POOL - NUM_CARDS_IN_HAND + 5; (*q)++) {
            ixs[4] = *q;

            return;

            return_point:
            ;
          }
        }
      }
    }
  }
}

int card_value_from_card_string(char *card_string,int *card_value)
{
  int rank;
  int suit;

  if (strlen(card_string) < 2)
    return 1;

  // get the rank index

  for (rank = 0; rank < NUM_CARDS_IN_SUIT; rank++) {
    if (card_string[0] == rank_chars[rank])
      break;
  }

  if (rank == NUM_CARDS_IN_SUIT)
    return 2;

  // get the suit index

  for (suit = 0; suit < NUM_SUITS; suit++) {
    if (card_string[1] == suit_chars[suit])
      break;
  }

  if (suit == NUM_SUITS)
    return 3;

  *card_value = suit * NUM_CARDS_IN_SUIT + rank;

  return 0;
}

int card_string_from_card_value(int card_value,char *card_string)
{
  int rank_ix;
  int suit_ix;

  if (card_value >= NUM_CARDS_IN_DECK)
    return 1;

  rank_ix = card_value % NUM_CARDS_IN_SUIT;
  suit_ix = card_value / NUM_CARDS_IN_SUIT;

  card_string[0] = rank_chars[rank_ix];
  card_string[1] = suit_chars[suit_ix];

  return 0;
}