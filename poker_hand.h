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

#define NUM_CARDS_IN_DECK (NUM_SUITS * NUM_CARDS_IN_SUIT)
#define NUM_CARDS_IN_SHORT_DECK 36

#define suit_of(card) (card / NUM_CARDS_IN_SUIT)
#define rank_of(card) (card % NUM_CARDS_IN_SUIT)

#define NUM_SUITED_NONPAIRS ((NUM_CARDS_IN_SUIT * (NUM_CARDS_IN_SUIT - 1)) / 2)
#define NUM_NONSUITED_NONPAIRS NUM_SUITED_NONPAIRS

#define NUM_HEADS_UP_CARDS 4
#define MAX_HEADS_UP_BURNT_CARDS 14
#define NUM_REMAINING_HEADS_UP_CARDS (NUM_CARDS_IN_DECK - NUM_HEADS_UP_CARDS)
#define NUM_HEADS_UP_FLOP_CARDS 7
#define NUM_REMAINING_HEADS_UP_FLOP_CARDS (NUM_CARDS_IN_DECK - NUM_HEADS_UP_FLOP_CARDS)
#define NUM_HEADS_UP_TURN_CARDS 8
#define NUM_REMAINING_HEADS_UP_TURN_CARDS (NUM_CARDS_IN_DECK - NUM_HEADS_UP_TURN_CARDS)

#define PAIR_PERIODICITY               221.0
#define ANY_PAIR_PERIODICITY            17.0

#define NUM_POCKS_PER_DENOM 6
#define NUM_POCKS_IN_DECK (NUM_POCKS_PER_DENOM * NUM_CARDS_IN_SUIT)

#define NUM_PREMIUM_HANDS_IN_DECK 60

#ifdef MAIN_MODULE
char suit_chars[] = "cdhs";
char rank_chars[] = "23456789TJQKA";

char *plain_hand_types[] = {
  "high card",
  "one pair",
  "two pair",
  "three of a kind",
  "straight",
  "flush",
  "full house",
  "four of a kind",
  "straight flush",
  "royal flush"
};
int plain_hand_type_lens[NUM_HAND_TYPES];

char *hand_abbrevs[] = {
  "22",
  "33",
  "44",
  "55",
  "66",
  "77",
  "88",
  "99",
  "TT",
  "JJ",
  "QQ",
  "KK",
  "AA",
  "32s",
  "42s",
  "52s",
  "62s",
  "72s",
  "82s",
  "92s",
  "T2s",
  "J2s",
  "Q2s",
  "K2s",
  "A2s",
  "43s",
  "53s",
  "63s",
  "73s",
  "83s",
  "93s",
  "T3s",
  "J3s",
  "Q3s",
  "K3s",
  "A3s",
  "54s",
  "64s",
  "74s",
  "84s",
  "94s",
  "T4s",
  "J4s",
  "Q4s",
  "K4s",
  "A4s",
  "65s",
  "75s",
  "85s",
  "95s",
  "T5s",
  "J5s",
  "Q5s",
  "K5s",
  "A5s",
  "76s",
  "86s",
  "96s",
  "T6s",
  "J6s",
  "Q6s",
  "K6s",
  "A6s",
  "87s",
  "97s",
  "T7s",
  "J7s",
  "Q7s",
  "K7s",
  "A7s",
  "98s",
  "T8s",
  "J8s",
  "Q8s",
  "K8s",
  "A8s",
  "T9s",
  "J9s",
  "Q9s",
  "K9s",
  "A9s",
  "JTs",
  "QTs",
  "KTs",
  "ATs",
  "QJs",
  "KJs",
  "AJs",
  "KQs",
  "AQs",
  "AKs",
  "32o",
  "42o",
  "52o",
  "62o",
  "72o",
  "82o",
  "92o",
  "T2o",
  "J2o",
  "Q2o",
  "K2o",
  "A2o",
  "43o",
  "53o",
  "63o",
  "73o",
  "83o",
  "93o",
  "T3o",
  "J3o",
  "Q3o",
  "K3o",
  "A3o",
  "54o",
  "64o",
  "74o",
  "84o",
  "94o",
  "T4o",
  "J4o",
  "Q4o",
  "K4o",
  "A4o",
  "65o",
  "75o",
  "85o",
  "95o",
  "T5o",
  "J5o",
  "Q5o",
  "K5o",
  "A5o",
  "76o",
  "86o",
  "96o",
  "T6o",
  "J6o",
  "Q6o",
  "K6o",
  "A6o",
  "87o",
  "97o",
  "T7o",
  "J7o",
  "Q7o",
  "K7o",
  "A7o",
  "98o",
  "T8o",
  "J8o",
  "Q8o",
  "K8o",
  "A8o",
  "T9o",
  "J9o",
  "Q9o",
  "K9o",
  "A9o",
  "JTo",
  "QTo",
  "KTo",
  "ATo",
  "QJo",
  "KJo",
  "AJo",
  "KQo",
  "AQo",
  "AKo"
};

char *premium_hand_abbrevs[] = {
  "AA",
  "KK",
  "QQ",
  "JJ",
  "AKs",
  "TT",
  "AKo",
  "AQs",
  "99",
  "AJs"
};
#define NUM_PREMIUM_HAND_ABBREVS (sizeof premium_hand_abbrevs / sizeof (char *))
int premium_hand_abbrev_lens[] = {
  2,  // AA
  2,  // KK
  2,  // QQ
  2,  // JJ
  3,  // AKs
  2,  // TT
  3,  // AKo
  3,  // AQs
  2,  // 99
  3   // AJs
};
int compare_key_called;
bool bQuick;
struct hand_and_type *hands_and_types;
#else
extern char suit_chars[];
extern char rank_chars[];
extern char *plain_hand_types[];
extern int plain_hand_type_lens[];
extern char *hand_abbrevs[];
extern char *premium_hand_abbrevs[];
extern int premium_hand_abbrev_lens[];
extern int compare_key_called;
extern bool bQuick;
extern struct hand_and_type *hands_and_types;
#endif
#define NUM_PREMIUM_HANDS 10

#define NUM_CARDS_IN_HAND 5

#define POKER_3_2_PERMUTATIONS          3
#define POKER_4_2_PERMUTATIONS          6
#define POKER_6_5_PERMUTATIONS          6
#define POKER_5_2_PERMUTATIONS         10
#define POKER_5_3_PERMUTATIONS         10
#define POKER_7_5_PERMUTATIONS         21
#define POKER_33_2_PERMUTATIONS       528
#define POKER_37_2_PERMUTATIONS       666
#define POKER_41_2_PERMUTATIONS       820
#define POKER_43_2_PERMUTATIONS       903
#define POKER_45_2_PERMUTATIONS       990
#define POKER_46_2_PERMUTATIONS      1035
#define POKER_47_2_PERMUTATIONS      1081
#define POKER_52_2_PERMUTATIONS      1326
#define POKER_52_3_PERMUTATIONS     22100
#define POKER_45_4_PERMUTATIONS    148995
#define POKER_52_4_PERMUTATIONS    270725
#define POKER_36_5_PERMUTATIONS    376992
#define POKER_40_5_PERMUTATIONS    658008
#define POKER_42_5_PERMUTATIONS    850668
#define POKER_44_5_PERMUTATIONS   1086008
#define POKER_46_5_PERMUTATIONS   1370754
#define POKER_48_5_PERMUTATIONS   1712304
#define POKER_52_5_PERMUTATIONS   2598960
#define POKER_36_7_PERMUTATIONS   8347680
#define POKER_52_7_PERMUTATIONS 133784560

#define NUM_HOLE_CARDS_IN_HOLDEM_HAND 2
#define NUM_HOLE_CARDS_IN_OMAHA_HAND 4
#define NUM_CARDS_IN_FLOP 3
#define NUM_CARDS_AFTER_FLOP 2
#define NUM_COMMUNITY_CARDS (NUM_CARDS_IN_FLOP + NUM_CARDS_AFTER_FLOP)
#define NUM_CARDS_IN_HOLDEM_POOL (NUM_HOLE_CARDS_IN_HOLDEM_HAND + NUM_CARDS_IN_FLOP + NUM_CARDS_AFTER_FLOP)
#define NUM_CARDS_IN_OMAHA_POOL (NUM_HOLE_CARDS_IN_OMAHA_HAND + NUM_CARDS_IN_FLOP + NUM_CARDS_AFTER_FLOP)
#define NUM_CARDS_AT_TURN (NUM_HOLE_CARDS_IN_HOLDEM_HAND + NUM_CARDS_IN_FLOP + 1)
#define NUM_HOLE_CARDS_IN_OMAHA_HAND 4
#define NUM_CARDS_AFTER_DEAL (NUM_CARDS_IN_FLOP + NUM_CARDS_AFTER_FLOP)
#define NUM_SELECTED_HOLE_CARDS_IN_OMAHA_HAND 2
#define NUM_SELECTED_COMMUNITY_CARDS_IN_OMAHA_HAND 3

#define MAX_STREET_MARKERS 6

#define NUM_HAND_ABBREVS 169

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

int get_rank_index(char rank_char,int *rank_index_ptr);
int get_suit_index(char suit_char,int *suit_index_ptr);

typedef struct {
  int cards[NUM_CARDS_IN_HAND];
} hand;

typedef struct {
  int cards[NUM_CARDS_AT_TURN];
} turn_hand;

typedef struct {
  int cards[NUM_CARDS_IN_HOLDEM_POOL];
} holdem_hand;

typedef struct {
  int cards[NUM_CARDS_IN_FLOP];
} flop;

struct hand_and_type {
  char cards[NUM_CARDS_IN_HAND];
  char hand_type;
  int quick_ix;
};

struct outcomes {
  int wins;
  int wins_hand_counts[NUM_HAND_TYPES];
  int losses;
  int losses_hand_counts[NUM_HAND_TYPES];
  int ties;
  int ties_hand_counts[NUM_HAND_TYPES];
};

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

  PokerHand(int card1,int card2,int card3,int card4, int card5, int num_cards_in_deck);
  PokerHand(int num_cards_in_deck);
  void NewCards(int card1,int card2,int card3,int card4, int card5);

  int GetRank(int card_ix);
  void Sort();
  void Evaluate0(int *hand_counts_ptr);
  HandType Evaluate();
  HandType EvaluateLow();
  int GetQuickIx();
  void UnEvaluate();
  bool Evaluated();
  bool RoyalFlush();
  bool StraightFlush();
  bool FourOfAKind();
  bool FullHouse();
  bool Flush();
  bool Straight();
  bool Broadway();
  bool ThreeOfAKind();
  bool TwoPair();
  bool OnePair();
  bool HighCard();

  char *GetHand();
  HandType GetHandType();

  int Compare(PokerHand& compare_hand,int in_holdem_best_poker_hand);
  int CompareLow(PokerHand& compare_hand,int in_holdem_best_poker_hand);

  void print(ostream& out) const;
  void Verbose();
  void Terse();
  void Plain();
  void Fancy();
  int *GetCards();

  void SetDebugLevel(int debug_level);

  int GetNumCardsInDeck();

  private:

  hand _card;
  hand _suit;
  hand _rank;
  hand _num_cards_with_same_rank;
  hand _order;

  bool _have_cards;
  bool _hand_sorted;
  bool _hand_evaluated;
  bool _verbose;
  bool _plain;

  HandType _hand_type;
  int _quick_ix;

  int _debug_level;
  int _num_cards_in_deck;
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

  void SetDebugLevel(int debug_level);

  private:

  holdem_hand _card;
  PokerHand _best_poker_hand;

  int _have_cards;

  int _debug_level;
};

ostream& operator<<(ostream& out,const HoldemPokerHand& board_hand);

class HoldemTurnHand {
  public:

  // default constructor
  HoldemTurnHand();
  // copy constructor
  HoldemTurnHand(const HoldemTurnHand&);
  // assignment operator
  HoldemTurnHand& operator=(const HoldemTurnHand&);
  // destructor
  ~HoldemTurnHand();

  HoldemTurnHand(int card1,int card2,int card3,int card4,int card5,int card6);
  void NewCards(int card1,int card2,int card3,int card4,int card5,int card6);

  PokerHand& BestPokerHand();

  void print(ostream& out) const;

  void SetDebugLevel(int debug_level);

  private:

  turn_hand _card;
  PokerHand _best_poker_hand;

  int _have_cards;

  int _debug_level;
};

ostream& operator<<(ostream& out,const HoldemTurnHand& board_hand);

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
  void Plain();
  void Fancy();

  void SetDebugLevel(int debug_level);

  private:

  flop _card;
  flop _suit;
  flop _rank;
  flop _num_cards_with_same_rank;
  flop _order;

  bool _have_cards;
  bool _flop_sorted;
  bool _flop_evaluated;
  bool _verbose;
  bool _plain;

  FlopType _flop_type;

  int _debug_level;
};

ostream& operator<<(ostream& out,const Flop& hand);

class OmahaPokerHand {
  public:

  // default constructor
  OmahaPokerHand();
  // copy constructor
  OmahaPokerHand(const OmahaPokerHand&);
  // assignment operator
  OmahaPokerHand& operator=(const OmahaPokerHand&);
  // destructor
  ~OmahaPokerHand();

  OmahaPokerHand(int card1,int card2,int card3,int card4,int card5,int card6,int card7,int card8,int card9);
  void NewCards(int card1,int card2,int card3,int card4,int card5,int card6,int card7,int card8,int card9);

  PokerHand& BestPokerHand();
  PokerHand& BestLowPokerHand();

  void print(ostream& out) const;

  void SetDebugLevel(int debug_level);

  private:

  hand _card;
  PokerHand _best_poker_hand;

  int _have_cards;

  int _debug_level;
};

ostream& operator<<(ostream& out,const OmahaPokerHand& board_hand);

class HeadsUp {
  public:

  // default constructor
  HeadsUp();
  // copy constructor
  HeadsUp(const HeadsUp&);
  // assignment operator
  HeadsUp& operator=(const HeadsUp&);
  // destructor
  ~HeadsUp();

  HeadsUp(int card1,int card2,int card3,int card4);
  void NewCards(int card1,int card2,int card3,int card4);
  void BurntCards(
    int card1,int card2,int card3,int card4, int card5,
    int card6,int card7,int card8,int card9, int card10,
    int card11,int card12,int card13,int card14);
  struct outcomes * GetOutcomes();

  void Evaluate(bool bVerbose);

  private:

  int _cards[NUM_HEADS_UP_CARDS];
  int _num_burnt_cards;
  int _burnt_cards[MAX_HEADS_UP_BURNT_CARDS];
  bool _have_cards;
  struct outcomes _outcomes[2];
  bool _evaluated;
};

class HeadsUpFlop {
  public:

  // default constructor
  HeadsUpFlop();
  // copy constructor
  HeadsUpFlop(const HeadsUpFlop&);
  // assignment operator
  HeadsUpFlop& operator=(const HeadsUpFlop&);
  // destructor
  ~HeadsUpFlop();

  HeadsUpFlop(int card1,int card2,int card3,int card4, int card5, int card6, int card7);
  void NewCards(int card1,int card2,int card3,int card4, int card5, int card6, int card7);
  struct outcomes * GetOutcomes();

  void Evaluate(bool bVerbose);

  private:

  int _cards[NUM_HEADS_UP_FLOP_CARDS];
  bool _have_cards;
  struct outcomes _outcomes[2];
  bool _evaluated;
};

class HeadsUpTurn {
  public:

  // default constructor
  HeadsUpTurn();
  // copy constructor
  HeadsUpTurn(const HeadsUpTurn&);
  // assignment operator
  HeadsUpTurn& operator=(const HeadsUpTurn&);
  // destructor
  ~HeadsUpTurn();

  HeadsUpTurn(int card1,int card2,int card3,int card4, int card5, int card6, int card7, int card8);
  void NewCards(int card1,int card2,int card3,int card4, int card5, int card6, int card7, int card8);
  struct outcomes * GetOutcomes();

  void Evaluate(bool bVerbose);

  private:

  int _cards[NUM_HEADS_UP_TURN_CARDS];
  bool _have_cards;
  struct outcomes _outcomes[2];
  bool _evaluated;
};

void get_permutation_instance_two(
  int set_size,
  int *m,int *n,
  int instance_ix
);

void get_permutation_instance_three(
  int set_size,
  int *m,int *n,int *o,
  int instance_ix
);

void get_permutation_instance_four(
  int set_size,
  int *m,int *n,int *o,int *p,
  int instance_ix
);

void get_permutation_instance_five(
  int set_size,
  int *m,int *n,int *o,int *p,int *q,
  int instance_ix
);

void get_permutation_instance_seven(
  int set_size,
  int *m,int *n,int *o,int *p,int *q,int *r,int *s,
  int instance_ix
);

int card_value_from_card_string(char *card_string,int *card_value);
int card_string_from_card_value(int card_value,char *card_string);

HandType get_hand_type(char *hand_type_str);

bool four_to_a_flush(int *cards);

void init_plain_hand_type_lens();

int read_hands_and_types(
  char *hands_and_types_filename,
  struct hand_and_type **hands_and_types
);

int compare_key(const void *elem1,const void *elem2);

int find_hand(
  hand *in_hand,
  struct hand_and_type *hands_and_types,
  int debug_level,
  struct hand_and_type **out_hand
);

int get_index_of_hand(int rank_ix1,int suit_ix1,int rank_ix2,int suit_ix2);
int index_of_hand(char *hand,int *index_pt);
int abbrev_index_of_hand(char *hand,int *index_pt);
int index_of_hand_abbrev(char *hand_abbrev,int *index_pt);
int get_52_2_index_of_hand(int *cards);
int get_52_2_index_of_hole_cards(char *hole_cards,int *index);
int get_52_2_index_of_hand2(int *cards);
int get_52_2_index_of_hole_cards2(char *hole_cards,int *index);
void get_abbrev(char *line,char *abbrev);
bool is_premium_hand(char *abbrev,int *premium_ix);
