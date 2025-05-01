/*
 * Copyright (C) 2018, 2022 nukeykt
 *
 * This file is part of Blood-RE.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef _LEVELS_H_
#define _LEVELS_H_

#include "typedefs.h"

#include "weather.h"

enum GAMETYPE : uint8_t
{
    GAMETYPE_0 = 0,
    GAMETYPE_1,
    GAMETYPE_2,
    GAMETYPE_3,
};

enum DIFFICULTY : uint8_t
{
    DIFFICULTY_0 = 0,
    DIFFICULTY_1,
    DIFFICULTY_2,
    DIFFICULTY_3,
    DIFFICULTY_4,
};

enum MONSTERSETTINGS : uint8_t
{
    MONSTERSETTINGS_0 = 0,
    MONSTERSETTINGS_1,
    MONSTERSETTINGS_2,
};

enum WEAPONSETTINGS : uint8_t
{
    WEAPONSETTINGS_0 = 0,
    WEAPONSETTINGS_1,
    WEAPONSETTINGS_2,
    WEAPONSETTINGS_3,
};

enum ITEMSETTINGS : uint8_t
{
    ITEMSETTINGS_0 = 0,
    ITEMSETTINGS_1,
    ITEMSETTINGS_2,
};

enum RESPAWNSETTINGS : uint8_t
{
    RESPAWNSETTINGS_0 = 0,
};

enum TEAMSETTINGS : uint8_t
{
    TEAMSETTINGS_0 = 0,
    TEAMSETTINGS_1,
    TEAMSETTINGS_2
};

#define kMaxFileKeyLen 16
#define kMaxMessages 32

#pragma pack(push, 1)
struct GAMEOPTIONS
{
    GAMETYPE        nGameType;
    DIFFICULTY      nDifficulty;
    int             nEpisode;
    int             nLevel;
    char            zLevelName[WMAX_PATH];
    char            zLevelSong[WMAX_PATH];
    int             nTrackNumber;
    char            szSaveGameName[kMaxFileKeyLen];
    char            szUserGameName[kMaxFileKeyLen];
    short           nSaveGameSlot;
    int             picEntry;
    uint32_t           uMapCRC;
    MONSTERSETTINGS nMonsterSettings;

    uint32_t           uGameFlags;

    // net game options/data only
    uint32_t           uNetGameFlags;

    WEAPONSETTINGS  nWeaponSettings;
    ITEMSETTINGS    nItemSettings;
    RESPAWNSETTINGS nRespawnSettings;
    TEAMSETTINGS    nTeamSettings;  // team and cooperative

    int             nMonsterRespawnTime;
    int             nWeaponRespawnTime;
    int             nItemRespawnTime;
    int             nSpecialRespawnTime;
};

struct LEVELINFO
{
    char at0[144]; // Filename
    char at90[32]; // Title
    char atb0[32]; // Author
    char atd0[16]; // Song;
    int ate0; // SongId
    int ate4; // EndingA
    int ate8; // EndingB
    char atec[kMaxMessages][64]; // Messages
    QBOOL at8ec; // Fog
    QBOOL at8ed; // Weather
}; // 0x8ee bytes

struct EPISODEINFO
{
    char at0[32];
    int nLevels;
    unsigned int bloodbath : 1;
    unsigned int cutALevel : 4;
    LEVELINFO at28[16];
    char at8f08[144];
    char at8f98[144];
    int at9028;
    int at902c;
    char at9030[144];
    char at90c0[144];
};

#pragma pack(pop)

extern EPISODEINFO gEpisodeInfo[];
extern GAMEOPTIONS gGameOptions;

extern int gEpisodeCount;

extern QBOOL gGameStarted;

extern WEATHERTYPE gWeatherType;

extern int gNextLevel;

extern GAMEOPTIONS gSingleGameOptions;

void levelEndLevel(int);
void levelSetupSecret(int);
void levelTriggerSecret(int);
char *levelGetTitle(void);
char *levelGetFilename(int nEpisode, int nLevel);
char *levelGetMessage(int);
void levelSetupOptions(int nEpisode, int nLevel);
void levelPlayIntroScene(int);
void levelAddUserMap(const char*);
void levelRestart(void);
void levelPlayEndScene(int nEpisode);
void func_269D8(const char* pzIni);
void func_26988(void);
void levelLoadDefaults(void);

#endif // !_LEVELS_H_
