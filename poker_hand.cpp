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

  return *this;
}

// destructor

PokerHand::~PokerHand()
{
}

PokerHand::PokerHand(int card1,int card2,int card3,int card4, int card5)
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

HandType PokerHand::Evaluate()
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
  else if (FullHouse())
    _hand_type = FULL_HOUSE;
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

int compare1(const void *elem1,const void *elem2)
{
  int int1;
  int int2;

  int1 = *(int *)elem1;
  int2 = *(int *)elem2;

  return int1 - int2;
}

HandType PokerHand::EvaluateQuick(struct hand_and_type *hands_and_types)
{
  int retval;
  hand sorted_hand;
  struct hand_and_type *found;

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

  sorted_hand = _card;
  qsort(&sorted_hand.cards[0],NUM_CARDS_IN_HAND,sizeof (int),compare1);

  retval = find_hand(&sorted_hand,hands_and_types,true,&found);

  if (!retval)
    return HIGH_CARD;

  _hand_type = (HandType)found->hand_type;
  _quick_ix = found->quick_ix;
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

  // first, handle the special case of a wheel (A, 2, 3, 4, 5)

  if ((_rank.cards[_order.cards[0]] == ACE) &&
      (_rank.cards[_order.cards[1]] == FIVE) &&
      (_rank.cards[_order.cards[2]] == FOUR) &&
      (_rank.cards[_order.cards[3]] == THREE) &&
      (_rank.cards[_order.cards[4]] == TWO))
    return 1;

  for (n = 1; n < NUM_CARDS_IN_HAND; n++) {
    if (_rank.cards[_order.cards[n-1]] != _rank.cards[_order.cards[n]] + 1)
      break;
  }

  if (n < NUM_CARDS_IN_HAND)
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

  return 0;
}

bool PokerHand::OnePair()
{
  if (_num_cards_with_same_rank.cards[_order.cards[0]] == 2)
    return 1;

  return 0;
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

  if (!_hand_evaluated)
    hand_type = Evaluate();
  else
    hand_type = _hand_type;

  if (!compare_hand.Evaluated())
    compare_hand_type = compare_hand.Evaluate();
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

int PokerHand::CompareQuick(PokerHand& compare_hand,struct hand_and_type *hands_and_types)
{
  int compare_quick_ix;

  if (!_hand_evaluated)
    EvaluateQuick(hands_and_types);

  if (!compare_hand.Evaluated()) {
    compare_hand.EvaluateQuick(hands_and_types);
    compare_quick_ix = compare_hand.GetQuickIx();
  }

  if (_quick_ix > compare_quick_ix)
    return 1;

  if (_quick_ix < compare_quick_ix)
    return -1;

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

// default constructor

HoldemPokerHand::HoldemPokerHand()
{
  _have_cards = false;
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

  for (r = 0; r < POKER_7_5_PERMUTATIONS; r++) {
    get_permutation_instance_five(
      NUM_CARDS_IN_HOLDEM_POOL,
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

PokerHand& HoldemPokerHand::BestPokerHandQuick(struct hand_and_type *hands_and_types)
{
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  PokerHand hand;
  int ret_compare;

  for (r = 0; r < POKER_7_5_PERMUTATIONS; r++) {
    get_permutation_instance_five(
      NUM_CARDS_IN_HOLDEM_POOL,
      &m,&n,&o,&p,&q,r);

    hand.NewCards(_card.cards[m],_card.cards[n],_card.cards[o],_card.cards[p],_card.cards[q]);

    if (!r)
      _best_poker_hand = hand;
    else {
      ret_compare = hand.CompareQuick(_best_poker_hand,hands_and_types);

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

// default constructor

HoldemTurnHand::HoldemTurnHand()
{
  _have_cards = false;
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

// default constructor

Flop::Flop()
{
  _have_cards = false;
  _flop_sorted = false;
  _flop_evaluated = false;
  _verbose = false;
  _plain = false;
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

PokerHand& OmahaPokerHand::BestPokerHand(bool bDebug)
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

      if (bDebug) {
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

      if (bDebug)
        cout << hand << endl;

      if (!o && !s)
        _best_poker_hand = hand;
      else {
        ret_compare = hand.Compare(_best_poker_hand,1);

        if (ret_compare == 1) {
          _best_poker_hand = hand;

          if (bDebug)
            cout << "new best poker hand" << endl;
        }
      }
    }
  }

  return _best_poker_hand;
}

PokerHand& OmahaPokerHand::BestLowPokerHand(bool bDebug)
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

      if (bDebug) {
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

      if (bDebug)
        cout << hand << endl;

      if (!o && !s)
        _best_poker_hand = hand;
      else {
        ret_compare = hand.CompareLow(_best_poker_hand,1);

        if (ret_compare == 1) {
          _best_poker_hand = hand;

          if (bDebug)
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
  int fhndl;
  int bytes_read;

  *hands_and_types = (struct hand_and_type *)malloc(sizeof (struct hand_and_type) * POKER_52_5_PERMUTATIONS);

  if (*hands_and_types == NULL)
    return 1;

  if ((fhndl = open(hands_and_types_filename,O_BINARY | O_RDONLY,0)) == -1) {
    free(*hands_and_types);
    return 2;
  }

  bytes_read = read(fhndl,*hands_and_types,sizeof (struct hand_and_type) * POKER_52_5_PERMUTATIONS);

  if (bytes_read != sizeof (struct hand_and_type) * POKER_52_5_PERMUTATIONS) {
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
  bool bBsearch,
  struct hand_and_type **out_hand
)
{
  int m;
  int p;
  struct hand_and_type *found;

  if (!bBsearch) {
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
  }
  else {
    found = (struct hand_and_type *)bsearch(in_hand,hands_and_types,POKER_52_5_PERMUTATIONS,
      sizeof (struct hand_and_type),compare_key);

    if (found) {
      *out_hand = found;
      return 1;
    }
  }

  return 0;
}
