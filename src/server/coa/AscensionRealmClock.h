/* Copyright (C) 2016+ AzerothCore, GNU AGPL v3. */

#ifndef ASCENSION_REALM_CLOCK_H
#define ASCENSION_REALM_CLOCK_H

#include "AscensionDayNight.h"
#include "Timer.h"

namespace AscensionRealmClock
{
inline bool IsNight()
{
    return AscensionDayNight::IsNightHour(Acore::Time::GetHours(Seconds(AscensionDayNight::Now())));
}
}

#endif
