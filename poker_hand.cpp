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

#include "poker_hand.h"

// default constructor

PokerHand::PokerHand()
{
  _have_cards = false;
  _hand_sorted = false;
  _hand_evaluated = false;
  _verbose = false;
  _plain = false;
  _debug_level = 0;
  _num_cards_in_deck = NUM_CARDS_IN_DECK;
}

// copy constructor

PokerHand::PokerHand(const PokerHand& hand)
{
  int n;

  for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
    _card.cards[n] = hand._card.cards[n];
    _suit.cards[n] = hand._suit.cards[n];
    _rank.cards[n] = hand._rank.cards[n];
    _num_cards_with_same_rank.cards[n] = hand._num_cards_with_same_rank.cards[n];
    _order.cards[n] = hand._order.cards[n];
  }

  _have_cards = hand._have_cards;
  _hand_sorted = hand._hand_sorted;
  _hand_evaluated = hand._hand_evaluated;
  _verbose = hand._verbose;
  _plain = hand._plain;

  _hand_type = hand._hand_type;
  _quick_ix = hand._quick_ix;

  _debug_level = hand._debug_level;
  _num_cards_in_deck = hand._num_cards_in_deck;
}

// assignment operator

PokerHand& PokerHand::operator=(const PokerHand& hand)
{
  int n;

  for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
    _card.cards[n] = hand._card.cards[n];
    _suit.cards[n] = hand._suit.cards[n];
    _rank.cards[n] = hand._rank.cards[n];
    _num_cards_with_same_rank.cards[n] = hand._num_cards_with_same_rank.cards[n];
    _order.cards[n] = hand._order.cards[n];
  }

  _have_cards = hand._have_cards;
  _hand_sorted = hand._hand_sorted;
  _hand_evaluated = hand._hand_evaluated;
  _verbose = hand._verbose;
  _plain = hand._plain;

  _hand_type = hand._hand_type;
  _quick_ix = hand._quick_ix;

  _debug_level = hand._debug_level;
  _num_cards_in_deck = hand._num_cards_in_deck;

  return *this;
}

// destructor

PokerHand::~PokerHand()
{
}

PokerHand::PokerHand(int card1,int card2,int card3,int card4, int card5, int num_cards_in_deck)
{
  int n;

  _card.cards[0] = card1 % NUM_CARDS_IN_DECK;
  _card.cards[1] = card2 % NUM_CARDS_IN_DECK;
  _card.cards[2] = card3 % NUM_CARDS_IN_DECK;
  _card.cards[3] = card4 % NUM_CARDS_IN_DECK;
  _card.cards[4] = card5 % NUM_CARDS_IN_DECK;

  for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
    _suit.cards[n] = suit_of(_card.cards[n]);
    _rank.cards[n] = rank_of(_card.cards[n]);
  }

  _have_cards = true;
  _hand_sorted = false;
  _hand_evaluated = false;
  _verbose = false;
  _plain = false;
  _num_cards_in_deck = num_cards_in_deck;
}

PokerHand::PokerHand(int num_cards_in_deck)
{
  _have_cards = false;
  _hand_sorted = false;
  _hand_evaluated = false;
  _verbose = false;
  _plain = false;
  _num_cards_in_deck = num_cards_in_deck;
}

void PokerHand::NewCards(int card1,int card2,int card3,int card4, int card5)
{
  int n;

  _card.cards[0] = card1 % NUM_CARDS_IN_DECK;
  _card.cards[1] = card2 % NUM_CARDS_IN_DECK;
  _card.cards[2] = card3 % NUM_CARDS_IN_DECK;
  _card.cards[3] = card4 % NUM_CARDS_IN_DECK;
  _card.cards[4] = card5 % NUM_CARDS_IN_DECK;

  for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
    _suit.cards[n] = suit_of(_card.cards[n]);
    _rank.cards[n] = rank_of(_card.cards[n]);
  }

  _have_cards = true;
  _hand_sorted = false;
  _hand_evaluated = false;
  _verbose = false;
  _plain = false;
}

int PokerHand::GetRank(int card_ix)
{
  if (!_have_cards)
    return -1;

  if (!_hand_sorted) {
    Sort();
    _hand_evaluated = false;
  }

  return _rank.cards[_order.cards[card_ix]];
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
    num_cards_with_same_rank = 1;

    for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
      if (n == m)
        continue;

      if (_rank.cards[m] == _rank.cards[n])
        num_cards_with_same_rank++;
    }

    _num_cards_with_same_rank.cards[m] = num_cards_with_same_rank;
  }

  for (n = 0; n < NUM_CARDS_IN_HAND; n++)
    _order.cards[n] = n;

  for (m = 0; m < NUM_CARDS_IN_HAND - 1; m++) {
    for (n = m + 1; n < NUM_CARDS_IN_HAND; n++) {
      if (_num_cards_with_same_rank.cards[_order.cards[m]] < _num_cards_with_same_rank.cards[_order.cards[n]]) {
        temp = _order.cards[m];
        _order.cards[m] = _order.cards[n];
        _order.cards[n] = temp;
      }
      else if (_num_cards_with_same_rank.cards[_order.cards[m]] == _num_cards_with_same_rank.cards[_order.cards[n]]) {
        if (_rank.cards[_order.cards[m]] < _rank.cards[_order.cards[n]]) {
          temp = _order.cards[m];
          _order.cards[m] = _order.cards[n];
          _order.cards[n] = temp;
        }
      }
    }
  }

  _hand_sorted = true;
}

int compare1(const void *elem1,const void *elem2)
{
  int int1;
  int int2;

  int1 = *(int *)elem1;
  int2 = *(int *)elem2;

  return int1 - int2;
}

void PokerHand::Evaluate0(int *hand_counts_ptr)
{
  hand sorted_hand;

  if (!_have_cards)
    return;

  if (!_hand_sorted) {
    Sort();

    if (!_hand_sorted)
      return;

    _hand_evaluated = false;
  }

  if (_hand_evaluated)
    return;

  if (RoyalFlush())
    hand_counts_ptr[ROYAL_FLUSH]++;
  if (StraightFlush())
    hand_counts_ptr[STRAIGHT_FLUSH]++;
  if (FourOfAKind())
    hand_counts_ptr[FOUR_OF_A_KIND]++;
  if (FullHouse())
    hand_counts_ptr[FULL_HOUSE]++;
  if (Flush())
    hand_counts_ptr[FLUSH]++;
  if (Straight())
    hand_counts_ptr[STRAIGHT]++;
  if (ThreeOfAKind())
    hand_counts_ptr[THREE_OF_A_KIND]++;
  if (TwoPair())
    hand_counts_ptr[TWO_PAIRS]++;
  if (OnePair())
    hand_counts_ptr[ONE_PAIR]++;
  if (HighCard())
    hand_counts_ptr[HIGH_CARD]++;
}

HandType PokerHand::Evaluate()
{
  hand sorted_hand;

  num_evaluations++;

  if (!_have_cards)
    return HIGH_CARD;

  if (!_hand_sorted) {
    Sort();

    if (!_hand_sorted)
      return HIGH_CARD;

    _hand_evaluated = false;
  }

  if (_hand_evaluated)
    return _hand_type;

  num_unique_evaluations++;

  if (_debug_level == 10) {
    sorted_hand = _card;
    qsort(&sorted_hand.cards[0],NUM_CARDS_IN_HAND,sizeof (int),compare1);
  }

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
  else if (TwoPair())
    _hand_type = TWO_PAIRS;
  else if (OnePair())
    _hand_type = ONE_PAIR;
  else
    _hand_type = HIGH_CARD;

  _hand_evaluated = true;

  return _hand_type;
}

HandType PokerHand::EvaluateLow()
{
  num_evaluations++;

  if (!_have_cards)
    return HIGH_CARD;

  if (!_hand_sorted) {
    Sort();

    if (!_hand_sorted)
      return HIGH_CARD;

    _hand_evaluated = false;
  }

  if (_hand_evaluated)
    return _hand_type;

  num_unique_evaluations++;

  if (FourOfAKind())
    _hand_type = FOUR_OF_A_KIND;
  if (FullHouse())
    _hand_type = FULL_HOUSE;
  if (ThreeOfAKind())
    _hand_type = THREE_OF_A_KIND;
  if (TwoPair())
    _hand_type = TWO_PAIRS;
  if (OnePair())
    _hand_type = ONE_PAIR;
  else
    _hand_type = HIGH_CARD;

  _hand_evaluated = true;

  return _hand_type;
}

int PokerHand::GetQuickIx()
{
  return _quick_ix;
}

void PokerHand::UnEvaluate()
{
  _hand_evaluated = false;
}

bool PokerHand::Evaluated()
{
  return _hand_evaluated;
}

bool PokerHand::RoyalFlush()
{
  if (!StraightFlush())
    return 0;

  if ((_rank.cards[_order.cards[0]] != ACE) || (_rank.cards[_order.cards[1]] != KING))
    return 0;

  return 1;
}

bool PokerHand::StraightFlush()
{
  if (!Straight())
    return 0;

  if (!Flush())
    return 0;

  return 1;
}

bool PokerHand::FourOfAKind()
{
  if (_num_cards_with_same_rank.cards[_order.cards[0]] == 4)
    return 1;

  return 0;
}

bool PokerHand::FullHouse()
{
  if ((_num_cards_with_same_rank.cards[_order.cards[0]] == 3) &&
      (_num_cards_with_same_rank.cards[_order.cards[3]] == 2))
    return 1;

  return 0;
}

bool PokerHand::Flush()
{
  int n;

  for (n = 1; n < NUM_CARDS_IN_HAND; n++) {
    if (_suit.cards[_order.cards[n]] != _suit.cards[_order.cards[0]])
      break;
  }

  if (n < NUM_CARDS_IN_HAND)
    return 0;

  return 1;
}

bool PokerHand::Straight()
{
  int n;

  // first, handle the special case of a wheel ({A, 2, 3, 4, 5} in a regular deck
  // and {A, 6, 7, 8, 9} in a short deck)

  if (_num_cards_in_deck == NUM_CARDS_IN_DECK) {
    if ((_rank.cards[_order.cards[0]] == ACE) &&
        (_rank.cards[_order.cards[1]] == FIVE) &&
        (_rank.cards[_order.cards[2]] == FOUR) &&
        (_rank.cards[_order.cards[3]] == THREE) &&
        (_rank.cards[_order.cards[4]] == TWO))
      return 1;
  }
  else if (_num_cards_in_deck == NUM_CARDS_IN_SHORT_DECK) {
    if ((_rank.cards[_order.cards[0]] == ACE) &&
        (_rank.cards[_order.cards[1]] == NINE) &&
        (_rank.cards[_order.cards[2]] == EIGHT) &&
        (_rank.cards[_order.cards[3]] == SEVEN) &&
        (_rank.cards[_order.cards[4]] == SIX))
      return 1;
  }

  for (n = 1; n < NUM_CARDS_IN_HAND; n++) {
    if (_rank.cards[_order.cards[n-1]] != _rank.cards[_order.cards[n]] + 1)
      break;
  }

  if (n < NUM_CARDS_IN_HAND)
    return 0;

  return 1;
}

bool PokerHand::Broadway()
{
  if (!Straight())
    return 0;

  if ((_rank.cards[_order.cards[0]] != ACE) || (_rank.cards[_order.cards[1]] != KING))
    return 0;

  return 1;
}

bool PokerHand::ThreeOfAKind()
{
  if (_num_cards_with_same_rank.cards[_order.cards[0]] == 3)
    return 1;

  return 0;
}

bool PokerHand::TwoPair()
{
  if ((_num_cards_with_same_rank.cards[_order.cards[0]] == 2) &&
      (_num_cards_with_same_rank.cards[_order.cards[2]] == 2))
    return 1;

  if ((_num_cards_with_same_rank.cards[_order.cards[0]] == 3) &&
      (_num_cards_with_same_rank.cards[_order.cards[3]] == 2))
    return 1;

  return 0;
}

bool PokerHand::OnePair()
{
  if (_num_cards_with_same_rank.cards[_order.cards[0]] == 2)
    return 1;

  return 0;
}

bool PokerHand::HighCard()
{
  return 1;
}

char *PokerHand::GetHand()
{
  int m;
  int n;
  char card_string[3];
  static char hand_string[15];

  if (!_hand_evaluated)
    Evaluate();

  for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
    if ((_hand_type == STRAIGHT) || (_hand_type == STRAIGHT_FLUSH) ||
        (_hand_type == ROYAL_FLUSH)) {
      if ((_rank.cards[_order.cards[0]] == ACE) && (_rank.cards[_order.cards[1]] == FIVE)) {
        if (!n) {
          card_string_from_card_value(
            _card.cards[_order.cards[0]],
            card_string);
        }
        else {
          card_string_from_card_value(
            _card.cards[_order.cards[NUM_CARDS_IN_HAND - 1 - (n - 1)]],
            card_string);
        }
      }
      else {
        card_string_from_card_value(
          _card.cards[_order.cards[NUM_CARDS_IN_HAND - 1 - n]],
          card_string);
      }
    }
    else
      card_string_from_card_value(_card.cards[_order.cards[n]],card_string);

    for (m = 0; m < 2; m++)
      hand_string[n * 3 + m] = card_string[m];

    if (n < NUM_CARDS_IN_HAND - 1)
      hand_string[n * 3 + 2] = ' ';
    else
      hand_string[n * 3 + 2] = 0;
  }

  return hand_string;
}

HandType PokerHand::GetHandType()
{
  if (!_hand_evaluated)
    Evaluate();

  return _hand_type;
}

int PokerHand::Compare(PokerHand& compare_hand,int in_holdem_best_poker_hand)
{
  int n;
  HandType hand_type;
  HandType compare_hand_type;
  int rank;
  int compare_rank;

  if (!in_holdem_best_poker_hand)
    num_comparisons++;
  else
    num_holdem_best_poker_hand_comparisons++;

  if (!_hand_evaluated) {
    if (_debug_level == 1)
      cout << "dbg: PokerHand::Compare(): calling Evaluate() 1" << endl;

    hand_type = Evaluate();
  }
  else
    hand_type = _hand_type;

  if (!compare_hand.Evaluated()) {
    if (_debug_level == 1)
      cout << "dbg: PokerHand::Compare(): calling Evaluate() 2" << endl;

    compare_hand_type = compare_hand.Evaluate();
  }
  else
    compare_hand_type = compare_hand.GetHandType();

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
      rank = GetRank(1);
      compare_rank = compare_hand.GetRank(1);

      if (rank > compare_rank)
        return 1;

      if (rank < compare_rank)
        return -1;

      // wheels must compare less than 6 highs, and 6 highs must compare
      // greater than wheels
      rank = GetRank(0);
      compare_rank = compare_hand.GetRank(0);

      if (rank > compare_rank)
        return -1;

      if (rank < compare_rank)
        return 1;

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
    case ROYAL_FLUSH:
      return 0;
  }

  return 0;
}

int PokerHand::CompareLow(PokerHand& compare_hand,int in_holdem_best_poker_hand)
{
  int n;
  HandType hand_type;
  HandType compare_hand_type;
  int rank;
  int compare_rank;

  if (!in_holdem_best_poker_hand)
    num_comparisons++;
  else
    num_holdem_best_poker_hand_comparisons++;

  if (!_hand_evaluated)
    hand_type = Evaluate();
  else
    hand_type = _hand_type;

  if (!compare_hand.Evaluated())
    compare_hand_type = compare_hand.Evaluate();
  else
    compare_hand_type = compare_hand.GetHandType();

  if (hand_type < compare_hand_type)
    return 1;

  if (hand_type > compare_hand_type)
    return -1;

  switch(hand_type) {
    case HIGH_CARD:
    case FLUSH:
      for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
        rank = GetRank(n);
        compare_rank = compare_hand.GetRank(n);

        if (rank < compare_rank)
          return 1;

        if (rank > compare_rank)
          return -1;
      }

      return 0;
    case ONE_PAIR:
      for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
        if (n == 1)
          continue;

        rank = GetRank(n);
        compare_rank = compare_hand.GetRank(n);

        if (rank < compare_rank)
          return 1;

        if (rank > compare_rank)
          return -1;
      }

      return 0;
    case TWO_PAIRS:
      for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
        if ((n == 1) || (n == 3))
          continue;

        rank = GetRank(n);
        compare_rank = compare_hand.GetRank(n);

        if (rank < compare_rank)
          return 1;

        if (rank > compare_rank)
          return -1;
      }

      return 0;
    case THREE_OF_A_KIND:
      for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
        if ((n == 1) || (n == 2))
          continue;

        rank = GetRank(n);
        compare_rank = compare_hand.GetRank(n);

        if (rank < compare_rank)
          return 1;

        if (rank > compare_rank)
          return -1;
      }

      return 0;
    case STRAIGHT:
    case STRAIGHT_FLUSH:
      rank = GetRank(1);
      compare_rank = compare_hand.GetRank(1);

      if (rank < compare_rank)
        return 1;

      if (rank > compare_rank)
        return -1;

      // wheels must compare less than 6 highs, and 6 highs must compare
      // greater than wheels
      rank = GetRank(0);
      compare_rank = compare_hand.GetRank(0);

      if (rank < compare_rank)
        return -1;

      if (rank > compare_rank)
        return 1;

      return 0;
    case FULL_HOUSE:
      for (n = 0; n < NUM_CARDS_IN_HAND - 1; n++) {
        if ((n == 1) || (n == 2))
          continue;

        rank = GetRank(n);
        compare_rank = compare_hand.GetRank(n);

        if (rank < compare_rank)
          return 1;

        if (rank > compare_rank)
          return -1;
      }

      return 0;
    case FOUR_OF_A_KIND:
      for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
        if ((n == 1) || (n == 2) || (n == 3))
          continue;

        rank = GetRank(n);
        compare_rank = compare_hand.GetRank(n);

        if (rank < compare_rank)
          return 1;

        if (rank > compare_rank)
          return -1;
      }

      return 0;
    case ROYAL_FLUSH:
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

  for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
    card_string_from_card_value(_card.cards[n],card_string);

    out << card_string;

    if (_hand_evaluated || (n < NUM_CARDS_IN_HAND - 1))
      out << " ";
  }

  if (!_hand_evaluated)
    return;

  if (_verbose) {
    for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
      if ((_hand_type == STRAIGHT) || (_hand_type == STRAIGHT_FLUSH) ||
          (_hand_type == ROYAL_FLUSH)) {
        if ((_rank.cards[_order.cards[0]] == ACE) && (_rank.cards[_order.cards[1]] == FIVE)) {
          if (!n) {
            card_string_from_card_value(
              _card.cards[_order.cards[0]],
              card_string);
          }
          else {
            card_string_from_card_value(
              _card.cards[_order.cards[NUM_CARDS_IN_HAND - 1 - (n - 1)]],
              card_string);
          }
        }
        else {
          card_string_from_card_value(
            _card.cards[_order.cards[NUM_CARDS_IN_HAND - 1 - n]],
            card_string);
        }
      }
      else
        card_string_from_card_value(_card.cards[_order.cards[n]],card_string);

      out << card_string;

      if (n < NUM_CARDS_IN_HAND - 1)
        out << " ";
      else
        out << endl;
    }
  }

  if (_plain)
    out << plain_hand_types[_hand_type];
  else {
    switch (hand_types[_hand_type].num_var_info_vars) {
      case 0:
        sprintf(print_buf,hand_types[_hand_type].hand_type_fmt);

        break;
      case 1:
        sprintf(print_buf,hand_types[_hand_type].hand_type_fmt,
          rank_strings[_rank.cards[_order.cards[hand_types[_hand_type].var_info_vars[0]]]]);

        break;
      case 2:
        // first, handle the special case of a wheel (A, 2, 3, 4, 5)
        if (((_hand_type == STRAIGHT) || (_hand_type == STRAIGHT_FLUSH) ||
             (_hand_type == ROYAL_FLUSH)) &&
             (_rank.cards[_order.cards[0]] == ACE) && (_rank.cards[_order.cards[1]] == FIVE))
          sprintf(print_buf,hand_types[_hand_type].hand_type_fmt,
            rank_strings[ACE],
            rank_strings[FIVE]);
        else
          sprintf(print_buf,hand_types[_hand_type].hand_type_fmt,
            rank_strings[_rank.cards[_order.cards[hand_types[_hand_type].var_info_vars[0]]]],
            rank_strings[_rank.cards[_order.cards[hand_types[_hand_type].var_info_vars[1]]]]);

        break;
    }
  }

  out << print_buf;
}

ostream& operator<<(ostream& out,const PokerHand& hand)
{
  hand.print(out);

  return out;
}

void PokerHand::Verbose()
{
  _verbose = true;
}

void PokerHand::Terse()
{
  _verbose = false;
}

void PokerHand::Plain()
{
  _plain = true;
}

void PokerHand::Fancy()
{
  _plain = false;
}

int *PokerHand::GetCards()
{
  return &_card.cards[0];
}

void PokerHand::SetDebugLevel(int debug_level)
{
  _debug_level = debug_level;
}

int PokerHand::GetNumCardsInDeck()
{
  return _num_cards_in_deck;
}

// default constructor

HoldemPokerHand::HoldemPokerHand()
{
  _have_cards = false;
  _debug_level = 0;
}

// copy constructor

HoldemPokerHand::HoldemPokerHand(const HoldemPokerHand& hand)
{
  int n;

  for (n = 0; n < NUM_CARDS_IN_HOLDEM_POOL; n++)
    _card.cards[n] = hand._card.cards[n];

  _have_cards = hand._have_cards;
}

// assignment operator

HoldemPokerHand& HoldemPokerHand::operator=(const HoldemPokerHand& hand)
{
  int n;

  for (n = 0; n < NUM_CARDS_IN_HOLDEM_POOL; n++)
    _card.cards[n] = hand._card.cards[n];

  _have_cards = hand._have_cards;
  _debug_level = hand._debug_level;

  return *this;
}

// destructor

HoldemPokerHand::~HoldemPokerHand()
{
}

HoldemPokerHand::HoldemPokerHand(int card1,int card2,int card3,int card4,int card5,int card6,int card7)
{
  _card.cards[0] = card1 % NUM_CARDS_IN_DECK;
  _card.cards[1] = card2 % NUM_CARDS_IN_DECK;
  _card.cards[2] = card3 % NUM_CARDS_IN_DECK;
  _card.cards[3] = card4 % NUM_CARDS_IN_DECK;
  _card.cards[4] = card5 % NUM_CARDS_IN_DECK;
  _card.cards[5] = card6 % NUM_CARDS_IN_DECK;
  _card.cards[6] = card7 % NUM_CARDS_IN_DECK;

  _have_cards = true;
}

void HoldemPokerHand::NewCards(int card1,int card2,int card3,int card4,int card5,int card6,int card7)
{
  _card.cards[0] = card1 % NUM_CARDS_IN_DECK;
  _card.cards[1] = card2 % NUM_CARDS_IN_DECK;
  _card.cards[2] = card3 % NUM_CARDS_IN_DECK;
  _card.cards[3] = card4 % NUM_CARDS_IN_DECK;
  _card.cards[4] = card5 % NUM_CARDS_IN_DECK;
  _card.cards[5] = card6 % NUM_CARDS_IN_DECK;
  _card.cards[6] = card7 % NUM_CARDS_IN_DECK;

  _have_cards = true;
}

PokerHand& HoldemPokerHand::BestPokerHand()
{
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  PokerHand hand;
  int ret_compare;

  hand.SetDebugLevel(_debug_level);

  for (r = 0; r < POKER_7_5_PERMUTATIONS; r++) {
    get_permutation_instance_five(
      NUM_CARDS_IN_HOLDEM_POOL,
      &m,&n,&o,&p,&q,r);

    hand.NewCards(_card.cards[m],_card.cards[n],_card.cards[o],_card.cards[p],_card.cards[q]);

    if (!r)
      _best_poker_hand = hand;
    else {
      ret_compare = hand.Compare(_best_poker_hand,1);

      if (_debug_level == 1) {
        cout << "dbg: HoldemPokerHand::BestPokerHand(): hand             = " << hand << endl;
        cout << "dbg: HoldemPokerHand::BestPokerHand(): _best_poker_hand = " << _best_poker_hand << endl;
        cout << "dbg: HoldemPokerHand::BestPokerHand(): ret_compare = " <<
          ret_compare << ", num_evaluations = " << num_evaluations << endl;
      }

      if (ret_compare == 1)
        _best_poker_hand = hand;
    }
  }

  return _best_poker_hand;
}

void HoldemPokerHand::print(ostream& out) const
{
  int n;
  char card_string[3];

  if (!_have_cards)
    return;

  card_string[2] = 0;

  for (n = 0; n < NUM_CARDS_IN_HOLDEM_POOL; n++) {
    card_string_from_card_value(_card.cards[n],card_string);

    out << card_string;

    if (n < NUM_CARDS_IN_HOLDEM_POOL - 1)
      out << " ";
  }
}

ostream& operator<<(ostream& out,const HoldemPokerHand& holdem_hand)
{
  holdem_hand.print(out);

  return out;
}

void HoldemPokerHand::SetDebugLevel(int debug_level)
{
  _debug_level = debug_level;
}

// default constructor

HoldemTurnHand::HoldemTurnHand()
{
  _have_cards = false;
  _debug_level = 0;
}

// copy constructor

HoldemTurnHand::HoldemTurnHand(const HoldemTurnHand& hand)
{
  int n;

  for (n = 0; n < NUM_CARDS_AT_TURN; n++)
    _card.cards[n] = hand._card.cards[n];

  _have_cards = hand._have_cards;
}

// assignment operator

HoldemTurnHand& HoldemTurnHand::operator=(const HoldemTurnHand& hand)
{
  int n;

  for (n = 0; n < NUM_CARDS_AT_TURN; n++)
    _card.cards[n] = hand._card.cards[n];

  _have_cards = hand._have_cards;
  _debug_level = hand._debug_level;

  return *this;
}

// destructor

HoldemTurnHand::~HoldemTurnHand()
{
}

HoldemTurnHand::HoldemTurnHand(int card1,int card2,int card3,int card4,int card5,int card6)
{
  _card.cards[0] = card1 % NUM_CARDS_IN_DECK;
  _card.cards[1] = card2 % NUM_CARDS_IN_DECK;
  _card.cards[2] = card3 % NUM_CARDS_IN_DECK;
  _card.cards[3] = card4 % NUM_CARDS_IN_DECK;
  _card.cards[4] = card5 % NUM_CARDS_IN_DECK;
  _card.cards[5] = card6 % NUM_CARDS_IN_DECK;

  _have_cards = true;
}

void HoldemTurnHand::NewCards(int card1,int card2,int card3,int card4,int card5,int card6)
{
  _card.cards[0] = card1 % NUM_CARDS_IN_DECK;
  _card.cards[1] = card2 % NUM_CARDS_IN_DECK;
  _card.cards[2] = card3 % NUM_CARDS_IN_DECK;
  _card.cards[3] = card4 % NUM_CARDS_IN_DECK;
  _card.cards[4] = card5 % NUM_CARDS_IN_DECK;
  _card.cards[5] = card6 % NUM_CARDS_IN_DECK;

  _have_cards = true;
}

PokerHand& HoldemTurnHand::BestPokerHand()
{
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  PokerHand hand;
  int ret_compare;

  for (r = 0; r < POKER_6_5_PERMUTATIONS; r++) {
    get_permutation_instance_five(
      NUM_CARDS_AT_TURN,
      &m,&n,&o,&p,&q,r);

    hand.NewCards(_card.cards[m],_card.cards[n],_card.cards[o],_card.cards[p],_card.cards[q]);

    if (!r)
      _best_poker_hand = hand;
    else {
      ret_compare = hand.Compare(_best_poker_hand,1);

      if (ret_compare == 1)
        _best_poker_hand = hand;
    }
  }

  return _best_poker_hand;
}

void HoldemTurnHand::print(ostream& out) const
{
  int n;
  char card_string[3];

  if (!_have_cards)
    return;

  card_string[2] = 0;

  for (n = 0; n < NUM_CARDS_AT_TURN; n++) {
    card_string_from_card_value(_card.cards[n],card_string);

    out << card_string;

    if (n < NUM_CARDS_IN_HOLDEM_POOL - 1)
      out << " ";
  }
}

ostream& operator<<(ostream& out,const HoldemTurnHand& holdem_hand)
{
  holdem_hand.print(out);

  return out;
}

void HoldemTurnHand::SetDebugLevel(int debug_level)
{
  _debug_level = debug_level;
}

// default constructor

Flop::Flop()
{
  _have_cards = false;
  _flop_sorted = false;
  _flop_evaluated = false;
  _verbose = false;
  _plain = false;
  _debug_level = 0;
}

// copy constructor

Flop::Flop(const Flop& flop)
{
  int n;

  for (n = 0; n < NUM_CARDS_IN_FLOP; n++) {
    _card.cards[n] = flop._card.cards[n];
    _suit.cards[n] = flop._suit.cards[n];
    _rank.cards[n] = flop._rank.cards[n];
    _num_cards_with_same_rank.cards[n] = flop._num_cards_with_same_rank.cards[n];
    _order.cards[n] = flop._order.cards[n];
  }

  _have_cards = flop._have_cards;
  _flop_sorted = flop._flop_sorted;
  _flop_evaluated = flop._flop_evaluated;
  _verbose = flop._verbose;
  _plain = flop._plain;

  _flop_type = flop._flop_type;
}

// assignment operator

Flop& Flop::operator=(const Flop& flop)
{
  int n;

  for (n = 0; n < NUM_CARDS_IN_FLOP; n++) {
    _card.cards[n] = flop._card.cards[n];
    _suit.cards[n] = flop._suit.cards[n];
    _rank.cards[n] = flop._rank.cards[n];
    _num_cards_with_same_rank.cards[n] = flop._num_cards_with_same_rank.cards[n];
    _order.cards[n] = flop._order.cards[n];
  }

  _have_cards = flop._have_cards;
  _flop_sorted = flop._flop_sorted;
  _flop_evaluated = flop._flop_evaluated;
  _verbose = flop._verbose;
  _plain = flop._plain;

  _flop_type = flop._flop_type;

  _debug_level = flop._debug_level;

  return *this;
}

// destructor

Flop::~Flop()
{
}

Flop::Flop(int card1,int card2,int card3)
{
  int n;

  _card.cards[0] = card1 % NUM_CARDS_IN_DECK;
  _card.cards[1] = card2 % NUM_CARDS_IN_DECK;
  _card.cards[2] = card3 % NUM_CARDS_IN_DECK;

  for (n = 0; n < NUM_CARDS_IN_FLOP; n++) {
    _suit.cards[n] = suit_of(_card.cards[n]);
    _rank.cards[n] = rank_of(_card.cards[n]);
  }

  _have_cards = true;
  _flop_sorted = false;
  _flop_evaluated = false;
  _verbose = false;
  _plain = false;
}

void Flop::NewCards(int card1,int card2,int card3)
{
  int n;

  _card.cards[0] = card1 % NUM_CARDS_IN_DECK;
  _card.cards[1] = card2 % NUM_CARDS_IN_DECK;
  _card.cards[2] = card3 % NUM_CARDS_IN_DECK;

  for (n = 0; n < NUM_CARDS_IN_FLOP; n++) {
    _suit.cards[n] = suit_of(_card.cards[n]);
    _rank.cards[n] = rank_of(_card.cards[n]);
  }

  _have_cards = true;
  _flop_sorted = false;
  _flop_evaluated = false;
  _verbose = false;
  _plain = false;
}

int Flop::GetCard(int card_ix)
{
  return _card.cards[card_ix];
}

int Flop::GetRank(int card_ix)
{
  if (!_have_cards)
    return -1;

  if (!_flop_sorted) {
    Sort();
    _flop_evaluated = false;
  }

  return _rank.cards[_order.cards[card_ix]];
}

void Flop::Sort()
{
  int m;
  int n;
  int num_cards_with_same_rank;
  int temp;

  if (!_have_cards)
    return;

  if (_flop_sorted)
    return;

  for (m = 0; m < NUM_CARDS_IN_FLOP; m++) {
    num_cards_with_same_rank = 1;

    for (n = 0; n < NUM_CARDS_IN_FLOP; n++) {
      if (n == m)
        continue;

      if (_rank.cards[m] == _rank.cards[n])
        num_cards_with_same_rank++;
    }

    _num_cards_with_same_rank.cards[m] = num_cards_with_same_rank;
  }

  for (n = 0; n < NUM_CARDS_IN_FLOP; n++)
    _order.cards[n] = n;

  for (m = 0; m < NUM_CARDS_IN_FLOP - 1; m++) {
    for (n = m + 1; n < NUM_CARDS_IN_FLOP; n++) {
      if (_num_cards_with_same_rank.cards[_order.cards[m]] < _num_cards_with_same_rank.cards[_order.cards[n]]) {
        temp = _order.cards[m];
        _order.cards[m] = _order.cards[n];
        _order.cards[n] = temp;
      }
      else if (_num_cards_with_same_rank.cards[_order.cards[m]] == _num_cards_with_same_rank.cards[_order.cards[n]]) {
        if (_rank.cards[_order.cards[m]] < _rank.cards[_order.cards[n]]) {
          temp = _order.cards[m];
          _order.cards[m] = _order.cards[n];
          _order.cards[n] = temp;
        }
      }
    }
  }

  _flop_sorted = true;
}

FlopType Flop::Evaluate()
{
  num_evaluations++;

  if (!_have_cards)
    return DRY;

  if (!_flop_sorted) {
    Sort();

    if (!_flop_sorted)
      return DRY;

    _flop_evaluated = false;
  }

  if (_flop_evaluated)
    return _flop_type;

  num_unique_evaluations++;

  if (Flush() || Straight() || PairOrBetter())
    _flop_type = WET;
  else
    _flop_type = DRY;

  _flop_evaluated = true;

  return _flop_type;
}

int Flop::Evaluated()
{
  return _flop_evaluated;
}

int Flop::Flush()
{
  int n;

  for (n = 1; n < NUM_CARDS_IN_FLOP; n++) {
    if (_suit.cards[_order.cards[n]] != _suit.cards[_order.cards[0]])
      break;
  }

  if (n < NUM_CARDS_IN_FLOP)
    return 0;

  return 1;
}

int Flop::Straight()
{
  if (PairOrBetter())
    return 0;

  // handle wheel case

  if (GetRank(0) == ACE) {
    if (GetRank(1) <= FIVE)
      return 1;
  }

  if (GetRank(0) - GetRank(2) <= 4)
    return 1;

  return 0;
}

int Flop::PairOrBetter()
{
  if (_num_cards_with_same_rank.cards[_order.cards[0]] >= 2)
    return 1;

  return 0;
}

FlopType Flop::GetFlopType()
{
  if (!_flop_evaluated)
    Evaluate();

  return _flop_type;
}

void Flop::print(ostream& out) const
{
  int n;
  char card_string[3];

  if (!_have_cards)
    return;

  card_string[2] = 0;

  //if (!_flop_evaluated) {
    for (n = 0; n < NUM_CARDS_IN_FLOP; n++) {
      card_string_from_card_value(_card.cards[n],card_string);

      out << card_string;

      if (n < NUM_CARDS_IN_FLOP - 1)
        out << " ";
    }

    return;
  //}

  //out << print_buf;
}

ostream& operator<<(ostream& out,const Flop& flop)
{
  flop.print(out);

  return out;
}

void Flop::Verbose()
{
  _verbose = true;
}

void Flop::Terse()
{
  _verbose = false;
}

void Flop::Plain()
{
  _plain = true;
}

void Flop::Fancy()
{
  _plain = false;
}

// default constructor

OmahaPokerHand::OmahaPokerHand()
{
  _have_cards = false;
  _debug_level = 0;
}

// copy constructor

OmahaPokerHand::OmahaPokerHand(const OmahaPokerHand& hand)
{
  int n;

  for (n = 0; n < NUM_CARDS_IN_OMAHA_POOL; n++)
    _card.cards[n] = hand._card.cards[n];

  _have_cards = hand._have_cards;
}

// assignment operator

OmahaPokerHand& OmahaPokerHand::operator=(const OmahaPokerHand& hand)
{
  int n;

  for (n = 0; n < NUM_CARDS_IN_OMAHA_POOL; n++)
    _card.cards[n] = hand._card.cards[n];

  _have_cards = hand._have_cards;
  _debug_level = hand._debug_level;

  return *this;
}

// destructor

OmahaPokerHand::~OmahaPokerHand()
{
}

OmahaPokerHand::OmahaPokerHand(int card1,int card2,int card3,int card4,int card5,int card6,int card7,int card8,int card9)
{
  _card.cards[0] = card1 % NUM_CARDS_IN_DECK;
  _card.cards[1] = card2 % NUM_CARDS_IN_DECK;
  _card.cards[2] = card3 % NUM_CARDS_IN_DECK;
  _card.cards[3] = card4 % NUM_CARDS_IN_DECK;
  _card.cards[4] = card5 % NUM_CARDS_IN_DECK;
  _card.cards[5] = card6 % NUM_CARDS_IN_DECK;
  _card.cards[6] = card7 % NUM_CARDS_IN_DECK;
  _card.cards[7] = card8 % NUM_CARDS_IN_DECK;
  _card.cards[8] = card9 % NUM_CARDS_IN_DECK;

  _have_cards = true;
}

void OmahaPokerHand::NewCards(int card1,int card2,int card3,int card4,int card5,int card6,int card7,int card8,int card9)
{
  _card.cards[0] = card1 % NUM_CARDS_IN_DECK;
  _card.cards[1] = card2 % NUM_CARDS_IN_DECK;
  _card.cards[2] = card3 % NUM_CARDS_IN_DECK;
  _card.cards[3] = card4 % NUM_CARDS_IN_DECK;
  _card.cards[4] = card5 % NUM_CARDS_IN_DECK;
  _card.cards[5] = card6 % NUM_CARDS_IN_DECK;
  _card.cards[6] = card7 % NUM_CARDS_IN_DECK;
  _card.cards[7] = card8 % NUM_CARDS_IN_DECK;
  _card.cards[8] = card9 % NUM_CARDS_IN_DECK;

  _have_cards = true;
}

PokerHand& OmahaPokerHand::BestPokerHand()
{
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  int s;
  PokerHand hand;
  int ret_compare;
  char card_string[3];

  for (o = 0; o < POKER_4_2_PERMUTATIONS; o++) {
    get_permutation_instance_two(
      NUM_HOLE_CARDS_IN_OMAHA_HAND,
      &m,&n,o);

    for (s = 0; s < POKER_5_3_PERMUTATIONS; s++) {
      get_permutation_instance_three(
        NUM_COMMUNITY_CARDS,
        &p,&q,&r,s);

      if (_debug_level == 1) {
        card_string_from_card_value(_card.cards[m],card_string);
        cout << card_string << " ";
        card_string_from_card_value(_card.cards[n],card_string);
        cout << card_string << " ";

        card_string_from_card_value(_card.cards[NUM_HOLE_CARDS_IN_OMAHA_HAND + p],
          card_string);
        cout << card_string << " ";
        card_string_from_card_value(_card.cards[NUM_HOLE_CARDS_IN_OMAHA_HAND + q],
          card_string);
        cout << card_string << " ";
        card_string_from_card_value(_card.cards[NUM_HOLE_CARDS_IN_OMAHA_HAND + r],
          card_string);
        cout << card_string << " ";
      }

      hand.NewCards(_card.cards[m],_card.cards[n],
        _card.cards[NUM_HOLE_CARDS_IN_OMAHA_HAND + p],
        _card.cards[NUM_HOLE_CARDS_IN_OMAHA_HAND + q],
        _card.cards[NUM_HOLE_CARDS_IN_OMAHA_HAND + r]);

      hand.Evaluate();

      if (_debug_level == 1)
        cout << hand << endl;

      if (!o && !s)
        _best_poker_hand = hand;
      else {
        ret_compare = hand.Compare(_best_poker_hand,1);

        if (ret_compare == 1) {
          _best_poker_hand = hand;

          if (_debug_level == 1)
            cout << "new best poker hand" << endl;
        }
      }
    }
  }

  return _best_poker_hand;
}

PokerHand& OmahaPokerHand::BestLowPokerHand()
{
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  int s;
  PokerHand hand;
  int ret_compare;
  char card_string[3];

  for (o = 0; o < POKER_4_2_PERMUTATIONS; o++) {
    get_permutation_instance_two(
      NUM_HOLE_CARDS_IN_OMAHA_HAND,
      &m,&n,o);

    for (s = 0; s < POKER_5_3_PERMUTATIONS; s++) {
      get_permutation_instance_three(
        NUM_COMMUNITY_CARDS,
        &p,&q,&r,s);

      if (_debug_level == 1) {
        card_string_from_card_value(_card.cards[m],card_string);
        cout << card_string << " ";
        card_string_from_card_value(_card.cards[n],card_string);
        cout << card_string << " ";

        card_string_from_card_value(_card.cards[NUM_HOLE_CARDS_IN_OMAHA_HAND + p],
          card_string);
        cout << card_string << " ";
        card_string_from_card_value(_card.cards[NUM_HOLE_CARDS_IN_OMAHA_HAND + q],
          card_string);
        cout << card_string << " ";
        card_string_from_card_value(_card.cards[NUM_HOLE_CARDS_IN_OMAHA_HAND + r],
          card_string);
        cout << card_string << " ";
      }

      hand.NewCards(_card.cards[m],_card.cards[n],
        _card.cards[NUM_HOLE_CARDS_IN_OMAHA_HAND + p],
        _card.cards[NUM_HOLE_CARDS_IN_OMAHA_HAND + q],
        _card.cards[NUM_HOLE_CARDS_IN_OMAHA_HAND + r]);

      hand.EvaluateLow();

      if (_debug_level == 1)
        cout << hand << endl;

      if (!o && !s)
        _best_poker_hand = hand;
      else {
        ret_compare = hand.CompareLow(_best_poker_hand,1);

        if (ret_compare == 1) {
          _best_poker_hand = hand;

          if (_debug_level == 1)
            cout << "new best poker hand" << endl;
        }
      }
    }
  }

  return _best_poker_hand;
}

void OmahaPokerHand::print(ostream& out) const
{
  int n;
  char card_string[3];

  if (!_have_cards)
    return;

  card_string[2] = 0;

  for (n = 0; n < NUM_CARDS_IN_OMAHA_POOL; n++) {
    card_string_from_card_value(_card.cards[n],card_string);

    out << card_string;

    if (n < NUM_CARDS_IN_OMAHA_POOL - 1)
      out << " ";
  }
}

ostream& operator<<(ostream& out,const OmahaPokerHand& holdem_hand)
{
  holdem_hand.print(out);

  return out;
}

// default constructor

HeadsUp::HeadsUp()
{
  _have_cards = false;
  _evaluated = false;
  _num_burnt_cards = 0;
}

// copy constructor

HeadsUp::HeadsUp(const HeadsUp& hu)
{
  int n;

  for (n = 0; n < NUM_HEADS_UP_CARDS; n++) {
    _cards[n] = hu._cards[n];
  }

  for (n = 0; n < hu._num_burnt_cards; n++) {
    _burnt_cards[n] = hu._burnt_cards[n];
  }

  _have_cards = hu._have_cards;
  _evaluated = hu._evaluated;
  _num_burnt_cards = hu._num_burnt_cards;
}

// assignment operator

HeadsUp& HeadsUp::operator=(const HeadsUp& hu)
{
  int n;

  for (n = 0; n < NUM_HEADS_UP_CARDS; n++) {
    _cards[n] = hu._cards[n];
  }

  for (n = 0; n < hu._num_burnt_cards; n++) {
    _burnt_cards[n] = hu._burnt_cards[n];
  }

  _have_cards = hu._have_cards;
  _evaluated = hu._evaluated;
  _num_burnt_cards = hu._num_burnt_cards;

  return *this;
}

// destructor

HeadsUp::~HeadsUp()
{
}

HeadsUp::HeadsUp(int card1,int card2,int card3,int card4)
{
  _cards[0] = card1 % NUM_CARDS_IN_DECK;
  _cards[1] = card2 % NUM_CARDS_IN_DECK;
  _cards[2] = card3 % NUM_CARDS_IN_DECK;
  _cards[3] = card4 % NUM_CARDS_IN_DECK;

  _have_cards = true;
  _evaluated = false;
}

void HeadsUp::NewCards(int card1,int card2,int card3,int card4)
{
  _cards[0] = card1 % NUM_CARDS_IN_DECK;
  _cards[1] = card2 % NUM_CARDS_IN_DECK;
  _cards[2] = card3 % NUM_CARDS_IN_DECK;
  _cards[3] = card4 % NUM_CARDS_IN_DECK;

  _have_cards = true;
  _evaluated = false;
}

void HeadsUp::BurntCards(
  int card1,int card2,int card3,int card4, int card5,
  int card6,int card7,int card8,int card9, int card10,
  int card11,int card12,int card13,int card14)
{
  if (card1 == -1) {
    _num_burnt_cards = 0;
    return;
  }

  _burnt_cards[0] = card1 % NUM_CARDS_IN_DECK;

  if (card2 == -1) {
    _num_burnt_cards = 1;
    return;
  }

  _burnt_cards[1] = card2 % NUM_CARDS_IN_DECK;

  if (card3 == -1) {
    _num_burnt_cards = 2;
    return;
  }

  _burnt_cards[2] = card3 % NUM_CARDS_IN_DECK;

  if (card4 == -1) {
    _num_burnt_cards = 3;
    return;
  }

  _burnt_cards[3] = card4 % NUM_CARDS_IN_DECK;

  if (card5 == -1) {
    _num_burnt_cards = 4;
    return;
  }

  _burnt_cards[4] = card5 % NUM_CARDS_IN_DECK;

  if (card6 == -1) {
    _num_burnt_cards = 5;
    return;
  }

  _burnt_cards[5] = card6 % NUM_CARDS_IN_DECK;

  if (card7 == -1) {
    _num_burnt_cards = 6;
    return;
  }

  _burnt_cards[6] = card7 % NUM_CARDS_IN_DECK;

  if (card8 == -1) {
    _num_burnt_cards = 7;
    return;
  }

  _burnt_cards[7] = card8 % NUM_CARDS_IN_DECK;

  if (card9 == -1) {
    _num_burnt_cards = 8;
    return;
  }

  _burnt_cards[8] = card9 % NUM_CARDS_IN_DECK;

  if (card10 == -1) {
    _num_burnt_cards = 9;
    return;
  }

  _burnt_cards[9] = card10 % NUM_CARDS_IN_DECK;

  if (card11 == -1) {
    _num_burnt_cards = 10;
    return;
  }

  _burnt_cards[10] = card11 % NUM_CARDS_IN_DECK;

  if (card12 == -1) {
    _num_burnt_cards = 11;
    return;
  }

  _burnt_cards[11] = card12 % NUM_CARDS_IN_DECK;

  if (card13 == -1) {
    _num_burnt_cards = 12;
    return;
  }

  _burnt_cards[12] = card13 % NUM_CARDS_IN_DECK;

  if (card14 == -1) {
    _num_burnt_cards = 13;
    return;
  }

  _burnt_cards[13] = card14 % NUM_CARDS_IN_DECK;
  _num_burnt_cards = 14;
}

struct outcomes * HeadsUp::GetOutcomes()
{
  return _outcomes;
}

void HeadsUp::Evaluate(bool bVerbose)
{
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  int num_remaining_cards;
  int remaining_cards[NUM_REMAINING_HEADS_UP_CARDS];
  HoldemPokerHand holdem_hand[2];
  PokerHand hand[2];
  int ret_compare;

  num_remaining_cards = NUM_REMAINING_HEADS_UP_CARDS - _num_burnt_cards;

  m = 0;

  for (n = 0; n < NUM_CARDS_IN_DECK; n++) {
    for (o = 0; o < NUM_HEADS_UP_CARDS; o++) {
      if (n == _cards[o])
        break;
    }

    if (o == NUM_HEADS_UP_CARDS) {
      for (o = 0; o < _num_burnt_cards; o++) {
        if (n == _burnt_cards[o])
          break;
      }

      if (o == _num_burnt_cards)
        remaining_cards[m++] = n;
    }
  }

  for (n = 0; n < 2; n++) {
    _outcomes[n].wins = 0;
    _outcomes[n].losses = 0;
    _outcomes[n].ties = 0;

    if (bVerbose) {
      for (m = 0; m < NUM_HAND_TYPES; m++) {
        _outcomes[n].wins_hand_counts[m] = 0;
        _outcomes[n].losses_hand_counts[m] = 0;
        _outcomes[n].ties_hand_counts[m] = 0;
      }
    }
  }

  for (r = 0; ; ) {
    get_permutation_instance_five(num_remaining_cards,&m,&n,&o,&p,&q,r);

    holdem_hand[0].NewCards(_cards[0],_cards[1],
      remaining_cards[m],remaining_cards[n],
      remaining_cards[o],remaining_cards[p],
      remaining_cards[q]);

    holdem_hand[1].NewCards(_cards[2],_cards[3],
      remaining_cards[m],remaining_cards[n],
      remaining_cards[o],remaining_cards[p],
      remaining_cards[q]);

    hand[0] = holdem_hand[0].BestPokerHand();
    hand[1] = holdem_hand[1].BestPokerHand();

    ret_compare = hand[0].Compare(hand[1],0);

    if (ret_compare == 1) {
      _outcomes[0].wins++;
      _outcomes[1].losses++;

      if (bVerbose) {
        _outcomes[0].wins_hand_counts[hand[0].GetHandType()]++;
        _outcomes[1].losses_hand_counts[hand[1].GetHandType()]++;
      }
    }
    else if (ret_compare == -1) {
      _outcomes[0].losses++;
      _outcomes[1].wins++;

      if (bVerbose) {
        _outcomes[0].losses_hand_counts[hand[0].GetHandType()]++;
        _outcomes[1].wins_hand_counts[hand[1].GetHandType()]++;
      }
    }
    else {
      _outcomes[0].ties++;
      _outcomes[1].ties++;

      if (bVerbose) {
        _outcomes[0].ties_hand_counts[hand[0].GetHandType()]++;
        _outcomes[1].ties_hand_counts[hand[1].GetHandType()]++;
      }
    }

    r++;

    if (m == num_remaining_cards - 5)
      break;
  }
}

// default constructor

HeadsUpFlop::HeadsUpFlop()
{
  _have_cards = false;
  _evaluated = false;
}

// copy constructor

HeadsUpFlop::HeadsUpFlop(const HeadsUpFlop& hut)
{
  int n;

  for (n = 0; n < NUM_HEADS_UP_FLOP_CARDS; n++) {
    _cards[n] = hut._cards[n];
  }

  _have_cards = hut._have_cards;
  _evaluated = hut._evaluated;
}

// assignment operator

HeadsUpFlop& HeadsUpFlop::operator=(const HeadsUpFlop& hut)
{
  int n;

  for (n = 0; n < NUM_HEADS_UP_FLOP_CARDS; n++) {
    _cards[n] = hut._cards[n];
  }

  _have_cards = hut._have_cards;
  _evaluated = hut._evaluated;

  return *this;
}

// destructor

HeadsUpFlop::~HeadsUpFlop()
{
}

HeadsUpFlop::HeadsUpFlop(int card1,int card2,int card3,int card4,int card5,int card6,int card7)
{
  _cards[0] = card1 % NUM_CARDS_IN_DECK;
  _cards[1] = card2 % NUM_CARDS_IN_DECK;
  _cards[2] = card3 % NUM_CARDS_IN_DECK;
  _cards[3] = card4 % NUM_CARDS_IN_DECK;
  _cards[4] = card5 % NUM_CARDS_IN_DECK;
  _cards[5] = card6 % NUM_CARDS_IN_DECK;
  _cards[6] = card7 % NUM_CARDS_IN_DECK;

  _have_cards = true;
  _evaluated = false;
}

void HeadsUpFlop::NewCards(int card1,int card2,int card3,int card4,int card5,int card6,int card7)
{
  _cards[0] = card1 % NUM_CARDS_IN_DECK;
  _cards[1] = card2 % NUM_CARDS_IN_DECK;
  _cards[2] = card3 % NUM_CARDS_IN_DECK;
  _cards[3] = card4 % NUM_CARDS_IN_DECK;
  _cards[4] = card5 % NUM_CARDS_IN_DECK;
  _cards[5] = card6 % NUM_CARDS_IN_DECK;
  _cards[6] = card7 % NUM_CARDS_IN_DECK;

  _have_cards = true;
  _evaluated = false;
}

struct outcomes * HeadsUpFlop::GetOutcomes()
{
  return _outcomes;
}

void HeadsUpFlop::Evaluate(bool bVerbose)
{
  int m;
  int n;
  int o;
  int p;
  int remaining_cards[NUM_REMAINING_HEADS_UP_FLOP_CARDS];
  HoldemPokerHand holdem_hand[2];
  PokerHand hand[2];
  int ret_compare;

  m = 0;

  for (n = 0; n < NUM_CARDS_IN_DECK; n++) {
    for (o = 0; o < NUM_HEADS_UP_FLOP_CARDS; o++) {
      if (n == _cards[o])
        break;
    }

    if (o == NUM_HEADS_UP_FLOP_CARDS)
      remaining_cards[m++] = n;
  }

  for (n = 0; n < 2; n++) {
    _outcomes[n].wins = 0;
    _outcomes[n].losses = 0;
    _outcomes[n].ties = 0;

    if (bVerbose) {
      for (m = 0; m < NUM_HAND_TYPES; m++) {
        _outcomes[n].wins_hand_counts[m] = 0;
        _outcomes[n].losses_hand_counts[m] = 0;
        _outcomes[n].ties_hand_counts[m] = 0;
      }
    }
  }

  for (o = 0; o < POKER_45_2_PERMUTATIONS; o++) {
    get_permutation_instance_two(NUM_REMAINING_HEADS_UP_FLOP_CARDS,&m,&n,o);

    holdem_hand[0].NewCards(_cards[0],_cards[1],
      _cards[4],_cards[5],_cards[6],
      remaining_cards[m],remaining_cards[n]);

    holdem_hand[1].NewCards(_cards[2],_cards[3],
      _cards[4],_cards[5],_cards[6],
      remaining_cards[m],remaining_cards[n]);

    for (p = 0; p < 2; p++)
      hand[p] = holdem_hand[p].BestPokerHand();

    ret_compare = hand[0].Compare(hand[1],0);

    if (ret_compare == 1) {
      _outcomes[0].wins++;
      _outcomes[1].losses++;

      if (bVerbose) {
        _outcomes[0].wins_hand_counts[hand[0].GetHandType()]++;
        _outcomes[1].losses_hand_counts[hand[1].GetHandType()]++;
      }
    }
    else if (ret_compare == -1) {
      _outcomes[0].losses++;
      _outcomes[1].wins++;

      if (bVerbose) {
        _outcomes[0].losses_hand_counts[hand[0].GetHandType()]++;
        _outcomes[1].wins_hand_counts[hand[1].GetHandType()]++;
      }
    }
    else {
      _outcomes[0].ties++;
      _outcomes[1].ties++;

      if (bVerbose) {
        _outcomes[0].ties_hand_counts[hand[0].GetHandType()]++;
        _outcomes[1].ties_hand_counts[hand[1].GetHandType()]++;
      }
    }
  }
}

// default constructor

HeadsUpTurn::HeadsUpTurn()
{
  _have_cards = false;
  _evaluated = false;
}

// copy constructor

HeadsUpTurn::HeadsUpTurn(const HeadsUpTurn& hut)
{
  int n;

  for (n = 0; n < NUM_HEADS_UP_TURN_CARDS; n++) {
    _cards[n] = hut._cards[n];
  }

  _have_cards = hut._have_cards;
  _evaluated = hut._evaluated;
}

// assignment operator

HeadsUpTurn& HeadsUpTurn::operator=(const HeadsUpTurn& hut)
{
  int n;

  for (n = 0; n < NUM_HEADS_UP_TURN_CARDS; n++) {
    _cards[n] = hut._cards[n];
  }

  _have_cards = hut._have_cards;
  _evaluated = hut._evaluated;

  return *this;
}

// destructor

HeadsUpTurn::~HeadsUpTurn()
{
}

HeadsUpTurn::HeadsUpTurn(int card1,int card2,int card3,int card4,int card5,int card6,int card7,int card8)
{
  _cards[0] = card1 % NUM_CARDS_IN_DECK;
  _cards[1] = card2 % NUM_CARDS_IN_DECK;
  _cards[2] = card3 % NUM_CARDS_IN_DECK;
  _cards[3] = card4 % NUM_CARDS_IN_DECK;
  _cards[4] = card5 % NUM_CARDS_IN_DECK;
  _cards[5] = card6 % NUM_CARDS_IN_DECK;
  _cards[6] = card7 % NUM_CARDS_IN_DECK;
  _cards[7] = card8 % NUM_CARDS_IN_DECK;

  _have_cards = true;
  _evaluated = false;
}

void HeadsUpTurn::NewCards(int card1,int card2,int card3,int card4,int card5,int card6,int card7,int card8)
{
  _cards[0] = card1 % NUM_CARDS_IN_DECK;
  _cards[1] = card2 % NUM_CARDS_IN_DECK;
  _cards[2] = card3 % NUM_CARDS_IN_DECK;
  _cards[3] = card4 % NUM_CARDS_IN_DECK;
  _cards[4] = card5 % NUM_CARDS_IN_DECK;
  _cards[5] = card6 % NUM_CARDS_IN_DECK;
  _cards[6] = card7 % NUM_CARDS_IN_DECK;
  _cards[7] = card8 % NUM_CARDS_IN_DECK;

  _have_cards = true;
  _evaluated = false;
}

struct outcomes * HeadsUpTurn::GetOutcomes()
{
  return _outcomes;
}

void HeadsUpTurn::Evaluate(bool bVerbose)
{
  int m;
  int n;
  int o;
  int p;
  int remaining_cards[NUM_REMAINING_HEADS_UP_TURN_CARDS];
  HoldemPokerHand holdem_hand[2];
  PokerHand hand[2];
  int ret_compare;

  m = 0;

  for (n = 0; n < NUM_CARDS_IN_DECK; n++) {
    for (o = 0; o < NUM_HEADS_UP_TURN_CARDS; o++) {
      if (n == _cards[o])
        break;
    }

    if (o == NUM_HEADS_UP_TURN_CARDS)
      remaining_cards[m++] = n;
  }

  for (n = 0; n < 2; n++) {
    _outcomes[n].wins = 0;
    _outcomes[n].losses = 0;
    _outcomes[n].ties = 0;

    if (bVerbose) {
      for (m = 0; m < NUM_HAND_TYPES; m++) {
        _outcomes[n].wins_hand_counts[m] = 0;
        _outcomes[n].losses_hand_counts[m] = 0;
        _outcomes[n].ties_hand_counts[m] = 0;
      }
    }
  }

  for (m = 0; m < NUM_REMAINING_HEADS_UP_TURN_CARDS; m++) {
    holdem_hand[0].NewCards(_cards[0],_cards[1],
      _cards[4],_cards[5],_cards[6],_cards[7],
      remaining_cards[m]);

    holdem_hand[1].NewCards(_cards[2],_cards[3],
      _cards[4],_cards[5],_cards[6],_cards[7],
      remaining_cards[m]);

    for (p = 0; p < 2; p++)
      hand[p] = holdem_hand[p].BestPokerHand();

    ret_compare = hand[0].Compare(hand[1],0);

    if (ret_compare == 1) {
      _outcomes[0].wins++;
      _outcomes[1].losses++;

      if (bVerbose) {
        _outcomes[0].wins_hand_counts[hand[0].GetHandType()]++;
        _outcomes[1].losses_hand_counts[hand[1].GetHandType()]++;
      }
    }
    else if (ret_compare == -1) {
      _outcomes[0].losses++;
      _outcomes[1].wins++;

      if (bVerbose) {
        _outcomes[0].losses_hand_counts[hand[0].GetHandType()]++;
        _outcomes[1].wins_hand_counts[hand[1].GetHandType()]++;
      }
    }
    else {
      _outcomes[0].ties++;
      _outcomes[1].ties++;

      if (bVerbose) {
        _outcomes[0].ties_hand_counts[hand[0].GetHandType()]++;
        _outcomes[1].ties_hand_counts[hand[1].GetHandType()]++;
      }
    }
  }
}

void get_permutation_instance_two(
  int set_size,
  int *m,int *n,
  int instance_ix
)
{
  if (instance_ix)
    goto after_return_point;

  for (*m = 0; *m < set_size - 2 + 1; (*m)++) {
    for (*n = *m + 1; *n < set_size - 2 + 2; (*n)++) {
      return;

      after_return_point:
      ;
    }
  }
}

void get_permutation_instance_three(
  int set_size,
  int *m,int *n,int *o,
  int instance_ix
)
{
  if (instance_ix)
    goto after_return_point;

  for (*m = 0; *m < set_size - 3 + 1; (*m)++) {
    for (*n = *m + 1; *n < set_size - 3 + 2; (*n)++) {
      for (*o = *n + 1; *o < set_size - 3 + 3; (*o)++) {
        return;

        after_return_point:
        ;
      }
    }
  }
}

void get_permutation_instance_four(
  int set_size,
  int *m,int *n,int *o,int *p,
  int instance_ix
)
{
  if (instance_ix)
    goto after_return_point;

  for (*m = 0; *m < set_size - 4 + 1; (*m)++) {
    for (*n = *m + 1; *n < set_size - 4 + 2; (*n)++) {
      for (*o = *n + 1; *o < set_size - 4 + 3; (*o)++) {
        for (*p = *o + 1; *p < set_size - 4 + 4; (*p)++) {
          return;

          after_return_point:
          ;
        }
      }
    }
  }
}

void get_permutation_instance_five(
  int set_size,
  int *m,int *n,int *o,int *p,int *q,
  int instance_ix
)
{
  if (instance_ix)
    goto after_return_point;

  for (*m = 0; *m < set_size - 5 + 1; (*m)++) {
    for (*n = *m + 1; *n < set_size - 5 + 2; (*n)++) {
      for (*o = *n + 1; *o < set_size - 5 + 3; (*o)++) {
        for (*p = *o + 1; *p < set_size - 5 + 4; (*p)++) {
          for (*q = *p + 1; *q < set_size - 5 + 5; (*q)++) {
            return;

            after_return_point:
            ;
          }
        }
      }
    }
  }
}

void get_permutation_instance_seven(
  int set_size,
  int *m,int *n,int *o,int *p,int *q,int *r,int *s,
  int instance_ix
)
{
  if (instance_ix)
    goto after_return_point;

  for (*m = 0; *m < set_size - 7 + 1; (*m)++) {
    for (*n = *m + 1; *n < set_size - 7 + 2; (*n)++) {
      for (*o = *n + 1; *o < set_size - 7 + 3; (*o)++) {
        for (*p = *o + 1; *p < set_size - 7 + 4; (*p)++) {
          for (*q = *p + 1; *q < set_size - 7 + 5; (*q)++) {
            for (*r = *q + 1; *r < set_size - 7 + 6; (*r)++) {
              for (*s = *r + 1; *s < set_size - 7 + 7; (*s)++) {
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

int card_value_from_card_string(char *card_string,int *card_value)
{
  int rank;
  int suit;

  if (get_rank_index(card_string[0],&rank))
    return 1;

  if (get_suit_index(card_string[1],&suit))
    return 2;

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

int get_rank_index(char rank_char,int *rank_index_ptr)
{
  int rank;

  for (rank = 0; rank < NUM_CARDS_IN_SUIT; rank++) {
    if (rank_char == rank_chars[rank])
      break;
  }

  if (rank == NUM_CARDS_IN_SUIT)
    return 1;

  *rank_index_ptr = rank;

  return 0;
}

int get_suit_index(char suit_char,int *suit_index_ptr)
{
  int suit;

  for (suit = 0; suit < NUM_SUITS; suit++) {
    if (suit_char == suit_chars[suit])
      break;
  }

  if (suit == NUM_SUITS)
    return 1;

  *suit_index_ptr = suit;

  return 0;
}

HandType get_hand_type(char *hand_type_str)
{
  int hand_type;

  for (hand_type = HIGH_CARD; hand_type < NUM_HAND_TYPES; hand_type++) {
    if (!strcmp(plain_hand_types[hand_type],hand_type_str))
      break;
  }

  if (hand_type == NUM_HAND_TYPES)
    hand_type = HIGH_CARD;

  return (HandType)hand_type;
}

bool four_to_a_flush(int *cards)
{
  int m;
  int n;
  int suits[NUM_HOLE_CARDS_IN_HOLDEM_HAND+NUM_CARDS_IN_FLOP+1];
  int suit_count;

  for (n = 0; n < NUM_HOLE_CARDS_IN_HOLDEM_HAND+NUM_CARDS_IN_FLOP+1; n++)
    suits[n] = suit_of(cards[n]);

  for (m = 0; m < NUM_SUITS; m++) {
    suit_count = 0;

    for (n = 0; n < NUM_HOLE_CARDS_IN_HOLDEM_HAND+NUM_CARDS_IN_FLOP+1; n++) {
      if (suits[n] == m)
        suit_count++;
    }

    if (suit_count == 4)
      return true;
  }

  return false;
}

void init_plain_hand_type_lens()
{
  int n;

  for (n = 0; n < NUM_HAND_TYPES; n++)
    plain_hand_type_lens[n] = strlen(plain_hand_types[n]);
}

int read_hands_and_types(
  char *hands_and_types_filename,
  struct hand_and_type **hands_and_types
)
{
  size_t malloc_size;
  int fhndl;
  int bytes_read;

  malloc_size = sizeof (struct hand_and_type) * POKER_52_5_PERMUTATIONS;
  *hands_and_types = (struct hand_and_type *)malloc(malloc_size);

  if (*hands_and_types == NULL)
    return 1;

  if ((fhndl = open(hands_and_types_filename,O_BINARY | O_RDONLY,0)) == -1) {
    free(*hands_and_types);
    return 2;
  }

  bytes_read = read(fhndl,*hands_and_types,malloc_size);

  if (bytes_read != malloc_size) {
    close(fhndl);
    free(*hands_and_types);
    return 3;
  }

  close(fhndl);

  return 0;
}

int compare_key(const void *vkey,const void *velem)
{
  int n;
  hand *key;
  struct hand_and_type *elem;

  compare_key_called++;
  key = (hand *)vkey;
  elem = (struct hand_and_type *)velem;

  for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
    if (key->cards[n] < (int)elem->cards[n])
      return -1;

    if (key->cards[n] > (int)elem->cards[n])
      return 1;
  }

  return 0;
}

int find_hand(
  hand *in_hand,
  struct hand_and_type *hands_and_types,
  int debug_level,
  struct hand_and_type **out_hand
)
{
  int m;
  int n;
  int p;
  struct hand_and_type *found;
  int ix;
  int lo;
  int hi;
  int compare;

  switch (debug_level) {
    case 2:
      for (p = 0; p < POKER_52_5_PERMUTATIONS; p++) {
        for (m = 0; m < NUM_CARDS_IN_HAND; m++) {
          if (hands_and_types[p].cards[m] != (char)in_hand->cards[m])
            break;
        }

        if (m == NUM_CARDS_IN_HAND) {
          *out_hand = &hands_and_types[p];
          return 1;
        }
      }

      break;

    case 3:
      lo = 0;
      hi = POKER_52_5_PERMUTATIONS - 1;

      ix = (hi - lo + 1) / 2;

      for ( ; ; ) {
        compare = 0;

        for (n = 0; n < NUM_CARDS_IN_HAND; n++) {
          if (in_hand->cards[n] < hands_and_types[ix].cards[n]) {
            compare = -1;
            break;
          }

          if (in_hand->cards[n] > hands_and_types[ix].cards[n]) {
            compare = 1;
            break;
          }
        }

        if (!compare) {
          *out_hand = &hands_and_types[ix];
          return 1;
        }

        if (compare == -1) {
          if (ix == lo)
            break;

          hi = ix;
          ix = lo + (hi - lo + 1) / 2;
        }
        else {
          if (ix == hi)
            break;

          lo = ix;
          ix = lo + (hi - lo + 1) / 2;
        }
      }

      break;

    default:
      found = (struct hand_and_type *)bsearch(in_hand,hands_and_types,POKER_52_5_PERMUTATIONS,
        sizeof (struct hand_and_type),compare_key);

      if (found) {
        *out_hand = found;
        return 1;
      }

     break;
  }

  return 0;
}

int get_index_of_hand(int rank_ix1,int suit_ix1,int rank_ix2,int suit_ix2)
{
  int n;
  int work;
  int index;
  int num_other_cards;

  if (rank_ix1 == rank_ix2)
    return rank_ix1;

  if (suit_ix1 == suit_ix2)
    index = NUM_CARDS_IN_SUIT;
  else
    index = NUM_CARDS_IN_SUIT + NUM_SUITED_NONPAIRS;

  if (rank_ix1 > rank_ix2) {
    work = rank_ix1;
    rank_ix1 = rank_ix2;
    rank_ix2 = work;
  }

  num_other_cards = NUM_CARDS_IN_SUIT - 1;

  for (n = 0; n < rank_ix1; n++) {
    index += num_other_cards;
    num_other_cards--;
  }

  index += rank_ix2 - rank_ix1 - 1;

  return index;
}

int index_of_hand(char *hand,int *index_pt)
{
  int n;
  int retval;
  int rank_ix[2];
  int suit_ix[2];

  for (n = 0; n < 2; n++) {
    retval = get_rank_index(hand[3*n],&rank_ix[n]);

    if (retval)
      return 1;

    retval = get_suit_index(hand[3*n+1],&suit_ix[n]);

    if (retval)
      return 2;
  }

  *index_pt = get_index_of_hand(rank_ix[0],suit_ix[0],rank_ix[1],suit_ix[1]);

  return 0;
}

int abbrev_index_of_hand(char *hand,int *index_pt)
{
  char hand_abbrev[4];

  hand_abbrev[0] = hand[0];
  hand_abbrev[1] = hand[3];

  if (hand[0] == hand[3])
    hand_abbrev[2] = 0;
  else {
    if (hand[1] == hand[4])
      hand_abbrev[2] = 's';
    else
      hand_abbrev[2] = 'o';

     hand_abbrev[3] = 0;
  }

  return index_of_hand_abbrev(hand_abbrev,index_pt);
}

int index_of_hand_abbrev(char *hand_abbrev,int *index_pt)
{
  int n;
  int retval;
  int rank_ix[2];
  int suit_ix[2];
  int len;

  for (n = 0; n < 2; n++) {
    retval = get_rank_index(hand_abbrev[n],&rank_ix[n]);

    if (retval)
      return 1;
  }

  len = strlen(hand_abbrev);

  if (len == 2) {
    if (rank_ix[0] != rank_ix[1])
      return 2;

    suit_ix[0] = 0;
    suit_ix[1] = 1;
  }
  else if (len == 3) {
    if (hand_abbrev[2] == 's') {
      suit_ix[0] = 0;
      suit_ix[1] = 0;
    }
    else if (hand_abbrev[2] == 'o') {
      suit_ix[0] = 0;
      suit_ix[1] = 1;
    }
    else
      return 3;
  }
  else
    return 4;

  *index_pt = get_index_of_hand(rank_ix[0],suit_ix[0],rank_ix[1],suit_ix[1]);

  return 0;
}

int get_52_2_index_of_hand(int *cards)
{
  int n;
  int index;
  int num_other_cards;
  int temp;

  if (cards[0] > cards[1]) {
    temp = cards[0];
    cards[0] = cards[1];
    cards[1] = temp;
  }

  index = 0;
  num_other_cards = NUM_CARDS_IN_DECK - 1;

  for (n = 0; n < cards[0]; n++) {
    index += num_other_cards;
    num_other_cards--;
  }

  index += cards[1] - cards[0] - 1;

  return index;
}

int get_52_2_index_of_hole_cards(char *hole_cards,int *index)
{
  int retval;
  int cards[2];

  retval = card_value_from_card_string(&hole_cards[0],&cards[0]);

  if (retval)
    return retval;

  retval = card_value_from_card_string(&hole_cards[3],&cards[1]);

  if (retval)
    return retval;

  *index = get_52_2_index_of_hand(cards);

  return 0;
}

int get_52_2_index_of_hand2(int *cards)
{
  if (cards[0] < cards[1])
    return cards[0] * (NUM_CARDS_IN_DECK - 1) + cards[1] - 1;
  else
    return cards[0] * (NUM_CARDS_IN_DECK - 1) + cards[1];
}

int get_52_2_index_of_hole_cards2(char *hole_cards,int *index)
{
  int retval;
  int cards[2];

  retval = card_value_from_card_string(&hole_cards[0],&cards[0]);

  if (retval)
    return retval;

  retval = card_value_from_card_string(&hole_cards[3],&cards[1]);

  if (retval)
    return retval;

  *index = get_52_2_index_of_hand2(cards);

  return 0;
}

void get_abbrev(char *line,char *abbrev)
{
  int rank_ix1;
  int rank_ix2;

  for (rank_ix1 = 0; rank_ix1 < NUM_CARDS_IN_SUIT; rank_ix1++) {
    if (line[0] == rank_chars[rank_ix1])
      break;
  }

  if (rank_ix1 == NUM_CARDS_IN_SUIT)
    rank_ix1 = 0;

  for (rank_ix2 = 0; rank_ix2 < NUM_CARDS_IN_SUIT; rank_ix2++) {
    if (line[3] == rank_chars[rank_ix2])
      break;
  }

  if (rank_ix2 == NUM_CARDS_IN_SUIT)
    rank_ix2 = 0;

  if (rank_ix1 >= rank_ix2) {
    abbrev[0] = line[0];
    abbrev[1] = line[3];
  }
  else {
    abbrev[0] = line[3];
    abbrev[1] = line[0];
  }

  if (abbrev[0] == abbrev[1])
    abbrev[2] = ' ';
  else {
    if (line[1] == line[4])
      abbrev[2] = 's';
    else
      abbrev[2] = 'o';
  }
}

bool is_premium_hand(char *abbrev)
{
  int m;

  for (m = 0; m < NUM_PREMIUM_HANDS; m++) {
    if (!strcmp(abbrev,premium_hand_abbrevs[m]))
      break;
  }

  return (m < NUM_PREMIUM_HANDS);
}
