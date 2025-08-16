#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <direct.h>
#else
#define _MAX_PATH 4096
#include <unistd.h>
#endif
using namespace std;

#define MAIN_MODULE
#include "poker_hand.h"

struct hand_ixs {
  int num_hand_ixs;
  int ix[2];
};

#define MAX_FILENAME_LEN 1024
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

#define MAX_TABLE_NAME_LEN 128
static char table_name[MAX_TABLE_NAME_LEN+1];

static char usage[] =
"usage: fdelta3 (-terse) (-verbose) (-debug_levelval) (-hand_typehand_type) (-handhand)\n"
"  (-skip_folded) (-abbrev) (-skip_zero) (-only_zero) (-show_board)\n"
"  (-show_hand_type) (-show_hand) (-saw_flop) (-saw_river) (-only_folded)\n"
"  (-river_money) (-no_river_money) (-stealth_two_pair) (-normalize) (-lost)\n"
"  (-only_count) (-won) (-showdown) (-no_showdown) (-table_showdown)\n"
"  (-showdown_countcount) (-showdown_count_gtcount)\n"
"  (-very_best_hand) (-table_countn) (-all_in) (-not_all_in)\n"
"  (-all_in_preflop) (-all_in_postflop) (-call_in)\n"
"  (-call_in_on_the_river) (-fall_in) (-not_fall_in)\n"
"  (-hit_felt) (-didnt_hit_felt) (-no_uncalled) (-no_collected) (-show_collected)\n"
"  (-show_voluntarilty_spent) (-show_involuntarily_spent) (-show_spent) (-show_uncalled) (-show_rollback)\n"
"  (-show_opm) (-wash) (-sum_quantum) (-sum_abs_delta) (-max_delta) (-min_delta) (-max_abs_delta)\n"
"  (-max_collected) (-max_delta_hand_type) (-no_delta) (-hole_cards_used)\n"
"  (-only_suited) (-only_nonsuited) (-flopped) (-pocket_pair) (-only_hand_numbern)\n"
"  (-hand_typ_idid) (-timestamp) (-index)\n"
"  (-show_hand_typ_id) (-didnt_see_flop)\n"
"  (-only_winning_session) (-only_losing_session) (-never_hit_felt_in_session)\n"
"  (-collected_gevalue) (-sum_by_table_count)\n"
"  (-show_table_name) (-show_table_count) (-show_seat_numbers) (-show_hand_count) (-bottom_two)\n"
"  (-counterfeit) (-show_num_decisions) (-won_side_pot) (-won_main_pot) (-last_hand_only)\n"
"  (-winning_percentage) (-get_date_from_filename) (-no_hole_cards)\n"
"  (-button) (-small_blind) (-big_blind) (-utg) (-cutoff) (-hijack) (-other) (-vpip) (-didnt_vpip)\n"
"  (-deuce_or_trey_off) (-voluntary_bet) (-no_voluntary_bet)\n"
"  (-chased_flush) (-river_card_used) (-both_hole_cards_used) (-show_river)\n"
"  (-hand_typ_id_geid) (-bad_river_money) (-show_wagered) (-uberflush)\n"
"  (-winning_hand_typehand_type (-delta_gevalue)\n"
"  (-table_boss) (-show_table_boss) (-ace_on_the_river)\n"
"  (-verbose_style2) (-only_knockout) (-only_double_up)\n"
"  (-show_num_possible_checks) (-filter_percentage)\n"
"  (-show_running_total) (-show_num_positive_deltas)\n"
"  (-only_discrepancy) (-showdown_handhand) (-showdown_hand2hand)\n"
"  (-winning_handhand) (-premium) (-not_premium) (-show_winning_hand_hole_cards)\n"
"  (-only_folded_preflop) (-only_folded_on_the_flop) (-only_folded_on_the_turn) (-only_folded_on_the_river)\n"
"  (-show_roi) (-sitting_out) (-hand_type_on_flophand_type) (-exact_countcount) (-first_hand_only)\n"
"  (-twin_abbrevs) (-twin_hands) (-identical_twin_hands) (-except_last_hand)\n"
"  (-hut_outs) (-hut_wins_ge_value) (-hut_wins_le_value) (-hut_wins_eq_value\n"
"  (-huf_outs) (-3t_outs)\n"
"  (-hut_losses_ge_value) (-hut_losses_le_value) (-hut_losses_eq_value\n"
"  (-hut_ties_ge_value) (-hut_ties_le_value) (-hut_ties_eq_value\n"
"  (-show_winning_hand_hole_card_ixs) (-show_winning_hand_hole_cards_abbrev)\n"
"  (-show_opponent_hole_cards) (-show_opponent_hole_card_ixs)\n"
"  (-show_opponent_hole_cards_abbrev)\n"
"  (-broadway) (-magic_flush) (-any_all_in) (-no_all_in)\n"
"  (-hut_outs_diff) (-ace_rag) (-suited_ace) (-ace_non_rag)\n"
"  (-only_uncalled) (-first_file_only) (-show_hand_type_on_flop)\n"
"  (-show_hand_type_on_turn) (-show_hand_type_on_river) player_name filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char pokerstars[] = "PokerStars";
#define POKERSTARS_LEN (sizeof (pokerstars) - 1)
static char stud[] = "7 Card Stud";
#define STUD_LEN (sizeof (stud) - 1)
static char razz[] = "Razz";
#define RAZZ_LEN (sizeof (razz) - 1)
static char in_chips[] = " in chips";
#define IN_CHIPS_LEN (sizeof (in_chips) - 1)
static char summary[] = "*** SUMMARY ***";
#define SUMMARY_LEN (sizeof (summary) - 1)
static char flop_str[] = "*** FLOP *** [";
#define FLOP_LEN (sizeof (flop_str) - 1)
static char turn[] = "*** TURN *** [";
#define TURN_LEN (sizeof (turn) - 1)
static char river[] = "*** RIVER *** [";
#define RIVER_LEN (sizeof (river) - 1)
static char street_marker[] = "*** ";
#define STREET_MARKER_LEN (sizeof (street_marker) - 1)
static char posts_the_ante[] = " posts the ante ";
#define POSTS_THE_ANTE_LEN (sizeof (posts_the_ante) - 1)
static char brings_in_for[] = " brings in for ";
#define BRINGS_IN_FOR_LEN (sizeof (brings_in_for) - 1)
static char posts[] = " posts ";
#define POSTS_LEN (sizeof (posts) - 1)
static char posts_small_blind[] = " posts small blind ";
#define POSTS_SMALL_BLIND_LEN (sizeof (posts_small_blind) - 1)
static char posts_big_blind[] = " posts big blind ";
#define POSTS_BIG_BLIND_LEN (sizeof (posts_big_blind) - 1)
static char dealt_to[] = "Dealt to ";
#define DEALT_TO_LEN (sizeof (dealt_to) - 1)
static char folds[] = " folds ";
#define FOLDS_LEN (sizeof (folds) - 1)
static char bets[] = " bets ";
#define BETS_LEN (sizeof (bets) - 1)
static char calls[] = " calls ";
#define CALLS_LEN (sizeof (calls) - 1)
static char raises[] = " raises ";
#define RAISES_LEN (sizeof (raises) - 1)
static char checks[] = " checks ";
#define CHECKS_LEN (sizeof (checks) - 1)
static char uncalled_bet[] = "Uncalled bet (";
#define UNCALLED_BET_LEN (sizeof (uncalled_bet) - 1)
static char collected[] = " collected ";
#define COLLECTED_LEN (sizeof (collected) - 1)
static char side_pot[] = " from side pot";
#define SIDE_POT_LEN (sizeof (side_pot) - 1)
static char main_pot[] = " from main pot";
#define MAIN_POT_LEN (sizeof (main_pot) - 1)
static char show_down[] = "*** SHOW DOWN ***";
#define SHOW_DOWN_LEN (sizeof (show_down) - 1)
static char all_in[] = "all-in";
#define ALL_IN_LEN (sizeof (all_in) - 1)
static char won[] = " won ";
#define WON_LEN (sizeof (won) - 1)
static char with[] = " with ";
#define WITH_LEN (sizeof (with) - 1)
static char with_a[] = " with a ";
#define WITH_A_LEN (sizeof (with_a) - 1)
static char finished[] = " finished the tournament ";
#define FINISHED_LEN (sizeof (finished) - 1)
static char showed[] = " showed [";
#define SHOWED_LEN (sizeof (showed) - 1)
static char and_won[] = " and won ";
#define AND_WON_LEN (sizeof (and_won) - 1)
static char sitting_out[] = " sitting out";
#define SITTING_OUT_LEN (sizeof (sitting_out) - 1)
static char board[] = "Board [";
#define BOARD_LEN (sizeof (board) - 1)

#define MAX_TABLE_COUNT 9

static int file_no;
static int dbg_file_no;
static int hand_no;
static int dbg_hand_no;

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static bool Contains(bool bCaseSens,char *line,int line_len,int line_no,int code_line_no,int debug_level,
  char *string,int string_len,int *index);
static int get_work_amount(char *line,int line_len);
static void normalize_hole_cards(char *hole_cards);
static int get_date_from_path(char *path,char **date_str_ptr);
static void get_table_name(char *line,int line_len,char *table_name,int max_table_name_len);
static int bottom_two(char *line,int line_len,char *hole_cards);
static int get_num_hands_in_file(FILE *fptr,char *player_name,int player_name_len);
static bool deuce_or_trey_off(char *hole_cards);
static bool ace_rag(char *hole_cards);
static bool suited_ace(char *hole_cards);
static bool ace_non_rag(char *hole_cards);
static bool hole_cards_off(char *hole_cards);
static HandType get_winning_hand_typ_id(char *line,int line_len);
static void run_filter(struct vars *varspt);
static void do_balance_processing(struct vars *varspt);
static char *style2(char *filename);
static int hand_ix_match(int hand_ix,struct hand_ixs *specified_hand_ixs);
static int get_button_seat(char *line);
static void set_position_booleans(struct vars *varspt);
static void populate_timestamp(char *line,char *timestamp);

enum quantum_typ {
  QUANTUM_TYPE_DELTA,
  QUANTUM_TYPE_COLLECTED,
  QUANTUM_TYPE_VOLUNTARILY_SPENT,
  QUANTUM_TYPE_INVOLUNTARILY_SPENT,
  QUANTUM_TYPE_SPENT,
  QUANTUM_TYPE_UNCALLED,
  QUANTUM_TYPE_ROLLBACK,
  QUANTUM_TYPE_OPM,
  QUANTUM_TYPE_NUMDECISIONS,
  QUANTUM_TYPE_WAGERED,
  QUANTUM_TYPE_TABLE_BOSS,
  QUANTUM_TYPE_NUM_POSSIBLE_CHECKS,
  QUANTUM_TYPE_RUNNING_TOTAL,
  QUANTUM_TYPE_NUM_POSITIVE_DELTAS,
  QUANTUM_TYPE_DISCREPANCY,
  QUANTUM_TYPE_ROI
};

// sample timestamp: 2023/09/13 11:35:05 ET
#define TIMESTAMP_LEN 22

#define MAX_SEATS 9

struct vars {
  char timestamp[TIMESTAMP_LEN+1];
  int line_no;
  int button_seat;
  bool bAmButton;
  int my_seat_ix;
  bool bAmUtg;
  bool bAmCutoff;
  bool bAmHijack;
  char seat_numbers[MAX_SEATS+1];
  bool bTimestamp;
  bool bIndex;
  bool bTerse;
  bool bVerbose;
  bool bVerboseStyle2;
  int debug_level;
  bool bHandTypeSpecified;
  bool bHandTypeOnFlopSpecified;
  bool bWinningHandTypeSpecified;
  bool bHandTypIdSpecified;
  bool bHandSpecified;
  bool bPremium;
  bool bNotPremium;
  bool bShowWinningHandHoleCards;
  bool bShowWinningHandHoleCardIxs;
  bool bShowWinningHandHoleCardsAbbrev;
  bool bShowOpponentHoleCards;
  bool bShowOpponentHoleCardIxs;
  bool bShowOpponentHoleCardsAbbrev;
  struct hand_ixs specified_hand_ixs;
  bool bShowdownHandSpecified;
  int specified_showdown_hand_index;
  bool bShowdownHand2Specified;
  int specified_showdown_hand2_index;
  bool bWinningHandSpecified;
  struct hand_ixs specified_winning_hand_hand_ixs;
  int curr_abbrev_index;
  int prev_abbrev_index;
  int curr_52_2_index;
  int prev_52_2_index;
  int curr_52_2_index2;
  int prev_52_2_index2;
  bool bSkipFolded;
  bool bAbbrev;
  bool bSkipZero;
  bool bOnlyZero;
  bool bShowBoard;
  bool bShowHandType;
  bool bShowHandTypeOnFlop;
  bool bShowHandTypeOnTurn;
  bool bShowHandTypeOnRiver;
  bool bShowHandTypId;
  bool bShowHand;
  bool bSawFlop;
  bool bSawRiver;
  bool bOnlyFolded;
  bool bOnlyFoldedPreflop;
  bool bOnlyFoldedOnTheFlop;
  bool bOnlyFoldedOnTheTurn;
  bool bOnlyFoldedOnTheRiver;
  bool bRiverMoney;
  bool bNoRiverMoney;
  bool bStealthTwoPair;
  bool bNormalize;
  bool bLost;
  int only_count;
  bool bExactCount;
  int exact_count;
  bool bWon;
  bool bShowdown;
  bool bTableShowdown;
  bool bNoShowdown;
  bool bShowdownCount;
  bool bShowdownCountGt;
  int showdown_count;
  bool bVeryBestHand;
  bool bTableCount;
  bool bAllIn;
  bool bNotAllIn;
  bool bAllInPreflop;
  bool bAllInPostflop;
  bool bCallIn;
  bool bCallInOnTheRiver;
  bool bFallIn;
  bool bNotFallIn;
  bool bHitFelt;
  bool bDidntHitFelt;
  bool bNoUncalled;
  bool bNoCollected;
  bool bSittingOut;
  bool bBroadway;
  bool bMagicFlush;
  bool bAnyAllIn;
  bool bNoAllIn;
  bool bAceRag;
  bool bSuitedAce;
  bool bAceNonRag;
  bool bOnlyUncalled;
  int show_collected;
  int show_voluntarily_spent;
  int show_involuntarily_spent;
  int show_spent;
  int show_uncalled;
  int show_rollback;
  int show_opm;
  int show_num_decisions;
  int show_wagered;
  int show_table_boss;
  int show_num_possible_checks;
  int num_possible_checks;
  int show_running_total;
  int running_total;
  int show_roi;
  int show_num_positive_deltas;
  int num_positive_deltas;
  int wins;
  bool bAceOnTheRiver;
  bool bWash;
  bool bNoDelta;
  bool bHoleCardsUsed;
  bool bOnlySuited;
  bool bOnlyNonsuited;
  bool bFlopped;
  bool bPocketPair;
  bool bDidntSeeFlop;
  bool bOnlyWinningSession;
  bool bOnlyLosingSession;
  bool bNeverHitFeltInSession;
  bool bCollectedGe;
  bool bSumByTableCount;
  bool bShowTableName;
  bool bShowTableCount;
  bool bShowSeatNumbers;
  bool bShowHandCount;
  bool bBottomTwo;
  bool bCounterfeit;
  bool bWonSidePot;
  bool bWonMainPot;
  bool bLastHandOnly;
  bool bExceptLastHand;
  bool bFirstHandOnly;
  bool bTwinAbbrevs;
  bool bTwinHands;
  bool bIdenticalTwinHands;
  bool bHutOuts;
  bool bHutOutsDiff;
  bool bHutWinsGeValue;
  int hut_wins_ge_value;
  bool bHutWinsLeValue;
  int hut_wins_le_value;
  bool bHutWinsEqValue;
  int hut_wins_eq_value;
  bool bHutLossesGeValue;
  int hut_losses_ge_value;
  bool bHutLossesLeValue;
  int hut_losses_le_value;
  bool bHutLossesEqValue;
  int hut_losses_eq_value;
  bool bHutTiesGeValue;
  int hut_ties_ge_value;
  bool bHutTiesLeValue;
  int hut_ties_le_value;
  bool bHutTiesEqValue;
  int hut_ties_eq_value;
  bool bHufOuts;
  bool b3tOuts;
  bool bGetDateFromFilename;
  bool bNoHoleCards;
  bool bButton;
  int button;
  bool bSmallBlind;
  int small_blind;
  bool bBigBlind;
  int big_blind;
  bool bUtg;
  int utg;
  bool bCutoff;
  int cutoff;
  bool bHijack;
  int hijack;
  bool bOther;
  int other;
  bool bVpip;
  bool bDidntVpip;
  bool bDeuceOrTreyOff;
  bool bVoluntaryBet;
  bool bNoVoluntaryBet;
  bool bChasedFlush;
  bool bRiverCardUsed;
  bool bBothHoleCardsUsed;
  bool bShowRiver;
  bool bHandTypIdGeSpecified;
  bool bBadRiverMoney;
  bool bOnlyDiscrepancy;
  bool bUberflush;
  bool bDeltaGe;
  int delta_ge_val;
  bool bTableBoss;
  bool bOnlyKnockout;
  bool bOnlyDoubleUp;
  int am_table_boss;
  int collected_ge_val;
  bool bStud;
  bool bRazz;
  int hand_number;
  bool bHaveSpecifiedHand;
  bool bHavePremiumHand;
  bool bHaveNotPremiumHand;
  bool bHaveSpecifiedShowdownHand;
  bool bHaveSpecifiedShowdownHand2;
  bool bHaveSpecifiedWinningHand;
  bool bHaveWinningHandHoleCards;
  bool bHaveOpponentHoleCards;
  bool bHaveFlop;
  bool bHaveFlop2;
  bool bHaveTurn2;
  bool bHaveRiver;
  bool bHaveRiver2;
  bool bHaveAceOnTheRiver;
  bool bHaveStealthTwoPair;
  bool bHaveBottomTwo;
  bool bSpentRiverMoney;
  bool bHaveShowdown;
  bool bHaveTableShowdown;
  bool bHaveShowdownCount;
  bool bHaveShowdownCountGt;
  bool bHaveAllIn;
  bool bHaveAllInPreflop;
  bool bHaveAllInPostflop;
  bool bHaveCallIn;
  bool bHaveCallInOnTheRiver;
  bool bHaveFallIn;
  bool bHaveCounterfeit;
  bool bHaveWonSidePot;
  bool bHaveWonMainPot;
  bool bSummarizing;
  HandType hand_typ_id;
  HandType hand_typ_on_flop_id;
  HandType winning_hand_typ_id;
  HandType curr_winning_hand_typ_id;
  HandType hand_typ_id_ge;
  bool bFolded;
  bool bFoldedPreflop;
  bool bFoldedOnTheFlop;
  bool bFoldedOnTheTurn;
  bool bFoldedOnTheRiver;
  bool bSkipping;
  bool bPostedSmallBlind;
  bool bPostedBigBlind;
  bool bHaveDeuceOrTreyOff;
  bool bHaveVoluntaryBet;
  bool bHaveChasedFlush;
  bool bHaveRiverCardUsed;
  bool bHaveBothHoleCardsUsed;
  bool bHaveBadRiverMoney;
  bool bHaveDiscrepancy;
  int discrepancy;
  bool bHaveKnockout;
  bool bHaveDoubleUp;
  bool bIsSittingOut;
  int delta;
  PokerHand poker_hand;
  PokerHand poker_hand_on_flop;
  PokerHand poker_hand_on_turn;
  PokerHand poker_hand_on_river;
  int table_count;
  int table_count_to_match;
  int vpip;
  int starting_balance;
  int ending_balance;
  int prev_ending_balance;
  int uncalled_bet_amount;
  int collected_from_pot;
  int collected_from_pot_count;
  char hole_cards[12];
  char hole_cards_abbrev[4];
  char winning_hand_hole_cards[6];
  int winning_hand_hole_card_ixs[2];
  char winning_hand_hole_cards_abbrev[4];
  char opponent_hole_cards[6];
  int opponent_hole_card_ixs[2];
  char opponent_hole_cards_abbrev[4];
  int hole_cards_used;
  int num_hands;
  int num_hands_in_file;
  int voluntarily_spent_this_hand;
  int involuntarily_spent_this_hand;
  int work;
  int numdecs;
  int wagered_amount;
  int total_delta;
  int winning_percentage;
  int filter_percentage;
  int summary_val;
  int summary_val2;
  int sum_quantum;
  double dwork;
  int sum_abs_delta;
  int max_delta;
  int min_delta;
  int max_abs_delta;
  int max_collected;
  int max_delta_hand_type;
  int max_delta_hand_typ;
  int sum_by_table_count[MAX_TABLE_COUNT - 1];
  int count_by_table_count[MAX_TABLE_COUNT - 1];
  char board_cards[15];
  char *date_string;
  quantum_typ quantum_type;
  int quantum;
  int hit_felt_in_session_count;
  struct outcomes *outcomes;
  bool bHaveMagicFlush;
  bool bHaveAnyAllIn;
  bool bHaveAceRag;
  bool bHaveSuitedAce;
  bool bHaveAceNonRag;
};

static int dbg_line_no;

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  int q;
  struct vars local_vars;
  int curr_arg;
  int player_name_ix;
  int player_name_len;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int showdown_count;
  int ix;
  int pot_ix;
  int showed_ix;
  int bracket_ix;
  int street;
  int num_street_markers;
  int max_streets;
  int ante;
  int bring_in;
  int voluntarily_spent_this_street;
  int involuntarily_spent_this_street;
  int end_ix;
  int dbg;
  int cards[NUM_CARDS_IN_HOLDEM_POOL];
  HoldemTurnHand turn_hand;
  HoldemPokerHand holdem_hand;
  char card_string[2];
  int retval;
  int *poker_hand_cards;
  bool bFirstHand;
  int curr_seat;
  int curr_stack;
  int boss_stack;
  int boss_seat_ix;
  int work_hand_index;
  char specified_hand[4];
  char specified_winning_hand[4];
  bool bFirstFileOnly;
  int premium_ix;

  if ((argc < 3) || (argc > 170)) {
    printf(usage);
    return 1;
  }

  init_plain_hand_type_lens();

  local_vars.bTimestamp = false;
  local_vars.bIndex = false;
  local_vars.line_no = 0;
  local_vars.button_seat = 0;
  local_vars.bAmButton = false;
  local_vars.bAmUtg = false;
  local_vars.bAmCutoff = false;
  local_vars.bAmHijack = false;
  local_vars.bTerse = false;
  local_vars.bVerbose = false;
  local_vars.bVerboseStyle2 = false;
  local_vars.debug_level = 0;
  local_vars.bHandTypeSpecified = false;
  local_vars.bHandTypeOnFlopSpecified = false;
  local_vars.bWinningHandTypeSpecified = false;
  local_vars.bHandTypIdSpecified = false;
  local_vars.bHandSpecified = false;
  local_vars.bPremium = false;
  local_vars.bNotPremium = false;
  local_vars.bShowWinningHandHoleCards = false;
  local_vars.bShowWinningHandHoleCardIxs = false;
  local_vars.bShowWinningHandHoleCardsAbbrev = false;
  local_vars.bShowOpponentHoleCards = false;
  local_vars.bShowOpponentHoleCardIxs = false;
  local_vars.bShowOpponentHoleCardsAbbrev = false;
  local_vars.bShowdownHandSpecified = false;
  local_vars.bShowdownHand2Specified = false;
  local_vars.bWinningHandSpecified = false;
  local_vars.bSkipFolded = false;
  local_vars.bAbbrev = false;
  local_vars.bSkipZero = false;
  local_vars.bOnlyZero = false;
  local_vars.bShowBoard = false;
  local_vars.bShowHandType = false;
  local_vars.bShowHandTypeOnFlop = false;
  local_vars.bShowHandTypeOnTurn = false;
  local_vars.bShowHandTypeOnRiver = false;
  local_vars.bShowHandTypId = false;
  local_vars.bShowHand = false;
  local_vars.bSawFlop = false;
  local_vars.bSawRiver = false;
  local_vars.bOnlyFolded = false;
  local_vars.bOnlyFoldedPreflop = false;
  local_vars.bOnlyFoldedOnTheFlop = false;
  local_vars.bOnlyFoldedOnTheTurn = false;
  local_vars.bOnlyFoldedOnTheRiver = false;
  local_vars.bRiverMoney = false;
  local_vars.bNoRiverMoney = false;
  local_vars.bStealthTwoPair = false;
  local_vars.bNormalize = false;
  local_vars.bLost = false;
  local_vars.only_count = 0;
  local_vars.bExactCount = false;
  local_vars.bWon = false;
  local_vars.bShowdown = false;
  local_vars.bTableShowdown = false;
  local_vars.bNoShowdown = false;
  local_vars.bShowdownCount = false;
  local_vars.bShowdownCountGt = false;
  local_vars.bVeryBestHand = false;
  local_vars.bTableCount = false;
  local_vars.bAllIn = false;
  local_vars.bNotAllIn = false;
  local_vars.bAllInPreflop = false;
  local_vars.bAllInPostflop = false;
  local_vars.bCallIn = false;
  local_vars.bCallInOnTheRiver = false;
  local_vars.bFallIn = false;
  local_vars.bNotFallIn = false;
  local_vars.bHitFelt = false;
  local_vars.bDidntHitFelt = false;
  local_vars.bNoUncalled = false;
  local_vars.bNoCollected = false;
  local_vars.bSittingOut = false;
  local_vars.bBroadway = false;
  local_vars.bMagicFlush = false;
  local_vars.bAnyAllIn = false;
  local_vars.bNoAllIn = false;
  local_vars.bAceRag = false;
  local_vars.bSuitedAce = false;
  local_vars.bAceNonRag = false;
  local_vars.bOnlyUncalled = false;
  local_vars.quantum_type = QUANTUM_TYPE_DELTA;
  local_vars.show_collected = 0;
  local_vars.show_voluntarily_spent = 0;
  local_vars.show_involuntarily_spent = 0;
  local_vars.show_spent = 0;
  local_vars.show_uncalled = 0;
  local_vars.show_rollback = 0;
  local_vars.show_opm = 0;
  local_vars.show_num_decisions = 0;
  local_vars.show_wagered = 0;
  local_vars.show_table_boss = 0;
  local_vars.show_num_possible_checks = 0;
  local_vars.show_running_total = 0;
  local_vars.show_roi = 0;
  local_vars.running_total = 0;
  local_vars.show_num_positive_deltas = 0;
  local_vars.bAceOnTheRiver = false;
  local_vars.bWash = false;
  local_vars.sum_quantum = 0;
  local_vars.sum_abs_delta = 0;
  local_vars.max_delta = 0;
  local_vars.min_delta = 0;
  local_vars.max_abs_delta = 0;
  local_vars.max_collected = 0;
  local_vars.max_delta_hand_type = 0;
  local_vars.winning_percentage = 0;
  local_vars.filter_percentage = 0;
  local_vars.bNoDelta = false;
  local_vars.bHoleCardsUsed = false;
  local_vars.bOnlySuited = false;
  local_vars.bOnlyNonsuited = false;
  local_vars.bFlopped = false;
  local_vars.bPocketPair = false;
  local_vars.bDidntSeeFlop = false;
  local_vars.bOnlyWinningSession = false;
  local_vars.bOnlyLosingSession = false;
  local_vars.bNeverHitFeltInSession = false;
  local_vars.bCollectedGe = false;
  local_vars.bSumByTableCount = false;
  local_vars.bShowTableName = false;
  local_vars.bShowTableCount = false;
  local_vars.bShowSeatNumbers = false;
  local_vars.bShowHandCount = false;
  local_vars.bBottomTwo = false;
  local_vars.bCounterfeit = false;
  local_vars.bWonSidePot = false;
  local_vars.bWonMainPot = false;
  local_vars.bLastHandOnly = false;
  local_vars.bExceptLastHand = false;
  local_vars.bFirstHandOnly = false;
  local_vars.bTwinAbbrevs = false;
  local_vars.bTwinHands = false;
  local_vars.bIdenticalTwinHands = false;
  local_vars.bHutOuts = false;
  local_vars.bHutOutsDiff = false;
  local_vars.bHutWinsGeValue = false;
  local_vars.bHutWinsLeValue = false;
  local_vars.bHutWinsEqValue = false;
  local_vars.bHutLossesGeValue = false;
  local_vars.bHutLossesLeValue = false;
  local_vars.bHutLossesEqValue = false;
  local_vars.bHutTiesGeValue = false;
  local_vars.bHutTiesLeValue = false;
  local_vars.bHutTiesEqValue = false;
  local_vars.bHufOuts = false;
  local_vars.b3tOuts = false;
  local_vars.bGetDateFromFilename = false;
  local_vars.bNoHoleCards = false;
  local_vars.bButton = false;
  local_vars.button = 0;
  local_vars.bSmallBlind = false;
  local_vars.small_blind = 0;
  local_vars.bBigBlind = false;
  local_vars.big_blind = 0;
  local_vars.bUtg = false;
  local_vars.utg = 0;
  local_vars.bCutoff = false;
  local_vars.cutoff = 0;
  local_vars.bHijack = false;
  local_vars.hijack = 0;
  local_vars.bOther = false;
  local_vars.other = 0;
  local_vars.bVpip = false;
  local_vars.bDidntVpip = false;
  local_vars.bDeuceOrTreyOff = false;
  local_vars.bVoluntaryBet = false;
  local_vars.bNoVoluntaryBet = false;
  local_vars.bChasedFlush = false;
  local_vars.bRiverCardUsed = false;
  local_vars.bBothHoleCardsUsed = false;
  local_vars.bShowRiver = false;
  local_vars.bHandTypIdGeSpecified = false;
  local_vars.bBadRiverMoney = false;
  local_vars.bOnlyDiscrepancy = false;
  local_vars.bUberflush = false;
  local_vars.bDeltaGe = false;
  local_vars.bTableBoss = false;
  local_vars.bOnlyKnockout = false;
  local_vars.bOnlyDoubleUp = false;
  local_vars.hand_number = -1;
  specified_hand[3] = 0;
  specified_winning_hand[3] = 0;
  bFirstFileOnly = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-timestamp"))
      local_vars.bTimestamp = true;
    else if (!strcmp(argv[curr_arg],"-index"))
      local_vars.bIndex = true;
    else if (!strcmp(argv[curr_arg],"-terse"))
      local_vars.bTerse = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      local_vars.bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-verbose_style2")) {
      local_vars.bVerbose = true;
      local_vars.bVerboseStyle2 = true;
    }
    else if (!strncmp(argv[curr_arg],"-debug_level",12))
      sscanf(&argv[curr_arg][12],"%d",&local_vars.debug_level);
    else if (!strncmp(argv[curr_arg],"-hand_type_on_flop",18)) {
      local_vars.hand_typ_on_flop_id = get_hand_type(&argv[curr_arg][18]);
      local_vars.bHandTypeOnFlopSpecified = true;
    }
    else if (!strncmp(argv[curr_arg],"-hand_type",10)) {
      local_vars.hand_typ_id = get_hand_type(&argv[curr_arg][10]);
      local_vars.bHandTypeSpecified = true;
    }
    else if (!strncmp(argv[curr_arg],"-winning_hand_type",18)) {
      local_vars.winning_hand_typ_id = get_hand_type(&argv[curr_arg][18]);
      local_vars.bWinningHandTypeSpecified = true;
      local_vars.bShowdown = true;
    }
    else if (!strncmp(argv[curr_arg],"-hand_typ_id_ge",15)) {
      sscanf(&argv[curr_arg][15],"%d",&local_vars.hand_typ_id_ge);
      local_vars.bHandTypIdGeSpecified = true;
    }
    else if (!strncmp(argv[curr_arg],"-hand_typ_id",12)) {
      sscanf(&argv[curr_arg][12],"%d",&local_vars.hand_typ_id);
      local_vars.bHandTypIdSpecified = true;
    }
    else if (!strncmp(argv[curr_arg],"-hand",5)) {
      local_vars.bHandSpecified = true;

      if ((strlen(&argv[curr_arg][5]) == 3) && (argv[curr_arg][7] == 'x')) {
        local_vars.specified_hand_ixs.num_hand_ixs = 2;

        for (n = 0; n < 2; n++)
          specified_hand[n] = argv[curr_arg][5+n];

        for (n = 0; n < 2; n++) {
          specified_hand[2] = (n ? 's' : 'o');
          retval = index_of_hand_abbrev(specified_hand,&local_vars.specified_hand_ixs.ix[n]);

          if (retval)
            local_vars.specified_hand_ixs.ix[n] = -1;
        }
      }
      else {
        local_vars.specified_hand_ixs.num_hand_ixs = 1;
        retval = index_of_hand_abbrev(&argv[curr_arg][5],&local_vars.specified_hand_ixs.ix[0]);

        if (retval)
          local_vars.specified_hand_ixs.ix[0] = -1;
      }
    }
    else if (!strcmp(argv[curr_arg],"-premium")) {
      local_vars.bPremium = true;
    }
    else if (!strcmp(argv[curr_arg],"-not_premium")) {
      local_vars.bNotPremium = true;
    }
    else if (!strcmp(argv[curr_arg],"-show_winning_hand_hole_cards")) {
      local_vars.bShowWinningHandHoleCards = true;
    }
    else if (!strcmp(argv[curr_arg],"-show_winning_hand_hole_card_ixs")) {
      local_vars.bShowWinningHandHoleCards = true;
      local_vars.bShowWinningHandHoleCardIxs = true;
      local_vars.bShowWinningHandHoleCardsAbbrev = false;
    }
    else if (!strcmp(argv[curr_arg],"-show_winning_hand_hole_cards_abbrev")) {
      local_vars.bShowWinningHandHoleCards = true;
      local_vars.bShowWinningHandHoleCardsAbbrev = true;
      local_vars.bShowWinningHandHoleCardIxs = false;
    }
    else if (!strcmp(argv[curr_arg],"-show_opponent_hole_cards")) {
      local_vars.bShowOpponentHoleCards = true;
    }
    else if (!strcmp(argv[curr_arg],"-show_opponent_hole_card_ixs")) {
      local_vars.bShowOpponentHoleCards = true;
      local_vars.bShowOpponentHoleCardIxs = true;
      local_vars.bShowOpponentHoleCardsAbbrev = false;
    }
    else if (!strcmp(argv[curr_arg],"-show_opponent_hole_cards_abbrev")) {
      local_vars.bShowOpponentHoleCards = true;
      local_vars.bShowOpponentHoleCardsAbbrev = true;
      local_vars.bShowOpponentHoleCardIxs = false;
    }
    else if (!strncmp(argv[curr_arg],"-showdown_hand2",15)) {
      local_vars.bShowdownHand2Specified = true;
      retval = index_of_hand_abbrev(&argv[curr_arg][15],&local_vars.specified_showdown_hand2_index);

      if (retval)
        local_vars.specified_showdown_hand2_index = -1;
    }
    else if (!strncmp(argv[curr_arg],"-showdown_hand",14)) {
      local_vars.bShowdownHandSpecified = true;
      retval = index_of_hand_abbrev(&argv[curr_arg][14],&local_vars.specified_showdown_hand_index);

      if (retval)
        local_vars.specified_showdown_hand_index = -1;
    }
    else if (!strncmp(argv[curr_arg],"-winning_hand",13)) {
      local_vars.bWinningHandSpecified = true;

      if ((strlen(&argv[curr_arg][13]) == 3) && (argv[curr_arg][15] == 'x')) {
        local_vars.specified_winning_hand_hand_ixs.num_hand_ixs = 2;

        for (n = 0; n < 2; n++)
          specified_winning_hand[n] = argv[curr_arg][13+n];

        for (n = 0; n < 2; n++) {
          specified_winning_hand[2] = (n ? 's' : 'o');
          retval = index_of_hand_abbrev(specified_winning_hand,&local_vars.specified_winning_hand_hand_ixs.ix[n]);

          if (retval)
            local_vars.specified_winning_hand_hand_ixs.ix[n] = -1;
        }
      }
      else {
        local_vars.specified_winning_hand_hand_ixs.num_hand_ixs = 1;
        retval = index_of_hand_abbrev(&argv[curr_arg][13],&local_vars.specified_winning_hand_hand_ixs.ix[0]);

        if (retval)
          local_vars.specified_winning_hand_hand_ixs.ix[0] = -1;
      }
    }
    else if (!strcmp(argv[curr_arg],"-skip_folded"))
      local_vars.bSkipFolded = true;
    else if (!strcmp(argv[curr_arg],"-abbrev"))
      local_vars.bAbbrev = true;
    else if (!strcmp(argv[curr_arg],"-skip_zero"))
      local_vars.bSkipZero = true;
    else if (!strcmp(argv[curr_arg],"-only_zero"))
      local_vars.bOnlyZero = true;
    else if (!strcmp(argv[curr_arg],"-show_board"))
      local_vars.bShowBoard = true;
    else if (!strcmp(argv[curr_arg],"-show_hand_type"))
      local_vars.bShowHandType = true;
    else if (!strcmp(argv[curr_arg],"-show_hand_type_on_flop"))
      local_vars.bShowHandTypeOnFlop = true;
    else if (!strcmp(argv[curr_arg],"-show_hand_type_on_turn"))
      local_vars.bShowHandTypeOnTurn = true;
    else if (!strcmp(argv[curr_arg],"-show_hand_type_on_river"))
      local_vars.bShowHandTypeOnRiver = true;
    else if (!strcmp(argv[curr_arg],"-show_hand_typ_id"))
      local_vars.bShowHandTypId = true;
    else if (!strcmp(argv[curr_arg],"-show_hand"))
      local_vars.bShowHand = true;
    else if (!strcmp(argv[curr_arg],"-saw_flop"))
      local_vars.bSawFlop = true;
    else if (!strcmp(argv[curr_arg],"-saw_river"))
      local_vars.bSawRiver = true;
    else if (!strcmp(argv[curr_arg],"-only_folded"))
      local_vars.bOnlyFolded = true;
    else if (!strcmp(argv[curr_arg],"-only_folded_preflop"))
      local_vars.bOnlyFoldedPreflop = true;
    else if (!strcmp(argv[curr_arg],"-only_folded_on_the_flop"))
      local_vars.bOnlyFoldedOnTheFlop = true;
    else if (!strcmp(argv[curr_arg],"-only_folded_on_the_turn"))
      local_vars.bOnlyFoldedOnTheTurn = true;
    else if (!strcmp(argv[curr_arg],"-only_folded_on_the_river"))
      local_vars.bOnlyFoldedOnTheRiver = true;
    else if (!strcmp(argv[curr_arg],"-river_money"))
      local_vars.bRiverMoney = true;
    else if (!strcmp(argv[curr_arg],"-no_river_money"))
      local_vars.bNoRiverMoney = true;
    else if (!strcmp(argv[curr_arg],"-stealth_two_pair"))
      local_vars.bStealthTwoPair = true;
    else if (!strcmp(argv[curr_arg],"-normalize"))
      local_vars.bNormalize = true;
    else if (!strcmp(argv[curr_arg],"-lost"))
      local_vars.bLost = true;
    else if (!strcmp(argv[curr_arg],"-only_count"))
      local_vars.only_count = 1;
    else if (!strncmp(argv[curr_arg],"-exact_count",12)) {
      local_vars.bExactCount = true;
      sscanf(&argv[curr_arg][12],"%d",&local_vars.exact_count);
      local_vars.only_count = 1;
    }
    else if (!strcmp(argv[curr_arg],"-won"))
      local_vars.bWon = true;
    else if (!strcmp(argv[curr_arg],"-showdown"))
      local_vars.bShowdown = true;
    else if (!strcmp(argv[curr_arg],"-table_showdown"))
      local_vars.bTableShowdown = true;
    else if (!strcmp(argv[curr_arg],"-no_showdown"))
      local_vars.bNoShowdown = true;
    else if (!strncmp(argv[curr_arg],"-showdown_count_gt",23)) {
      local_vars.bShowdownCountGt = true;
      sscanf(&argv[curr_arg][23],"%d",&local_vars.showdown_count);
    }
    else if (!strncmp(argv[curr_arg],"-showdown_count",20)) {
      local_vars.bShowdownCount = true;
      sscanf(&argv[curr_arg][20],"%d",&local_vars.showdown_count);
    }
    else if (!strcmp(argv[curr_arg],"-very_best_hand"))
      local_vars.bVeryBestHand = true;
    else if (!strncmp(argv[curr_arg],"-table_count",12)) {
      local_vars.bTableCount = true;
      sscanf(&argv[curr_arg][12],"%d",&local_vars.table_count_to_match);
    }
    else if (!strcmp(argv[curr_arg],"-all_in"))
      local_vars.bAllIn = true;
    else if (!strcmp(argv[curr_arg],"-not_all_in"))
      local_vars.bNotAllIn = true;
    else if (!strcmp(argv[curr_arg],"-all_in_preflop"))
      local_vars.bAllInPreflop = true;
    else if (!strcmp(argv[curr_arg],"-all_in_postflop"))
      local_vars.bAllInPostflop = true;
    else if (!strcmp(argv[curr_arg],"-call_in"))
      local_vars.bCallIn = true;
    else if (!strcmp(argv[curr_arg],"-call_in_on_the_river"))
      local_vars.bCallInOnTheRiver = true;
    else if (!strcmp(argv[curr_arg],"-fall_in"))
      local_vars.bFallIn = true;
    else if (!strcmp(argv[curr_arg],"-not_fall_in"))
      local_vars.bNotFallIn = true;
    else if (!strcmp(argv[curr_arg],"-hit_felt"))
      local_vars.bHitFelt = true;
    else if (!strcmp(argv[curr_arg],"-didnt_hit_felt"))
      local_vars.bDidntHitFelt = true;
    else if (!strcmp(argv[curr_arg],"-no_uncalled"))
      local_vars.bNoUncalled = true;
    else if (!strcmp(argv[curr_arg],"-no_collected"))
      local_vars.bNoCollected = true;
    else if (!strcmp(argv[curr_arg],"-show_collected")) {
      local_vars.show_collected = 1;
      local_vars.quantum_type = QUANTUM_TYPE_COLLECTED;
    }
    else if (!strcmp(argv[curr_arg],"-show_voluntarily_spent")) {
      local_vars.show_voluntarily_spent = 1;
      local_vars.quantum_type = QUANTUM_TYPE_VOLUNTARILY_SPENT;
    }
    else if (!strcmp(argv[curr_arg],"-show_involuntarily_spent")) {
      local_vars.show_involuntarily_spent = 1;
      local_vars.quantum_type = QUANTUM_TYPE_INVOLUNTARILY_SPENT;
    }
    else if (!strcmp(argv[curr_arg],"-show_spent")) {
      local_vars.show_spent = 1;
      local_vars.quantum_type = QUANTUM_TYPE_SPENT;
    }
    else if (!strcmp(argv[curr_arg],"-show_uncalled")) {
      local_vars.show_uncalled = 1;
      local_vars.quantum_type = QUANTUM_TYPE_UNCALLED;
    }
    else if (!strcmp(argv[curr_arg],"-show_rollback")) {
      local_vars.show_rollback = 1;
      local_vars.quantum_type = QUANTUM_TYPE_ROLLBACK;
    }
    else if (!strcmp(argv[curr_arg],"-show_opm")) {
      local_vars.show_opm = 1;
      local_vars.quantum_type = QUANTUM_TYPE_OPM;
    }
    else if (!strcmp(argv[curr_arg],"-show_num_decisions")) {
      local_vars.show_num_decisions = 1;
      local_vars.quantum_type = QUANTUM_TYPE_NUMDECISIONS;
    }
    else if (!strcmp(argv[curr_arg],"-show_wagered")) {
      local_vars.show_wagered = 1;
      local_vars.quantum_type = QUANTUM_TYPE_WAGERED;
    }
    else if (!strcmp(argv[curr_arg],"-show_table_boss")) {
      local_vars.show_table_boss = 1;
      local_vars.quantum_type = QUANTUM_TYPE_TABLE_BOSS;
    }
    else if (!strcmp(argv[curr_arg],"-show_num_possible_checks")) {
      local_vars.show_num_possible_checks = 1;
      local_vars.quantum_type = QUANTUM_TYPE_NUM_POSSIBLE_CHECKS;
    }
    else if (!strcmp(argv[curr_arg],"-show_running_total")) {
      local_vars.show_running_total = 1;
      local_vars.quantum_type = QUANTUM_TYPE_RUNNING_TOTAL;
    }
    else if (!strcmp(argv[curr_arg],"-show_num_positive_deltas")) {
      local_vars.show_num_positive_deltas = 1;
      local_vars.quantum_type = QUANTUM_TYPE_NUM_POSITIVE_DELTAS;
    }
    else if (!strcmp(argv[curr_arg],"-ace_on_the_river"))
      local_vars.bAceOnTheRiver = true;
    else if (!strcmp(argv[curr_arg],"-wash"))
      local_vars.bWash = true;
    else if (!strcmp(argv[curr_arg],"-sum_quantum"))
      local_vars.sum_quantum = 1;
    else if (!strcmp(argv[curr_arg],"-sum_abs_delta"))
      local_vars.sum_abs_delta = 1;
    else if (!strcmp(argv[curr_arg],"-max_delta"))
      local_vars.max_delta = 1;
    else if (!strcmp(argv[curr_arg],"-min_delta"))
      local_vars.min_delta = 1;
    else if (!strcmp(argv[curr_arg],"-max_abs_delta"))
      local_vars.max_abs_delta = 1;
    else if (!strcmp(argv[curr_arg],"-max_collected"))
      local_vars.max_collected = 1;
    else if (!strcmp(argv[curr_arg],"-max_delta_hand_type"))
      local_vars.max_delta_hand_type = 1;
    else if (!strcmp(argv[curr_arg],"-no_delta"))
      local_vars.bNoDelta = true;
    else if (!strcmp(argv[curr_arg],"-hole_cards_used"))
      local_vars.bHoleCardsUsed = true;
    else if (!strcmp(argv[curr_arg],"-only_suited"))
      local_vars.bOnlySuited = true;
    else if (!strcmp(argv[curr_arg],"-only_nonsuited"))
      local_vars.bOnlyNonsuited = true;
    else if (!strcmp(argv[curr_arg],"-flopped"))
      local_vars.bFlopped = true;
    else if (!strcmp(argv[curr_arg],"-pocket_pair"))
      local_vars.bPocketPair = true;
    else if (!strncmp(argv[curr_arg],"-only_local_vars.hand_number",17))
      sscanf(&argv[curr_arg][17],"%d",&local_vars.hand_number);
    else if (!strcmp(argv[curr_arg],"-didnt_see_flop"))
      local_vars.bDidntSeeFlop = true;
    else if (!strcmp(argv[curr_arg],"-only_winning_session"))
      local_vars.bOnlyWinningSession = true;
    else if (!strcmp(argv[curr_arg],"-only_losing_session"))
      local_vars.bOnlyLosingSession = true;
    else if (!strcmp(argv[curr_arg],"-never_hit_felt_in_session"))
      local_vars.bNeverHitFeltInSession = true;
    else if (!strncmp(argv[curr_arg],"-collected_ge",13)) {
      sscanf(&argv[curr_arg][13],"%d",&local_vars.collected_ge_val);
      local_vars.bCollectedGe = true;
    }
    else if (!strcmp(argv[curr_arg],"-sum_by_table_count")) {
      local_vars.bTerse = true;
      local_vars.bSumByTableCount = true;
    }
    else if (!strcmp(argv[curr_arg],"-show_table_name"))
      local_vars.bShowTableName = true;
    else if (!strcmp(argv[curr_arg],"-show_table_count"))
      local_vars.bShowTableCount = true;
    else if (!strcmp(argv[curr_arg],"-show_seat_numbers"))
      local_vars.bShowSeatNumbers = true;
    else if (!strcmp(argv[curr_arg],"-show_hand_count"))
      local_vars.bShowHandCount = true;
    else if (!strcmp(argv[curr_arg],"-bottom_two"))
      local_vars.bBottomTwo = true;
    else if (!strcmp(argv[curr_arg],"-counterfeit")) {
      local_vars.bCounterfeit = true;
      local_vars.bSawRiver = true;
    }
    else if (!strcmp(argv[curr_arg],"-won_side_pot"))
      local_vars.bWonSidePot = true;
    else if (!strcmp(argv[curr_arg],"-won_main_pot"))
      local_vars.bWonMainPot = true;
    else if (!strcmp(argv[curr_arg],"-last_hand_only"))
      local_vars.bLastHandOnly = true;
    else if (!strcmp(argv[curr_arg],"-except_last_hand"))
      local_vars.bExceptLastHand = true;
    else if (!strcmp(argv[curr_arg],"-first_hand_only"))
      local_vars.bFirstHandOnly = true;
    else if (!strcmp(argv[curr_arg],"-winning_percentage")) {
      local_vars.winning_percentage = 1;
      local_vars.bShowHandCount = true;
    }
    else if (!strcmp(argv[curr_arg],"-filter_percentage")) {
      local_vars.filter_percentage = 1;
      local_vars.bShowHandCount = true;
    }
    else if (!strcmp(argv[curr_arg],"-get_date_from_filename"))
      local_vars.bGetDateFromFilename = true;
    else if (!strcmp(argv[curr_arg],"-no_hole_cards"))
      local_vars.bNoHoleCards = true;
    else if (!strcmp(argv[curr_arg],"-button")) {
      local_vars.bButton = true;
      local_vars.button = 1;
    }
    else if (!strcmp(argv[curr_arg],"-small_blind")) {
      local_vars.bSmallBlind = true;
      local_vars.small_blind = 1;
    }
    else if (!strcmp(argv[curr_arg],"-big_blind")) {
      local_vars.bBigBlind = true;
      local_vars.big_blind = 1;
    }
    else if (!strcmp(argv[curr_arg],"-utg")) {
      local_vars.bUtg = true;
      local_vars.utg = 1;
    }
    else if (!strcmp(argv[curr_arg],"-cutoff")) {
      local_vars.bCutoff = true;
      local_vars.cutoff = 1;
    }
    else if (!strcmp(argv[curr_arg],"-hijack")) {
      local_vars.bHijack = true;
      local_vars.hijack = 1;
    }
    else if (!strcmp(argv[curr_arg],"-other")) {
      local_vars.bOther = true;
      local_vars.other = 1;
    }
    else if (!strcmp(argv[curr_arg],"-vpip"))
      local_vars.bVpip = true;
    else if (!strcmp(argv[curr_arg],"-didnt_vpip"))
      local_vars.bDidntVpip = true;
    else if (!strcmp(argv[curr_arg],"-deuce_or_trey_off"))
      local_vars.bDeuceOrTreyOff = true;
    else if (!strcmp(argv[curr_arg],"-voluntary_bet"))
      local_vars.bVoluntaryBet = true;
    else if (!strcmp(argv[curr_arg],"-no_voluntary_bet"))
      local_vars.bNoVoluntaryBet = true;
    else if (!strcmp(argv[curr_arg],"-chased_flush"))
      local_vars.bChasedFlush = true;
    else if (!strcmp(argv[curr_arg],"-river_card_used"))
      local_vars.bRiverCardUsed = true;
    else if (!strcmp(argv[curr_arg],"-both_hole_cards_used"))
      local_vars.bBothHoleCardsUsed = true;
    else if (!strcmp(argv[curr_arg],"-show_river"))
      local_vars.bShowRiver = true;
    else if (!strcmp(argv[curr_arg],"-bad_river_money"))
      local_vars.bBadRiverMoney = true;
    else if (!strcmp(argv[curr_arg],"-uberflush"))
      local_vars.bUberflush = true;
    else if (!strncmp(argv[curr_arg],"-delta_ge",9)) {
      sscanf(&argv[curr_arg][9],"%d",&local_vars.delta_ge_val);
      local_vars.bDeltaGe = true;
    }
    else if (!strcmp(argv[curr_arg],"-table_boss"))
      local_vars.bTableBoss = true;
    else if (!strcmp(argv[curr_arg],"-only_knockout"))
      local_vars.bOnlyKnockout = true;
    else if (!strcmp(argv[curr_arg],"-only_double_up"))
      local_vars.bOnlyDoubleUp = true;
    else if (!strcmp(argv[curr_arg],"-only_discrepancy")) {
      local_vars.bOnlyDiscrepancy = true;
      local_vars.quantum_type = QUANTUM_TYPE_DISCREPANCY;
    }
    else if (!strcmp(argv[curr_arg],"-show_roi")) {
      local_vars.show_roi = 1;
      local_vars.quantum_type = QUANTUM_TYPE_ROI;
    }
    else if (!strcmp(argv[curr_arg],"-sitting_out"))
      local_vars.bSittingOut = true;
    else if (!strcmp(argv[curr_arg],"-broadway"))
      local_vars.bBroadway = true;
    else if (!strcmp(argv[curr_arg],"-twin_abbrevs"))
      local_vars.bTwinAbbrevs = true;
    else if (!strcmp(argv[curr_arg],"-twin_hands"))
      local_vars.bTwinHands = true;
    else if (!strcmp(argv[curr_arg],"-identical_twin_hands"))
      local_vars.bIdenticalTwinHands = true;
    else if (!strcmp(argv[curr_arg],"-hut_outs"))
      local_vars.bHutOuts = true;
    else if (!strcmp(argv[curr_arg],"-hut_outs_diff"))
      local_vars.bHutOutsDiff = true;
    else if (!strncmp(argv[curr_arg],"-hut_wins_ge_value",18)) {
      local_vars.bHutWinsGeValue = true;
      sscanf(&argv[curr_arg][18],"%d",&local_vars.hut_wins_ge_value);
      local_vars.bHutOuts = true;
    }
    else if (!strncmp(argv[curr_arg],"-hut_wins_le_value",18)) {
      local_vars.bHutWinsLeValue = true;
      sscanf(&argv[curr_arg][18],"%d",&local_vars.hut_wins_le_value);
      local_vars.bHutOuts = true;
    }
    else if (!strncmp(argv[curr_arg],"-hut_wins_eq_value",18)) {
      local_vars.bHutWinsEqValue = true;
      sscanf(&argv[curr_arg][18],"%d",&local_vars.hut_wins_eq_value);
      local_vars.bHutOuts = true;
    }
    else if (!strncmp(argv[curr_arg],"-hut_losses_ge_value",20)) {
      local_vars.bHutLossesGeValue = true;
      sscanf(&argv[curr_arg][20],"%d",&local_vars.hut_losses_ge_value);
      local_vars.bHutOuts = true;
    }
    else if (!strncmp(argv[curr_arg],"-hut_losses_le_value",20)) {
      local_vars.bHutLossesLeValue = true;
      sscanf(&argv[curr_arg][20],"%d",&local_vars.hut_losses_le_value);
      local_vars.bHutOuts = true;
    }
    else if (!strncmp(argv[curr_arg],"-hut_losses_eq_value",20)) {
      local_vars.bHutLossesEqValue = true;
      sscanf(&argv[curr_arg][20],"%d",&local_vars.hut_losses_eq_value);
      local_vars.bHutOuts = true;
    }
    else if (!strncmp(argv[curr_arg],"-hut_ties_ge_value",18)) {
      local_vars.bHutTiesGeValue = true;
      sscanf(&argv[curr_arg][18],"%d",&local_vars.hut_ties_ge_value);
      local_vars.bHutOuts = true;
    }
    else if (!strncmp(argv[curr_arg],"-hut_ties_le_value",18)) {
      local_vars.bHutTiesLeValue = true;
      sscanf(&argv[curr_arg][18],"%d",&local_vars.hut_ties_le_value);
      local_vars.bHutOuts = true;
    }
    else if (!strncmp(argv[curr_arg],"-hut_ties_eq_value",18)) {
      local_vars.bHutTiesEqValue = true;
      sscanf(&argv[curr_arg][18],"%d",&local_vars.hut_ties_eq_value);
      local_vars.bHutOuts = true;
    }
    else if (!strcmp(argv[curr_arg],"-huf_outs"))
      local_vars.bHufOuts = true;
    else if (!strcmp(argv[curr_arg],"-3t_outs"))
      local_vars.b3tOuts = true;
    else if (!strcmp(argv[curr_arg],"-magic_flush"))
      local_vars.bMagicFlush = true;
    else if (!strcmp(argv[curr_arg],"-any_all_in"))
      local_vars.bAnyAllIn = true;
    else if (!strcmp(argv[curr_arg],"-no_all_in"))
      local_vars.bNoAllIn = true;
    else if (!strcmp(argv[curr_arg],"-ace_rag"))
      local_vars.bAceRag = true;
    else if (!strcmp(argv[curr_arg],"-suited_ace"))
      local_vars.bSuitedAce = true;
    else if (!strcmp(argv[curr_arg],"-ace_non_rag"))
      local_vars.bAceNonRag = true;
    else if (!strcmp(argv[curr_arg],"-only_uncalled"))
      local_vars.bOnlyUncalled = true;
    else if (!strcmp(argv[curr_arg],"-first_file_only"))
      bFirstFileOnly = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if (local_vars.bTerse && local_vars.bVerbose) {
    printf("can't specify both -terse and -verbose\n");
    return 3;
  }

  if (local_vars.bSawFlop && local_vars.bSawRiver) {
    printf("can't specify both -saw_flop and -saw_river\n");
    return 4;
  }

  if (local_vars.bAbbrev && local_vars.bShowHand) {
    printf("can't specify both -abbrev and -show_hand\n");
    return 5;
  }

  if (local_vars.bAbbrev && local_vars.bShowHandType) {
    printf("can't specify both -abbrev and -show_hand_type\n");
    return 6;
  }

  if (local_vars.bAbbrev && local_vars.bShowHandTypId) {
    printf("can't specify both -abbrev and -show_hand_typ_id\n");
    return 7;
  }

  if (local_vars.bStealthTwoPair && local_vars.bBottomTwo) {
    printf("can't specify both -stealth_two_pair and -bottom_two\n");
    return 8;
  }

  if (local_vars.bAbbrev && local_vars.bStealthTwoPair) {
    printf("can't specify both -abbrev and -stealth_two_pair\n");
    return 9;
  }

  if (local_vars.bAbbrev && local_vars.bBottomTwo) {
    printf("can't specify both -abbrev and -bottom_two\n");
    return 10;
  }

  if (local_vars.bSkipFolded && local_vars.bOnlyFolded) {
    printf("can't specify both -skip_folded and -only_folded\n");
    return 11;
  }

  if (!local_vars.bSawRiver && local_vars.bRiverMoney)
    local_vars.bSawRiver = true;

  if (local_vars.bRiverMoney && local_vars.bNoRiverMoney) {
    printf("can't specify both -river_money and -no_river_money\n");
    return 12;
  }

  if (local_vars.bStealthTwoPair && local_vars.bBottomTwo) {
    printf("can't specify both -stealth_two_pair and -hand_typehand_type\n");
    return 13;
  }

  if (local_vars.bBottomTwo && local_vars.bHandTypeSpecified) {
    printf("can't specify both -bottom_two and -hand_typehand_type\n");
    return 14;
  }

  if (local_vars.bStealthTwoPair && local_vars.bHandTypIdSpecified) {
    printf("can't specify both -stealth_two_pair and -hand_typ_idid\n");
    return 15;
  }

  if (local_vars.bBottomTwo && local_vars.bHandTypIdSpecified) {
    printf("can't specify both -bottom_two and -hand_typ_idid\n");
    return 16;
  }

  if (local_vars.bStealthTwoPair && local_vars.bHandSpecified) {
    printf("can't specify both -stealth_two_pair and -hand\n");
    return 17;
  }

  if (local_vars.bBottomTwo && local_vars.bHandSpecified) {
    printf("can't specify both -bottom_two and -hand\n");
    return 18;
  }

  if (local_vars.bStealthTwoPair && !local_vars.bSawFlop && !local_vars.bSawRiver)
    local_vars.bSawFlop = true;

  if (local_vars.bBottomTwo && !local_vars.bSawFlop && !local_vars.bSawRiver)
    local_vars.bSawFlop = true;

  if (local_vars.bUberflush) {
    local_vars.bLost = true;
    local_vars.hand_typ_id = FLUSH;
    local_vars.bHandTypeSpecified = true;
  }

  if (local_vars.bBroadway) {
    local_vars.hand_typ_id = STRAIGHT;
    local_vars.bHandTypeSpecified = true;
  }

  if (local_vars.bMagicFlush) {
    local_vars.hand_typ_id = FLUSH;
    local_vars.bHandTypeSpecified = true;
  }

  if (local_vars.bHandTypeSpecified && !local_vars.bSawFlop && !local_vars.bSawRiver)
    local_vars.bSawFlop = true;

  if (local_vars.bHandTypeOnFlopSpecified && !local_vars.bSawFlop && !local_vars.bSawRiver)
    local_vars.bSawFlop = true;

  if (local_vars.bHandTypIdSpecified && !local_vars.bSawFlop && !local_vars.bSawRiver)
    local_vars.bSawFlop = true;

  if (local_vars.bHandTypIdGeSpecified && !local_vars.bSawFlop && !local_vars.bSawRiver)
    local_vars.bSawFlop = true;

  if (local_vars.bLost && local_vars.bWon) {
    printf("can't specify both -lost and -won\n");
    return 19;
  }

  if (local_vars.bSkipZero && local_vars.bOnlyZero) {
    printf("can't specify both -skip_zero and -only_zero\n");
    return 20;
  }

  if (local_vars.bShowdown && local_vars.bNoShowdown) {
    printf("can't specify both -showdown and -no_showdown\n");
    return 21;
  }

  if (local_vars.bAllIn && local_vars.bNotAllIn) {
    printf("can't specify both -all_in and -not_all_in\n");
    return 22;
  }

  if (local_vars.bAllIn && local_vars.bAllInPreflop) {
    printf("can't specify both -all_in and -all_in_preflop\n");
    return 23;
  }

  if (local_vars.bNotAllIn && local_vars.bAllInPreflop) {
    printf("can't specify both -not_all_in and -all_in_preflop\n");
    return 24;
  }

  if (local_vars.show_collected +
    local_vars.show_voluntarily_spent + local_vars.show_involuntarily_spent + local_vars.show_spent +
    local_vars.show_uncalled + local_vars.show_rollback + local_vars.show_opm + local_vars.show_num_decisions +
    local_vars.show_wagered + local_vars.show_table_boss + local_vars.show_num_possible_checks +
    local_vars.show_running_total + local_vars.show_num_positive_deltas +
    local_vars.show_roi > 1) {
    printf("can only specify one of -show_collected,\n"
      "  -show_voluntarily_spent, -show_involuntarily_spent, -show_spent, -show_uncalled,\n"
      "  -show_rollback, -show_opm, -show_num_decisions, -show_wagered, -show_table_boss,\n"
      "  -show_num_possible_checks, -show_running_total, -show_num_positive_deltas,\n"
      "  and -show_roi\n");
    return 25;
  }

  if (local_vars.bLost && local_vars.bWash) {
    printf("can't specify both -lost and -wash\n");
    return 26;
  }

  if (local_vars.bWon && local_vars.bWash) {
    printf("can't specify both -won and -wash\n");
    return 27;
  }

  if (local_vars.only_count + local_vars.sum_quantum + local_vars.sum_abs_delta + local_vars.max_delta + local_vars.min_delta +
    local_vars.max_abs_delta + local_vars.max_collected + local_vars.max_delta_hand_type +
    local_vars.winning_percentage + local_vars.filter_percentage > 1) {
    printf("can only specify one of -only_count, -sum_quantum, -sum_abs_delta, -max_delta, -min_delta, -max_abs_delta, -max_collected, -max_delta_hand_type, -winning_percentage, and filter_percentage\n");
    return 28;
  }

  if ((local_vars.only_count) || (local_vars.sum_quantum) || (local_vars.sum_abs_delta) || (local_vars.max_delta) ||
      (local_vars.min_delta) || (local_vars.max_abs_delta) || (local_vars.max_collected) ||
      (local_vars.max_delta_hand_type) || (local_vars.winning_percentage) || (local_vars.filter_percentage)) {
    local_vars.bTerse = true;
    local_vars.bGetDateFromFilename = true;
    local_vars.bSummarizing = true;
  }
  else
    local_vars.bSummarizing = false;

  if (local_vars.bHoleCardsUsed) {
    if (local_vars.bAbbrev) {
      printf("can't specify both -abbrev and -hole_cards_used\n");
      return 29;
    }

    local_vars.bSawFlop = true;
  }

  if (local_vars.bHitFelt && local_vars.bDidntHitFelt) {
    printf("can't specify both -hit_felt and -didnt_hit_felt\n");
    return 30;
  }

  if (local_vars.bVeryBestHand && local_vars.bFlopped) {
    printf("can't specify both -very_best_hand and -flopped\n");
    return 31;
  }

  if (local_vars.bHandTypeSpecified && local_vars.bHandTypIdSpecified) {
    printf("can't specify both -hand_typehand_type and -hand_typ_idid\n");
    return 32;
  }

  if (local_vars.bShowHandType && local_vars.bShowHandTypId) {
    printf("can't specify both -show_hand_type and -show_hand_typ_id\n");
    return 33;
  }

  if (local_vars.bSawFlop && local_vars.bDidntSeeFlop) {
    printf("can't specify both -saw_flop and -didnt_see_flop\n");
    return 34;
  }

  if (local_vars.bOnlyWinningSession && local_vars.bOnlyLosingSession) {
    printf("can't specify both -only_winning_session and -only_losing_session\n");
    return 35;
  }

  if (local_vars.bSumByTableCount && local_vars.bShowTableName) {
    printf("can't specify both -sum_by_table_count and -show_table_name\n");
    return 36;
  }

  if (local_vars.bSumByTableCount && local_vars.bShowTableCount) {
    printf("can't specify both -sum_by_table_count and -show_table_count\n");
    return 37;
  }

  if (local_vars.button + local_vars.small_blind + local_vars.big_blind +
     local_vars.utg + local_vars.cutoff + local_vars.hijack + local_vars.other > 1) {
    printf("can only specify one of -button, -small_blind, -big_blind, -utg, -cutoff, -hijack, and -other\n");
    return 38;
  }

  if (local_vars.bHandSpecified && local_vars.bDeuceOrTreyOff) {
    printf("can't specify both -handhand and -deuce_or_trey_off\n");
    return 39;
  }

  if (local_vars.bVoluntaryBet && local_vars.bNoVoluntaryBet) {
    printf("can't specify both -voluntary_bet and -no_voluntary_bet\n");
    return 40;
  }

  if (local_vars.bShowBoard && local_vars.bShowRiver) {
    printf("can't specify both -show_board and -show_river\n");
    return 41;
  }

  if (local_vars.bHandTypIdSpecified && local_vars.bHandTypIdGeSpecified) {
    printf("can't specify both -hand_typ_idid and -hand_typ_id_geid\n");
    return 42;
  }

  if (local_vars.bStealthTwoPair && local_vars.bHandTypIdGeSpecified) {
    printf("can't specify both -stealth_two_pair and -hand_typ_id_geid\n");
    return 43;
  }

  if (local_vars.bBottomTwo && local_vars.bHandTypIdGeSpecified) {
    printf("can't specify both -bottom_two and -hand_typ_id_geid\n");
    return 44;
  }

  if (local_vars.bHandTypeSpecified && local_vars.bHandTypIdGeSpecified) {
    printf("can't specify both -hand_typehand_type and -hand_typ_id_geid\n");
    return 45;
  }

  if (local_vars.bAllIn && local_vars.bAllInPostflop) {
    printf("can't specify both -all_in and -all_in_postflop\n");
    return 46;
  }

  if (local_vars.bAllInPreflop && local_vars.bAllInPostflop) {
    printf("can't specify both -all_in_preflop and -all_in_postflop\n");
    return 47;
  }

  if (local_vars.bFallIn && local_vars.bNotFallIn) {
    printf("can't specify both -fall_in and -not_fall_in\n");
    return 48;
  }

  if (local_vars.bTwinAbbrevs && local_vars.bTwinHands) {
    printf("can't specify both -twin_abbrevs and -twin_hands\n");
    return 49;
  }

  if (local_vars.bTwinAbbrevs && local_vars.bIdenticalTwinHands) {
    printf("can't specify both -twin_abbrevs and -identical_twin_hands\n");
    return 50;
  }

  if (local_vars.bTwinHands && local_vars.bIdenticalTwinHands) {
    printf("can't specify both -twin_hands and -identical_twin_hands\n");
    return 51;
  }

  if (local_vars.bLastHandOnly && local_vars.bExceptLastHand) {
    printf("can't specify both -last_hand_only and -except_last_hand\n");
    return 52;
  }

  if (local_vars.bHutWinsGeValue && local_vars.bHutWinsLeValue) {
    printf("can't specify both -hut_wins_ge_value and -hut_wins_le_value\n");
    return 53;
  }

  if (local_vars.bHutWinsGeValue && local_vars.bHutWinsEqValue) {
    printf("can't specify both -hut_wins_ge_value and -hut_wins_eq_value\n");
    return 54;
  }

  if (local_vars.bHutWinsLeValue && local_vars.bHutWinsEqValue) {
    printf("can't specify both -hut_wins_le_value and -hut_wins_eq_value\n");
    return 55;
  }

  if (local_vars.bHutLossesGeValue && local_vars.bHutLossesLeValue) {
    printf("can't specify both -hut_losses_ge_value and -hut_losses_le_value\n");
    return 56;
  }

  if (local_vars.bHutLossesGeValue && local_vars.bHutLossesEqValue) {
    printf("can't specify both -hut_losses_ge_value and -hut_losses_eq_value\n");
    return 57;
  }

  if (local_vars.bHutLossesLeValue && local_vars.bHutLossesEqValue) {
    printf("can't specify both -hut_losses_le_value and -hut_losses_eq_value\n");
    return 58;
  }

  if (local_vars.bHutTiesGeValue && local_vars.bHutTiesLeValue) {
    printf("can't specify both -hut_ties_ge_value and -hut_ties_le_value\n");
    return 59;
  }

  if (local_vars.bHutTiesGeValue && local_vars.bHutTiesEqValue) {
    printf("can't specify both -hut_ties_ge_value and -hut_ties_eq_value\n");
    return 60;
  }

  if (local_vars.bHutTiesLeValue && local_vars.bHutTiesEqValue) {
    printf("can't specify both -hut_ties_le_value and -hut_ties_eq_value\n");
    return 61;
  }

  if (local_vars.bPremium && local_vars.bNotPremium) {
    printf("can't specify both -premium and -not_premium\n");
    return 62;
  }

  if (local_vars.bVpip && local_vars.bDidntVpip) {
    printf("can't specify both -vpip and -didnt_vpip\n");
    return 63;
  }

  if (local_vars.bHutOuts || local_vars.bHutOutsDiff || local_vars.bHufOuts) {
    local_vars.bShowdown = true;
    local_vars.bShowdownCount = true;
    local_vars.showdown_count = 2;
  }
  else if (local_vars.b3tOuts) {
    local_vars.bShowdown = true;
    local_vars.bShowdownCount = true;
    local_vars.showdown_count = 3;
  }

  player_name_ix = curr_arg++;
  player_name_len = strlen(argv[player_name_ix]);

  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 64;
  }

  if (!local_vars.bSawRiver && (local_vars.bChasedFlush || local_vars.bRiverCardUsed || local_vars.bShowRiver))
    local_vars.bSawRiver = true;

  local_vars.ending_balance = -1;

  file_no = 0;
  dbg_file_no = -1;
  hand_no = 0;

  local_vars.hole_cards_abbrev[3] = 0;
  local_vars.winning_hand_hole_cards_abbrev[3] = 0;
  local_vars.opponent_hole_cards_abbrev[3] = 0;

  if (!local_vars.bAbbrev)
    local_vars.hole_cards[11] = 0;

  local_vars.winning_hand_hole_cards[5] = 0;
  local_vars.opponent_hole_cards[5] = 0;

  if (local_vars.bSumByTableCount) {
    for (n = 0; n < MAX_TABLE_COUNT - 1; n++) {
      local_vars.sum_by_table_count[n] = 0;
      local_vars.sum_by_table_count[n] = 0;
    }
  }

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    if (local_vars.bGetDateFromFilename) {
      retval = get_date_from_path(filename,&local_vars.date_string);

      if (retval) {
        printf("get_date_from_path() failed on %s: %d\n",filename,retval);
        continue;
      }
    }

    file_no++;

    if (local_vars.bSummarizing) {
      local_vars.total_delta = 0;
      local_vars.hit_felt_in_session_count = 0;
      local_vars.summary_val = 0;
      local_vars.summary_val2 = 0;
    }

    if (dbg_file_no == file_no)
      dbg = 1;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    if (local_vars.bLastHandOnly || local_vars.bExceptLastHand) {
      local_vars.num_hands_in_file = get_num_hands_in_file(fptr,
        argv[player_name_ix],player_name_len);
    }

    local_vars.line_no = 0;
    local_vars.bFolded = false;
    local_vars.bFoldedPreflop = false;
    local_vars.bFoldedOnTheFlop = false;
    local_vars.bFoldedOnTheTurn = false;
    local_vars.bFoldedOnTheRiver = false;
    local_vars.bSkipping = false;
    local_vars.num_hands = 0;
    local_vars.bHaveSpecifiedHand = false;
    local_vars.bHaveSpecifiedShowdownHand = false;
    local_vars.bHaveSpecifiedShowdownHand2 = false;
    local_vars.bHaveSpecifiedWinningHand = false;
    local_vars.bHaveWinningHandHoleCards = false;
    local_vars.bHaveOpponentHoleCards = false;
    local_vars.bHaveFlop = false;
    local_vars.bHaveFlop2 = false;
    local_vars.bHaveTurn2 = false;
    local_vars.bHaveRiver = false;
    local_vars.bHaveRiver2 = false;
    local_vars.bHaveAceOnTheRiver = false;
    local_vars.bSpentRiverMoney = false;
    local_vars.bHaveStealthTwoPair = false;
    local_vars.bHaveBottomTwo = false;
    local_vars.bHaveShowdown = false;
    local_vars.bHaveTableShowdown = false;
    showdown_count = 0;
    local_vars.bHaveShowdownCount = false;
    local_vars.bHaveShowdownCountGt = false;
    local_vars.bHaveAllIn = false;
    local_vars.bHaveAllInPreflop = false;
    local_vars.bHaveAllInPostflop = false;
    local_vars.bHaveCallIn = false;
    local_vars.bHaveCallInOnTheRiver = false;
    local_vars.bHaveFallIn = false;
    local_vars.bHaveCounterfeit = false;
    local_vars.bPostedSmallBlind = false;
    local_vars.bPostedBigBlind = false;
    local_vars.bAmButton = false;
    local_vars.bAmUtg = false;
    local_vars.bAmCutoff = false;
    local_vars.bAmHijack = false;
    local_vars.bHaveDeuceOrTreyOff = false;
    local_vars.bHaveVoluntaryBet = false;
    local_vars.bHaveChasedFlush = false;
    local_vars.bHaveBadRiverMoney = false;
    local_vars.bHaveDiscrepancy = false;
    local_vars.bHavePremiumHand = false;
    local_vars.bHaveNotPremiumHand = false;
    local_vars.bHaveMagicFlush = false;
    local_vars.bHaveAnyAllIn = false;
    local_vars.bHaveAceRag = false;
    local_vars.bHaveSuitedAce = false;
    local_vars.bHaveAceNonRag = false;
    bFirstHand = true;

    if (local_vars.bWonSidePot)
      local_vars.bHaveWonSidePot = false;

    if (local_vars.bWonMainPot)
      local_vars.bHaveWonMainPot = false;

    if (local_vars.show_num_decisions)
      local_vars.numdecs = 0;

    if (local_vars.show_num_possible_checks)
      local_vars.num_possible_checks = 0;

    if (local_vars.show_running_total)
      local_vars.running_total = 0;

    if (local_vars.show_num_positive_deltas)
      local_vars.num_positive_deltas = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr)) {
        run_filter(&local_vars);
        hand_no++;

        if (local_vars.bSummarizing) {
          if (!local_vars.bSkipZero || local_vars.summary_val) {
            if (!local_vars.bOnlyWinningSession || (local_vars.total_delta > 0)) {
              if (!local_vars.bOnlyLosingSession || (local_vars.total_delta < 0)) {
                if (!local_vars.bNeverHitFeltInSession || (!local_vars.hit_felt_in_session_count)) {
                  if (!local_vars.bExactCount || (local_vars.summary_val == local_vars.exact_count)) {
                    if (!local_vars.max_delta_hand_type) {
                      if (!local_vars.bShowHandCount)
                        printf("%d\t%s\n",local_vars.summary_val,local_vars.date_string);
                      else {
                        if (local_vars.winning_percentage || local_vars.filter_percentage) {
                          if (!local_vars.summary_val)
                            local_vars.dwork = (double)0;
                          else
                            local_vars.dwork = (double)local_vars.summary_val2 / (double)local_vars.summary_val;

                          printf("%lf\t%s\t(%d %d)\n",local_vars.dwork,local_vars.date_string,local_vars.summary_val2,local_vars.summary_val);
                        }
                        else
                          printf("%d\t%s\t(%d)\n",local_vars.summary_val,local_vars.date_string,local_vars.num_hands);
                      }
                    }
                    else {
                      if (!local_vars.bShowHandCount) {
                        printf("%d\t%s\t%s\n",local_vars.summary_val,
                          plain_hand_types[local_vars.max_delta_hand_typ],local_vars.date_string);
                      }
                      else {
                        printf("%d\t%s\t%s\t(%d)\n",local_vars.summary_val,
                          plain_hand_types[local_vars.max_delta_hand_typ],local_vars.date_string,local_vars.num_hands);
                      }
                    }
                  }
                }
              }
            }
          }
        }

        break;
      }

      local_vars.line_no++;

      if (local_vars.line_no == dbg_line_no)
        dbg = 1;

      if (local_vars.debug_level == 3)
        printf("file %d hand %d line %d %s\n",file_no,file_no,hand_no,local_vars.line_no,line);

      if (Contains(true,
        line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
        pokerstars,POKERSTARS_LEN,
        &ix)) {

        if (local_vars.bTimestamp)
          populate_timestamp(line,local_vars.timestamp);

#ifdef STUD_AND_RAZZ
        local_vars.bStud = false;
        local_vars.bRazz = false;

        if (Contains(true,
          line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
          stud,STUD_LEN,
          &ix)) {

          local_vars.bStud = true;
        }
        else if (Contains(true,
          line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
          razz,RAZZ_LEN,
          &ix)) {

          local_vars.bRazz = true;
        }

        if (local_vars.bStud || local_vars.bRazz)
          max_streets = 4;
        else
#endif
          max_streets = 3;
      }
      else if (!strncmp(line,"Table '",7)) {
        local_vars.button_seat = get_button_seat(line);

        if (bFirstHand)
          bFirstHand = false;
        else {
          run_filter(&local_vars);
          hand_no++;
        }

        if (local_vars.bTwinAbbrevs)
          local_vars.prev_abbrev_index = local_vars.curr_abbrev_index;
        else if (local_vars.bTwinHands)
          local_vars.prev_52_2_index = local_vars.curr_52_2_index;
        else if (local_vars.bIdenticalTwinHands)
          local_vars.prev_52_2_index2 = local_vars.curr_52_2_index2;

        get_table_name(line,line_len,table_name,MAX_TABLE_NAME_LEN);

        local_vars.table_count = 0;
        local_vars.vpip = 0;
        boss_stack = 0;
        local_vars.am_table_boss = 0;
        local_vars.bHaveKnockout = false;
        local_vars.bHaveDoubleUp = false;
        local_vars.bIsSittingOut = false;

        for ( ; ; ) {
          GetLine(fptr,line,&line_len,MAX_LINE_LEN);

          if (feof(fptr))
            break;

          local_vars.line_no++;

          if (local_vars.line_no == dbg_line_no)
            dbg = 1;

          if (!strncmp(line,"*** ",4)) {
            num_street_markers++;

            if (local_vars.my_seat_ix == boss_seat_ix)
              local_vars.am_table_boss = 1;

            break;
          }

          if (!strncmp(line,"Seat ",5)) {
            sscanf(&line[5],"%d",&curr_seat);
            local_vars.seat_numbers[local_vars.table_count++] = line[5];
            local_vars.seat_numbers[local_vars.table_count] = 0;

            if (Contains(true,
              line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
              in_chips,IN_CHIPS_LEN,
              &ix)) {

              line[ix] = 0;

              for (ix--; (ix >= 0) && (line[ix] != '('); ix--)
                ;

              sscanf(&line[ix+1],"%d",&curr_stack);

              if (curr_stack > boss_stack) {
                boss_stack = curr_stack;
                boss_seat_ix = local_vars.table_count - 1;
              }

              if (Contains(true,
                line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
                argv[player_name_ix],player_name_len,
                &ix)) {

                if (curr_seat == local_vars.button_seat)
                  local_vars.bAmButton = true;
                else
                  local_vars.bAmButton = false;

                local_vars.my_seat_ix = local_vars.table_count - 1;
                local_vars.starting_balance = curr_stack;
                local_vars.num_hands++;
                local_vars.bFolded = false;
                local_vars.bFoldedPreflop = false;
                local_vars.bFoldedOnTheFlop = false;
                local_vars.bFoldedOnTheTurn = false;
                local_vars.bFoldedOnTheRiver = false;
                local_vars.bSkipping = false;
                local_vars.bHaveSpecifiedHand = false;
                local_vars.bHaveSpecifiedShowdownHand = false;
                local_vars.bHaveSpecifiedShowdownHand2 = false;
                local_vars.bHaveSpecifiedWinningHand = false;
                local_vars.bHaveWinningHandHoleCards = false;
                local_vars.bHaveOpponentHoleCards = false;
                local_vars.bHaveFlop = false;
                local_vars.bHaveFlop2 = false;
                local_vars.bHaveTurn2 = false;
                local_vars.bHaveRiver = false;
                local_vars.bHaveRiver2 = false;
                local_vars.bHaveAceOnTheRiver = false;
                local_vars.bSpentRiverMoney = false;
                local_vars.bHaveStealthTwoPair = false;
                local_vars.bHaveBottomTwo = false;
                local_vars.bHaveShowdown = false;
                local_vars.bHaveTableShowdown = false;
                showdown_count = 0;
                local_vars.bHaveShowdownCount = false;
                local_vars.bHaveShowdownCountGt = false;
                local_vars.bHaveAllIn = false;
                local_vars.bHaveAllInPreflop = false;
                local_vars.bHaveAllInPostflop = false;
                local_vars.bHaveCallIn = false;
                local_vars.bHaveCallInOnTheRiver = false;
                local_vars.bHaveFallIn = false;
                local_vars.bHaveCounterfeit = false;
                local_vars.bPostedSmallBlind = false;
                local_vars.bPostedBigBlind = false;
                local_vars.bHaveDeuceOrTreyOff = false;
                local_vars.bHaveVoluntaryBet = false;
                local_vars.bHaveChasedFlush = false;
                local_vars.bHaveBadRiverMoney = false;
                local_vars.bHaveDiscrepancy = false;
                local_vars.bHavePremiumHand = false;
                local_vars.bHaveNotPremiumHand = false;
                local_vars.bHaveMagicFlush = false;
                local_vars.bHaveAnyAllIn = false;
                local_vars.bHaveAceRag = false;
                local_vars.bHaveSuitedAce = false;
                local_vars.bHaveAceNonRag = false;

                if (local_vars.bWonSidePot)
                  local_vars.bHaveWonSidePot = false;

                if (local_vars.bWonMainPot)
                  local_vars.bHaveWonMainPot = false;

                if (local_vars.show_num_decisions)
                  local_vars.numdecs = 0;

                if (local_vars.show_num_possible_checks)
                  local_vars.num_possible_checks = 0;

                if (local_vars.show_num_positive_deltas)
                  local_vars.num_positive_deltas = 0;

                street = 0;
                num_street_markers = 0;
                ante = 0;
                bring_in = 0;
                voluntarily_spent_this_street = 0;
                involuntarily_spent_this_street = 0;
                local_vars.voluntarily_spent_this_hand = 0;
                local_vars.involuntarily_spent_this_hand = 0;
                local_vars.uncalled_bet_amount = 0;
                local_vars.collected_from_pot = 0;
                local_vars.collected_from_pot_count = 0;

                if (local_vars.debug_level == 1)
                  printf("file %d hand %d line %d starting_balance = %d\n",file_no,hand_no,local_vars.line_no,local_vars.starting_balance);

                if (Contains(true,
                  line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
                  sitting_out,SITTING_OUT_LEN,
                  &ix)) {
                  local_vars.bIsSittingOut = true;
                }
              }
            }
          }
          else if (Contains(true,
            line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
            argv[player_name_ix],player_name_len,
            &ix)) {

            if (Contains(true,
              line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
              posts_the_ante,POSTS_THE_ANTE_LEN,
              &ix)) {
              ante = get_work_amount(line,line_len);
              local_vars.involuntarily_spent_this_hand = ante;

              if (Contains(true,
                line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
                all_in,ALL_IN_LEN,
                &ix)) {

                local_vars.bHaveAllIn = true;
                local_vars.bHaveFallIn = true;
                local_vars.bHaveAnyAllIn = true;
              }
            }
            else if (Contains(true,
              line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
              posts,POSTS_LEN,
              &ix)) {
              local_vars.work = get_work_amount(line,line_len);
              involuntarily_spent_this_street += local_vars.work;

              if (Contains(true,
                line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
                posts_small_blind,POSTS_SMALL_BLIND_LEN,
                &ix)) {

                local_vars.bPostedSmallBlind = true;

                if (local_vars.debug_level == 2) {
                  printf("set bPostedSmallBlind\n");
                  printf("file %d hand %d line %d: line %d\n",file_no,hand_no,__LINE__,local_vars.line_no);
                }
              }
              else if (Contains(true,
                line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
                posts_big_blind,POSTS_BIG_BLIND_LEN,
                &ix)) {

                local_vars.bPostedBigBlind = true;
              }

              if (local_vars.debug_level == 1) {
                printf("file %d hand %d line %d street %d POSTS work = %d, voluntarily_spent_this_street = %d, num_street_markers = %d, %d\n",
                  file_no,hand_no,local_vars.line_no,street,local_vars.work,voluntarily_spent_this_street,num_street_markers,
                  (street == num_street_markers));
              }

              if (Contains(true,
                line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
                all_in,ALL_IN_LEN,
                &ix)) {

                local_vars.bHaveAllIn = true;
                local_vars.bHaveFallIn = true;
                local_vars.bHaveAnyAllIn = true;
              }
            }
          }
        }

        if (local_vars.table_count > MAX_TABLE_COUNT) {
          printf("%s: too many players at the table\n",filename);
          return 65;
        }

        continue;
      }
      else if (local_vars.bSkipping) {
        if (local_vars.bWinningHandTypeSpecified) {
          if (Contains(true,
            line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
            won,WON_LEN,
            &ix)) {

            local_vars.curr_winning_hand_typ_id = get_winning_hand_typ_id(line,line_len);
          }
        }
        else if (local_vars.bShowdownHandSpecified && !local_vars.bHaveSpecifiedShowdownHand) {
          if (Contains(true,
            line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
            showed,SHOWED_LEN,
            &ix)) {

            retval = index_of_hand(&line[ix+SHOWED_LEN],&work_hand_index);

            if (!retval) {
              if (work_hand_index == local_vars.specified_showdown_hand_index)
                local_vars.bHaveSpecifiedShowdownHand = true;
            }
          }
        }
        else if (local_vars.bShowdownHand2Specified && !local_vars.bHaveSpecifiedShowdownHand2) {
          if (Contains(true,
            line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
            showed,SHOWED_LEN,
            &ix)) {

            retval = index_of_hand(&line[ix+SHOWED_LEN],&work_hand_index);

            if (!retval) {
              if (work_hand_index == local_vars.specified_showdown_hand2_index)
                local_vars.bHaveSpecifiedShowdownHand2 = true;
            }
          }
        }
        else if (local_vars.bWinningHandSpecified && !local_vars.bHaveSpecifiedWinningHand) {
          if (Contains(true,
            line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
            showed,SHOWED_LEN,
            &showed_ix)) {

            if (Contains(true,
              line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
              and_won,AND_WON_LEN,
              &ix)) {
              retval = index_of_hand(&line[showed_ix+SHOWED_LEN],&work_hand_index);

              if (!retval) {
                if (hand_ix_match(work_hand_index,&local_vars.specified_winning_hand_hand_ixs))
                  local_vars.bHaveSpecifiedWinningHand = true;
              }
            }
          }
        }
        else if (local_vars.bShowWinningHandHoleCards && !local_vars.bHaveWinningHandHoleCards) {
          if (Contains(true,
            line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
            showed,SHOWED_LEN,
            &showed_ix)) {

            if (Contains(true,
              line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
              and_won,AND_WON_LEN,
              &ix)) {

              for (n = 0; n < 5; n++)
                local_vars.winning_hand_hole_cards[n] = line[showed_ix + SHOWED_LEN + n];

              for (n = 0; n < 2; n++)
                retval = card_value_from_card_string(&local_vars.winning_hand_hole_cards[n*3],&local_vars.winning_hand_hole_card_ixs[n]);

              get_abbrev(local_vars.winning_hand_hole_cards,&local_vars.winning_hand_hole_cards_abbrev[0]);

              local_vars.bHaveWinningHandHoleCards = true;
            }
          }
        }
        else if (!local_vars.bHaveOpponentHoleCards) {
          if (Contains(true,
            line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
            "[",1,
            &bracket_ix)) {

            if (!Contains(true,
              line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
              argv[player_name_ix],player_name_len,
              &ix)) {

              if (!Contains(true,
                line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
                board,BOARD_LEN,
                &ix)) {

                for (n = 0; n < 5; n++)
                  local_vars.opponent_hole_cards[n] = line[bracket_ix + 1 + n];

                for (n = 0; n < 2; n++)
                  retval = card_value_from_card_string(&local_vars.opponent_hole_cards[n*3],&local_vars.opponent_hole_card_ixs[n]);

                get_abbrev(local_vars.opponent_hole_cards,&local_vars.opponent_hole_cards_abbrev[0]);

                local_vars.bHaveOpponentHoleCards = true;
              }
            }
          }
        }

        continue;
      }
      else if (Contains(true,
        line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
        argv[player_name_ix],player_name_len,
        &ix)) {

        if (Contains(true,
          line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
          all_in,ALL_IN_LEN,
          &ix)) {

          local_vars.bHaveAllIn = true;
          local_vars.bHaveAnyAllIn = true;

          if (!local_vars.bHaveFlop)
            local_vars.bHaveAllInPreflop = true;
          else
            local_vars.bHaveAllInPostflop = true;
        }

        if (!strncmp(line,dealt_to,DEALT_TO_LEN)) {
          for (n = 0; n < line_len; n++) {
            if (line[n] == '[')
              break;
          }

          if (n < line_len) {
            n++;

            for (m = n; m < line_len; m++) {
              if (line[m] == ']')
                break;
            }

            if (m < line_len) {
              get_abbrev(&line[n],&local_vars.hole_cards_abbrev[0]);

              if (!local_vars.bAbbrev) {
                for (p = 0; p < 11; p++) {
                  if (line[n+p] == ']') {
                    if (p == 5)
                      local_vars.hole_cards[p] = 0;

                    break;
                  }
                  else
                    local_vars.hole_cards[p] = line[n+p];
                }

                if (local_vars.bDeuceOrTreyOff)
                  local_vars.bHaveDeuceOrTreyOff = deuce_or_trey_off(local_vars.hole_cards);
                else if (local_vars.bAceRag)
                  local_vars.bHaveAceRag = ace_rag(local_vars.hole_cards);
                else if (local_vars.bSuitedAce)
                  local_vars.bHaveSuitedAce = suited_ace(local_vars.hole_cards);
                else if (local_vars.bAceNonRag)
                  local_vars.bHaveAceNonRag = ace_non_rag(local_vars.hole_cards);

                if (local_vars.bMagicFlush)
                  local_vars.bHaveMagicFlush = hole_cards_off(local_vars.hole_cards);

                if (local_vars.bNormalize)
                  normalize_hole_cards(local_vars.hole_cards);

                for (q = 0; q < NUM_HOLE_CARDS_IN_HOLDEM_HAND; q++) {
                  card_string[0] = local_vars.hole_cards[q*3 + 0];
                  card_string[1] = local_vars.hole_cards[q*3 + 1];

                  retval = card_value_from_card_string(
                    card_string,&cards[q]);

                  if (retval) {
                    printf("invalid card string %s on line %d\n",
                      card_string,local_vars.line_no);
                    return 66;
                  }
                }
              }
            }

            if (local_vars.bHandSpecified) {
              retval = index_of_hand(&line[n],&work_hand_index);

              if (!retval) {
                if (hand_ix_match(work_hand_index,&local_vars.specified_hand_ixs))
                  local_vars.bHaveSpecifiedHand = true;
              }
            }

            if (local_vars.bPremium)
              local_vars.bHavePremiumHand = is_premium_hand(local_vars.hole_cards_abbrev,&premium_ix);
            else if (local_vars.bNotPremium)
              local_vars.bHaveNotPremiumHand = !is_premium_hand(local_vars.hole_cards_abbrev,&premium_ix);

            if (local_vars.bTwinAbbrevs) {
              retval = abbrev_index_of_hand(&line[n],&local_vars.curr_abbrev_index);
            }
            else if (local_vars.bTwinHands) {
              retval = get_52_2_index_of_hole_cards(&line[n],&local_vars.curr_52_2_index);
            }
            else if (local_vars.bIdenticalTwinHands) {
              retval = get_52_2_index_of_hole_cards2(&line[n],&local_vars.curr_52_2_index2);
            }
          }
        }
        else if (Contains(true,
          line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
          collected,COLLECTED_LEN,
          &ix)) {

          if (local_vars.bWonSidePot) {
            if (Contains(true,
              line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
              side_pot,SIDE_POT_LEN,
              &pot_ix)) {
              local_vars.bHaveWonSidePot = true;
            }
          }

          if (local_vars.bWonMainPot) {
            if (Contains(true,
              line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
              main_pot,MAIN_POT_LEN,
              &pot_ix)) {
              local_vars.bHaveWonMainPot = true;
            }
          }

          for (end_ix = ix + COLLECTED_LEN; end_ix < line_len; end_ix++) {
            if (line[end_ix] == ' ')
              break;
          }

          line[end_ix] = 0;
          sscanf(&line[ix + COLLECTED_LEN],"%d",&local_vars.work);

          if (!local_vars.collected_from_pot_count) {
            local_vars.voluntarily_spent_this_hand += voluntarily_spent_this_street;
            local_vars.involuntarily_spent_this_hand += involuntarily_spent_this_street;
            street++;
            voluntarily_spent_this_street = 0;
            involuntarily_spent_this_street = 0;
          }

          local_vars.collected_from_pot += local_vars.work;
          local_vars.collected_from_pot_count++;

          if (local_vars.debug_level == 1) {
            printf("file %d hand %d line %d street %d COLLECTED work = %d, collected_from_pot = %d\n",
              file_no,hand_no,local_vars.line_no,street,local_vars.work,local_vars.collected_from_pot);
          }

          continue;
        }
        else if (!strncmp(line,uncalled_bet,UNCALLED_BET_LEN)) {
          sscanf(&line[UNCALLED_BET_LEN],"%d",&local_vars.uncalled_bet_amount);

          if (local_vars.debug_level == 1) {
            printf("file %d hand %d line %d street %d UNCALLED uncalled_bet_amount = %d, voluntarily_spent_this_street = %d, num_street_markers = %d, %d\n",
              file_no,hand_no,local_vars.line_no,street,local_vars.uncalled_bet_amount,voluntarily_spent_this_street,num_street_markers,
              (street == num_street_markers));
          }

          continue;
        }
        else if (Contains(true,
          line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
          folds,FOLDS_LEN,
          &ix)) {

          local_vars.voluntarily_spent_this_hand += voluntarily_spent_this_street;
          local_vars.involuntarily_spent_this_hand += involuntarily_spent_this_street;

          voluntarily_spent_this_street = 0;
          involuntarily_spent_this_street = 0;

          if (local_vars.debug_level == 1) {
            printf("file %d hand %d line %d street %d FOLDS voluntarily_spent_this_street = %d, local_vars.voluntarily_spent_this_hand = %d, num_street_markers = %d, %d\n",
              file_no,hand_no,local_vars.line_no,street,voluntarily_spent_this_street,local_vars.voluntarily_spent_this_hand,num_street_markers,
              (street == num_street_markers));
          }

          local_vars.bFolded = true;

          if (!local_vars.bHaveFlop2)
            local_vars.bFoldedPreflop = true;
          else if (!local_vars.bHaveTurn2)
            local_vars.bFoldedOnTheFlop = true;
          else if (!local_vars.bHaveRiver2)
            local_vars.bFoldedOnTheTurn = true;
          else
            local_vars.bFoldedOnTheRiver = true;

          if (local_vars.show_num_decisions)
            local_vars.numdecs++;

          do_balance_processing(&local_vars);

          continue;
        }
        else if (Contains(true,
          line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
          bets,BETS_LEN,
          &ix)) {
          local_vars.work = get_work_amount(line,line_len);
          voluntarily_spent_this_street += local_vars.work;

          if (street == 3)
            local_vars.bSpentRiverMoney = true;

          if (local_vars.debug_level == 1) {
            printf("file %d hand %d line %d street %d BETS work = %d, voluntarily_spent_this_street = %d, num_street_markers = %d, %d\n",
              file_no,hand_no,local_vars.line_no,street,local_vars.work,voluntarily_spent_this_street,num_street_markers,
              (street == num_street_markers));
          }

          if (local_vars.show_num_decisions)
            local_vars.numdecs++;

          if (local_vars.show_num_possible_checks)
            local_vars.num_possible_checks++;

          local_vars.bHaveVoluntaryBet = true;
        }
        else if (Contains(true,
          line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
          calls,CALLS_LEN,
          &ix)) {
          local_vars.work = get_work_amount(line,line_len);
          voluntarily_spent_this_street += local_vars.work;

          if (!street)
            local_vars.vpip = 1;
          else if (street == 3)
            local_vars.bSpentRiverMoney = true;

          if (local_vars.debug_level == 1) {
            printf("file %d hand %d line %d street %d CALLS work = %d, voluntarily_spent_this_street = %d, num_street_markers = %d, %d\n",
              file_no,hand_no,local_vars.line_no,street,local_vars.work,voluntarily_spent_this_street,num_street_markers,
              (street == num_street_markers));
          }

          if (local_vars.show_num_decisions)
            local_vars.numdecs++;

          local_vars.bHaveVoluntaryBet = true;

          if (Contains(true,
            line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
            all_in,ALL_IN_LEN,
            &ix)) {

            local_vars.bHaveCallIn = true;

            if (street == 3)
              local_vars.bHaveCallInOnTheRiver = true;
          }
        }
        else if (Contains(true,
          line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
          raises,RAISES_LEN,
          &ix)) {
          local_vars.work = get_work_amount(line,line_len);

          voluntarily_spent_this_street = local_vars.work - involuntarily_spent_this_street;

          if (!street)
            local_vars.vpip = 1;
          else if (street == 3)
            local_vars.bSpentRiverMoney = true;

          if (local_vars.debug_level == 1) {
            printf("file %d hand %d line %d street %d RAISES work = %d, voluntarily_spent_this_street = %d, num_street_markers = %d, %d\n",
              file_no,hand_no,local_vars.line_no,street,local_vars.work,voluntarily_spent_this_street,num_street_markers,
              (street == num_street_markers));
          }

          if (local_vars.show_num_decisions)
            local_vars.numdecs++;

          local_vars.bHaveVoluntaryBet = true;
        }
        else if (Contains(true,
          line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
          checks,CHECKS_LEN,
          &ix)) {

          if (local_vars.show_num_decisions)
            local_vars.numdecs++;

          if (local_vars.show_num_possible_checks)
            local_vars.num_possible_checks++;
        }
        else if ((local_vars.bStud || local_vars.bRazz) && Contains(true,
          line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
          brings_in_for,BRINGS_IN_FOR_LEN,
          &ix)) {
          bring_in = get_work_amount(line,line_len);
          voluntarily_spent_this_street += bring_in;
          continue;
        }
        else if ((local_vars.bShowdownCount || local_vars.bShowdownCountGt) &&
          Contains(true,
          line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
          ": shows",7,
          &ix)) {

          showdown_count++;
        }
        else if ((local_vars.bShowdownCount || local_vars.bShowdownCountGt) &&
          Contains(true,
          line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
          ": mucks",7,
          &ix)) {

          showdown_count++;
        }
        else if (local_vars.bOnlyKnockout && Contains(true,
            line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
            finished,FINISHED_LEN,
            &ix)) {

          local_vars.bHaveKnockout = true;
        }
      }
      else if (Contains(true,
        line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
        all_in,ALL_IN_LEN,
        &ix)) {

        local_vars.bHaveAnyAllIn = true;
      }
      else {
        if (!strncmp(line,flop_str,FLOP_LEN)) {
          if (local_vars.bStealthTwoPair && !local_vars.bFolded) {
            if (local_vars.hole_cards[0] != local_vars.hole_cards[3]) {
              if (((local_vars.hole_cards[0] == line[FLOP_LEN]) ||
                   (local_vars.hole_cards[0] == line[FLOP_LEN+3]) ||
                   (local_vars.hole_cards[0] == line[FLOP_LEN+6])) &&
                  ((local_vars.hole_cards[3] == line[FLOP_LEN]) ||
                   (local_vars.hole_cards[3] == line[FLOP_LEN+3]) ||
                   (local_vars.hole_cards[3] == line[FLOP_LEN+6]))) {
                local_vars.bHaveStealthTwoPair = true;
              }
            }
          }

          if (local_vars.bBottomTwo && !local_vars.bFolded) {
            if (bottom_two(line,line_len,local_vars.hole_cards))
              local_vars.bHaveBottomTwo = true;
          }

          for (n = 0; n < 8; n++)
            local_vars.board_cards[n] = line[FLOP_LEN+n];

          local_vars.board_cards[n] = 0;

          for (q = 0; q < NUM_CARDS_IN_FLOP; q++) {
            card_string[0] = local_vars.board_cards[q*3 + 0];
            card_string[1] = local_vars.board_cards[q*3 + 1];

            retval = card_value_from_card_string(
              card_string,&cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND+q]);

            if (retval) {
              printf("invalid card string %s on line %d\n",
                card_string,local_vars.line_no);
              return 67;
            }
          }

          local_vars.poker_hand.NewCards(cards[0],cards[1],cards[2],cards[3],cards[4]);
          local_vars.poker_hand.Evaluate();
          local_vars.poker_hand_on_flop = local_vars.poker_hand;
          local_vars.bHaveFlop2 = true;

          if (!local_vars.bFolded)
            local_vars.bHaveFlop = true;
        }
        else if (!strncmp(line,turn,TURN_LEN)) {
          n = 8;

          local_vars.board_cards[n++] = ' ';

          for (m = 0; m < 2; m++,n++)
            local_vars.board_cards[n] = line[TURN_LEN+11+m];

          local_vars.board_cards[n] = 0;

          card_string[0] = local_vars.board_cards[9];
          card_string[1] = local_vars.board_cards[10];

          retval = card_value_from_card_string(
            card_string,&cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND+NUM_CARDS_IN_FLOP]);

          if (retval) {
            printf("invalid card string %s on line %d\n",
              card_string,local_vars.line_no);
            return 68;
          }

          turn_hand.NewCards(cards[0],cards[1],cards[2],
            cards[3],cards[4],cards[5]);

          local_vars.poker_hand_on_turn = turn_hand.BestPokerHand();
          local_vars.bHaveTurn2 = true;

          if (!local_vars.bFlopped && (!local_vars.bFolded || local_vars.bVeryBestHand))
            local_vars.poker_hand = local_vars.poker_hand_on_turn;

          if (local_vars.bChasedFlush)
            local_vars.bHaveChasedFlush = four_to_a_flush(cards);
        }
        else if (!strncmp(line,river,RIVER_LEN)) {
          if (!local_vars.bStud && !local_vars.bRazz) {
            n = 11;

            local_vars.board_cards[n++] = ' ';

            if (local_vars.bAceOnTheRiver && (line[RIVER_LEN+14] == 'A'))
              local_vars.bHaveAceOnTheRiver = true;

            for (m = 0; m < 2; m++,n++)
              local_vars.board_cards[n] = line[RIVER_LEN+14+m];

            local_vars.board_cards[n] = 0;

            card_string[0] = local_vars.board_cards[12];
            card_string[1] = local_vars.board_cards[13];

            retval = card_value_from_card_string(
              card_string,&cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND+NUM_CARDS_IN_FLOP+1]);

            if (retval) {
              printf("invalid card string %s on line %d\n",
                card_string,local_vars.line_no);
              return 69;
            }

            holdem_hand.NewCards(cards[0],cards[1],cards[2],
              cards[3],cards[4],cards[5],cards[6]);

            local_vars.poker_hand_on_river = holdem_hand.BestPokerHand();
            local_vars.bHaveRiver2 = true;

            if (!local_vars.bFlopped && (!local_vars.bFolded || local_vars.bVeryBestHand))
              local_vars.poker_hand = local_vars.poker_hand_on_river;

            if (!local_vars.bFolded)
              local_vars.bHaveRiver = true;

            if (local_vars.bCounterfeit) {
              for (m = 0; m < NUM_COMMUNITY_CARDS - 1; m++) {
                if (line[RIVER_LEN+m*3] == line[RIVER_LEN+14]) {
                  local_vars.bHaveCounterfeit = true;
                  break;
                }
              }
            }
          }
        }
        else if (!strncmp(line,show_down,SHOW_DOWN_LEN)) {
          if (!local_vars.bFolded)
            local_vars.bHaveShowdown = true;

          local_vars.bHaveTableShowdown = true;
        }
        else if ((local_vars.bShowdownCount || local_vars.bShowdownCountGt) &&
          Contains(true,
          line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
          ": shows",7,
          &ix)) {

          showdown_count++;
        }
        else if ((local_vars.bShowdownCount || local_vars.bShowdownCountGt) &&
          Contains(true,
          line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
          ": mucks",7,
          &ix)) {

          showdown_count++;
        }
        else if (local_vars.bOnlyKnockout && Contains(true,
            line,line_len,local_vars.line_no,__LINE__,local_vars.debug_level,
            finished,FINISHED_LEN,
            &ix)) {

          local_vars.bHaveKnockout = true;
        }
        else if (!strncmp(line,summary,SUMMARY_LEN)) {
          if (local_vars.debug_level == 1)
            printf("file %d hand %d line %d SUMMARY line detected; skipping\n",file_no,hand_no,local_vars.line_no);

          set_position_booleans(&local_vars);

          local_vars.bSkipping = true;

          if (!local_vars.bFolded)
            do_balance_processing(&local_vars);

          if (local_vars.bRiverCardUsed) {
            local_vars.bHaveRiverCardUsed = false;

            if (local_vars.bHaveRiver) {
              poker_hand_cards = local_vars.poker_hand.GetCards();

              for (q = 0; q < NUM_CARDS_IN_HAND; q++) {
                if (poker_hand_cards[q] == cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND+NUM_CARDS_IN_FLOP+1]) {
                  local_vars.bHaveRiverCardUsed = true;
                  break;
                }
              }
            }
          }

          if (local_vars.bHoleCardsUsed || local_vars.bBothHoleCardsUsed) {
            poker_hand_cards = local_vars.poker_hand.GetCards();
            local_vars.hole_cards_used = 0;

            for (p = 0; p < NUM_HOLE_CARDS_IN_HOLDEM_HAND; p++) {
              for (q = 0; q < NUM_CARDS_IN_HAND; q++) {
                if (poker_hand_cards[q] == cards[p]) {
                  local_vars.hole_cards_used++;
                  break;
                }
              }
            }

            if (local_vars.bBothHoleCardsUsed) {
              if (local_vars.hole_cards_used != 2)
                local_vars.bHaveBothHoleCardsUsed = false;
              else
                local_vars.bHaveBothHoleCardsUsed = true;
            }
          }

          if (local_vars.bShowdownCount && (showdown_count == local_vars.showdown_count))
            local_vars.bHaveShowdownCount = true;

          if (local_vars.bShowdownCountGt && (showdown_count > local_vars.showdown_count))
            local_vars.bHaveShowdownCountGt = true;

          continue;
        }

        if (!strncmp(line,street_marker,STREET_MARKER_LEN)) {
          num_street_markers++;

          if (num_street_markers > 1) {
            if (street <= max_streets) {
              local_vars.voluntarily_spent_this_hand += voluntarily_spent_this_street;
              local_vars.involuntarily_spent_this_hand += involuntarily_spent_this_street;
            }

            street++;
            voluntarily_spent_this_street = 0;
            involuntarily_spent_this_street = 0;
          }
        }
      }
    }

    fclose(fptr);

    if (bFirstFileOnly)
      break;
  }

  fclose(fptr0);

  if (local_vars.bSumByTableCount) {
    for (n = 0; n < MAX_TABLE_COUNT - 1; n++) {
      printf("%d %10d (%8d)\n",n+2,local_vars.sum_by_table_count[n],
        local_vars.sum_by_table_count[n]);
    }
  }

  return 0;
}

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
{
  int chara;
  int local_line_len;

  local_line_len = 0;

  for ( ; ; ) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    if (chara == '\n')
      break;

    if (local_line_len < maxllen - 1)
      line[local_line_len++] = (char)chara;
  }

  line[local_line_len] = 0;
  *line_len = local_line_len;
}

static bool Contains(bool bCaseSens,char *line,int line_len,int line_no,int code_line_no,int debug_level,
  char *string,int string_len,int *index)
{
  int m;
  int n;
  int tries;
  char chara;
  int dbg;
  bool retval;

  retval = false;

  if (code_line_no == dbg_line_no)
    dbg = 1;

  tries = line_len - string_len + 1;

  if (tries <= 0)
    goto do_return;

  for (m = 0; m < tries; m++) {
    for (n = 0; n < string_len; n++) {
      chara = line[m + n];

      if (!bCaseSens) {
        if ((chara >= 'A') && (chara <= 'Z'))
          chara += 'a' - 'A';
      }

      if (chara != string[n])
        break;
    }

    if (n == string_len) {
      *index = m;
      retval = true;
      goto do_return;
    }
  }

  do_return:

  if (debug_level == 2) {
    printf("%d: %d: %s %s %s\n",
      code_line_no,line_no,line,string,
      (retval ? "true" : "false"));
  }

  return retval;
}

static int get_work_amount(char *line,int line_len)
{
  int ix;
  int chara;
  int work_amount;

  work_amount = 0;

  for (ix = line_len - 1; (ix >= 0); ix--) {
    chara = line[ix];

    if ((chara >= '0') && (chara <= '9'))
      break;
  }

  if (ix + 1 != line_len);
    line[ix + 1] = 0;

  for ( ; (ix >= 0); ix--) {
    chara = line[ix];

    if ((chara < '0') || (chara > '9'))
      break;
  }

  sscanf(&line[ix+1],"%d",&work_amount);

  return work_amount;
}

static void normalize_hole_cards(char *hole_cards)
{
  int suit1;
  int suit2;
  char temp;
  int rank1;
  int rank2;

  if (hole_cards[0] == hole_cards[3]) {
    // pair

    if (get_suit_index(hole_cards[1],&suit1))
      return;

    if (get_suit_index(hole_cards[4],&suit2))
      return;

    if (suit1 < suit2) {
      temp = hole_cards[1];
      hole_cards[1] = hole_cards[4];
      hole_cards[4] = temp;
    }
  }
  else {
    // non-pair

    if (get_rank_index(hole_cards[0],&rank1))
      return;

    if (get_rank_index(hole_cards[3],&rank2))
      return;

    if (rank1 < rank2) {
      temp = hole_cards[0];
      hole_cards[0] = hole_cards[3];
      hole_cards[3] = temp;
    }
  }
}

#define MAX_DATE_STR_LEN 128
static char date_str[MAX_DATE_STR_LEN+1];

static int get_date_from_path(char *path,char **date_str_ptr)
{
  int m;
  int n;
  int len;

  len = strlen(path);

  for (n = len - 1; (n >= 0); n--) {
    if (path[n] == '\\')
      break;
  }

  if (n < 0)
    return 1;

  for (m = 0; (m < n) && (m < MAX_DATE_STR_LEN - 1); m++)
    date_str[m] = path[m];

  date_str[m] = 0;

  *date_str_ptr = date_str;

  return 0;
}

static void get_table_name(char *line,int line_len,char *table_name,int max_table_name_len)
{
  int n;

  for (n = 0; n < max_table_name_len; n++) {
    table_name[n] = line[6+n];

    if (7 + n == line_len) {
      n++;
      break;
    }

    if ((n > 0) && (line[6+n] == '\'')) {
      n++;
      break;
    }
  }

  table_name[n] = 0;
}

static int bottom_two(char *line,int line_len,char *hole_cards)
{
  int n;
  int retval;
  int hole_card_ranks[2];
  int flop_ranks[3];
  char flop_bottom_two[2];

  if (hole_cards[0] == hole_cards[3])
    return 0;

  if ((line[FLOP_LEN] == line[FLOP_LEN + 3]) ||
      (line[FLOP_LEN] == line[FLOP_LEN + 6]) ||
      (line[FLOP_LEN+3] == line[FLOP_LEN + 6]))
    return 0;

  for (n = 0; n < NUM_HOLE_CARDS_IN_HOLDEM_HAND; n++) {
    retval = get_rank_index(hole_cards[n*3],&hole_card_ranks[n]);

    if (retval)
      return 0;
  }

  for (n = 0; n < NUM_CARDS_IN_FLOP; n++) {
    retval = get_rank_index(line[FLOP_LEN + n*3],&flop_ranks[n]);

    if (retval)
      return 0;
  }

  if (flop_ranks[0] > flop_ranks[1]) {
    flop_bottom_two[0] = flop_ranks[1];

    if (flop_ranks[0] > flop_ranks[2])
      flop_bottom_two[1] = flop_ranks[2];
    else
      flop_bottom_two[1] = flop_ranks[0];
  }
  else {
    flop_bottom_two[0] = flop_ranks[0];

    if (flop_ranks[1] > flop_ranks[2])
      flop_bottom_two[1] = flop_ranks[2];
    else
      flop_bottom_two[1] = flop_ranks[1];
  }

  if ((hole_card_ranks[0] == flop_bottom_two[0]) && (hole_card_ranks[1] == flop_bottom_two[1]))
    return 1;

  if ((hole_card_ranks[0] == flop_bottom_two[1]) && (hole_card_ranks[1] == flop_bottom_two[0]))
    return 1;

  return 0;
}

static int get_num_hands_in_file(FILE *fptr,char *player_name,int player_name_len)
{
  int num_hands_in_file;
  int line_len;
  int ix;

  num_hands_in_file = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if (Contains(true,
      line,line_len,-1,__LINE__,-1,
      player_name,player_name_len,
      &ix)) {

      if (Contains(true,
        line,line_len,-1,__LINE__,-1,
        in_chips,IN_CHIPS_LEN,
        &ix)) {

        num_hands_in_file++;
      }
    }
  }

  fseek(fptr,0L,SEEK_SET);

  return num_hands_in_file;
}

static bool deuce_or_trey_off(char *hole_cards)
{
  // if not offsuit, return false
  if (hole_cards[1] == hole_cards[4])
    return false;

  // if a pair, return false
  if (hole_cards[0] == hole_cards[3])
    return false;

  // if either is a deuce, return true
  if ((hole_cards[0] == '2') || (hole_cards[3] == '2'))
    return true;

  // if either is a trey, return true
  if ((hole_cards[0] == '3') || (hole_cards[3] == '3'))
    return true;

  return false;
}

static bool ace_rag(char *hole_cards)
{
  // if not offsuit, return false
  if (hole_cards[1] == hole_cards[4])
    return false;

  if ((hole_cards[0] == 'A') && (hole_cards[3] >= '2') && (hole_cards[3] <= '9'))
    return true;

  if ((hole_cards[3] == 'A') && (hole_cards[0] >= '2') && (hole_cards[0] <= '9'))
    return true;

  return false;
}

static bool suited_ace(char *hole_cards)
{
  // if not suited, return false
  if (hole_cards[1] != hole_cards[4])
    return false;

  if ((hole_cards[0] == 'A') || (hole_cards[3] == 'A'))
    return true;

  return false;
}

static bool ace_non_rag(char *hole_cards)
{
  // if ace rag, return false
  if (ace_rag(hole_cards))
    return false;

  if ((hole_cards[0] == 'A') || (hole_cards[3] == 'A'))
    return true;

  return false;
}

static bool hole_cards_off(char *hole_cards)
{
  // if not offsuit, return false
  if (hole_cards[1] == hole_cards[4])
    return false;

  return true;
}

static HandType get_winning_hand_typ_id(char *line,int line_len)
{
  int n;
  int ix;
  int ix2;

  if (Contains(true,
    line,line_len,-1,__LINE__,-1,
    with,WITH_LEN,
    &ix)) {

    if (Contains(true,
      line,line_len,-1,__LINE__,-1,
      with_a,WITH_A_LEN,
      &ix2)) {

      ix = ix2 + WITH_A_LEN;
    }
    else
      ix += WITH_LEN;

    for (n = 0; n < NUM_HAND_TYPES; n++) {
      if (!strncmp(&line[ix],plain_hand_types[n],plain_hand_type_lens[n]))
        break;
    }

    if (n < NUM_HAND_TYPES)
      return (HandType)n;
  }

  return HIGH_CARD;
}

static int run_filter_calls1;
static int run_filter_calls2;

void run_filter(struct vars *varspt)
{
  int dbg;
  double dwork;
  HeadsUpTurn hut;
  HeadsUpFlop huf;
  int cards[NUM_HEADS_UP_TURN_CARDS];
  bool bSkip;
  int print_location;

  run_filter_calls1++;

  if (hand_no == dbg_hand_no)
    dbg = 1;

  if (varspt->filter_percentage)
    varspt->summary_val++;

  print_location = 0;

  if (varspt->debug_level == 2) {
    printf("top of run_filter()\n");
    printf("file %d hand %d line %d: line %d\n",file_no,hand_no,__LINE__,varspt->line_no);
  }

  if (varspt->bSummarizing || !varspt->bSkipZero || (varspt->delta != 0)) {
  if (!varspt->bOnlyZero || (varspt->delta == 0)) {
  if (!varspt->bSkipFolded || !varspt->bFolded) {
  if (!varspt->bOnlyFolded || varspt->bFolded) {
  if (!varspt->bOnlyFoldedPreflop || varspt->bFoldedPreflop) {
  if (!varspt->bOnlyFoldedOnTheFlop || varspt->bFoldedOnTheFlop) {
  if (!varspt->bOnlyFoldedOnTheTurn || varspt->bFoldedOnTheTurn) {
  if (!varspt->bOnlyFoldedOnTheRiver || varspt->bFoldedOnTheRiver) {
  if (!varspt->bSawFlop || varspt->bHaveFlop) {
  if (!varspt->bDidntSeeFlop || !varspt->bHaveFlop) {
  if (!varspt->bStealthTwoPair || varspt->bHaveStealthTwoPair) {
  if (!varspt->bBottomTwo || varspt->bHaveBottomTwo) {
  if (!varspt->bSawRiver || varspt->bHaveRiver) {
  if (!varspt->bAceOnTheRiver || varspt->bHaveAceOnTheRiver) {
  if (!varspt->bRiverMoney || varspt->bSpentRiverMoney) {
  if (!varspt->bNoRiverMoney || !varspt->bSpentRiverMoney) {
  if (!varspt->bHandTypeSpecified || (varspt->hand_typ_id == varspt->poker_hand.GetHandType())) {
  if (!varspt->bBroadway || varspt->poker_hand.Broadway()) {
  if (!varspt->bMagicFlush || varspt->bHaveMagicFlush) {
  if (!varspt->bAnyAllIn || varspt->bHaveAnyAllIn) {
  if (!varspt->bNoAllIn || !varspt->bHaveAnyAllIn) {
  if (!varspt->bHandTypeOnFlopSpecified || (varspt->hand_typ_on_flop_id == varspt->poker_hand_on_flop.GetHandType())) {
  if (!varspt->bWinningHandTypeSpecified || (varspt->curr_winning_hand_typ_id == varspt->winning_hand_typ_id)) {
  if (!varspt->bHandTypIdSpecified || (varspt->poker_hand.GetHandType() == varspt->hand_typ_id)) {
  if (!varspt->bHandSpecified || varspt->bHaveSpecifiedHand) {
  if (!varspt->bPremium || varspt->bHavePremiumHand) {
  if (!varspt->bNotPremium || varspt->bHaveNotPremiumHand) {
  if (!varspt->bShowdownHandSpecified || varspt->bHaveSpecifiedShowdownHand) {
  if (!varspt->bShowdownHand2Specified || varspt->bHaveSpecifiedShowdownHand2) {
  if (!varspt->bWinningHandSpecified || varspt->bHaveSpecifiedWinningHand) {
  if (!varspt->bLost || (varspt->delta < 0)) {
  if (!varspt->bWon || (varspt->delta > 0)) {
  if (!varspt->bWash || (varspt->delta == 0)) {
  if (!varspt->bShowdown || varspt->bHaveShowdown) {
  if (!varspt->bTableShowdown || varspt->bHaveTableShowdown) {
  if (!varspt->bShowdownCount || varspt->bHaveShowdownCount) {
  if (!varspt->bShowdownCountGt || varspt->bHaveShowdownCountGt) {
  if (!varspt->bNoShowdown || !varspt->bHaveShowdown) {
  if (!varspt->bTableCount || (varspt->table_count == varspt->table_count_to_match)) {
  if (!varspt->bAllIn || varspt->bHaveAllIn) {
  if (!varspt->bNotAllIn || !varspt->bHaveAllIn) {
  if (!varspt->bAllInPreflop || varspt->bHaveAllInPreflop) {
  if (!varspt->bAllInPostflop || varspt->bHaveAllInPostflop) {
  if (!varspt->bCallIn || varspt->bHaveCallIn) {
  if (!varspt->bCallInOnTheRiver || varspt->bHaveCallInOnTheRiver) {
  if (!varspt->bFallIn || varspt->bHaveFallIn) {
  if (!varspt->bNotFallIn || !varspt->bHaveFallIn) {
  if (!varspt->bHitFelt || (varspt->ending_balance == 0)) {
  if (!varspt->bDidntHitFelt || (varspt->ending_balance > 0)) {
  if (!varspt->bNoUncalled || (varspt->uncalled_bet_amount == 0)) {
  if (!varspt->bNoCollected || (varspt->collected_from_pot == 0)) {
  if (!varspt->bOnlySuited || (varspt->hole_cards[1] == varspt->hole_cards[4])) {
  if (!varspt->bOnlyNonsuited || (varspt->hole_cards[1] != varspt->hole_cards[4])) {
  if (!varspt->bPocketPair || (!varspt->bAbbrev && (varspt->hole_cards[0] == varspt->hole_cards[3])) || (varspt->bAbbrev && (varspt->hole_cards_abbrev[0] == varspt->hole_cards_abbrev[1]))) {
  if ((varspt->hand_number == -1) || (varspt->num_hands == varspt->hand_number)) {
  if (!varspt->bDeltaGe || (varspt->delta >= varspt->delta_ge_val)) {
  if (!varspt->bCollectedGe || (varspt->collected_from_pot >= varspt->collected_ge_val)) {
  if (!varspt->bCounterfeit || varspt->bHaveCounterfeit) {
  if (!varspt->bWonSidePot || varspt->bHaveWonSidePot) {
  if (!varspt->bWonMainPot || varspt->bHaveWonMainPot) {
  if (!varspt->bLastHandOnly || (varspt->num_hands == varspt->num_hands_in_file)) {
  if (!varspt->bExceptLastHand || (varspt->num_hands < varspt->num_hands_in_file)) {
  if (!varspt->bFirstHandOnly || (varspt->num_hands == 1)) {
  if (!varspt->bButton || varspt->bAmButton) {
  if (!varspt->bSmallBlind || varspt->bPostedSmallBlind) {
  if (!varspt->bBigBlind || varspt->bPostedBigBlind) {
  if (!varspt->bUtg || varspt->bAmUtg) {
  if (!varspt->bCutoff || varspt->bAmCutoff) {
  if (!varspt->bHijack || varspt->bAmHijack) {
  if (!varspt->bOther || (!varspt->bAmButton && !varspt->bPostedSmallBlind && !varspt->bPostedBigBlind &&
    !varspt->bAmUtg && !varspt->bAmCutoff && !varspt->bAmHijack)) {
  if (!varspt->bVpip || varspt->vpip) {
  if (!varspt->bDidntVpip || !varspt->vpip) {
  if (!varspt->bDeuceOrTreyOff || varspt->bHaveDeuceOrTreyOff) {
  if (!varspt->bVoluntaryBet || varspt->bHaveVoluntaryBet) {
  if (!varspt->bNoVoluntaryBet || !varspt->bHaveVoluntaryBet) {
  if (!varspt->bChasedFlush || varspt->bHaveChasedFlush) {
  if (!varspt->bRiverCardUsed || varspt->bHaveRiverCardUsed) {
  if (!varspt->bBothHoleCardsUsed || varspt->bHaveBothHoleCardsUsed) {
  if (!varspt->bHandTypIdGeSpecified || (varspt->poker_hand.GetHandType() >= varspt->hand_typ_id_ge)) {
  if (!varspt->bBadRiverMoney || varspt->bHaveBadRiverMoney) {
  if (!varspt->bTableBoss || varspt->am_table_boss) {
  if (!varspt->bOnlyKnockout || varspt->bHaveKnockout) {
  if (!varspt->bOnlyDoubleUp || varspt->bHaveDoubleUp) {
  if (!varspt->bOnlyDiscrepancy || varspt->bHaveDiscrepancy) {
  if (!varspt->bSittingOut || varspt->bIsSittingOut) {
  if (!varspt->bTwinAbbrevs || (varspt->curr_abbrev_index == varspt->prev_abbrev_index)) {
  if (!varspt->bTwinHands || (varspt->curr_52_2_index == varspt->prev_52_2_index)) {
  if (!varspt->bIdenticalTwinHands || (varspt->curr_52_2_index2 == varspt->prev_52_2_index2)) {
  if (!varspt->bAceRag || (varspt->bHaveAceRag)) {
  if (!varspt->bAceNonRag || (varspt->bHaveAceNonRag)) {
  if (!varspt->bSuitedAce || (varspt->bHaveSuitedAce)) {
  if (!varspt->bOnlyUncalled || (varspt->uncalled_bet_amount)) {
  if (varspt->bHutOuts || varspt->bHutOutsDiff) {
    card_value_from_card_string(&varspt->hole_cards[0],&cards[0]);
    card_value_from_card_string(&varspt->hole_cards[3],&cards[1]);
    card_value_from_card_string(&varspt->opponent_hole_cards[0],&cards[2]);
    card_value_from_card_string(&varspt->opponent_hole_cards[3],&cards[3]);
    card_value_from_card_string(&varspt->board_cards[0],&cards[4]);
    card_value_from_card_string(&varspt->board_cards[3],&cards[5]);
    card_value_from_card_string(&varspt->board_cards[6],&cards[6]);
    card_value_from_card_string(&varspt->board_cards[9],&cards[7]);
    hut.NewCards(cards[0],cards[1],cards[2],cards[3],cards[4],cards[5],cards[6],cards[7]);
    hut.Evaluate(false);
    varspt->outcomes = hut.GetOutcomes();

    bSkip = false;

    if (varspt->bHutWinsGeValue) {
      if (varspt->outcomes[0].wins < varspt->hut_wins_ge_value)
        bSkip = true;
    }
    else if (varspt->bHutWinsLeValue) {
      if (varspt->outcomes[0].wins > varspt->hut_wins_le_value)
        bSkip = true;
    }
    else if (varspt->bHutWinsEqValue) {
      if (varspt->outcomes[0].wins != varspt->hut_wins_eq_value)
        bSkip = true;
    }

    if (varspt->bHutLossesGeValue) {
      if (varspt->outcomes[0].losses < varspt->hut_losses_ge_value)
        bSkip = true;
    }
    else if (varspt->bHutLossesLeValue) {
      if (varspt->outcomes[0].losses > varspt->hut_losses_le_value)
        bSkip = true;
    }
    else if (varspt->bHutLossesEqValue) {
      if (varspt->outcomes[0].losses != varspt->hut_losses_eq_value)
        bSkip = true;
    }

    if (varspt->bHutTiesGeValue) {
      if (varspt->outcomes[0].ties < varspt->hut_ties_ge_value)
        bSkip = true;
    }
    else if (varspt->bHutTiesLeValue) {
      if (varspt->outcomes[0].ties > varspt->hut_ties_le_value)
        bSkip = true;
    }
    else if (varspt->bHutTiesEqValue) {
      if (varspt->outcomes[0].ties != varspt->hut_ties_eq_value)
        bSkip = true;
    }
  }
  else if (varspt->bHufOuts) {
    card_value_from_card_string(&varspt->hole_cards[0],&cards[0]);
    card_value_from_card_string(&varspt->hole_cards[3],&cards[1]);
    card_value_from_card_string(&varspt->opponent_hole_cards[0],&cards[2]);
    card_value_from_card_string(&varspt->opponent_hole_cards[3],&cards[3]);
    card_value_from_card_string(&varspt->board_cards[0],&cards[4]);
    card_value_from_card_string(&varspt->board_cards[3],&cards[5]);
    card_value_from_card_string(&varspt->board_cards[6],&cards[6]);
    huf.NewCards(cards[0],cards[1],cards[2],cards[3],cards[4],cards[5],cards[6]);
    huf.Evaluate(false);
    varspt->outcomes = huf.GetOutcomes();

    bSkip = false;
  }

  if (!bSkip) {
    if (varspt->bTerse) {
      if (!varspt->bSummarizing && !varspt->bSumByTableCount) {
        if (varspt->quantum_type == QUANTUM_TYPE_OPM) {
          if (!varspt->bShowTableCount) {
            if (!varspt->bShowSeatNumbers)
              printf("%lf\n",varspt->dwork);
            else
              printf("%lf %s\n",varspt->dwork,varspt->seat_numbers);
          }
          else {
            if (!varspt->bShowSeatNumbers)
              printf("%lf %d\n",varspt->dwork,varspt->table_count);
            else
              printf("%lf %d %s\n",varspt->dwork,varspt->table_count,varspt->seat_numbers);
          }
        }
        else {
          if (!varspt->bShowTableCount) {
            if (!varspt->bShowSeatNumbers)
              printf("%d\n",varspt->quantum);
            else
              printf("%d %s\n",varspt->quantum,varspt->seat_numbers);
          }
          else {
            if (!varspt->bShowSeatNumbers)
              printf("%d %d\n",varspt->quantum,varspt->table_count);
            else
              printf("%d %d %s\n",varspt->quantum,varspt->table_count,varspt->seat_numbers);
          }
        }

        print_location = 1;
      }
      else if (!varspt->bSumByTableCount) {
        varspt->total_delta += varspt->quantum;

        if (varspt->winning_percentage) {
          varspt->summary_val++;

          if (varspt->delta > 0)
            varspt->summary_val2++;
        }
        else if (varspt->filter_percentage)
          varspt->summary_val2++;
        else if (varspt->only_count)
          varspt->summary_val++;
        else if (varspt->sum_quantum)
          varspt->summary_val += varspt->quantum;
        else if (varspt->sum_abs_delta) {
          if (varspt->delta > 0)
            varspt->summary_val += varspt->delta;
          else
            varspt->summary_val -= varspt->delta;
        }
        else if (varspt->max_delta) {
          if (varspt->delta > varspt->summary_val)
            varspt->summary_val = varspt->delta;
        }
        else if (varspt->min_delta) {
          if (varspt->delta < varspt->summary_val)
            varspt->summary_val = varspt->delta;
        }
        else if (varspt->max_abs_delta) {
          if (varspt->delta < 0) {
            varspt->work = varspt->delta * -1;

            if (varspt->work > varspt->summary_val)
              varspt->summary_val = varspt->work;
          }
          else {
            if (varspt->delta > varspt->summary_val)
              varspt->summary_val = varspt->delta;
          }
        }
        else if (varspt->max_collected) {
          if (varspt->collected_from_pot > varspt->summary_val)
            varspt->summary_val = varspt->collected_from_pot;
        }
        else {
          if (varspt->delta > varspt->summary_val) {
            varspt->summary_val = varspt->delta;
            varspt->max_delta_hand_typ = varspt->poker_hand.GetHandType();
          }
        }
      }
      else {
        varspt->sum_by_table_count[varspt->table_count - 2] += varspt->delta;
        varspt->sum_by_table_count[varspt->table_count - 2]++;
      }
    }
    else {
      switch(varspt->quantum_type) {
        case QUANTUM_TYPE_DELTA:
          if (!varspt->bHoleCardsUsed) {
            if (!varspt->bNoHoleCards) {
              if (varspt->bShowWinningHandHoleCards && varspt->bHaveWinningHandHoleCards) {
                if (varspt->bShowWinningHandHoleCardIxs) {
                  if (!varspt->bNoDelta) {
                    printf("%10d %s %d %d",varspt->delta,
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards),
                      varspt->winning_hand_hole_card_ixs[0],
                      varspt->winning_hand_hole_card_ixs[1]);
                  }
                  else {
                    printf("%s %d %d",
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards),
                      varspt->winning_hand_hole_card_ixs[0],
                      varspt->winning_hand_hole_card_ixs[1]);
                  }

                  print_location = 2;
                }
                else if (varspt->bShowWinningHandHoleCardsAbbrev) {
                  if (!varspt->bNoDelta) {
                    printf("%10d %s %s",varspt->delta,
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards),
                      varspt->winning_hand_hole_cards_abbrev);
                  }
                  else {
                    printf("%s %s",
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards),
                      varspt->winning_hand_hole_cards_abbrev);
                  }

                  print_location = 3;
                }
                else {
                  if (!varspt->bNoDelta) {
                    printf("%10d %s %s",varspt->delta,
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards),
                      varspt->winning_hand_hole_cards);
                  }
                  else {
                    printf("%s %s",
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards),
                      varspt->winning_hand_hole_cards);
                  }

                  print_location = 4;
                }
              }
              else if (varspt->bShowOpponentHoleCards && varspt->bHaveOpponentHoleCards) {
                if (varspt->bShowOpponentHoleCardIxs) {
                  if (!varspt->bNoDelta) {
                    printf("%10d %s %d %d",varspt->delta,
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards),
                      varspt->opponent_hole_card_ixs[0],
                      varspt->opponent_hole_card_ixs[1]);
                  }
                  else {
                    printf("%s %d %d",
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards),
                      varspt->opponent_hole_card_ixs[0],
                      varspt->opponent_hole_card_ixs[1]);
                  }

                  print_location = 5;
                }
                else if (varspt->bShowOpponentHoleCardsAbbrev) {
                  if (!varspt->bNoDelta) {
                    printf("%10d %s %s",varspt->delta,
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards),
                      varspt->opponent_hole_cards_abbrev);
                  }
                  else {
                    printf("%s %s",
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards),
                      varspt->opponent_hole_cards_abbrev);
                  }

                  print_location = 6;
                }
                else {
                  if (!varspt->bNoDelta) {
                    printf("%10d %s %s",varspt->delta,
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards),
                      varspt->opponent_hole_cards);
                  }
                  else {
                    printf("%s %s",
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards),
                      varspt->opponent_hole_cards);
                  }

                  print_location = 7;
                }
              }
              else {
                if (varspt->bHutOuts || varspt->bHufOuts) {
                  if (!varspt->bNoDelta) {
                    printf("%10d %3d %3d %3d %s",varspt->delta,
                      varspt->outcomes[0].wins,
                      varspt->outcomes[0].losses,
                      varspt->outcomes[0].ties,
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards));
                  }
                  else {
                    printf("%3d %3d %3d %s",
                      varspt->outcomes[0].wins,
                      varspt->outcomes[0].losses,
                      varspt->outcomes[0].ties,
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards));
                  }

                  print_location = 8;
                }
                else if (varspt->bHutOutsDiff) {
                  if (!varspt->bNoDelta) {
                    printf("%10d %2d %s",varspt->delta,
                      varspt->outcomes[0].wins -
                      varspt->outcomes[0].losses +
                      varspt->outcomes[0].ties,
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards));
                  }
                  else {
                    printf("%2d %s",
                      varspt->outcomes[0].wins -
                      varspt->outcomes[0].losses +
                      varspt->outcomes[0].ties,
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards));
                  }

                  print_location = 9;
                }
                else {
                  if (!varspt->bNoDelta) {
                    printf("%10d %s (%10d %10d)",varspt->delta,
                      (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards),
                      varspt->ending_balance,varspt->starting_balance);
                  }
                  else {
                    if (!varspt->bTimestamp) {
                      printf("%s",
                        (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards));
                    }
                    else {
                      printf("%s %s",
                        (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards),varspt->timestamp);
                    }
                  }

                  print_location = 10;
                }
              }
            }
            else if (!varspt->bNoDelta) {
              printf("%10d",varspt->delta);
              print_location = 11;
            }
          }
          else {
            if (!varspt->bNoHoleCards) {
              if (!varspt->bNoDelta) {
                printf("%10d %s (%d)",varspt->delta,
                  (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards),
                  varspt->hole_cards_used);
              }
              else {
                printf("%s (%d)",
                  (varspt->bAbbrev ? varspt->hole_cards_abbrev : varspt->hole_cards),
                  varspt->hole_cards_used);
              }

              print_location = 12;
            }
            else {
              if (!varspt->bNoDelta)
                printf("%10d (%d)",varspt->delta,varspt->hole_cards_used);
              else
                printf("(%d)",varspt->hole_cards_used);

              print_location = 13;
            }
          }

          break;
        case QUANTUM_TYPE_OPM:
          printf("%6.4lf (%10d %10d) %s",varspt->dwork,
            varspt->delta,varspt->collected_from_pot,varspt->hole_cards);
          print_location = 14;

          break;
        case QUANTUM_TYPE_COLLECTED:
        case QUANTUM_TYPE_VOLUNTARILY_SPENT:
        case QUANTUM_TYPE_INVOLUNTARILY_SPENT:
        case QUANTUM_TYPE_SPENT:
        case QUANTUM_TYPE_UNCALLED:
        case QUANTUM_TYPE_ROLLBACK:
        case QUANTUM_TYPE_NUMDECISIONS:
        case QUANTUM_TYPE_WAGERED:
        case QUANTUM_TYPE_TABLE_BOSS:
        case QUANTUM_TYPE_NUM_POSSIBLE_CHECKS:
        case QUANTUM_TYPE_RUNNING_TOTAL:
        case QUANTUM_TYPE_NUM_POSITIVE_DELTAS:
        case QUANTUM_TYPE_DISCREPANCY:
          printf("%10d %s",varspt->quantum,varspt->hole_cards);
          print_location = 15;

          break;
        case QUANTUM_TYPE_ROI:
          if (!varspt->wagered_amount)
            dwork = (double)0;
          else
            dwork = (double)varspt->delta / (double)varspt->wagered_amount;

          printf("%lf (%d %d) %s",dwork,
            varspt->delta,varspt->wagered_amount,varspt->hole_cards);
          print_location = 16;

          break;
      }

      if (varspt->bShowBoard && varspt->bHaveFlop2)
        printf(" %s",varspt->board_cards);

      if (varspt->bShowHandType && varspt->bHaveFlop)
        printf(" %s",plain_hand_types[varspt->poker_hand.GetHandType()]);
      else if (varspt->bShowHandTypeOnFlop && varspt->bHaveFlop2)
        printf(" %s",plain_hand_types[varspt->poker_hand_on_flop.GetHandType()]);
      else if (varspt->bShowHandTypeOnTurn && varspt->bHaveTurn2)
        printf(" %s",plain_hand_types[varspt->poker_hand_on_turn.GetHandType()]);
      else if (varspt->bShowHandTypeOnRiver && varspt->bHaveRiver2)
        printf(" %s",plain_hand_types[varspt->poker_hand_on_river.GetHandType()]);

      if (varspt->bShowRiver)
        printf(" %s",&varspt->board_cards[12]);

      if (varspt->bShowHandTypId && varspt->bHaveFlop)
        printf(" %d",varspt->poker_hand.GetHandType());

      if (varspt->bShowHand && varspt->bHaveFlop)
        printf(" %s",varspt->poker_hand.GetHand());

      if (varspt->bShowTableName)
        printf(" %s",table_name);

      if (varspt->bShowTableCount)
        printf(" %d",varspt->table_count);

      if (varspt->bShowSeatNumbers)
        printf(" %s",varspt->seat_numbers);

      if (varspt->bVerbose) {
        if (!varspt->bGetDateFromFilename) {
          if (!varspt->bVerboseStyle2)
            printf(" %s %3d (%3d)\n",filename,varspt->num_hands,hand_no+1);
          else
            printf(" %s%d.txt\n",style2(filename),varspt->num_hands);
        }
        else
          printf("\t%s\n",varspt->date_string);
      }
      else {
        if (varspt->bIndex) {
          printf("%d",hand_no+1);
        }

        putchar(0x0a);
      }
    }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }
  }

  if (print_location)
    run_filter_calls2++;
}

static void do_balance_processing(struct vars *varspt)
{
  varspt->ending_balance = varspt->starting_balance - varspt->voluntarily_spent_this_hand
    - varspt->involuntarily_spent_this_hand + varspt->collected_from_pot + varspt->uncalled_bet_amount;

  if (varspt->starting_balance * 2 <= varspt->ending_balance)
    varspt->bHaveDoubleUp = true;

  if (varspt->bSummarizing && !varspt->ending_balance)
    varspt->hit_felt_in_session_count++;

  varspt->delta = varspt->ending_balance - varspt->starting_balance;
  varspt->wagered_amount = varspt->voluntarily_spent_this_hand + varspt->uncalled_bet_amount;

  if (varspt->show_running_total)
    varspt->running_total += varspt->delta;

  if (varspt->show_num_positive_deltas) {
    if (varspt->delta > 0)
      varspt->num_positive_deltas += 1;
  }

  if (varspt->bOnlyDiscrepancy) {
    if (varspt->num_hands != 1 && varspt->prev_ending_balance != -1) {
      if (varspt->starting_balance != varspt->prev_ending_balance) {
        varspt->bHaveDiscrepancy = true;
        varspt->discrepancy = varspt->starting_balance - varspt->prev_ending_balance;
      }
    }

    varspt->prev_ending_balance = varspt->ending_balance;
  }

  switch(varspt->quantum_type) {
    case QUANTUM_TYPE_DELTA:
      varspt->quantum = varspt->delta;

      break;
    case QUANTUM_TYPE_COLLECTED:
      varspt->quantum = varspt->collected_from_pot;

      break;
    case QUANTUM_TYPE_VOLUNTARILY_SPENT:
      varspt->quantum = varspt->voluntarily_spent_this_hand;

      break;
    case QUANTUM_TYPE_INVOLUNTARILY_SPENT:
      varspt->quantum = varspt->involuntarily_spent_this_hand;

      break;
    case QUANTUM_TYPE_SPENT:
      varspt->quantum = varspt->voluntarily_spent_this_hand + varspt->involuntarily_spent_this_hand;

      break;
    case QUANTUM_TYPE_UNCALLED:
      varspt->quantum = varspt->uncalled_bet_amount;

      break;
    case QUANTUM_TYPE_ROLLBACK:
      varspt->quantum = (varspt->involuntarily_spent_this_hand * -1) - varspt->delta;

      break;
    case QUANTUM_TYPE_OPM:
      varspt->dwork = (double)0;

      break;
    case QUANTUM_TYPE_NUMDECISIONS:
      varspt->quantum = varspt->numdecs;

      break;
    case QUANTUM_TYPE_WAGERED:
      varspt->quantum = varspt->wagered_amount;

      break;
    case QUANTUM_TYPE_TABLE_BOSS:
      varspt->quantum = varspt->am_table_boss;

      break;
    case QUANTUM_TYPE_NUM_POSSIBLE_CHECKS:
      varspt->quantum = varspt->num_possible_checks;
    case QUANTUM_TYPE_RUNNING_TOTAL:
      varspt->quantum = varspt->running_total;

      break;
    case QUANTUM_TYPE_NUM_POSITIVE_DELTAS:
      varspt->quantum = varspt->num_positive_deltas;

      break;
    case QUANTUM_TYPE_DISCREPANCY:
      varspt->quantum = varspt->discrepancy;

      break;
  }
}

#define MAX_STYLE2_LEN 512
static char style2_buf[MAX_STYLE2_LEN];

static char *style2(char *filename)
{
  int n;

  strcpy(style2_buf,filename);

  for (n = strlen(style2_buf) - 1; (n >= 0); n--) {
    if (style2_buf[n] == '\\')
      break;
  }

  n++;

  sprintf(&style2_buf[n],"hand");

  return style2_buf;
}

static int hand_ix_match(int hand_ix,struct hand_ixs *specified_hand_ixs)
{
  int n;

  for (n = 0; n < specified_hand_ixs->num_hand_ixs; n++) {
    if (specified_hand_ixs->ix[n] == hand_ix)
      return 1;
  }

  return 0;
}

static int get_button_seat(char *line)
{
  char *cpt;
  int button_seat;

  if (cpt = strstr(line,"Seat #")) {
    sscanf(&cpt[6],"%d",&button_seat);
    return button_seat;
  }

  return 0; // should never get here
}

static void set_position_booleans(struct vars *varspt)
{
  int button_ix;
  int seat_number;
  int utg_ix;
  int cutoff_ix;
  int hijack_ix;

  varspt->bAmUtg = false;
  varspt->bAmCutoff = false;
  varspt->bAmHijack = false;

  if ((varspt->bAmButton) || (varspt->bPostedSmallBlind) || (varspt->bPostedBigBlind))
    return;

  if (varspt->table_count < 4)
    return;

  for (button_ix = 0; button_ix < varspt->table_count; button_ix++) {
    seat_number = varspt->seat_numbers[button_ix] - '0';

    if (seat_number == varspt->button_seat)
      break;
  }

  if (button_ix == varspt->table_count)
    return; // should never get here

  utg_ix = button_ix;

  // check if I'm under the gun; the utg player is in the third position
  // to the left of the dealer
  utg_ix += 3;

  // check for overflow
  if (utg_ix >= varspt->table_count)
    utg_ix -= varspt->table_count;

  if (varspt->my_seat_ix == utg_ix) {
    varspt->bAmUtg = true;
    return;
  }

  if (varspt->table_count < 5)
    return;

  cutoff_ix = button_ix;

  // check if I'm in the cutoff; the cutoff player is in the first position
  // to the right of the dealer
  cutoff_ix -= 1;

  // check for underflow
  if (cutoff_ix < 0)
    cutoff_ix += varspt->table_count;

  if (varspt->my_seat_ix == cutoff_ix) {
    varspt->bAmCutoff = true;
    return;
  }

  if (varspt->table_count < 6)
    return;

  hijack_ix = button_ix;

  // check if I'm in the hijack; the hijack player is in the second position
  // to the right of the dealer
  hijack_ix -= 2;

  // check for underflow
  if (hijack_ix < 0)
    hijack_ix += varspt->table_count;

  if (varspt->my_seat_ix == hijack_ix) {
    varspt->bAmHijack = true;
    return;
  }
}

static void populate_timestamp(char *line,char *timestamp)
{
  int m;
  int n;
  int len;

  len = strlen(line);

  for (n = len - 1; n >= 0; n--) {
    if (line[n] == '-')
      break;
  }

  if (n < 0) {
    // should never get here
    timestamp[0] = 0;
    return;
  }

  m = n + 2;

  for (n = 0; (n < TIMESTAMP_LEN) && (n + m < len); n++) {
    timestamp[n] = line[n + m];
  }

  timestamp[n] = 0;
}
