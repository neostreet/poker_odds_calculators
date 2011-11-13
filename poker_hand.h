enum Suit {
  CLUBS,
  DIAMONDS,
  HEARTS,
  SPADES,
  NUM_SUITS
};

enum Rank {
  TWO,
  THREE,
  FOUR,
  FIVE,
  SIX,
  SEVEN,
  EIGHT,
  NINE,
  TEN,
  JACK,
  QUEEN,
  KING,
  ACE,
  NUM_CARDS_IN_SUIT
};

#define NUM_CARDS_IN_DECK (NUM_SUITS * NUM_CARDS_IN_SUIT)

#define suit_of(card) (card / NUM_CARDS_IN_SUIT)
#define rank_of(card) (card % NUM_CARDS_IN_SUIT)

#ifdef MAIN_MODULE
char suit_chars[] = "cdhs";
char rank_chars[] = "23456789TJQKA";
#else
extern char suit_chars[];
extern char rank_chars[];
#endif

#define NUM_CARDS_IN_HAND 5

#define POKER_7_5_PERMUTATIONS         21
#define POKER_40_1_PERMUTATIONS        40
#define POKER_42_1_PERMUTATIONS        42
#define POKER_44_1_PERMUTATIONS        44
#define POKER_41_2_PERMUTATIONS       820
#define POKER_43_2_PERMUTATIONS       903
#define POKER_45_2_PERMUTATIONS       990
#define POKER_46_2_PERMUTATIONS      1035
#define POKER_47_2_PERMUTATIONS      1081
#define POKER_52_2_PERMUTATIONS      1326
#define POKER_52_3_PERMUTATIONS     22100
#define POKER_44_5_PERMUTATIONS   1086008
#define POKER_46_5_PERMUTATIONS   1370754
#define POKER_48_5_PERMUTATIONS   1712304
#define POKER_52_5_PERMUTATIONS   2598960
#define POKER_52_7_PERMUTATIONS 133784560

#define NUM_HOLE_CARDS_IN_HOLDEM_HAND 2
#define NUM_CARDS_IN_FLOP 3
#define NUM_CARDS_AFTER_FLOP 2
#define NUM_CARDS_IN_HOLDEM_POOL (NUM_HOLE_CARDS_IN_HOLDEM_HAND + NUM_CARDS_IN_FLOP + NUM_CARDS_AFTER_FLOP)
#define NUM_HOLE_CARDS_IN_OMAHA_HAND 4
#define NUM_CARDS_AFTER_TURN 2
#define NUM_CARDS_AFTER_DEAL (NUM_CARDS_IN_FLOP + NUM_CARDS_AFTER_FLOP)

enum HandType {
  HIGH_CARD,
  ONE_PAIR,
  TWO_PAIRS,
  THREE_OF_A_KIND,
  STRAIGHT,
  FLUSH,
  FULL_HOUSE,
  FOUR_OF_A_KIND,
  STRAIGHT_FLUSH,
  ROYAL_FLUSH,
  NUM_HAND_TYPES
};

enum FlopType {
  DRY,
  WET,
  NUM_FLOP_TYPES
};

#ifdef MAIN_MODULE
char *hand_type_abbrevs[] = {
  "hc",
  "1p",
  "2p",
  "3k",
  "st",
  "fl",
  "fh",
  "4k",
  "sf",
  "rf"
};
char *flop_type_abbrevs[] = {
  "dry",
  "wet"
};
int num_evaluations;
int num_unique_evaluations;
int num_comparisons;
int num_holdem_best_poker_hand_comparisons;
#else
extern char *hand_type_abbrevs[];
extern char *flop_type_abbrevs[];
extern int num_evaluations;
extern int num_unique_evaluations;
extern int num_comparisons;
extern int num_holdem_best_poker_hand_comparisons;
#endif

class PokerHand {
  public:

  // default constructor
  PokerHand();
  // copy constructor
  PokerHand(const PokerHand&);
  // assignment operator
  PokerHand& operator=(const PokerHand&);
  // destructor
  ~PokerHand();

  PokerHand(int card1,int card2,int card3,int card4, int card5);
  void NewCards(int card1,int card2,int card3,int card4, int card5);

  int GetRank(int card_ix);
  void Sort();
  HandType Evaluate();
  int Evaluated();
  int RoyalFlush();
  int StraightFlush();
  int FourOfAKind();
  int FullHouse();
  int Flush();
  int Straight();
  int ThreeOfAKind();
  int TwoPairs();
  int OnePair();

  HandType GetHandType();

  int Compare(PokerHand& compare_hand,int in_holdem_best_poker_hand);

  void print(ostream& out) const;
  void Verbose();
  void Terse();

  private:

  int _card[NUM_CARDS_IN_HAND];
  int _suit[NUM_CARDS_IN_HAND];
  int _rank[NUM_CARDS_IN_HAND];
  int _num_cards_with_same_rank[NUM_CARDS_IN_HAND];
  int _order[NUM_CARDS_IN_HAND];

  int _have_cards;
  int _hand_sorted;
  int _hand_evaluated;
  int _verbose;

  HandType _hand_type;
};

ostream& operator<<(ostream& out,const PokerHand& hand);

class HoldemPokerHand {
  public:

  // default constructor
  HoldemPokerHand();
  // copy constructor
  HoldemPokerHand(const HoldemPokerHand&);
  // assignment operator
  HoldemPokerHand& operator=(const HoldemPokerHand&);
  // destructor
  ~HoldemPokerHand();

  HoldemPokerHand(int card1,int card2,int card3,int card4,int card5,int card6,int card7);
  void NewCards(int card1,int card2,int card3,int card4,int card5,int card6,int card7);

  PokerHand& BestPokerHand();

  void print(ostream& out) const;

  private:

  int _card[NUM_CARDS_IN_HOLDEM_POOL];
  PokerHand _best_poker_hand;

  int _have_cards;
};

ostream& operator<<(ostream& out,const HoldemPokerHand& board_hand);

class Flop {
  public:

  // default constructor
  Flop();
  // copy constructor
  Flop(const Flop&);
  // assignment operator
  Flop& operator=(const Flop&);
  // destructor
  ~Flop();

  Flop(int card1,int card2,int card3);
  void NewCards(int card1,int card2,int card3);

  int GetCard(int card_ix);
  int GetRank(int card_ix);
  void Sort();
  FlopType Evaluate();
  int Evaluated();
  int Flush();
  int Straight();
  int PairOrBetter();

  FlopType GetFlopType();

  void print(ostream& out) const;
  void Verbose();
  void Terse();

  private:

  int _card[NUM_CARDS_IN_FLOP];
  int _suit[NUM_CARDS_IN_FLOP];
  int _rank[NUM_CARDS_IN_FLOP];
  int _num_cards_with_same_rank[NUM_CARDS_IN_FLOP];
  int _order[NUM_CARDS_IN_FLOP];

  int _have_cards;
  int _flop_sorted;
  int _flop_evaluated;
  int _verbose;

  FlopType _flop_type;
};

ostream& operator<<(ostream& out,const Flop& hand);

int card_value_from_card_string(char *card_string,int *card_value);
int card_string_from_card_value(int card_value,char *card_string);
