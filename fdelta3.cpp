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

#define MAX_FILENAME_LEN 1024
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

#define MAX_TABLE_NAME_LEN 128
static char table_name[MAX_TABLE_NAME_LEN+1];

static char usage[] =
"usage: fdelta3 (-terse) (-verbose) (-debug) (-hand_typehand_type) (-handhand)\n"
"  (-skip_folded) (-abbrev) (-skip_zero) (-only_zero) (-show_board)\n"
"  (-show_hand_type) (-show_hand) (-saw_flop) (-saw_river) (-only_folded)\n"
"  (-spent_money_on_the_river) (-stealth_two_pair) (-normalize) (-only_lost)\n"
"  (-only_count) (-only_won) (-only_showdown) (-only_no_showdown) (-only_heads_up_showdown)\n"
"  (-very_best_hand) (-table_countn) (-all_in) (-not_all_in)\n"
"  (-all_in_preflop) (-all_in_postflop)\n"
"  (-hit_felt) (-didnt_hit_felt) (-no_uncalled) (-no_collected)\n"
"  (-show_collected) (-show_spent) (-show_opm) (-only_wash)\n"
"  (-sum_delta) (-sum_abs_delta) (-max_delta) (-min_delta) (-max_abs_delta)\n"
"  (-max_collected)\n"
"  (-max_delta_hand_type) (-no_delta) (-hole_cards_used)\n"
"  (-only_suited) (-flopped) (-pocket_pair) (-only_hand_numbern)\n"
"  (-hand_typ_idid)\n"
"  (-show_hand_typ_id) (-didnt_see_flop)\n"
"  (-only_winning_session) (-only_losing_session) (-never_hit_felt_in_session)\n"
"  (-collected_genum) (-sum_by_table_count)\n"
"  (-show_table_name) (-show_table_count) (-show_hand_count) (-bottom_two)\n"
"  (-counterfeit) (-num_decisions) (-won_side_pot) (-won_main_pot) (-last_hand_only)\n"
"  (-winning_percentage) (-get_date_from_filename) (-no_hole_cards)\n"
"  (-small_blind) (-big_blind) (-small_or_big_blind) (-no_blind)\n"
"  (-deuce_or_trey_off) (-voluntary_bet) (-no_voluntary_bet)\n"
"  (-chased_flush) (-river_card_used) (-both_hole_cards_used) (-show_river)\n"
"  (-hand_typ_id_geid) (-bad_river_money) (-show_wagered) (-uberflush)\n"
"  (-winning_hand_typehand_type player_name filename\n";
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
static char flop[] = "*** FLOP *** [";
#define FLOP_LEN (sizeof (flop) - 1)
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

#define MAX_TABLE_COUNT 9

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);
static int get_work_amount(char *line,int line_len);
static void normalize_hole_cards(char *hole_cards);
static int get_date_from_path(char *path,char slash_char,int num_slashes,char **date_string_ptr);
static void get_table_name(char *line,int line_len,char *table_name,int max_table_name_len);
static int bottom_two(char *line,int line_len,char *hole_cards);
static int get_num_hands_in_file(FILE *fptr,char *player_name,int player_name_len);
static bool deuce_or_trey_off(char *hole_cards);
static HandType get_winning_hand_typ_id(char *line,int line_len);

struct vars {
  bool bTerse;
  bool bVerbose;
  bool bDebug;
  bool bHandTypeSpecified;
  bool bWinningHandTypeSpecified;
  bool bHandTypIdSpecified;
  bool bHandSpecified;
  bool bSkipFolded;
  bool bAbbrev;
  bool bSkipZero;
  bool bOnlyZero;
  bool bShowBoard;
  bool bShowHandType;
  bool bShowHandTypId;
  bool bShowHand;
  bool bSawFlop;
  bool bSawRiver;
  bool bOnlyFolded;
  bool bSpentMoneyOnTheRiver;
  bool bStealthTwoPair;
  bool bNormalize;
  bool bOnlyLost;
  int only_count;
  bool bOnlyWon;
  bool bOnlyShowdown;
  bool bOnlyNoShowdown;
  bool bOnlyHeadsUpShowdown;
  bool bVeryBestHand;
  bool bTableCount;
  bool bAllIn;
  bool bNotAllIn;
  bool bAllInPreflop;
  bool bAllInPostflop;
  bool bHitFelt;
  bool bDidntHitFelt;
  bool bNoUncalled;
  bool bNoCollected;
  bool bShowCollected;
  bool bShowSpent;
  bool bShowOpm;
  bool bOnlyWash;
  bool bNoDelta;
  bool bHoleCardsUsed;
  bool bOnlySuited;
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
  bool bShowHandCount;
  bool bBottomTwo;
  bool bCounterfeit;
  bool bNumDecisions;
  bool bWonSidePot;
  bool bWonMainPot;
  bool bLastHandOnly;
  bool bGetDateFromFilename;
  bool bNoHoleCards;
  bool bSmallBlind;
  int small_blind;
  bool bBigBlind;
  int big_blind;
  bool bSmallOrBigBlind;
  int small_or_big_blind;
  bool bNoBlind;
  bool bDeuceOrTreyOff;
  bool bVoluntaryBet;
  bool bNoVoluntaryBet;
  bool bChasedFlush;
  bool bRiverCardUsed;
  bool bBothHoleCardsUsed;
  bool bShowRiver;
  bool bHandTypIdGeSpecified;
  bool bBadRiverMoney;
  bool bShowWagered;
  bool bUberflush;
  int no_blind;
  int collected_ge_num;
  bool bStud;
  bool bRazz;
  int hand_number;
  bool bSuited;
  bool bHaveFlop;
  bool bHaveFlop2;
  bool bHaveRiver;
  bool bHaveStealthTwoPair;
  bool bHaveBottomTwo;
  bool bSpentRiverMoney;
  bool bHaveShowdown;
  bool bHaveHeadsUpShowdown;
  bool bHaveAllIn;
  bool bHaveAllInPreflop;
  bool bHaveAllInPostflop;
  bool bHaveCounterfeit;
  bool bHaveWonSidePot;
  bool bHaveWonMainPot;
  bool bSummarizing;
  HandType hand_typ_id;
  HandType winning_hand_typ_id;
  HandType curr_winning_hand_typ_id;
  HandType hand_typ_id_ge;
  bool bFolded;
  bool bSkipping;
  bool bPostedSmallBlind;
  bool bPostedBigBlind;
  bool bHaveDeuceOrTreyOff;
  bool bHaveVoluntaryBet;
  bool bHaveChasedFlush;
  bool bHaveRiverCardUsed;
  bool bHaveBothHoleCardsUsed;
  bool bHaveBadRiverMoney;
};

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  int q;
  struct vars local_vars;
  char *hand;
  int curr_arg;
  int player_name_ix;
  int player_name_len;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  int dbg_line_no;
  int table_count;
  int showdown_count;
  int ix;
  int pot_ix;
  int street;
  int num_street_markers;
  int max_streets;
  int starting_balance;
  int ante;
  int bring_in;
  int spent_this_street;
  int spent_this_hand;
  int end_ix;
  int wagered_amount;
  int uncalled_bet_amount;
  int collected_from_pot;
  int collected_from_pot_count;
  int ending_balance;
  int delta;
  int file_no;
  int dbg_file_no;
  int num_hands_in_file;
  int num_hands;
  int dbg;
  int work;
  double dwork;
  char hole_cards[12];
  char board_cards[15];
  int table_count_to_match;
  int sum_delta;
  int sum_abs_delta;
  int max_delta;
  int min_delta;
  int max_abs_delta;
  int max_collected;
  int max_delta_hand_type;
  int max_delta_hand_typ;
  int winning_percentage;
  int rank_ix1;
  int rank_ix2;
  int cards[NUM_CARDS_IN_HOLDEM_POOL];
  PokerHand poker_hand;
  HoldemTurnHand turn_hand;
  HoldemPokerHand holdem_hand;
  char card_string[2];
  int retval;
  int total_delta;
  int hit_felt_in_session_count;
  int summary_val;
  int summary_val2;
  char *date_string;
  int *poker_hand_cards;
  int hole_cards_used;
  int sum_by_table_count[MAX_TABLE_COUNT - 1];
  int count_by_table_count[MAX_TABLE_COUNT - 1];
  int numdecs;

  if ((argc < 3) || (argc > 90)) {
    printf(usage);
    return 1;
  }

  init_plain_hand_type_lens();

  local_vars.bTerse = false;
  local_vars.bVerbose = false;
  local_vars.bDebug = false;
  local_vars.bHandTypeSpecified = false;
  local_vars.bWinningHandTypeSpecified = false;
  local_vars.bHandTypIdSpecified = false;
  local_vars.bHandSpecified = false;
  local_vars.bSkipFolded = false;
  local_vars.bAbbrev = false;
  local_vars.bSkipZero = false;
  local_vars.bOnlyZero = false;
  local_vars.bShowBoard = false;
  local_vars.bShowHandType = false;
  local_vars.bShowHandTypId = false;
  local_vars.bShowHand = false;
  local_vars.bSawFlop = false;
  local_vars.bSawRiver = false;
  local_vars.bOnlyFolded = false;
  local_vars.bSpentMoneyOnTheRiver = false;
  local_vars.bStealthTwoPair = false;
  local_vars.bNormalize = false;
  local_vars.bOnlyLost = false;
  local_vars.only_count = 0;
  local_vars.bOnlyWon = false;
  local_vars.bOnlyShowdown = false;
  local_vars.bOnlyNoShowdown = false;
  local_vars.bOnlyHeadsUpShowdown = false;
  local_vars.bVeryBestHand = false;
  local_vars.bTableCount = false;
  local_vars.bAllIn = false;
  local_vars.bNotAllIn = false;
  local_vars.bAllInPreflop = false;
  local_vars.bAllInPostflop = false;
  local_vars.bHitFelt = false;
  local_vars.bDidntHitFelt = false;
  local_vars.bNoUncalled = false;
  local_vars.bNoCollected = false;
  local_vars.bShowCollected = false;
  local_vars.bShowSpent = false;
  local_vars.bShowOpm = false;
  local_vars.bOnlyWash = false;
  sum_delta = 0;
  sum_abs_delta = 0;
  max_delta = 0;
  min_delta = 0;
  max_abs_delta = 0;
  max_collected = 0;
  max_delta_hand_type = 0;
  winning_percentage = 0;
  local_vars.bNoDelta = false;
  local_vars.bHoleCardsUsed = false;
  local_vars.bOnlySuited = false;
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
  local_vars.bShowHandCount = false;
  local_vars.bBottomTwo = false;
  local_vars.bCounterfeit = false;
  local_vars.bNumDecisions = false;
  local_vars.bWonSidePot = false;
  local_vars.bWonMainPot = false;
  local_vars.bLastHandOnly = false;
  local_vars.bGetDateFromFilename = false;
  local_vars.bNoHoleCards = false;
  local_vars.bSmallBlind = false;
  local_vars.small_blind = 0;
  local_vars.bBigBlind = false;
  local_vars.big_blind = 0;
  local_vars.bSmallOrBigBlind = false;
  local_vars.small_or_big_blind = 0;
  local_vars.bNoBlind = false;
  local_vars.no_blind = 0;
  local_vars.bDeuceOrTreyOff = false;
  local_vars.bVoluntaryBet = false;
  local_vars.bNoVoluntaryBet = false;
  local_vars.bChasedFlush = false;
  local_vars.bRiverCardUsed = false;
  local_vars.bBothHoleCardsUsed = false;
  local_vars.bShowRiver = false;
  local_vars.bHandTypIdGeSpecified = false;
  local_vars.bBadRiverMoney = false;
  local_vars.bShowWagered = false;
  local_vars.bUberflush = false;
  local_vars.hand_number = -1;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      local_vars.bTerse = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      local_vars.bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-debug"))
      local_vars.bDebug = true;
    else if (!strncmp(argv[curr_arg],"-hand_type",10)) {
      local_vars.hand_typ_id = get_hand_type(&argv[curr_arg][10]);
      local_vars.bHandTypeSpecified = true;
    }
    else if (!strncmp(argv[curr_arg],"-winning_hand_type",18)) {
      local_vars.winning_hand_typ_id = get_hand_type(&argv[curr_arg][18]);
      local_vars.bWinningHandTypeSpecified = true;
      local_vars.bOnlyShowdown = true;
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
      hand = &argv[curr_arg][5];
      local_vars.bHandSpecified = true;

      if ((strlen(&argv[curr_arg][5]) == 3) && (argv[curr_arg][7] == 's'))
        local_vars.bSuited = true;
      else
        local_vars.bSuited = false;
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
    else if (!strcmp(argv[curr_arg],"-spent_money_on_the_river"))
      local_vars.bSpentMoneyOnTheRiver = true;
    else if (!strcmp(argv[curr_arg],"-stealth_two_pair"))
      local_vars.bStealthTwoPair = true;
    else if (!strcmp(argv[curr_arg],"-normalize"))
      local_vars.bNormalize = true;
    else if (!strcmp(argv[curr_arg],"-only_lost"))
      local_vars.bOnlyLost = true;
    else if (!strcmp(argv[curr_arg],"-local_vars.only_count"))
      local_vars.only_count = 1;
    else if (!strcmp(argv[curr_arg],"-only_won"))
      local_vars.bOnlyWon = true;
    else if (!strcmp(argv[curr_arg],"-only_showdown"))
      local_vars.bOnlyShowdown = true;
    else if (!strcmp(argv[curr_arg],"-only_no_showdown"))
      local_vars.bOnlyNoShowdown = true;
    else if (!strcmp(argv[curr_arg],"-only_heads_up_showdown"))
      local_vars.bOnlyHeadsUpShowdown = true;
    else if (!strcmp(argv[curr_arg],"-very_best_hand"))
      local_vars.bVeryBestHand = true;
    else if (!strncmp(argv[curr_arg],"-table_count",12)) {
      local_vars.bTableCount = true;
      sscanf(&argv[curr_arg][12],"%d",&table_count_to_match);
    }
    else if (!strcmp(argv[curr_arg],"-all_in"))
      local_vars.bAllIn = true;
    else if (!strcmp(argv[curr_arg],"-not_all_in"))
      local_vars.bNotAllIn = true;
    else if (!strcmp(argv[curr_arg],"-all_in_preflop"))
      local_vars.bAllInPreflop = true;
    else if (!strcmp(argv[curr_arg],"-all_in_postflop"))
      local_vars.bAllInPostflop = true;
    else if (!strcmp(argv[curr_arg],"-hit_felt"))
      local_vars.bHitFelt = true;
    else if (!strcmp(argv[curr_arg],"-didnt_hit_felt"))
      local_vars.bDidntHitFelt = true;
    else if (!strcmp(argv[curr_arg],"-no_uncalled"))
      local_vars.bNoUncalled = true;
    else if (!strcmp(argv[curr_arg],"-no_collected"))
      local_vars.bNoCollected = true;
    else if (!strcmp(argv[curr_arg],"-show_collected"))
      local_vars.bShowCollected = true;
    else if (!strcmp(argv[curr_arg],"-show_spent"))
      local_vars.bShowSpent = true;
    else if (!strcmp(argv[curr_arg],"-show_opm"))
      local_vars.bShowOpm = true;
    else if (!strcmp(argv[curr_arg],"-only_wash"))
      local_vars.bOnlyWash = true;
    else if (!strcmp(argv[curr_arg],"-sum_delta"))
      sum_delta = 1;
    else if (!strcmp(argv[curr_arg],"-sum_abs_delta"))
      sum_abs_delta = 1;
    else if (!strcmp(argv[curr_arg],"-max_delta"))
      max_delta = 1;
    else if (!strcmp(argv[curr_arg],"-min_delta"))
      min_delta = 1;
    else if (!strcmp(argv[curr_arg],"-max_abs_delta"))
      max_abs_delta = 1;
    else if (!strcmp(argv[curr_arg],"-max_collected"))
      max_collected = 1;
    else if (!strcmp(argv[curr_arg],"-max_delta_hand_type"))
      max_delta_hand_type = 1;
    else if (!strcmp(argv[curr_arg],"-no_delta"))
      local_vars.bNoDelta = true;
    else if (!strcmp(argv[curr_arg],"-hole_cards_used"))
      local_vars.bHoleCardsUsed = true;
    else if (!strcmp(argv[curr_arg],"-only_suited"))
      local_vars.bOnlySuited = true;
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
      sscanf(&argv[curr_arg][13],"%d",&local_vars.collected_ge_num);
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
    else if (!strcmp(argv[curr_arg],"-show_hand_count"))
      local_vars.bShowHandCount = true;
    else if (!strcmp(argv[curr_arg],"-bottom_two"))
      local_vars.bBottomTwo = true;
    else if (!strcmp(argv[curr_arg],"-counterfeit")) {
      local_vars.bCounterfeit = true;
      local_vars.bSawRiver = true;
    }
    else if (!strcmp(argv[curr_arg],"-num_decisions"))
      local_vars.bNumDecisions = true;
    else if (!strcmp(argv[curr_arg],"-won_side_pot"))
      local_vars.bWonSidePot = true;
    else if (!strcmp(argv[curr_arg],"-won_main_pot"))
      local_vars.bWonMainPot = true;
    else if (!strcmp(argv[curr_arg],"-last_hand_only"))
      local_vars.bLastHandOnly = true;
    else if (!strcmp(argv[curr_arg],"-winning_percentage")) {
      winning_percentage = 1;
      local_vars.bShowHandCount = true;
    }
    else if (!strcmp(argv[curr_arg],"-get_date_from_filename"))
      local_vars.bGetDateFromFilename = true;
    else if (!strcmp(argv[curr_arg],"-no_hole_cards"))
      local_vars.bNoHoleCards = true;
    else if (!strcmp(argv[curr_arg],"-local_vars.small_blind")) {
      local_vars.bSmallBlind = true;
      local_vars.small_blind = 0;
    }
    else if (!strcmp(argv[curr_arg],"-big_blind")) {
      local_vars.bBigBlind = true;
      local_vars.big_blind = 1;
    }
    else if (!strcmp(argv[curr_arg],"-local_vars.small_or_big_blind")) {
      local_vars.bSmallOrBigBlind = true;
      local_vars.small_or_big_blind = 1;
    }
    else if (!strcmp(argv[curr_arg],"-local_vars.no_blind")) {
      local_vars.bNoBlind = true;
      local_vars.no_blind = 1;
    }
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
    else if (!strcmp(argv[curr_arg],"-show_wagered"))
      local_vars.bShowWagered = true;
    else if (!strcmp(argv[curr_arg],"-uberflush"))
      local_vars.bUberflush = true;
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

  if (!local_vars.bSawRiver && local_vars.bSpentMoneyOnTheRiver)
    local_vars.bSawRiver = true;

  if (local_vars.bStealthTwoPair && local_vars.bBottomTwo) {
    printf("can't specify both -stealth_two_pair and -hand_typehand_type\n");
    return 12;
  }

  if (local_vars.bBottomTwo && local_vars.bHandTypeSpecified) {
    printf("can't specify both -bottom_two and -hand_typehand_type\n");
    return 13;
  }

  if (local_vars.bStealthTwoPair && local_vars.bHandTypIdSpecified) {
    printf("can't specify both -stealth_two_pair and -hand_typ_idid\n");
    return 14;
  }

  if (local_vars.bBottomTwo && local_vars.bHandTypIdSpecified) {
    printf("can't specify both -bottom_two and -hand_typ_idid\n");
    return 15;
  }

  if (local_vars.bStealthTwoPair && local_vars.bHandSpecified) {
    printf("can't specify both -stealth_two_pair and -hand\n");
    return 16;
  }

  if (local_vars.bBottomTwo && local_vars.bHandSpecified) {
    printf("can't specify both -bottom_two and -hand\n");
    return 17;
  }

  if (local_vars.bStealthTwoPair && !local_vars.bSawFlop && !local_vars.bSawRiver)
    local_vars.bSawFlop = true;

  if (local_vars.bBottomTwo && !local_vars.bSawFlop && !local_vars.bSawRiver)
    local_vars.bSawFlop = true;

  if (local_vars.bUberflush) {
    local_vars.bOnlyLost = true;
    local_vars.hand_typ_id = FLUSH;
    local_vars.bHandTypeSpecified = true;
  }

  if (local_vars.bHandTypeSpecified && !local_vars.bSawFlop && !local_vars.bSawRiver)
    local_vars.bSawFlop = true;

  if (local_vars.bHandTypIdSpecified && !local_vars.bSawFlop && !local_vars.bSawRiver)
    local_vars.bSawFlop = true;

  if (local_vars.bHandTypIdGeSpecified && !local_vars.bSawFlop && !local_vars.bSawRiver)
    local_vars.bSawFlop = true;

  if (local_vars.bOnlyLost && local_vars.bOnlyWon) {
    printf("can't specify both -only_lost and -only_won\n");
    return 18;
  }

  if (local_vars.bSkipZero && local_vars.bOnlyZero) {
    printf("can't specify both -skip_zero and -only_zero\n");
    return 19;
  }

  if (local_vars.bOnlyShowdown && local_vars.bOnlyNoShowdown) {
    printf("can't specify both -only_showdown and -only_no_showdown\n");
    return 20;
  }

  if (local_vars.bOnlyShowdown && local_vars.bOnlyHeadsUpShowdown) {
    printf("can't specify both -only_showdown and -only_heads_up_showdown\n");
    return 21;
  }

  if (local_vars.bOnlyNoShowdown && local_vars.bOnlyHeadsUpShowdown) {
    printf("can't specify both -only_no_showdown and -only_heads_up_showdown\n");
    return 22;
  }

  if (local_vars.bAllIn && local_vars.bNotAllIn) {
    printf("can't specify both -all_in and -not_all_in\n");
    return 23;
  }

  if (local_vars.bAllIn && local_vars.bAllInPreflop) {
    printf("can't specify both -all_in and -all_in_preflop\n");
    return 24;
  }

  if (local_vars.bNotAllIn && local_vars.bAllInPreflop) {
    printf("can't specify both -not_all_in and -all_in_preflop\n");
    return 25;
  }

  if (local_vars.bShowCollected && local_vars.bShowOpm) {
    printf("can't specify both -show_collected and -show_opm\n");
    return 26;
  }

  if (local_vars.bOnlyLost && local_vars.bOnlyWash) {
    printf("can't specify both -only_lost and -only_wash\n");
    return 27;
  }

  if (local_vars.bOnlyWon && local_vars.bOnlyWash) {
    printf("can't specify both -only_won and -only_wash\n");
    return 28;
  }

  if (local_vars.only_count + sum_delta + sum_abs_delta + max_delta + min_delta +
    max_abs_delta + max_collected + max_delta_hand_type +
    winning_percentage > 1) {
    printf("can only specify one of -local_vars.only_count, -sum_delta, -sum_abs_delta, -max_delta, -min_delta, -max_abs_delta, -max_collected, -max_delta_hand_type, and -winning_percentage\n");
    return 29;
  }

  if ((local_vars.only_count) || (sum_delta) || (sum_abs_delta) || (max_delta) ||
      (min_delta) || (max_abs_delta) || (max_collected) ||
      (max_delta_hand_type) || (winning_percentage)) {
    local_vars.bTerse = true;
    local_vars.bGetDateFromFilename = true;
    local_vars.bSummarizing = true;
  }
  else
    local_vars.bSummarizing = false;

  if (local_vars.bHoleCardsUsed) {
    if (local_vars.bAbbrev) {
      printf("can't specify both -abbrev and -hole_cards_used\n");
      return 30;
    }

    local_vars.bSawFlop = true;
  }

  if (local_vars.bShowCollected && local_vars.bShowSpent) {
    printf("can't specify both -show_collected and -show_spent\n");
    return 31;
  }

  if (local_vars.bShowCollected && local_vars.bShowWagered) {
    printf("can't specify both -show_collected and -show_wagered\n");
    return 32;
  }

  if (local_vars.bShowSpent && local_vars.bShowWagered) {
    printf("can't specify both -show_spent and -show_wagered\n");
    return 33;
  }

  if (local_vars.bHitFelt && local_vars.bDidntHitFelt) {
    printf("can't specify both -hit_felt and -didnt_hit_felt\n");
    return 34;
  }

  if (local_vars.bVeryBestHand && local_vars.bFlopped) {
    printf("can't specify both -very_best_hand and -flopped\n");
    return 35;
  }

  if (local_vars.bHandTypeSpecified && local_vars.bHandTypIdSpecified) {
    printf("can't specify both -hand_typehand_type and -hand_typ_idid\n");
    return 36;
  }

  if (local_vars.bShowHandType && local_vars.bShowHandTypId) {
    printf("can't specify both -show_hand_type and -show_hand_typ_id\n");
    return 37;
  }

  if (local_vars.bSawFlop && local_vars.bDidntSeeFlop) {
    printf("can't specify both -saw_flop and -didnt_see_flop\n");
    return 38;
  }

  if (local_vars.bOnlyWinningSession && local_vars.bOnlyLosingSession) {
    printf("can't specify both -only_winning_session and -only_losing_session\n");
    return 39;
  }

  if (local_vars.bSumByTableCount && local_vars.bShowTableName) {
    printf("can't specify both -sum_by_table_count and -show_table_name\n");
    return 40;
  }

  if (local_vars.bSumByTableCount && local_vars.bShowTableCount) {
    printf("can't specify both -sum_by_table_count and -show_table_count\n");
    return 41;
  }

  if (local_vars.small_blind + local_vars.big_blind + local_vars.small_or_big_blind + local_vars.no_blind > 1) {
    printf("can only specify one of -local_vars.small_blind, -big_blind, -local_vars.small_or_big_blind, and -local_vars.no_blind\n");
    return 42;
  }

  if (local_vars.bHandSpecified && local_vars.bDeuceOrTreyOff) {
    printf("can't specify both -handhand and -deuce_or_trey_off\n");
    return 43;
  }

  if (local_vars.bVoluntaryBet && local_vars.bNoVoluntaryBet) {
    printf("can't specify both -voluntary_bet and -no_voluntary_bet\n");
    return 44;
  }

  if (local_vars.bShowBoard && local_vars.bShowRiver) {
    printf("can't specify both -show_board and -show_river\n");
    return 45;
  }

  if (local_vars.bHandTypIdSpecified && local_vars.bHandTypIdGeSpecified) {
    printf("can't specify both -hand_typ_idid and -hand_typ_id_geid\n");
    return 46;
  }

  if (local_vars.bStealthTwoPair && local_vars.bHandTypIdGeSpecified) {
    printf("can't specify both -stealth_two_pair and -hand_typ_id_geid\n");
    return 47;
  }

  if (local_vars.bBottomTwo && local_vars.bHandTypIdGeSpecified) {
    printf("can't specify both -bottom_two and -hand_typ_id_geid\n");
    return 48;
  }

  if (local_vars.bHandTypeSpecified && local_vars.bHandTypIdGeSpecified) {
    printf("can't specify both -hand_typehand_type and -hand_typ_id_geid\n");
    return 49;
  }

  if (local_vars.bAllIn && local_vars.bAllInPostflop) {
    printf("can't specify both -all_in and -all_in_postflop\n");
    return 50;
  }

  if (local_vars.bAllInPreflop && local_vars.bAllInPostflop) {
    printf("can't specify both -all_in_preflop and -all_in_postflop\n");
    return 51;
  }

  player_name_ix = curr_arg++;
  player_name_len = strlen(argv[player_name_ix]);

  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 52;
  }

  if (!local_vars.bSawRiver && (local_vars.bChasedFlush || local_vars.bRiverCardUsed || local_vars.bShowRiver))
    local_vars.bSawRiver = true;

  ending_balance = -1;

  file_no = 0;
  dbg_file_no = -1;

  if (!local_vars.bAbbrev)
    hole_cards[11] = 0;
  else
    hole_cards[3] = 0;

  if (local_vars.bSumByTableCount) {
    for (n = 0; n < MAX_TABLE_COUNT - 1; n++) {
      sum_by_table_count[n] = 0;
      count_by_table_count[n] = 0;
    }
  }

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    if (local_vars.bGetDateFromFilename) {
      retval = get_date_from_path(filename,'\\',3,&date_string);

      if (retval) {
        printf("get_date_from_path() failed on %s: %d\n",filename,retval);
        continue;
      }
    }

    file_no++;

    if (local_vars.bSummarizing) {
      total_delta = 0;
      hit_felt_in_session_count = 0;
      summary_val = 0;
      summary_val2 = 0;
    }

    if (dbg_file_no == file_no)
      dbg = 1;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    if (local_vars.bLastHandOnly) {
      num_hands_in_file = get_num_hands_in_file(fptr,
        argv[player_name_ix],player_name_len);
    }

    line_no = 0;
    local_vars.bFolded = false;
    local_vars.bSkipping = false;
    num_hands = 0;
    local_vars.bHaveFlop = false;
    local_vars.bHaveFlop2 = false;
    local_vars.bHaveRiver = false;
    local_vars.bSpentRiverMoney = false;
    local_vars.bHaveStealthTwoPair = false;
    local_vars.bHaveBottomTwo = false;
    local_vars.bHaveShowdown = false;
    showdown_count = 0;
    local_vars.bHaveHeadsUpShowdown = false;
    local_vars.bHaveAllIn = false;
    local_vars.bHaveAllInPreflop = false;
    local_vars.bHaveAllInPostflop = false;
    local_vars.bHaveCounterfeit = false;
    local_vars.bPostedSmallBlind = false;
    local_vars.bPostedBigBlind = false;
    local_vars.bHaveDeuceOrTreyOff = false;
    local_vars.bHaveVoluntaryBet = false;
    local_vars.bHaveChasedFlush = false;
    local_vars.bHaveBadRiverMoney = false;

    if (local_vars.bWonSidePot)
      local_vars.bHaveWonSidePot = false;

    if (local_vars.bWonMainPot)
      local_vars.bHaveWonMainPot = false;

    if (local_vars.bNumDecisions)
      numdecs = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr)) {
        if (local_vars.bSummarizing) {
          if (!local_vars.bSkipZero || summary_val) {
            if (!local_vars.bOnlyWinningSession || (total_delta > 0)) {
              if (!local_vars.bOnlyLosingSession || (total_delta < 0)) {
                if (!local_vars.bNeverHitFeltInSession || (!hit_felt_in_session_count)) {
                  if (!max_delta_hand_type) {
                    if (!local_vars.bShowHandCount)
                      printf("%d\t%s\n",summary_val,date_string);
                    else {
                      if (!winning_percentage)
                        printf("%d\t%s\t(%d)\n",summary_val,date_string,num_hands);
                      else {
                        dwork = (double)summary_val2 / (double)summary_val;
                        printf("%lf\t%s\t(%d %d)\n",dwork,date_string,summary_val2,summary_val);
                      }
                    }
                  }
                  else {
                    if (!local_vars.bShowHandCount) {
                      printf("%d\t%s\t%s\n",summary_val,
                        plain_hand_types[max_delta_hand_typ],date_string);
                    }
                    else {
                      printf("%d\t%s\t%s\t(%d)\n",summary_val,
                        plain_hand_types[max_delta_hand_typ],date_string,num_hands);
                    }
                  }
                }
              }
            }
          }
        }

        break;
      }

      line_no++;

      if (line_no == dbg_line_no)
        dbg = 1;

      if (local_vars.bDebug)
        printf("line %d %s\n",line_no,line);

      if (Contains(true,
        line,line_len,
        pokerstars,POKERSTARS_LEN,
        &ix)) {

        local_vars.bStud = false;
        local_vars.bRazz = false;

        if (Contains(true,
          line,line_len,
          stud,STUD_LEN,
          &ix)) {

          local_vars.bStud = true;
        }
        else if (Contains(true,
          line,line_len,
          razz,RAZZ_LEN,
          &ix)) {

          local_vars.bRazz = true;
        }

        if (local_vars.bStud || local_vars.bRazz)
          max_streets = 4;
        else
          max_streets = 3;
      }
      else if (!strncmp(line,"Table '",7)) {
        get_table_name(line,line_len,table_name,MAX_TABLE_NAME_LEN);

        table_count = 0;

        for ( ; ; ) {
          GetLine(fptr,line,&line_len,MAX_LINE_LEN);

          if (feof(fptr))
            break;

          line_no++;

          if (!strncmp(line,"*** ",4)) {
            num_street_markers++;
            break;
          }

          if (!strncmp(line,"Seat ",5)) {
            table_count++;

            if (Contains(true,
              line,line_len,
              argv[player_name_ix],player_name_len,
              &ix)) {

              if (Contains(true,
                line,line_len,
                in_chips,IN_CHIPS_LEN,
                &ix)) {

                num_hands++;
                local_vars.bFolded = false;
                local_vars.bSkipping = false;
                local_vars.bHaveFlop = false;
                local_vars.bHaveFlop2 = false;
                local_vars.bHaveRiver = false;
                local_vars.bSpentRiverMoney = false;
                local_vars.bHaveStealthTwoPair = false;
                local_vars.bHaveBottomTwo = false;
                local_vars.bHaveShowdown = false;
                showdown_count = 0;
                local_vars.bHaveHeadsUpShowdown = false;
                local_vars.bHaveAllIn = false;
                local_vars.bHaveAllInPreflop = false;
                local_vars.bHaveAllInPostflop = false;
                local_vars.bHaveCounterfeit = false;
                local_vars.bPostedSmallBlind = false;
                local_vars.bPostedBigBlind = false;
                local_vars.bHaveDeuceOrTreyOff = false;
                local_vars.bHaveVoluntaryBet = false;
                local_vars.bHaveChasedFlush = false;
                local_vars.bHaveBadRiverMoney = false;

                if (local_vars.bWonSidePot)
                  local_vars.bHaveWonSidePot = false;

                if (local_vars.bWonMainPot)
                  local_vars.bHaveWonMainPot = false;

                if (local_vars.bNumDecisions)
                  numdecs = 0;

                street = 0;
                num_street_markers = 0;
                ante = 0;
                bring_in = 0;
                spent_this_street = 0;
                spent_this_hand = 0;
                uncalled_bet_amount = 0;
                collected_from_pot = 0;
                collected_from_pot_count = 0;

                line[ix] = 0;

                for (ix--; (ix >= 0) && (line[ix] != '('); ix--)
                  ;

                sscanf(&line[ix+1],"%d",&starting_balance);

                if (local_vars.bDebug)
                  printf("line %d starting_balance = %d\n",line_no,starting_balance);
              }
            }
          }
          else if (Contains(true,
            line,line_len,
            argv[player_name_ix],player_name_len,
            &ix)) {

            if (Contains(true,
              line,line_len,
              posts_the_ante,POSTS_THE_ANTE_LEN,
              &ix)) {
              ante = get_work_amount(line,line_len);
              spent_this_hand = ante;
            }
            else if (Contains(true,
              line,line_len,
              posts,POSTS_LEN,
              &ix)) {
              work = get_work_amount(line,line_len);
              spent_this_street += work;

              if (Contains(true,
                line,line_len,
                posts_small_blind,POSTS_SMALL_BLIND_LEN,
                &ix)) {

                local_vars.bPostedSmallBlind = true;
              }
              else if (Contains(true,
                line,line_len,
                posts_big_blind,POSTS_BIG_BLIND_LEN,
                &ix)) {

                local_vars.bPostedBigBlind = true;
              }

              if (local_vars.bDebug) {
                printf("line %d street %d POSTS work = %d, spent_this_street = %d\n",
                  line_no,street,work,spent_this_street);
              }
            }
          }
        }

        if (table_count > MAX_TABLE_COUNT) {
          printf("%s: too many players at the table\n",filename);
          return 53;
        }

        continue;
      }
      else if (local_vars.bSkipping) {
        if (local_vars.bWinningHandTypeSpecified) {
          if (Contains(true,
            line,line_len,
            won,WON_LEN,
            &ix)) {

            local_vars.curr_winning_hand_typ_id = get_winning_hand_typ_id(line,line_len);
          }
        }

        continue;
      }
      else if (Contains(true,
        line,line_len,
        argv[player_name_ix],player_name_len,
        &ix)) {

        if (Contains(true,
          line,line_len,
          all_in,ALL_IN_LEN,
          &ix)) {

          local_vars.bHaveAllIn = true;

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
              if (!local_vars.bAbbrev) {
                for (p = 0; p < 11; p++) {
                  if (line[n+p] == ']') {
                    if (p == 5)
                      hole_cards[p] = 0;

                    break;
                  }
                  else
                    hole_cards[p] = line[n+p];
                }

                if (local_vars.bDeuceOrTreyOff)
                  local_vars.bHaveDeuceOrTreyOff = deuce_or_trey_off(hole_cards);

                if (local_vars.bNormalize)
                  normalize_hole_cards(hole_cards);

                for (q = 0; q < NUM_HOLE_CARDS_IN_HOLDEM_HAND; q++) {
                  card_string[0] = hole_cards[q*3 + 0];
                  card_string[1] = hole_cards[q*3 + 1];

                  retval = card_value_from_card_string(
                    card_string,&cards[q]);

                  if (retval) {
                    printf("invalid card string %s on line %d\n",
                      card_string,line_no);
                    return 54;
                  }
                }
              }
              else {
                for (rank_ix1 = 0; rank_ix1 < NUM_CARDS_IN_SUIT; rank_ix1++) {
                  if (line[n] == rank_chars[rank_ix1])
                    break;
                }

                if (rank_ix1 == NUM_CARDS_IN_SUIT)
                  rank_ix1 = 0;

                for (rank_ix2 = 0; rank_ix2 < NUM_CARDS_IN_SUIT; rank_ix2++) {
                  if (line[n+3] == rank_chars[rank_ix2])
                    break;
                }

                if (rank_ix2 == NUM_CARDS_IN_SUIT)
                  rank_ix2 = 0;

                if (rank_ix1 >= rank_ix2) {
                  hole_cards[0] = line[n];
                  hole_cards[1] = line[n+3];
                }
                else {
                  hole_cards[0] = line[n+3];
                  hole_cards[1] = line[n];
                }

                if (hole_cards[0] == hole_cards[1])
                  hole_cards[2] = ' ';
                else {
                  if (line[n+1] == line[n+4])
                    hole_cards[2] = 's';
                  else
                    hole_cards[2] = 'o';
                }
              }
            }

            if (local_vars.bHandSpecified) {
              if (local_vars.bSuited) {
                if (hole_cards[1] != hole_cards[4])
                  local_vars.bSkipping = true;
              }
              else {
                if (hole_cards[1] == hole_cards[4])
                  local_vars.bSkipping = true;
              }

              if (((hole_cards[0] != hand[0]) || (hole_cards[3] != hand[1])) &&
                  ((hole_cards[0] != hand[1]) || (hole_cards[3] != hand[0])))
                local_vars.bSkipping = true;
            }
          }
        }
        else if (Contains(true,
          line,line_len,
          collected,COLLECTED_LEN,
          &ix)) {

          if (local_vars.bWonSidePot) {
            if (Contains(true,
              line,line_len,
              side_pot,SIDE_POT_LEN,
              &pot_ix)) {
              local_vars.bHaveWonSidePot = true;
            }
          }

          if (local_vars.bWonMainPot) {
            if (Contains(true,
              line,line_len,
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
          sscanf(&line[ix + COLLECTED_LEN],"%d",&work);

          if (!collected_from_pot_count) {
            spent_this_hand += spent_this_street;
            street++;
            spent_this_street = 0;
          }

          collected_from_pot += work;
          collected_from_pot_count++;

          if (local_vars.bDebug) {
            printf("line %d street %d COLLECTED work = %d, collected_from_pot = %d\n",
              line_no,street,work,collected_from_pot);
          }

          continue;
        }
        else if (!strncmp(line,uncalled_bet,UNCALLED_BET_LEN)) {
          sscanf(&line[UNCALLED_BET_LEN],"%d",&uncalled_bet_amount);
          spent_this_street -= uncalled_bet_amount;

          if (local_vars.bDebug) {
            printf("line %d street %d UNCALLED uncalled_bet_amount = %d, spent_this_street = %d\n",
              line_no,street,uncalled_bet_amount,spent_this_street);
          }

          continue;
        }
        else if (Contains(true,
          line,line_len,
          folds,FOLDS_LEN,
          &ix)) {

          spent_this_hand += spent_this_street;

          spent_this_street = 0;

          if (local_vars.bDebug) {
            printf("line %d street %d FOLDS spent_this_street = %d, spent_this_hand = %d\n",
              line_no,street,spent_this_street,spent_this_hand);
          }

          local_vars.bFolded = true;

          ending_balance = starting_balance - spent_this_hand + collected_from_pot;

          if (local_vars.bSummarizing && !ending_balance)
            hit_felt_in_session_count++;

          delta = ending_balance - starting_balance;
          wagered_amount = spent_this_hand + uncalled_bet_amount;

          if (local_vars.bShowOpm)
            dwork = (double)0;

          if (local_vars.bNumDecisions)
            numdecs++;

          continue;
        }
        else if (Contains(true,
          line,line_len,
          bets,BETS_LEN,
          &ix)) {
          work = get_work_amount(line,line_len);
          spent_this_street += work;

          if (street == 3)
            local_vars.bSpentRiverMoney = true;

          if (local_vars.bDebug) {
            printf("line %d street %d BETS work = %d, spent_this_street = %d\n",
              line_no,street,work,spent_this_street);
          }

          if (local_vars.bNumDecisions)
            numdecs++;

          local_vars.bHaveVoluntaryBet = true;
        }
        else if (Contains(true,
          line,line_len,
          calls,CALLS_LEN,
          &ix)) {
          work = get_work_amount(line,line_len);
          spent_this_street += work;

          if (street == 3)
            local_vars.bSpentRiverMoney = true;

          if (local_vars.bDebug) {
            printf("line %d street %d CALLS work = %d, spent_this_street = %d\n",
              line_no,street,work,spent_this_street);
          }

          if (local_vars.bNumDecisions)
            numdecs++;

          local_vars.bHaveVoluntaryBet = true;
        }
        else if (Contains(true,
          line,line_len,
          raises,RAISES_LEN,
          &ix)) {
          work = get_work_amount(line,line_len);
          spent_this_street = work;

          if (street == 3)
            local_vars.bSpentRiverMoney = true;

          if (local_vars.bDebug) {
            printf("line %d street %d RAISES work = %d, spent_this_street = %d\n",
              line_no,street,work,spent_this_street);
          }

          if (local_vars.bNumDecisions)
            numdecs++;

          local_vars.bHaveVoluntaryBet = true;
        }
        else if (Contains(true,
          line,line_len,
          checks,CHECKS_LEN,
          &ix)) {

          if (local_vars.bNumDecisions)
            numdecs++;
        }
        else if ((local_vars.bStud || local_vars.bRazz) && Contains(true,
          line,line_len,
          brings_in_for,BRINGS_IN_FOR_LEN,
          &ix)) {
          bring_in = get_work_amount(line,line_len);
          spent_this_street += bring_in;
          continue;
        }
        else if (local_vars.bOnlyHeadsUpShowdown && Contains(true,
          line,line_len,
          ": shows",7,
          &ix)) {

          showdown_count++;
        }
        else if (local_vars.bOnlyHeadsUpShowdown && Contains(true,
          line,line_len,
          ": mucks",7,
          &ix)) {

          showdown_count++;
        }
      }
      else {
        if (!strncmp(line,flop,FLOP_LEN)) {
          if (local_vars.bStealthTwoPair && !local_vars.bFolded) {
            if (hole_cards[0] != hole_cards[3]) {
              if (((hole_cards[0] == line[FLOP_LEN]) ||
                   (hole_cards[0] == line[FLOP_LEN+3]) ||
                   (hole_cards[0] == line[FLOP_LEN+6])) &&
                  ((hole_cards[3] == line[FLOP_LEN]) ||
                   (hole_cards[3] == line[FLOP_LEN+3]) ||
                   (hole_cards[3] == line[FLOP_LEN+6]))) {
                local_vars.bHaveStealthTwoPair = true;
              }
            }
          }

          if (local_vars.bBottomTwo && !local_vars.bFolded) {
            if (bottom_two(line,line_len,hole_cards))
              local_vars.bHaveBottomTwo = true;
          }

          for (n = 0; n < 8; n++)
            board_cards[n] = line[FLOP_LEN+n];

          board_cards[n] = 0;

          for (q = 0; q < NUM_CARDS_IN_FLOP; q++) {
            card_string[0] = board_cards[q*3 + 0];
            card_string[1] = board_cards[q*3 + 1];

            retval = card_value_from_card_string(
              card_string,&cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND+q]);

            if (retval) {
              printf("invalid card string %s on line %d\n",
                card_string,line_no);
              return 55;
            }
          }

          poker_hand.NewCards(cards[0],cards[1],cards[2],cards[3],cards[4]);
          poker_hand.Evaluate();

          if (!local_vars.bFolded)
            local_vars.bHaveFlop = true;

          local_vars.bHaveFlop2 = true;
        }
        else if (!strncmp(line,turn,TURN_LEN)) {
          n = 8;

          board_cards[n++] = ' ';

          for (m = 0; m < 2; m++,n++)
            board_cards[n] = line[TURN_LEN+11+m];

          board_cards[n] = 0;

          card_string[0] = board_cards[9];
          card_string[1] = board_cards[10];

          retval = card_value_from_card_string(
            card_string,&cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND+NUM_CARDS_IN_FLOP]);

          if (retval) {
            printf("invalid card string %s on line %d\n",
              card_string,line_no);
            return 56;
          }

          if (!local_vars.bFlopped && (!local_vars.bFolded || local_vars.bVeryBestHand)) {
            turn_hand.NewCards(cards[0],cards[1],cards[2],
              cards[3],cards[4],cards[5]);

            poker_hand = turn_hand.BestPokerHand();
          }

          if (local_vars.bChasedFlush)
            local_vars.bHaveChasedFlush = four_to_a_flush(cards);
        }
        else if (!strncmp(line,river,RIVER_LEN)) {
          if (!local_vars.bStud && !local_vars.bRazz) {
            n = 11;

            board_cards[n++] = ' ';

            for (m = 0; m < 2; m++,n++)
              board_cards[n] = line[RIVER_LEN+14+m];

            board_cards[n] = 0;

            card_string[0] = board_cards[12];
            card_string[1] = board_cards[13];

            retval = card_value_from_card_string(
              card_string,&cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND+NUM_CARDS_IN_FLOP+1]);

            if (retval) {
              printf("invalid card string %s on line %d\n",
                card_string,line_no);
              return 57;
            }

            if (!local_vars.bFlopped && (!local_vars.bFolded || local_vars.bVeryBestHand)) {
              holdem_hand.NewCards(cards[0],cards[1],cards[2],
                cards[3],cards[4],cards[5],cards[6]);

              poker_hand = holdem_hand.BestPokerHand();

              if (!local_vars.bFolded)
                local_vars.bHaveRiver = true;
            }

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
        }
        else if (local_vars.bOnlyHeadsUpShowdown && Contains(true,
          line,line_len,
          ": shows",7,
          &ix)) {

          showdown_count++;
        }
        else if (local_vars.bOnlyHeadsUpShowdown && Contains(true,
          line,line_len,
          ": mucks",7,
          &ix)) {

          showdown_count++;
        }
        else if (!strncmp(line,summary,SUMMARY_LEN)) {
          if (local_vars.bDebug)
            printf("line %d SUMMARY line detected; skipping\n",line_no);

          local_vars.bSkipping = true;

          if (!local_vars.bFolded) {
            ending_balance = starting_balance - spent_this_hand + collected_from_pot;

            if (local_vars.bSummarizing && !ending_balance)
              hit_felt_in_session_count++;

            delta = ending_balance - starting_balance;
            wagered_amount = spent_this_hand + uncalled_bet_amount;

            if (local_vars.bShowOpm) {
              if (collected_from_pot && (delta > 0))
                dwork = (double)delta / (double)collected_from_pot;
              else
                dwork = (double)0;
            }
          }

          if (local_vars.bRiverCardUsed) {
            local_vars.bHaveRiverCardUsed = false;

            if (local_vars.bHaveRiver) {
              poker_hand_cards = poker_hand.GetCards();

              for (q = 0; q < NUM_CARDS_IN_HAND; q++) {
                if (poker_hand_cards[q] == cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND+NUM_CARDS_IN_FLOP+1]) {
                  local_vars.bHaveRiverCardUsed = true;
                  break;
                }
              }
            }
          }

          if (local_vars.bHoleCardsUsed || local_vars.bBothHoleCardsUsed) {
            poker_hand_cards = poker_hand.GetCards();
            hole_cards_used = 0;

            for (p = 0; p < NUM_HOLE_CARDS_IN_HOLDEM_HAND; p++) {
              for (q = 0; q < NUM_CARDS_IN_HAND; q++) {
                if (poker_hand_cards[q] == cards[p]) {
                  hole_cards_used++;
                  break;
                }
              }
            }

            if (local_vars.bBothHoleCardsUsed) {
              if (hole_cards_used != 2)
                local_vars.bHaveBothHoleCardsUsed = false;
              else
                local_vars.bHaveBothHoleCardsUsed = true;
            }
          }

          if (local_vars.bOnlyHeadsUpShowdown && local_vars.bHaveShowdown && (showdown_count == 2))
            local_vars.bHaveHeadsUpShowdown = true;

          if (local_vars.bSummarizing || !local_vars.bSkipZero || (delta != 0)) {
            if (!local_vars.bOnlyZero || (delta == 0)) {
              if (!local_vars.bSkipFolded || !local_vars.bFolded) {
                if (!local_vars.bOnlyFolded || local_vars.bFolded) {
                  if (!local_vars.bSawFlop || local_vars.bHaveFlop) {
                    if (!local_vars.bDidntSeeFlop || !local_vars.bHaveFlop) {
                      if (!local_vars.bStealthTwoPair || local_vars.bHaveStealthTwoPair) {
                        if (!local_vars.bBottomTwo || local_vars.bHaveBottomTwo) {
                          if (!local_vars.bSawRiver || local_vars.bHaveRiver) {
                            if (!local_vars.bSpentMoneyOnTheRiver || local_vars.bSpentRiverMoney) {
                              if (!local_vars.bHandTypeSpecified || (local_vars.hand_typ_id == poker_hand.GetHandType())) {
                                if (!local_vars.bWinningHandTypeSpecified || (local_vars.curr_winning_hand_typ_id == local_vars.winning_hand_typ_id)) {
                                  if (!local_vars.bHandTypIdSpecified || (poker_hand.GetHandType() == local_vars.hand_typ_id)) {
                                    if (!local_vars.bOnlyLost || (delta < 0)) {
                                      if (!local_vars.bOnlyWon || (delta > 0)) {
                                        if (!local_vars.bOnlyWash || (delta == 0)) {
                                          if (!local_vars.bOnlyShowdown || local_vars.bHaveShowdown) {
                                            if (!local_vars.bOnlyHeadsUpShowdown || local_vars.bHaveHeadsUpShowdown) {
                                              if (!local_vars.bOnlyNoShowdown || !local_vars.bHaveShowdown) {
                                                if (!local_vars.bTableCount || (table_count == table_count_to_match)) {
                                                  if (!local_vars.bAllIn || local_vars.bHaveAllIn) {
                                                    if (!local_vars.bNotAllIn || !local_vars.bHaveAllIn) {
                                                      if (!local_vars.bAllInPreflop || local_vars.bHaveAllInPreflop) {
                                                        if (!local_vars.bAllInPostflop || local_vars.bHaveAllInPostflop) {
                                                          if (!local_vars.bHitFelt || (ending_balance == 0)) {
                                                            if (!local_vars.bDidntHitFelt || (ending_balance > 0)) {
                                                              if (!local_vars.bNoUncalled || (uncalled_bet_amount == 0)) {
                                                                if (!local_vars.bNoCollected || (collected_from_pot == 0)) {
                                                                  if (!local_vars.bOnlySuited || (hole_cards[1] == hole_cards[4])) {
                                                                    if (!local_vars.bPocketPair || (hole_cards[0] == hole_cards[3])) {
                                                                      if ((local_vars.hand_number == -1) || (num_hands == local_vars.hand_number)) {
                                                                        if (!local_vars.bCollectedGe || (collected_from_pot >= local_vars.collected_ge_num)) {
                                                                          if (!local_vars.bCounterfeit || local_vars.bHaveCounterfeit) {
                                                                            if (!local_vars.bWonSidePot || local_vars.bHaveWonSidePot) {
                                                                              if (!local_vars.bWonMainPot || local_vars.bHaveWonMainPot) {
                                                                                if (!local_vars.bLastHandOnly || (num_hands == num_hands_in_file)) {
                                                                                  if (!local_vars.bSmallBlind || local_vars.bPostedSmallBlind) {
                                                                                    if (!local_vars.bBigBlind || local_vars.bPostedBigBlind) {
                                                                                      if (!local_vars.bSmallOrBigBlind || local_vars.bPostedSmallBlind || local_vars.bPostedBigBlind) {
                                                                                        if (!local_vars.bNoBlind || (!local_vars.bPostedSmallBlind && !local_vars.bPostedBigBlind)) {
                                                                                          if (!local_vars.bDeuceOrTreyOff || local_vars.bHaveDeuceOrTreyOff) {
                                                                                            if (!local_vars.bVoluntaryBet || local_vars.bHaveVoluntaryBet) {
                                                                                              if (!local_vars.bNoVoluntaryBet || !local_vars.bHaveVoluntaryBet) {
                                                                                                if (!local_vars.bChasedFlush || local_vars.bHaveChasedFlush) {
                                                                                                  if (!local_vars.bRiverCardUsed || local_vars.bHaveRiverCardUsed) {
                                                                                                    if (!local_vars.bBothHoleCardsUsed || local_vars.bHaveBothHoleCardsUsed) {
                                                                                                      if (!local_vars.bHandTypIdGeSpecified || (poker_hand.GetHandType() >= local_vars.hand_typ_id_ge)) {
                                                                                                        if (!local_vars.bBadRiverMoney || local_vars.bHaveBadRiverMoney) {
                                                                                                          if (local_vars.bTerse) {
                                                                                                            if (!local_vars.bSummarizing && !local_vars.bSumByTableCount) {
                                                                                                              if (local_vars.bShowCollected)
                                                                                                                printf("%d\n",collected_from_pot);
                                                                                                              else if (local_vars.bShowSpent)
                                                                                                                printf("%d\n",spent_this_hand);
                                                                                                              else if (local_vars.bShowOpm)
                                                                                                                printf("%lf\n",dwork);
                                                                                                              else if (local_vars.bNumDecisions)
                                                                                                                printf("%d\n",numdecs);
                                                                                                              else if (local_vars.bShowWagered)
                                                                                                                printf("%d\n",wagered_amount);
                                                                                                              else
                                                                                                                printf("%d\n",delta);
                                                                                                            }
                                                                                                            else if (!local_vars.bSumByTableCount) {
                                                                                                              total_delta += delta;

                                                                                                              if (winning_percentage) {
                                                                                                                summary_val++;

                                                                                                                if (delta > 0)
                                                                                                                  summary_val2++;
                                                                                                              }
                                                                                                              else if (local_vars.only_count)
                                                                                                                summary_val++;
                                                                                                              else if (sum_delta) {
                                                                                                                if (local_vars.bShowCollected)
                                                                                                                  summary_val += collected_from_pot;
                                                                                                                else if (local_vars.bShowSpent)
                                                                                                                  summary_val += spent_this_hand;
                                                                                                                else if (local_vars.bShowWagered)
                                                                                                                  summary_val += wagered_amount;
                                                                                                                else if (local_vars.bNumDecisions)
                                                                                                                  summary_val += numdecs;
                                                                                                                else
                                                                                                                  summary_val += delta;
                                                                                                              }
                                                                                                              else if (sum_abs_delta) {
                                                                                                                if (delta > 0)
                                                                                                                  summary_val += delta;
                                                                                                                else
                                                                                                                  summary_val -= delta;
                                                                                                              }
                                                                                                              else if (max_delta) {
                                                                                                                if (delta > summary_val)
                                                                                                                  summary_val = delta;
                                                                                                              }
                                                                                                              else if (min_delta) {
                                                                                                                if (delta < summary_val)
                                                                                                                  summary_val = delta;
                                                                                                              }
                                                                                                              else if (max_abs_delta) {
                                                                                                                if (delta < 0) {
                                                                                                                  work = delta * -1;

                                                                                                                  if (work > summary_val)
                                                                                                                    summary_val = work;
                                                                                                                }
                                                                                                                else {
                                                                                                                  if (delta > summary_val)
                                                                                                                    summary_val = delta;
                                                                                                                }
                                                                                                              }
                                                                                                              else if (max_collected) {
                                                                                                                if (collected_from_pot > summary_val)
                                                                                                                  summary_val = collected_from_pot;
                                                                                                              }
                                                                                                              else {
                                                                                                                if (delta > summary_val) {
                                                                                                                  summary_val = delta;
                                                                                                                  max_delta_hand_typ = poker_hand.GetHandType();
                                                                                                                }
                                                                                                              }
                                                                                                            }
                                                                                                            else {
                                                                                                              sum_by_table_count[table_count - 2] += delta;
                                                                                                              count_by_table_count[table_count - 2]++;
                                                                                                            }
                                                                                                          }
                                                                                                          else {
                                                                                                            if (local_vars.bShowCollected)
                                                                                                              printf("%10d %s",collected_from_pot,hole_cards);
                                                                                                            else if (local_vars.bShowSpent)
                                                                                                              printf("%10d %s",spent_this_hand,hole_cards);
                                                                                                            else if (local_vars.bShowOpm) {
                                                                                                              printf("%6.4lf (%10d %10d) %s",dwork,
                                                                                                                delta,collected_from_pot,hole_cards);
                                                                                                            }
                                                                                                            else if (local_vars.bNumDecisions)
                                                                                                              printf("%10d %s",numdecs,hole_cards);
                                                                                                            else if (local_vars.bShowWagered)
                                                                                                              printf("%10d %s",wagered_amount,hole_cards);
                                                                                                            else  {
                                                                                                              if (!local_vars.bNoDelta) {
                                                                                                                if (!local_vars.bHoleCardsUsed) {
                                                                                                                  if (!local_vars.bNoHoleCards)
                                                                                                                    printf("%10d %s",delta,hole_cards);
                                                                                                                  else
                                                                                                                    printf("%10d",delta);
                                                                                                                }
                                                                                                                else {
                                                                                                                  if (!local_vars.bNoHoleCards)
                                                                                                                    printf("%10d %s (%d)",delta,hole_cards,hole_cards_used);
                                                                                                                  else
                                                                                                                    printf("%10d (%d)",delta,hole_cards_used);
                                                                                                                }
                                                                                                              }
                                                                                                              else if (!local_vars.bNoHoleCards)
                                                                                                                printf("%s",hole_cards);
                                                                                                            }

                                                                                                            if (local_vars.bShowBoard && local_vars.bHaveFlop2)
                                                                                                              printf(" %s",board_cards);

                                                                                                            if (local_vars.bShowHandType && local_vars.bHaveFlop)
                                                                                                              printf(" %s",plain_hand_types[poker_hand.GetHandType()]);

                                                                                                            if (local_vars.bShowRiver)
                                                                                                              printf(" %s",&board_cards[12]);

                                                                                                            if (local_vars.bShowHandTypId && local_vars.bHaveFlop)
                                                                                                              printf(" %d",poker_hand.GetHandType());

                                                                                                            if (local_vars.bShowHand && local_vars.bHaveFlop)
                                                                                                              printf(" %s",poker_hand.GetHand());

                                                                                                            if (local_vars.bShowTableName)
                                                                                                              printf(" %s",table_name);

                                                                                                            if (local_vars.bShowTableCount)
                                                                                                              printf(" %d",table_count);

                                                                                                            if (local_vars.bVerbose) {
                                                                                                              if (!local_vars.bGetDateFromFilename)
                                                                                                                printf(" %s %3d\n",filename,num_hands);
                                                                                                              else
                                                                                                                printf("\t%s\n",date_string);
                                                                                                            }
                                                                                                            else
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

          continue;
        }

        if (!strncmp(line,street_marker,STREET_MARKER_LEN)) {
          num_street_markers++;

          if (num_street_markers > 1) {
            if (street <= max_streets)
              spent_this_hand += spent_this_street;

            street++;
            spent_this_street = 0;
          }
        }
      }
    }

    fclose(fptr);
  }

  fclose(fptr0);

  if (local_vars.bSumByTableCount) {
    for (n = 0; n < MAX_TABLE_COUNT - 1; n++) {
      printf("%d %10d (%8d)\n",n+2,sum_by_table_count[n],
        count_by_table_count[n]);
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

static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index)
{
  int m;
  int n;
  int tries;
  char chara;

  tries = line_len - string_len + 1;

  if (tries <= 0)
    return false;

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
      return true;
    }
  }

  return false;
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

static char sql_date_string[11];

static int get_date_from_path(char *path,char slash_char,int num_slashes,char **date_string_ptr)
{
  int n;
  int len;
  int slash_count;

  len = strlen(path);
  slash_count = 0;

  for (n = len - 1; (n >= 0); n--) {
    if (path[n] == slash_char) {
      slash_count++;

      if (slash_count == num_slashes)
        break;
    }
  }

  if (slash_count != num_slashes)
    return 1;

  if (path[n+5] != slash_char)
    return 2;

  strncpy(sql_date_string,&path[n+1],4);
  sql_date_string[4] = '-';
  strncpy(&sql_date_string[5],&path[n+6],2);
  sql_date_string[7] = '-';
  strncpy(&sql_date_string[8],&path[n+8],2);
  sql_date_string[10] = 0;

  *date_string_ptr = sql_date_string;

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
      line,line_len,
      player_name,player_name_len,
      &ix)) {

      if (Contains(true,
        line,line_len,
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

static HandType get_winning_hand_typ_id(char *line,int line_len)
{
  int n;
  int ix;
  int ix2;

  if (Contains(true,
    line,line_len,
    with,WITH_LEN,
    &ix)) {

    if (Contains(true,
      line,line_len,
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
