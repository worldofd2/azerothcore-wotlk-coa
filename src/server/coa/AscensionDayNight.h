/* Copyright (C) 2016+ AzerothCore, GNU AGPL v3. */

#ifndef ASCENSION_DAY_NIGHT_H
#define ASCENSION_DAY_NIGHT_H

#include "AscensionDayNightRules.h"
#include <ctime>

class Player;

namespace AscensionDayNight
{
float Factor();
time_t Now();
void SendClientClock(Player* player);
void SendClientClock(Player* player, time_t shownTime);
}

#endif
