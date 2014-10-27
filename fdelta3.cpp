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
"  (-only_count) (-only_won) (-only_showdown) (-only_no_showdown)\n"
"  (-very_best_hand) (-table_countn) (-all_in) (-not_all_in)\n"
"  (-all_in_preflop) (-hit_felt) (-didnt_hit_felt) (-no_uncalled) (-no_collected)\n"
"  (-show_collected) (-show_spent) (-show_opm) (-only_wash)\n"
"  (-sum_delta) (-sum_abs_delta) (-max_delta) (-min_delta) (-max_abs_delta) (-max_collected)\n"
"  (-max_delta_hand_type) (-no_delta) (-hole_cards_used)\n"
"  (-only_suited) (-flopped) (-pocket_pair) (-only_hand_numbern) (-hand_typ_idid\n"
"  (-show_hand_typ_id) (-didnt_see_flop)\n"
"  (-only_winning_session) (-only_losing_session) (-never_hit_felt_in_session)\n"
"  (-collected_genum) (-sum_by_table_count)\n"
"  (-show_table_name) (-show_table_count) (-show_hand_count) player_name filename\n";
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
static char uncalled_bet[] = "Uncalled bet (";
#define UNCALLED_BET_LEN (sizeof (uncalled_bet) - 1)
static char collected[] = " collected ";
#define COLLECTED_LEN (sizeof (collected) - 1)
static char show_down[] = "*** SHOW DOWN ***";
#define SHOW_DOWN_LEN (sizeof (show_down) - 1)
static char all_in[] = "all-in";
#define ALL_IN_LEN (sizeof (all_in) - 1)

#define MAX_TABLE_COUNT 9

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);
static int get_work_amount(char *line,int line_len);
static void normalize_hole_cards(char *hole_cards);
static int get_date_from_path(char *path,char slash_char,int num_slashes,char **date_string_ptr);
static void get_table_name(char *line,int line_len,char *table_name,int max_table_name_len);

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  int q;
  int curr_arg;
  bool bTerse;
  bool bVerbose;
  bool bDebug;
  int player_name_ix;
  int player_name_len;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  int dbg_line_no;
  int table_count;
  int ix;
  int street;
  int num_street_markers;
  int max_streets;
  int starting_balance;
  int ante;
  int bring_in;
  int spent_this_street;
  int spent_this_hand;
  int end_ix;
  int uncalled_bet_amount;
  int collected_from_pot;
  int collected_from_pot_count;
  int ending_balance;
  int delta;
  int file_no;
  int dbg_file_no;
  int num_hands;
  int dbg;
  int work;
  double opm;
  char hole_cards[12];
  char board_cards[15];
  bool bHandTypeSpecified;
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
  bool bVeryBestHand;
  bool bTableCount;
  bool bAllIn;
  bool bNotAllIn;
  bool bAllInPreflop;
  bool bHitFelt;
  bool bDidntHitFelt;
  bool bNoUncalled;
  bool bNoCollected;
  bool bShowCollected;
  bool bShowSpent;
  bool bShowOpm;
  bool bOnlyWash;
  int table_count_to_match;
  int sum_delta;
  int sum_abs_delta;
  int max_delta;
  int min_delta;
  int max_abs_delta;
  int max_collected;
  int max_delta_hand_type;
  int max_delta_hand_typ;
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
  int collected_ge_num;
  bool bStud;
  bool bRazz;
  int hand_number;
  bool bSuited;
  bool bHaveFlop;
  bool bHaveRiver;
  bool bHaveStealthTwoPair;
  bool bSpentRiverMoney;
  bool bHaveShowdown;
  bool bHaveAllIn;
  bool bHaveAllInPreflop;
  bool bSummarizing;
  char *hand_type;
  HandType hand_typ_id;
  char *hand;
  bool bFolded;
  bool bSkipping;
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
  char *date_string;
  int *poker_hand_cards;
  int hole_cards_used;
  int sum_by_table_count[MAX_TABLE_COUNT - 1];
  int count_by_table_count[MAX_TABLE_COUNT - 1];

  if ((argc < 3) || (argc > 63)) {
    printf(usage);
    return 1;
  }

  bTerse = false;
  bVerbose = false;
  bDebug = false;
  bHandTypeSpecified = false;
  bHandTypIdSpecified = false;
  bHandSpecified = false;
  bSkipFolded = false;
  bAbbrev = false;
  bSkipZero = false;
  bOnlyZero = false;
  bShowBoard = false;
  bShowHandType = false;
  bShowHandTypId = false;
  bShowHand = false;
  bSawFlop = false;
  bSawRiver = false;
  bOnlyFolded = false;
  bSpentMoneyOnTheRiver = false;
  bStealthTwoPair = false;
  bNormalize = false;
  bOnlyLost = false;
  only_count = 0;
  bOnlyWon = false;
  bOnlyShowdown = false;
  bOnlyNoShowdown = false;
  bVeryBestHand = false;
  bTableCount = false;
  bAllIn = false;
  bNotAllIn = false;
  bAllInPreflop = false;
  bHitFelt = false;
  bDidntHitFelt = false;
  bNoUncalled = false;
  bNoCollected = false;
  bShowCollected = false;
  bShowSpent = false;
  bShowOpm = false;
  bOnlyWash = false;
  sum_delta = 0;
  sum_abs_delta = 0;
  max_delta = 0;
  min_delta = 0;
  max_abs_delta = 0;
  max_collected = 0;
  max_delta_hand_type = 0;
  bNoDelta = false;
  bHoleCardsUsed = false;
  bOnlySuited = false;
  bFlopped = false;
  bPocketPair = false;
  bDidntSeeFlop = false;
  bOnlyWinningSession = false;
  bOnlyLosingSession = false;
  bNeverHitFeltInSession = false;
  bCollectedGe = false;
  bSumByTableCount = false;
  bShowTableName = false;
  bShowTableCount = false;
  bShowHandCount = false;
  hand_number = -1;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strncmp(argv[curr_arg],"-hand_type",10)) {
      hand_type = &argv[curr_arg][10];
      bHandTypeSpecified = true;
    }
    else if (!strncmp(argv[curr_arg],"-hand_typ_id",12)) {
      sscanf(&argv[curr_arg][12],"%d",&hand_typ_id);
      bHandTypIdSpecified = true;
    }
    else if (!strncmp(argv[curr_arg],"-hand",5)) {
      hand = &argv[curr_arg][5];
      bHandSpecified = true;

      if ((strlen(&argv[curr_arg][5]) == 3) && (argv[curr_arg][7] == 's'))
        bSuited = true;
      else
        bSuited = false;
    }
    else if (!strcmp(argv[curr_arg],"-skip_folded"))
      bSkipFolded = true;
    else if (!strcmp(argv[curr_arg],"-abbrev"))
      bAbbrev = true;
    else if (!strcmp(argv[curr_arg],"-skip_zero"))
      bSkipZero = true;
    else if (!strcmp(argv[curr_arg],"-only_zero"))
      bOnlyZero = true;
    else if (!strcmp(argv[curr_arg],"-show_board"))
      bShowBoard = true;
    else if (!strcmp(argv[curr_arg],"-show_hand_type"))
      bShowHandType = true;
    else if (!strcmp(argv[curr_arg],"-show_hand_typ_id"))
      bShowHandTypId = true;
    else if (!strcmp(argv[curr_arg],"-show_hand"))
      bShowHand = true;
    else if (!strcmp(argv[curr_arg],"-saw_flop"))
      bSawFlop = true;
    else if (!strcmp(argv[curr_arg],"-saw_river"))
      bSawRiver = true;
    else if (!strcmp(argv[curr_arg],"-only_folded"))
      bOnlyFolded = true;
    else if (!strcmp(argv[curr_arg],"-spent_money_on_the_river"))
      bSpentMoneyOnTheRiver = true;
    else if (!strcmp(argv[curr_arg],"-stealth_two_pair"))
      bStealthTwoPair = true;
    else if (!strcmp(argv[curr_arg],"-normalize"))
      bNormalize = true;
    else if (!strcmp(argv[curr_arg],"-only_lost"))
      bOnlyLost = true;
    else if (!strcmp(argv[curr_arg],"-only_count"))
      only_count = 1;
    else if (!strcmp(argv[curr_arg],"-only_won"))
      bOnlyWon = true;
    else if (!strcmp(argv[curr_arg],"-only_showdown"))
      bOnlyShowdown = true;
    else if (!strcmp(argv[curr_arg],"-only_no_showdown"))
      bOnlyNoShowdown = true;
    else if (!strcmp(argv[curr_arg],"-very_best_hand"))
      bVeryBestHand = true;
    else if (!strncmp(argv[curr_arg],"-table_count",12)) {
      bTableCount = true;
      sscanf(&argv[curr_arg][12],"%d",&table_count_to_match);
    }
    else if (!strcmp(argv[curr_arg],"-all_in"))
      bAllIn = true;
    else if (!strcmp(argv[curr_arg],"-not_all_in"))
      bNotAllIn = true;
    else if (!strcmp(argv[curr_arg],"-all_in_preflop"))
      bAllInPreflop = true;
    else if (!strcmp(argv[curr_arg],"-hit_felt"))
      bHitFelt = true;
    else if (!strcmp(argv[curr_arg],"-didnt_hit_felt"))
      bDidntHitFelt = true;
    else if (!strcmp(argv[curr_arg],"-no_uncalled"))
      bNoUncalled = true;
    else if (!strcmp(argv[curr_arg],"-no_collected"))
      bNoCollected = true;
    else if (!strcmp(argv[curr_arg],"-show_collected"))
      bShowCollected = true;
    else if (!strcmp(argv[curr_arg],"-show_spent"))
      bShowSpent = true;
    else if (!strcmp(argv[curr_arg],"-show_opm"))
      bShowOpm = true;
    else if (!strcmp(argv[curr_arg],"-only_wash"))
      bOnlyWash = true;
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
      bNoDelta = true;
    else if (!strcmp(argv[curr_arg],"-hole_cards_used"))
      bHoleCardsUsed = true;
    else if (!strcmp(argv[curr_arg],"-only_suited"))
      bOnlySuited = true;
    else if (!strcmp(argv[curr_arg],"-flopped"))
      bFlopped = true;
    else if (!strcmp(argv[curr_arg],"-pocket_pair"))
      bPocketPair = true;
    else if (!strncmp(argv[curr_arg],"-only_hand_number",17))
      sscanf(&argv[curr_arg][17],"%d",&hand_number);
    else if (!strcmp(argv[curr_arg],"-didnt_see_flop"))
      bDidntSeeFlop = true;
    else if (!strcmp(argv[curr_arg],"-only_winning_session"))
      bOnlyWinningSession = true;
    else if (!strcmp(argv[curr_arg],"-only_losing_session"))
      bOnlyLosingSession = true;
    else if (!strcmp(argv[curr_arg],"-never_hit_felt_in_session"))
      bNeverHitFeltInSession = true;
    else if (!strncmp(argv[curr_arg],"-collected_ge",13)) {
      sscanf(&argv[curr_arg][13],"%d",&collected_ge_num);
      bCollectedGe = true;
    }
    else if (!strcmp(argv[curr_arg],"-sum_by_table_count")) {
      bTerse = true;
      bSumByTableCount = true;
    }
    else if (!strcmp(argv[curr_arg],"-show_table_name"))
      bShowTableName = true;
    else if (!strcmp(argv[curr_arg],"-show_table_count"))
      bShowTableCount = true;
    else if (!strcmp(argv[curr_arg],"-show_hand_count"))
      bShowHandCount = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if (bTerse && bVerbose) {
    printf("can't specify both -terse and -verbose\n");
    return 3;
  }

  if (bSawFlop && bSawRiver) {
    printf("can't specify both -saw_flop and -saw_river\n");
    return 4;
  }

  if (bAbbrev && bShowHand) {
    printf("can't specify both -abbrev and -show_hand\n");
    return 5;
  }

  if (bAbbrev && bShowHandType) {
    printf("can't specify both -abbrev and -show_hand_type\n");
    return 6;
  }

  if (bAbbrev && bShowHandTypId) {
    printf("can't specify both -abbrev and -show_hand_typ_id\n");
    return 7;
  }

  if (bAbbrev && bStealthTwoPair) {
    printf("can't specify both -abbrev and -stealth_two_pair\n");
    return 8;
  }

  if (bSkipFolded && bOnlyFolded) {
    printf("can't specify both -skip_folded and -only_folded\n");
    return 9;
  }

  if (!bSawRiver && bSpentMoneyOnTheRiver)
    bSawRiver = true;

  if (bStealthTwoPair && bHandTypeSpecified) {
    printf("can't specify both -stealth_two_pair and -hand_typehand_type\n");
    return 10;
  }

  if (bStealthTwoPair && bHandTypIdSpecified) {
    printf("can't specify both -stealth_two_pair and -hand_typ_idid\n");
    return 11;
  }

  if (bStealthTwoPair && bHandSpecified) {
    printf("can't specify both -stealth_two_pair and -hand\n");
    return 12;
  }

  if (bStealthTwoPair && !bSawFlop && !bSawRiver)
    bSawFlop = true;

  if (bHandTypeSpecified && !bSawFlop && !bSawRiver)
    bSawFlop = true;

  if (bHandTypIdSpecified && !bSawFlop && !bSawRiver)
    bSawFlop = true;

  if (bOnlyLost && bOnlyWon) {
    printf("can't specify both -only_lost and -only_won\n");
    return 13;
  }

  if (bSkipZero && bOnlyZero) {
    printf("can't specify both -skip_zero and -only_zero\n");
    return 14;
  }

  if (bOnlyShowdown && bOnlyNoShowdown) {
    printf("can't specify both -only_showdown and -only_no_showdown\n");
    return 15;
  }

  if (bAllIn && bNotAllIn) {
    printf("can't specify both -all_in and -not_all_in\n");
    return 16;
  }

  if (bAllIn && bAllInPreflop) {
    printf("can't specify both -all_in and -all_in_preflop\n");
    return 17;
  }

  if (bNotAllIn && bAllInPreflop) {
    printf("can't specify both -not_all_in and -all_in_preflop\n");
    return 18;
  }

  if (bShowCollected && bShowOpm) {
    printf("can't specify both -show_collected and -show_opm\n");
    return 19;
  }

  if (bOnlyLost && bOnlyWash) {
    printf("can't specify both -only_lost and -only_wash\n");
    return 20;
  }

  if (bOnlyWon && bOnlyWash) {
    printf("can't specify both -only_won and -only_wash\n");
    return 21;
  }

  if (only_count + sum_delta + sum_abs_delta + max_delta + min_delta + max_abs_delta + max_collected + max_delta_hand_type > 1) {
    printf("can only specify one of -only_count, -sum_delta, -sum_abs_delta, -max_delta, -min_delta, -max_abs_delta, -max_collected, and -max_delta_hand_type\n");
    return 22;
  }

  if ((only_count) || (sum_delta) || (sum_abs_delta) || (max_delta) || (min_delta) || (max_abs_delta) || (max_collected) || (max_delta_hand_type)) {
    bTerse = true;
    bSummarizing = true;
  }
  else
    bSummarizing = false;

  if (bHoleCardsUsed) {
    if (bAbbrev) {
      printf("can't specify both -abbrev and -hole_cards_used\n");
      return 23;
    }

    bSawFlop = true;
  }

  if (bShowCollected && bShowSpent) {
    printf("can't specify both -show_collected and -show_spent\n");
    return 24;
  }

  if (bHitFelt && bDidntHitFelt) {
    printf("can't specify both -hit_felt and -didnt_hit_felt\n");
    return 25;
  }

  if (bVeryBestHand && bFlopped) {
    printf("can't specify both -very_best_hand and -flopped\n");
    return 26;
  }

  if (bHandTypeSpecified && bHandTypIdSpecified) {
    printf("can't specify both -hand_typehand_type and -hand_typ_idid\n");
    return 27;
  }

  if (bShowHandType && bShowHandTypId) {
    printf("can't specify both -show_hand_type and -show_hand_typ_id\n");
    return 28;
  }

  if (bSawFlop && bDidntSeeFlop) {
    printf("can't specify both -saw_flop and -didnt_see_flop\n");
    return 29;
  }

  if (bOnlyWinningSession && bOnlyLosingSession) {
    printf("can't specify both -only_winning_session and -only_losing_session\n");
    return 30;
  }

  if (bSumByTableCount && bShowTableName) {
    printf("can't specify both -sum_by_table_count and -show_table_name\n");
    return 31;
  }

  if (bSumByTableCount && bShowTableCount) {
    printf("can't specify both -sum_by_table_count and -show_table_count\n");
    return 32;
  }

  player_name_ix = curr_arg++;
  player_name_len = strlen(argv[player_name_ix]);

  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 33;
  }

  ending_balance = -1;

  file_no = 0;
  dbg_file_no = -1;

  if (!bAbbrev)
    hole_cards[11] = 0;
  else
    hole_cards[3] = 0;

  if (bSumByTableCount) {
    for (n = 0; n < MAX_TABLE_COUNT - 1; n++) {
      sum_by_table_count[n] = 0;
      count_by_table_count[n] = 0;
    }
  }

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    file_no++;

    if (bSummarizing) {
      total_delta = 0;
      hit_felt_in_session_count = 0;
      summary_val = 0;
    }

    if (dbg_file_no == file_no)
      dbg = 1;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    line_no = 0;
    bFolded = false;
    bSkipping = false;
    num_hands = 0;
    bHaveFlop = false;
    bHaveRiver = false;
    bSpentRiverMoney = false;
    bHaveStealthTwoPair = false;
    bHaveShowdown = false;
    bHaveAllIn = false;
    bHaveAllInPreflop = false;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr)) {
        if (bSummarizing) {
          if (!bSkipZero || summary_val) {
            if (!bOnlyWinningSession || (total_delta > 0)) {
              if (!bOnlyLosingSession || (total_delta < 0)) {
                if (!bNeverHitFeltInSession || (!hit_felt_in_session_count)) {
                  retval = get_date_from_path(filename,'\\',3,&date_string);

                  if (!max_delta_hand_type) {
                    if (!bShowHandCount)
                      printf("%d\t%s\n",summary_val,date_string);
                    else
                      printf("%d\t%s\t(%d)\n",summary_val,date_string,num_hands);
                  }
                  else {
                    if (!bShowHandCount) {
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

      if (bDebug)
        printf("line %d %s\n",line_no,line);

      if (Contains(true,
        line,line_len,
        pokerstars,POKERSTARS_LEN,
        &ix)) {

        bStud = false;
        bRazz = false;

        if (Contains(true,
          line,line_len,
          stud,STUD_LEN,
          &ix)) {

          bStud = true;
        }
        else if (Contains(true,
          line,line_len,
          razz,RAZZ_LEN,
          &ix)) {

          bRazz = true;
        }

        if (bStud || bRazz)
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
                bFolded = false;
                bSkipping = false;
                bHaveFlop = false;
                bHaveRiver = false;
                bSpentRiverMoney = false;
                bHaveStealthTwoPair = false;
                bHaveShowdown = false;
                bHaveAllIn = false;
                bHaveAllInPreflop = false;

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

                if (bDebug)
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

              if (bDebug) {
                printf("line %d street %d POSTS work = %d, spent_this_street = %d\n",
                  line_no,street,work,spent_this_street);
              }
            }
          }
        }

        if (table_count > MAX_TABLE_COUNT) {
          printf("%s: too many players at the table\n",filename);
          return 34;
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

          bHaveAllIn = true;

          if (!bHaveFlop)
            bHaveAllInPreflop = true;
        }

        if (bSkipping)
          continue;
        else if (!strncmp(line,dealt_to,DEALT_TO_LEN)) {
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
              if (!bAbbrev) {
                for (p = 0; p < 11; p++) {
                  if (line[n+p] == ']') {
                    if (p == 5)
                      hole_cards[p] = 0;

                    break;
                  }
                  else
                    hole_cards[p] = line[n+p];
                }

                if (bNormalize)
                  normalize_hole_cards(hole_cards);

                for (q = 0; q < NUM_HOLE_CARDS_IN_HOLDEM_HAND; q++) {
                  card_string[0] = hole_cards[q*3 + 0];
                  card_string[1] = hole_cards[q*3 + 1];

                  retval = card_value_from_card_string(
                    card_string,&cards[q]);

                  if (retval) {
                    printf("invalid card string %s on line %d\n",
                      card_string,line_no);
                    return 35;
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

            if (bHandSpecified) {
              if (bSuited) {
                if (hole_cards[1] != hole_cards[4])
                  bSkipping = true;
              }
              else {
                if (hole_cards[1] == hole_cards[4])
                  bSkipping = true;
              }

              if (((hole_cards[0] != hand[0]) || (hole_cards[3] != hand[1])) &&
                  ((hole_cards[0] != hand[1]) || (hole_cards[3] != hand[0])))
                bSkipping = true;
            }
          }
        }
        else if (Contains(true,
          line,line_len,
          collected,COLLECTED_LEN,
          &ix)) {

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

          if (bDebug) {
            printf("line %d street %d COLLECTED work = %d, collected_from_pot = %d\n",
              line_no,street,work,collected_from_pot);
          }

          continue;
        }
        else if (!strncmp(line,uncalled_bet,UNCALLED_BET_LEN)) {
          sscanf(&line[UNCALLED_BET_LEN],"%d",&uncalled_bet_amount);
          spent_this_street -= uncalled_bet_amount;

          if (bDebug) {
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

          if (bDebug) {
            printf("line %d street %d FOLDS spent_this_street = %d, spent_this_hand = %d\n",
              line_no,street,spent_this_street,spent_this_hand);
          }

          bFolded = true;

          ending_balance = starting_balance - spent_this_hand + collected_from_pot;

          if (bSummarizing && !ending_balance)
            hit_felt_in_session_count++;

          delta = ending_balance - starting_balance;

          if (bShowOpm)
            opm = (double)0;

          continue;
        }
        else if (Contains(true,
          line,line_len,
          bets,BETS_LEN,
          &ix)) {
          work = get_work_amount(line,line_len);
          spent_this_street += work;

          if (street == 3)
            bSpentRiverMoney = true;

          if (bDebug) {
            printf("line %d street %d BETS work = %d, spent_this_street = %d\n",
              line_no,street,work,spent_this_street);
          }
        }
        else if (Contains(true,
          line,line_len,
          calls,CALLS_LEN,
          &ix)) {
          work = get_work_amount(line,line_len);
          spent_this_street += work;

          if (street == 3)
            bSpentRiverMoney = true;

          if (bDebug) {
            printf("line %d street %d CALLS work = %d, spent_this_street = %d\n",
              line_no,street,work,spent_this_street);
          }
        }
        else if (Contains(true,
          line,line_len,
          raises,RAISES_LEN,
          &ix)) {
          work = get_work_amount(line,line_len);
          spent_this_street = work;

          if (street == 3)
            bSpentRiverMoney = true;

          if (bDebug) {
            printf("line %d street %d RAISES work = %d, spent_this_street = %d\n",
              line_no,street,work,spent_this_street);
          }
        }
        else if ((bStud || bRazz) && Contains(true,
          line,line_len,
          brings_in_for,BRINGS_IN_FOR_LEN,
          &ix)) {
          bring_in = get_work_amount(line,line_len);
          spent_this_street += bring_in;
          continue;
        }
      }
      else if (bSkipping)
        continue;
      else {
        if (!strncmp(line,flop,FLOP_LEN)) {
          if (bStealthTwoPair && !bFolded) {
            if (hole_cards[0] != hole_cards[3]) {
              if (((hole_cards[0] == line[FLOP_LEN]) ||
                   (hole_cards[0] == line[FLOP_LEN+3]) ||
                   (hole_cards[0] == line[FLOP_LEN+6])) &&
                  ((hole_cards[3] == line[FLOP_LEN]) ||
                   (hole_cards[3] == line[FLOP_LEN+3]) ||
                   (hole_cards[3] == line[FLOP_LEN+6]))) {
                bHaveStealthTwoPair = true;
              }
            }
          }

          for (n = 0; n < 8; n++)
            board_cards[n] = line[FLOP_LEN+n];

          board_cards[n] = ' ';

          for (q = 0; q < NUM_CARDS_IN_FLOP; q++) {
            card_string[0] = board_cards[q*3 + 0];
            card_string[1] = board_cards[q*3 + 1];

            retval = card_value_from_card_string(
              card_string,&cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND+q]);

            if (retval) {
              printf("invalid card string %s on line %d\n",
                card_string,line_no);
              return 36;
            }
          }

          poker_hand.NewCards(cards[0],cards[1],cards[2],cards[3],cards[4]);
          poker_hand.Evaluate();

          if (!bFolded)
            bHaveFlop = true;
        }
        else if (!strncmp(line,turn,TURN_LEN)) {
          n = 9;

          for (m = 0; m < 2; m++,n++)
            board_cards[n] = line[TURN_LEN+11+m];

          board_cards[n++] = ' ';

          card_string[0] = board_cards[9];
          card_string[1] = board_cards[10];

          retval = card_value_from_card_string(
            card_string,&cards[NUM_HOLE_CARDS_IN_HOLDEM_HAND+NUM_CARDS_IN_FLOP]);

          if (retval) {
            printf("invalid card string %s on line %d\n",
              card_string,line_no);
            return 37;
          }

          if (!bFlopped && (!bFolded || bVeryBestHand)) {
            turn_hand.NewCards(cards[0],cards[1],cards[2],
              cards[3],cards[4],cards[5]);

            poker_hand = turn_hand.BestPokerHand();
          }
        }
        else if (!strncmp(line,river,RIVER_LEN)) {
          if (!bStud && !bRazz) {
            n = 12;

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
              return 38;
            }

            if (!bFlopped && (!bFolded || bVeryBestHand)) {
              holdem_hand.NewCards(cards[0],cards[1],cards[2],
                cards[3],cards[4],cards[5],cards[6]);

              poker_hand = holdem_hand.BestPokerHand();

              if (!bFolded)
                bHaveRiver = true;
            }
          }
        }
        else if (!strncmp(line,show_down,SHOW_DOWN_LEN)) {
          if (!bFolded)
            bHaveShowdown = true;
        }
        else if (!strncmp(line,summary,SUMMARY_LEN)) {
          if (bDebug)
            printf("line %d SUMMARY line detected; skipping\n",line_no);

          bSkipping = true;

          if (!bFolded) {
            ending_balance = starting_balance - spent_this_hand + collected_from_pot;

            if (bSummarizing && !ending_balance)
              hit_felt_in_session_count++;

            delta = ending_balance - starting_balance;

            if (bShowOpm) {
              if (collected_from_pot && (delta > 0))
                opm = (double)delta / (double)collected_from_pot;
              else
                opm = (double)0;
            }
          }

          if (bSummarizing || !bSkipZero || (delta != 0)) {
            if (!bOnlyZero || (delta == 0)) {
              if (!bSkipFolded || !bFolded) {
                if (!bOnlyFolded || bFolded) {
                  if (!bSawFlop || bHaveFlop) {
                    if (!bDidntSeeFlop || !bHaveFlop) {
                      if (!bStealthTwoPair || bHaveStealthTwoPair) {
                        if (!bSawRiver || bHaveRiver) {
                          if (!bSpentMoneyOnTheRiver || bSpentRiverMoney) {
                            if (!bHandTypeSpecified || !strcmp(hand_type,plain_hand_types[poker_hand.GetHandType()])) {
                              if (!bHandTypIdSpecified || (hand_typ_id == poker_hand.GetHandType())) {
                                if (!bOnlyLost || (delta < 0)) {
                                  if (!bOnlyWon || (delta > 0)) {
                                    if (!bOnlyWash || (delta == 0)) {
                                      if (!bOnlyShowdown || bHaveShowdown) {
                                        if (!bOnlyNoShowdown || !bHaveShowdown) {
                                          if (!bTableCount || (table_count == table_count_to_match)) {
                                            if (!bAllIn || bHaveAllIn) {
                                              if (!bNotAllIn || !bHaveAllIn) {
                                                if (!bAllInPreflop || bHaveAllInPreflop) {
                                                  if (!bHitFelt || (ending_balance == 0)) {
                                                    if (!bDidntHitFelt || (ending_balance > 0)) {
                                                      if (!bNoUncalled || (uncalled_bet_amount == 0)) {
                                                        if (!bNoCollected || (collected_from_pot == 0)) {
                                                          if (!bOnlySuited || (hole_cards[1] == hole_cards[4])) {
                                                            if (!bPocketPair || (hole_cards[0] == hole_cards[3])) {
                                                              if ((hand_number == -1) || (num_hands == hand_number)) {
                                                                if (!bCollectedGe || (collected_from_pot >= collected_ge_num)) {
                                                                  if (bHoleCardsUsed) {
                                                                    poker_hand_cards = poker_hand.GetCards();
                                                                    hole_cards_used = 0;

                                                                    for (p = 0; p < NUM_HOLE_CARDS_IN_HOLDEM_HAND; p++) {
                                                                      for (q = 0; q < NUM_CARDS_IN_HAND; q++) {
                                                                        if (cards[p] == poker_hand_cards[q]) {
                                                                          hole_cards_used++;
                                                                          break;
                                                                        }
                                                                      }
                                                                    }
                                                                  }

                                                                  if (bTerse) {
                                                                    if (!bSummarizing && !bSumByTableCount) {
                                                                      if (bShowCollected)
                                                                        printf("%d\n",collected_from_pot);
                                                                      else if (bShowSpent)
                                                                        printf("%d\n",spent_this_hand);
                                                                      else if (bShowOpm)
                                                                        printf("%lf\n",opm);
                                                                      else
                                                                        printf("%d\n",delta);
                                                                    }
                                                                    else if (!bSumByTableCount) {
                                                                      total_delta += delta;

                                                                      if (only_count)
                                                                        summary_val++;
                                                                      else if (sum_delta)
                                                                        summary_val += delta;
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
                                                                    if (bShowCollected)
                                                                      printf("%10d %s",collected_from_pot,hole_cards);
                                                                    else if (bShowOpm) {
                                                                      printf("%6.4lf (%10d %10d) %s",opm,
                                                                        delta,collected_from_pot,hole_cards);
                                                                    }
                                                                    else  {
                                                                      if (!bNoDelta) {
                                                                        if (!bHoleCardsUsed)
                                                                          printf("%10d %s",delta,hole_cards);
                                                                        else
                                                                          printf("%10d %s (%d)",delta,hole_cards,hole_cards_used);
                                                                      }
                                                                      else
                                                                        printf("%s",hole_cards);
                                                                    }

                                                                    if (bShowBoard && bHaveRiver)
                                                                      printf(" %s",board_cards);

                                                                    if (bShowHandType && bHaveFlop)
                                                                      printf(" %s",plain_hand_types[poker_hand.GetHandType()]);

                                                                    if (bShowHandTypId && bHaveFlop)
                                                                      printf(" %d",poker_hand.GetHandType());

                                                                    if (bShowHand && bHaveFlop)
                                                                      printf(" %s",poker_hand.GetHand());

                                                                    if (bShowTableName)
                                                                      printf(" %s",table_name);

                                                                    if (bShowTableCount)
                                                                      printf(" %d",table_count);

                                                                    if (bVerbose)
                                                                      printf(" %s %3d\n",filename,num_hands);
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

  if (bSumByTableCount) {
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
