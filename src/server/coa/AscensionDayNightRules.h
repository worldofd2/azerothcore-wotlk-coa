/* Copyright (C) 2016+ AzerothCore, GNU AGPL v3. */

#ifndef ASCENSION_DAY_NIGHT_RULES_H
#define ASCENSION_DAY_NIGHT_RULES_H

#include <cstdint>

namespace AscensionDayNight
{
constexpr float MinFactor = 1.0f;
constexpr float MaxFactor = 24.0f;
constexpr float RealTimeClientSpeed = 0.01666667f;

inline float ClampFactor(float factor)
{
    if (!(factor >= MinFactor))
        return MinFactor;
    return factor > MaxFactor ? MaxFactor : factor;
}

inline int64_t VirtualTime(int64_t now, int64_t serverStart, float factor)
{
    factor = ClampFactor(factor);
    if (factor == MinFactor || now <= serverStart)
        return now;
    return serverStart + static_cast<int64_t>(static_cast<double>(now - serverStart) * factor);
}

inline float ClientSpeed(float factor)
{
    return RealTimeClientSpeed * ClampFactor(factor);
}

inline bool IsNightHour(uint32_t hour)
{
    return hour < 6 || hour >= 18;
}
}

#endif
