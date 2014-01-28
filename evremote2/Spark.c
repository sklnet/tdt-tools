/*
 * Spark.c
 *
 * (c) 2010 duckbox project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <termios.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <time.h>

#include "global.h"
#include "map.h"
#include "remotes.h"
#include "Spark.h"


#define	SPARK_RC04_PREDATA		"CC33"
#define	SPARK_RC05_PREDATA		"11EE"
#define	SPARK_RC08_PREDATA		"44BB"
#define	SPARK_RC09_PREDATA		"9966"
#define	SPARK_RC12_PREDATA		"08F7"
#define	SPARK_DEFAUYLT_PREDATA	"A25D"
#define	SPARK_EDV_RC1			"C43B"
#define	SPARK_EDV_RC2			"1CE3"

static tLongKeyPressSupport cLongKeyPressSupport = {
  10, 120,
};

/* Edision argus-spark RCU */
static tButton cButtonsEdisionSpark[] = {
    {"STANDBY"        , "25", KEY_POWER},
    {"MUTE"           , "85", KEY_MUTE},
    {"V.FORMAT"       , "ad", KEY_V},
    {"TV/SAT"         , "c5", KEY_AUX},

    {"0BUTTON"        , "57", KEY_0},
    {"1BUTTON"        , "b5", KEY_1},
    {"2BUTTON"        , "95", KEY_2},
    {"3BUTTON"        , "bd", KEY_3},
    {"4BUTTON"        , "f5", KEY_4},
    {"5BUTTON"        , "d5", KEY_5},
    {"6BUTTON"        , "fd", KEY_6},
    {"7BUTTON"        , "35", KEY_7},
    {"8BUTTON"        , "15", KEY_8},
    {"9BUTTON"        , "3d", KEY_9},

    {"BACK"           , "7f", KEY_BACK},
    {"INFO"           , "a7", KEY_INFO}, //THIS IS WRONG SHOULD BE KEY_INFO
    {"AUDIO"          , "35", KEY_AUDIO},

    {"VOL+"           , "C7", KEY_VOLUMEUP},
    {"VOL-"           , "DD", KEY_VOLUMEDOWN},
    {"PAGE+"          , "07", KEY_PAGEUP},
    {"PAGE-"          , "5F", KEY_PAGEDOWN},

    {"DOWN"           , "0f", KEY_DOWN},
    {"UP"             , "27", KEY_UP},
    {"RIGHT"          , "af", KEY_RIGHT},
    {"LEFT"           , "6d", KEY_LEFT},
    {"OK/LIST"        , "2f", KEY_OK},
    {"MENU"           , "65", KEY_MENU},
    {"GUIDE"          , "8f", KEY_EPG},
    {"EXIT"           , "4d", KEY_EXIT},
    {"FAV"            , "87", KEY_FAVORITES},

    {"RED"            , "7d", KEY_RED},
    {"GREEN"          , "ff", KEY_GREEN},
    {"YELLOW"         , "3f", KEY_YELLOW},
    {"BLUE"           , "bf", KEY_BLUE},

    {"REWIND"         , "1f", KEY_REWIND},
    {"PAUSE"          , "37", KEY_PAUSE},
    {"PLAY"           , "b7", KEY_PLAY},
    {"FASTFORWARD"    , "97", KEY_FASTFORWARD},
    {"RECORD"         , "45", KEY_RECORD},
    {"STOP"           , "f7", KEY_STOP},
    {"SLOWMOTION"     , "5d", KEY_SLOW},
    {"ARCHIVE"        , "75", KEY_ARCHIVE},
    {"SAT"            , "1d", KEY_SAT},
    {"STEPBACK"       , "55", KEY_PREVIOUS},
    {"STEPFORWARD"    , "d7", KEY_NEXT},
    {"MARK"           , "8f", KEY_EPG},
    {"TV/RADIO"       , "77", KEY_TV2}, //WE USE TV2 AS TV/RADIO SWITCH BUTTON
    {"USB"            , "95", KEY_CLOSE},
    {"TIMER"          , "8d", KEY_TIME},
    {""               , ""  , KEY_NULL},
};

/* spark RC8 */
static tButton cButtonsSparkRc08[] = {
    {"POWER"          , "4D", KEY_POWER},
    {"MUTE"           , "DD", KEY_MUTE},
    {"TIME"           , "ED", KEY_TIME},
    {"V.FORMAT"       , "AD", KEY_V},
    {"F1"             , "0F", KEY_F1},
    {"TV/SAT"         , "37", KEY_AUX},
    {"USB"            , "0D", KEY_CLOSE},
    {"FIND"           , "35", KEY_FIND},
    {"0BUTTON"        , "27", KEY_0},
    {"1BUTTON"        , "7D", KEY_1},
    {"2BUTTON"        , "3F", KEY_2},
    {"3BUTTON"        , "BD", KEY_3},
    {"4BUTTON"        , "5D", KEY_4},
    {"5BUTTON"        , "1F", KEY_5},
    {"6BUTTON"        , "9D", KEY_6},
    {"7BUTTON"        , "55", KEY_7},
    {"8BUTTON"        , "17", KEY_8},
    {"9BUTTON"        , "95", KEY_9},
    {"TV/RADIO"       , "65", KEY_TV2}, //WE USE TV2 AS TV/RADIO SWITCHB
    {"RECALL"         , "A5", KEY_BACK},
    {"MENU"           , "5F", KEY_MENU},
    {"INFO"           , "1D", KEY_INFO},
    {"UP"             , "9F", KEY_UP},
    {"DOWN"           , "AF", KEY_DOWN},
    {"LEFT"           , "3D", KEY_LEFT},
    {"RIGHT"          , "7F", KEY_RIGHT},
    {"OK"             , "BF", KEY_OK},
    {"EXIT"           , "2D", KEY_EXIT},
    {"EPG"            , "25", KEY_EPG},
    {"VOL+"           , "8D", KEY_VOLUMEUP},
    {"VOL-"           , "B5", KEY_VOLUMEDOWN},
    {"REC"            , "EF", KEY_RECORD},
    {"PAGE+"          , "B7", KEY_PAGEUP},
    {"PAGE-"          , "77", KEY_PAGEDOWN},
    {"FOLDER"         , "E5", KEY_ARCHIVE},
    {"STOP"           , "A7", KEY_STOP},
    {"PLAY"           , "75", KEY_PLAY},
    {"PAUSE"          , "F5", KEY_PAUSE},
    {"FASTFORWARD"    , "CD", KEY_FASTFORWARD},
    {"REWIND"         , "D5", KEY_REWIND},
    {"PREV"           , "8F", KEY_PREVIOUS},
    {"NEXT"           , "57", KEY_NEXT},
    {"Tms"            , "4F", KEY_T},
    {"FAST"           , "97", KEY_F},
    {"SLOW"           , "15", KEY_SLOW},
    {"PLAY_MODE"      , "6F", KEY_P},
    {"WHITE"          , "67", KEY_W},
    {"RED"            , "05", KEY_RED},
    {"GREEN"          , "87", KEY_GREEN},
    {"YELLOW"         , "C5", KEY_YELLOW},
    {"BLUE"           , "47", KEY_BLUE},
    {"FAV"            , "F7", KEY_FAVORITES},
    {"SAT"            , "2F", KEY_SAT},
    {"TTX"            , "DF", KEY_TITLE},
    {"AUDIO"          , "D7", KEY_SUBTITLE},
    {""               , ""  , KEY_NULL},
};

/* Amiko alien-spark RCU */
static tButton cButtonsSparkRc09[] = {
    {"POWER"          , "25", KEY_POWER},
    {"MUTE"           , "85", KEY_MUTE},
    {"TIME"           , "8D", KEY_TIME},
    {"V.FORMAT"       , "AD", KEY_V},
    {"TV/SAT"         , "A5", KEY_AUX},
    {"PICASA"         , "E5", KEY_SUBTITLE},
    {"SHOUTCAST"      , "ED", KEY_AUDIO},
    {"YOUTUBE"        , "CD", KEY_VIDEO},
    {"SPARK"          , "C5", KEY_S},
    {"0BUTTON"        , "57", KEY_0},
    {"1BUTTON"        , "B5", KEY_1},
    {"2BUTTON"        , "95", KEY_2},
    {"3BUTTON"        , "BD", KEY_3},
    {"4BUTTON"        , "F5", KEY_4},
    {"5BUTTON"        , "D5", KEY_5},
    {"6BUTTON"        , "FD", KEY_6},
    {"7BUTTON"        , "35", KEY_7},
    {"8BUTTON"        , "15", KEY_8},
    {"9BUTTON"        , "3D", KEY_9},
    {"TV/RADIO"       , "77", KEY_TV2}, //WE USE TV2 AS TV/RADIO SWITCHB
    {"RECALL"         , "7F", KEY_BACK},
    {"SAT"            , "9D", KEY_SAT},
    {"FAV"     	      , "45", KEY_FAVORITES},
    {"VOL-"           , "C7", KEY_VOLUMEDOWN},
    {"VOL+"           , "DD", KEY_VOLUMEUP},
    {"PAGE-"          , "07", KEY_CHANNELDOWN},
    {"PAGE+"          , "5F", KEY_CHANNELUP},
    {"INFO"           , "1D", KEY_INFO},
    {"EPG"            , "87", KEY_EPG},
    {"MENU"           , "65", KEY_MENU},
    {"EXIT"           , "A7", KEY_EXIT},
    {"UP"	      , "27", KEY_UP},
    {"DOWN"	      , "0F", KEY_DOWN},
    {"LEFT" 	      , "6D", KEY_LEFT},
    {"RIGHT"	      , "AF", KEY_RIGHT},
    {"OK"	      , "2F", KEY_OK},
    {"FIND"           , "4D", KEY_FIND},
    {"REC"            , "8F", KEY_RECORD},
    {"RED"            , "75", KEY_RED},
    {"GREEN"          , "F7", KEY_GREEN},
    {"YELLOW"         , "37", KEY_YELLOW},
    {"BLUE"           , "B7", KEY_BLUE},
    {"REWIND"	      , "55", KEY_REWIND},
    {"PLAY" 	      , "D7", KEY_PLAY},
    {"PAUSE"	      , "17", KEY_PAUSE},
    {"FASTFORWARD"    , "97", KEY_FASTFORWARD},
    {"FOLDER"	      , "5D", KEY_ARCHIVE},
    {"PLAY_MODE"      , "DF", KEY_P},
    {"USB"            , "1F", KEY_CLOSE},
    {"STOP" 	      , "9F", KEY_STOP},
    {"F1"             , "7D", KEY_HELP},
    {"F2"             , "FF", KEY_PREVIOUS},
    {"F3"             , "3F", KEY_NEXT},
    {"F4"             , "BF", KEY_TITLE},
    {""               , ""  , KEY_NULL},
};

static tButton cButtonsSparkRc12[] = {

    {"MUTE"           , "87", KEY_MUTE},
    {"POWER"          , "45", KEY_POWER},
    {"PLAY_MODE"      , "A7", KEY_P},
    {"V.FORMAT"       , "E5", KEY_V},
    {"TIME"           , "C5", KEY_TIME},
    {"USB"            , "47", KEY_CLOSE},
    {"FOLDER"         , "65", KEY_ARCHIVE},
    {"STOP"           , "25", KEY_STOP},
    {"PLAY"           , "3D", KEY_PLAY},
    {"PAUSE"          , "1D", KEY_PAUSE},
    {"FASTFORWARD"    , "C7", KEY_FASTFORWARD},
    {"REWIND"         , "FD", KEY_REWIND},
    {"PREV"           , "BF", KEY_PREVIOUS},
    {"NEXT"           , "E7", KEY_NEXT},
    {"FAST"           , "67", KEY_F},
    {"SLOW"           , "9F", KEY_SLOW},
    {"MENU"           , "DD", KEY_MENU},
    {"Tms"            , "BD", KEY_T},
    {"INFO"           , "FF", KEY_INFO},
    {"UP"	      , "5D", KEY_UP},
    {"DOWN"           , "55", KEY_DOWN},
    {"LEFT"           , "1F", KEY_LEFT},
    {"RIGHT"          , "7F", KEY_RIGHT},
    {"OK"             , "7D", KEY_OK},
    {"EXIT"           , "3F", KEY_EXIT},
    {"REC"            , "9D", KEY_RECORD},
    {"EPG"            , "5F", KEY_EPG},
    {"TV/SAT"         , "D5", KEY_AUX},
    {"RECALL"         , "DF", KEY_BACK},
    {"FIND"           , "95", KEY_FIND},
    {"VOL+"           , "17", KEY_VOLUMEUP},
    {"VOL-"           , "37", KEY_VOLUMEDOWN},
    {"SAT"            , "15", KEY_SAT},
    {"FAV"            , "35", KEY_FAVORITES},
    {"PAGE+"          , "57", KEY_PAGEUP},
    {"PAGE-"          , "D7", KEY_PAGEDOWN},
    {"KEY_0"          , "0D", KEY_0},
    {"KEY_1"          , "F5", KEY_1},
    {"KEY_2"          , "B5", KEY_2},
    {"KEY_3"          , "F7", KEY_3},
    {"KEY_4"          , "CD", KEY_4},
    {"KEY_5"          , "AD", KEY_5},
    {"KEY_6"          , "77", KEY_6},
    {"KEY_7"          , "0F", KEY_7},
    {"KEY_8"          , "8D", KEY_8},
    {"KEY_9"          , "4F", KEY_9},
    {"TV/RADIO"       , "ED", KEY_TV2},
    {"RED"            , "2F", KEY_RED},
    {"GREEN"          , "6F", KEY_GREEN},
    {"YELLOW"         , "EF", KEY_YELLOW},
    {"BLUE"           , "05", KEY_BLUE},
    {"WHITE"          , "2D", KEY_W},
    {""               , ""  , KEY_NULL},
};

/* spark Default */
static tButton cButtonsSparkDefault[] = {
    {"POWER"          , "87", KEY_POWER},
    {"V.FORMAT"       , "0F", KEY_V},
    {"TV/SAT"         , "2F", KEY_AUX},
    {"TIME"           , "65", KEY_TIME},
    {"MUTE"           , "A5", KEY_MUTE},
    {"0BUTTON"        , "37", KEY_0},
    {"1BUTTON"        , "A7", KEY_1},
    {"2BUTTON"        , "07", KEY_2},
    {"3BUTTON"        , "E5", KEY_3},
    {"4BUTTON"        , "97", KEY_4},
    {"5BUTTON"        , "27", KEY_5},
    {"6BUTTON"        , "D5", KEY_6},
    {"7BUTTON"        , "B7", KEY_7},
    {"8BUTTON"        , "17", KEY_8},
    {"9BUTTON"        , "F5", KEY_9},
    {"TV/RADIO"       , "CD", KEY_TV2}, //WE USE TV2 AS TV/RADIO SWITCHB
    {"RECALL"         , "CF", KEY_BACK},
    {"PAGE+"          , "ED", KEY_PAGEUP},
    {"PAGE-"          , "DD", KEY_PAGEDOWN},
    {"FIND"           , "C5", KEY_FIND},
    {"FOLDER"         , "67", KEY_ARCHIVE},
    {"VOL+"           , "AF", KEY_VOLUMEUP},
    {"VOL-"           , "9F", KEY_VOLUMEDOWN},
    {"MENU"           , "C7", KEY_MENU},
    {"INFO"           , "1F", KEY_INFO},
    {"UP"             , "3F", KEY_UP},
    {"DOWN"           , "85", KEY_DOWN},
    {"LEFT"           , "BF", KEY_LEFT},
    {"RIGHT"          , "FD", KEY_RIGHT},
    {"OK"             , "05", KEY_OK},
    {"EXIT"           , "F9", KEY_EXIT},
    {"EPG"            , "45", KEY_EPG},
    {"FAV"            , "3D", KEY_FAVORITES},
    {"SAT"            , "0D", KEY_SAT},
    {"RED"            , "6D", KEY_RED},
    {"GREEN"          , "8D", KEY_GREEN},
    {"YELLOW"         , "77", KEY_YELLOW},
    {"BLUE"           , "AD", KEY_BLUE},
    {"REC"            , "F7", KEY_RECORD},
    {"STOP"           , "BB", KEY_STOP},
    {"PLAY"           , "57", KEY_PLAY},
    {"PAUSE"          , "4D", KEY_PAUSE},
    {"FASTFORWARD"    , "35", KEY_FASTFORWARD},
    {"REWIND"         , "7F", KEY_REWIND},
    {"PREV"           , "FB", KEY_PREVIOUS},
    {"NEXT"           , "3B", KEY_NEXT},
    {"FAST"           , "E7", KEY_F},
    {"SLOW"           , "7B", KEY_SLOW},
    {"PLAY_MODE"      , "B5", KEY_P},
    {"USB"            , "DF", KEY_CLOSE},
    {"Tms"            , "55", KEY_T},
    {"F1"             , "95", KEY_F1},
    {"F2"             , "15", KEY_F2},
    {"F3"             , "D7", KEY_F3},
    {""               , ""  , KEY_NULL},
};

/* spark Default */
static tButton cButtonsSparkRc04[] = {
    {"POWER"          , "45", KEY_POWER},
    {"V.FORMAT"       , "4D", KEY_V},
    {"TIME"           , "ED", KEY_TIME},
    {"MUTE"           , "8D", KEY_MUTE},
    {"Tms"            , "57", KEY_T},
    {"F1"             , "37", KEY_F1},
    {"TV/SAT"         , "DD", KEY_AUX},
    {"KEY_1"          , "3F", KEY_1},
    {"KEY_2"          , "07", KEY_2},
    {"KEY_3"          , "BD", KEY_3},
    {"KEY_4"          , "5D", KEY_4},
    {"KEY_5"          , "1F", KEY_5},
    {"KEY_6"          , "9D", KEY_6},
    {"KEY_7"          , "55", KEY_7},
    {"KEY_8"          , "17", KEY_8},
    {"KEY_9"          , "95", KEY_9},
    {"KEY_0"          , "27", KEY_0},
    {"TV/RADIO"       , "65", KEY_TV2}, //WE USE TV2 AS TV/RADIO SWITCHB
    {"RECALL"         , "A5", KEY_BACK},
    {"FIND"           , "75", KEY_FIND},
    {"REC"            , "D5", KEY_RECORD},
    {"SAT"            , "A7", KEY_SAT},
    {"FAV"            , "B5", KEY_FAVORITES},
    {"MENU"           , "1D", KEY_MENU},
    {"INFO"           , "5F", KEY_INFO},
    {"OK"             , "BF", KEY_OK},
    {"UP"             , "9F", KEY_UP},
    {"DOWN"           , "AF", KEY_DOWN},
    {"LEFT"           , "3D", KEY_LEFT},
    {"RIGHT"          , "7F", KEY_RIGHT},
    {"EXIT"           , "2D", KEY_EXIT},
    {"EPG"            , "6F", KEY_EPG},
    {"FOLDER"         , "0D", KEY_ARCHIVE},
    {"STOP"           , "8F", KEY_STOP},
    {"PAUSE"          , "CD", KEY_PAUSE},
    {"PLAY"           , "4F", KEY_PLAY},
    {"PREV"           , "35", KEY_PREVIOUS},
    {"NEXT"           , "B7", KEY_NEXT},
    {"FASTFORWARD"    , "77", KEY_FASTFORWARD},
    {"REWIND"         , "F5", KEY_REWIND},
    {"FAST"           , "97", KEY_F},
    {"SLOW"           , "15", KEY_SLOW},
    {"PLAY_MODE"      , "E5", KEY_P},
    {"USB"            , "67", KEY_CLOSE},
    {"UHF"            , "0F", KEY_U},
    {"AUDIO"          , "25", KEY_SUBTITLE},
    {"RED"            , "05", KEY_RED},
    {"GREEN"          , "87", KEY_GREEN},
    {"YELLOW"         , "C5", KEY_YELLOW},
    {"BLUE"           , "47", KEY_BLUE},
    {""               , ""  , KEY_NULL},
};

/* GALAXY RC */
static tButton cButtonsGalaxy[] = {
    {"POWER"          , "25", KEY_POWER},
    {"R"              , "A5", KEY_R},
    {"V.FORMAT"       , "AD", KEY_V},
    {"TIME"           , "8D", KEY_TIME},
    {"MUTE"           , "85", KEY_MUTE},
    {"TV/SAT"         , "C5", KEY_AUX},
    {"Tms"            , "E5", KEY_T},
    {"PRESENTATION"   , "ED", KEY_PRESENTATION},
    {"F1"             , "CD", KEY_F1},
    {"0BUTTON"        , "57", KEY_0},
    {"1BUTTON"        , "B5", KEY_1},
    {"2BUTTON"        , "95", KEY_2},
    {"3BUTTON"        , "BD", KEY_3},
    {"4BUTTON"        , "F5", KEY_4},
    {"5BUTTON"        , "D5", KEY_5},
    {"6BUTTON"        , "FD", KEY_6},
    {"7BUTTON"        , "35", KEY_7},
    {"8BUTTON"        , "15", KEY_8},
    {"9BUTTON"        , "3D", KEY_9},
    {"TV/RADIO"       , "77", KEY_TV2},
    {"RECALL"         , "7F", KEY_BACK},
    {"VOL+"           , "C7", KEY_VOLUMEDOWN},
    {"VOL-"           , "DD", KEY_VOLUMEUP},
    {"PAGE-"          , "5F", KEY_PAGEDOWN},
    {"PAGE+"          , "07", KEY_PAGEUP},
    {"FIND"           , "9D", KEY_FIND},
    {"SAT"            , "1D", KEY_SAT},
    {"REC"            , "45", KEY_RECORD},
    {"FAV"            , "87", KEY_FAVORITES},
    {"MENU"           , "65", KEY_MENU},
    {"INFO"           , "A7", KEY_INFO},
    {"EXIT"           , "4D", KEY_EXIT},
    {"EPG"            , "8F", KEY_EPG},
    {"OK"             , "2F", KEY_OK},
    {"UP"             , "27", KEY_UP},
    {"DOWN"           , "0F", KEY_DOWN},
    {"LEFT"           , "6D", KEY_LEFT},
    {"RIGHT"          , "AF", KEY_RIGHT},
    {"FOLDER"         , "75", KEY_ARCHIVE},
    {"STOP"           , "F7", KEY_STOP},
    {"PAUSE"          , "37", KEY_PAUSE},
    {"PLAY"           , "B7", KEY_PLAY},
    {"PREV"           , "55", KEY_PREVIOUS},
    {"NEXT"           , "D7", KEY_NEXT},
    {"REWIND"         , "17", KEY_REWIND},
    {"FORWARD"        , "97", KEY_FORWARD},
    {"USB"            , "9F", KEY_CLOSE},
    {"RED"            , "7D", KEY_RED},
    {"GREEN"          , "FF", KEY_GREEN},
    {"YELLOW"         , "3F", KEY_YELLOW},
    {"BLUE"           , "BF", KEY_BLUE},
    {"PLAY_MODE"      , "1F", KEY_P},
    {"SLOW"           , "5D", KEY_SLOW},
    {"FAST"	          , "DF", KEY_FASTFORWARD},
    {""               , ""  , KEY_NULL},
};

/* spark Default */
static tButton cButtonsSparkEdv[] = {
    {"POWER"          , "87", KEY_POWER},
    {"V.FORMAT"       , "2f", KEY_V},
    {"TV/SAT"         , "95", KEY_AUX},
    {"TIME"           , "65", KEY_TIME},
    {"MUTE"           , "A5", KEY_MUTE},
    {"0BUTTON"        , "37", KEY_0},
    {"1BUTTON"        , "A7", KEY_1},
    {"2BUTTON"        , "07", KEY_2},
    {"3BUTTON"        , "E5", KEY_3},
    {"4BUTTON"        , "97", KEY_4},
    {"5BUTTON"        , "27", KEY_5},
    {"6BUTTON"        , "D5", KEY_6},
    {"7BUTTON"        , "B7", KEY_7},
    {"8BUTTON"        , "17", KEY_8},
    {"9BUTTON"        , "F5", KEY_9},
    {"TV/RADIO"       , "CD", KEY_TV2}, //WE USE TV2 AS TV/RADIO SWITCHB
    {"RECALL"         , "CF", KEY_BACK},
    {"PAGE+"          , "ED", KEY_PAGEUP},
    {"PAGE-"          , "DD", KEY_PAGEDOWN},
    {"FIND"           , "C5", KEY_FIND},
    {"FOLDER"         , "67", KEY_ARCHIVE},
    {"VOL+"           , "AF", KEY_VOLUMEUP},
    {"VOL-"           , "9F", KEY_VOLUMEDOWN},
    {"MENU"           , "C7", KEY_MENU},
    {"INFO"           , "1F", KEY_INFO},
    {"UP"             , "3F", KEY_UP},
    {"DOWN"           , "85", KEY_DOWN},
    {"LEFT"           , "BF", KEY_LEFT},
    {"RIGHT"          , "FD", KEY_RIGHT},
    {"OK"             , "05", KEY_OK},
    {"EXIT"           , "F9", KEY_EXIT},
    {"EPG"            , "45", KEY_EPG},
    {"FAV"            , "3D", KEY_FAVORITES},
    {"SAT"            , "0D", KEY_SAT},
    {"RED"            , "6D", KEY_RED},
    {"GREEN"          , "8D", KEY_GREEN},
    {"YELLOW"         , "77", KEY_YELLOW},
    {"BLUE"           , "AD", KEY_BLUE},
    {"REC"            , "F7", KEY_RECORD},
    {"STOP"           , "BB", KEY_STOP},
    {"PLAY"           , "57", KEY_PLAY},
    {"PAUSE"          , "4D", KEY_PAUSE},
    {"FASTFORWARD"    , "35", KEY_FASTFORWARD},
    {"REWIND"         , "7F", KEY_REWIND},
    {"PREV"           , "FB", KEY_PREVIOUS},
    {"NEXT"           , "3B", KEY_NEXT},
    {"FAST"           , "E7", KEY_F},
    {"SLOW"           , "7B", KEY_SLOW},
    {"PLAY_MODE"      , "B5", KEY_P},
    {"USB"            , "DF", KEY_CLOSE},
    {"Tms"            , "55", KEY_T},
    {"F1"             , "15", KEY_F1},
    {"F2"             , "D1", KEY_F2},
    {""               , ""  , KEY_NULL},
};

/* fixme: move this to a structure and
 * use the private structure of RemoteControl_t
 */
static struct sockaddr_un  vAddr;

#define STB_ID_GOLDENMEDIA_GM990        "09:00:07"
#define STB_ID_EDISION_PINGULUX	        "09:00:08"
#define STB_ID_AMIKO_ALIEN_SDH8900      "09:00:0A"
#define STB_ID_GALAXYINNOVATIONS_S8120  "09:00:0B"

static tButton *pSparkGetButton(char *pData)

{
	tButton	*pButtons = cButtonsEdisionSpark;
	if (!strncasecmp(pData, SPARK_RC05_PREDATA, sizeof(SPARK_RC05_PREDATA)))
	{
		pButtons = cButtonsEdisionSpark;
	}
	else if (!strncasecmp(pData, SPARK_RC08_PREDATA, sizeof(SPARK_RC08_PREDATA)))
	{
		pButtons = cButtonsSparkRc08;
	}
	else if (!strncasecmp(pData, SPARK_RC09_PREDATA, sizeof(SPARK_RC09_PREDATA))) {
		static tButton *cButtons = NULL;
		if (!cButtons) {
			int fn = open("/proc/cmdline", O_RDONLY);
			if (fn > -1) {
				char procCmdLine[1024];
				int len = read(fn, procCmdLine, sizeof(procCmdLine) - 1);
				if (len > 0) {
					procCmdLine[len] = 0;
					if (strstr(procCmdLine, "STB_ID=" STB_ID_EDISION_PINGULUX))
						cButtons = cButtonsEdisionSpark;
					if (strstr(procCmdLine, "STB_ID=" STB_ID_GALAXYINNOVATIONS_S8120))
						cButtons = cButtonsGalaxy;
				}
				close(fn);
			}
			if (!cButtons)
				cButtons = cButtonsSparkRc09; /* Amiko Alien 8900 */
		}
		return cButtons;
	}
	else if (!strncasecmp(pData, SPARK_DEFAUYLT_PREDATA, sizeof(SPARK_DEFAUYLT_PREDATA)))
	{
		pButtons = cButtonsSparkDefault;
	}
	else if (!strncasecmp(pData, SPARK_RC12_PREDATA, sizeof(SPARK_RC12_PREDATA)))
	{
		pButtons = cButtonsSparkRc12;
	}
	else if (!strncasecmp(pData, SPARK_RC04_PREDATA, sizeof(SPARK_RC04_PREDATA)))
	{
		pButtons = cButtonsSparkRc04;
	}
	else if (!strncasecmp(pData, SPARK_EDV_RC1, sizeof(SPARK_EDV_RC1)))
	{
		pButtons = cButtonsSparkEdv;
	}
	else if (!strncasecmp(pData, SPARK_EDV_RC2, sizeof(SPARK_EDV_RC2)))
	{
		pButtons = cButtonsSparkEdv;
	}
	return pButtons;
}


static int pInit(Context_t* context, int argc, char* argv[]) {

    int vHandle;

    vAddr.sun_family = AF_UNIX;
    strcpy(vAddr.sun_path, "/var/run/lirc/lircd");

    vHandle = socket(AF_UNIX,SOCK_STREAM, 0);

    if(vHandle == -1)  {
        perror("socket");
        return -1;
    }

    if(connect(vHandle,(struct sockaddr *)&vAddr,sizeof(vAddr)) == -1)
    {
        perror("connect");
        return -1;
    }

    return vHandle;
}

static int pShutdown(Context_t* context ) {

    close(context->fd);

    return 0;
}

static int pRead(Context_t* context ) {
    char                vBuffer[128];
    char                vData[10];
    const int           cSize         = 128;
    int                 vCurrentCode  = -1;
    int                 rc;
    tButton             *cButtons = cButtonsEdisionSpark;

    memset(vBuffer, 0, 128);
    //wait for new command
    rc = read (context->fd, vBuffer, cSize);
    if(rc <= 0)return -1;

    //parse and send key event
    vData[0] = vBuffer[17];
    vData[1] = vBuffer[18];
    vData[2] = '\0';


    vData[0] = vBuffer[8];
    vData[1] = vBuffer[9];
    vData[2] = vBuffer[10];
    vData[3] = vBuffer[11];
    vData[4] = '\0';
    cButtons = pSparkGetButton(vData);

    vData[0] = vBuffer[14];
    vData[1] = vBuffer[15];
    vData[2] = '\0';

    printf("[RCU] key: %s -> %s\n", vData, &vBuffer[0]);
    vCurrentCode = getInternalCode(cButtons, vData);

    if(vCurrentCode != 0) {
        static int nextflag = 0;
        if (('0' == vBuffer[17]) && ('0' == vBuffer[18]))
        {
            nextflag++;
        }
        vCurrentCode += (nextflag << 16);
    }
    return vCurrentCode;
}

static int pNotification(Context_t* context, const int cOn) {

    struct aotom_ioctl_data vfd_data;
    int ioctl_fd = -1;

    if(cOn)
    {
       ioctl_fd = open("/dev/vfd", O_RDONLY);
       vfd_data.u.icon.icon_nr = 35;
       vfd_data.u.icon.on = 1;
       ioctl(ioctl_fd, VFDICONDISPLAYONOFF, &vfd_data);
       close(ioctl_fd);
    }
    else
    {
       usleep(100000);
       ioctl_fd = open("/dev/vfd", O_RDONLY);
       vfd_data.u.icon.icon_nr = 35;
       vfd_data.u.icon.on = 0;
       ioctl(ioctl_fd, VFDICONDISPLAYONOFF, &vfd_data);
       close(ioctl_fd);
    }

    return 0;
}

RemoteControl_t Spark_RC = {
	"Spark RemoteControl",
	Spark,
	&pInit,
	&pShutdown,
	&pRead,
	&pNotification,
	cButtonsEdisionSpark,
	NULL,
	NULL,
	1,
	&cLongKeyPressSupport,
};

