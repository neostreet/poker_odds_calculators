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

#define NUM_CARDS_IN_POOL 7
#define NUM_CARDS_IN_HAND 5

#define POKER_7_5_PERMUTATIONS       21
#define POKER_40_1_PERMUTATIONS      40
#define POKER_44_1_PERMUTATIONS      44
#define POKER_41_2_PERMUTATIONS     820
#define POKER_45_2_PERMUTATIONS     990
#define POKER_47_2_PERMUTATIONS    1081
#define POKER_52_2_PERMUTATIONS    1326
#define POKER_48_5_PERMUTATIONS 1712304

#define NUM_HOLE_CARDS_IN_HOLDEM_HAND 2
#define NUM_CARDS_IN_FLOP 3
#define NUM_CARDS_AFTER_FLOP 2
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
int num_evaluations;
int num_unique_evaluations;
int num_comparisons;
#else
extern char *hand_type_abbrevs[];
extern int num_evaluations;
extern int num_unique_evaluations;
extern int num_comparisons;
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

  int GetRank(int card);
  void Sort();
  HandType Evaluate();
  int RoyalFlush();
  int StraightFlush();
  int FourOfAKind();
  int FullHouse();
  int Flush();
  int Straight();
  int ThreeOfAKind();
  int TwoPairs();
  int OnePair();
  void SetNoWheel(int no_wheel);
  int GetNoWheel();

  HandType GetHandType();

  int Compare(PokerHand& compare_hand);

  void print(ostream& out) const;

  private:

  int _card[NUM_CARDS_IN_HAND];
  int _suit[NUM_CARDS_IN_HAND];
  int _rank[NUM_CARDS_IN_HAND];
  int _sort_val[NUM_CARDS_IN_HAND];
  int _order[NUM_CARDS_IN_HAND];

  int _have_cards;
  int _hand_sorted;
  int _hand_evaluated;
  int _no_wheel;

  HandType _hand_type;
};

ostream& operator<<(ostream& out,const PokerHand& hand);

class BoardPokerHand {
  public:

  // default constructor
  BoardPokerHand();
  // copy constructor
  BoardPokerHand(const BoardPokerHand&);
  // assignment operator
  BoardPokerHand& operator=(const BoardPokerHand&);
  // destructor
  ~BoardPokerHand();

  BoardPokerHand(int card1,int card2,int card3,int card4,int card5,int card6,int card7);
  void NewCards(int card1,int card2,int card3,int card4,int card5,int card6,int card7);

  void SetNoWheel(int no_wheel);
  int GetNoWheel();

  PokerHand& BestPokerHand();
  PokerHand& BestPokerHandWheel();
  PokerHand& BestPokerHandNoWheel();

  void print(ostream& out) const;

  private:

  int _card[NUM_CARDS_IN_POOL];
  PokerHand _best_poker_hand;

  int _have_cards;
  int _no_wheel;
};

ostream& operator<<(ostream& out,const BoardPokerHand& board_hand);

int card_value_from_card_string(char *card_string,int *card_value);
int card_string_from_card_value(int card_value,char *card_string);
