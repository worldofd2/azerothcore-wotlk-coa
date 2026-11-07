/* Copyright (C) 2016+ AzerothCore, GNU AGPL v3. */

#include "AscensionDayNightRules.h"
#include <gtest/gtest.h>

namespace
{
constexpr int64_t ServerStart = 1793836800;
uint32_t HourOf(int64_t time)
{
    return static_cast<uint32_t>((time % 86400) / 3600);
}
}

TEST(DayNightRules, FactorOneKeepsTheRealClockAndNight)
{
    for (int64_t now = ServerStart; now < ServerStart + 3 * 86400; now += 600)
    {
        EXPECT_EQ(AscensionDayNight::VirtualTime(now, ServerStart, 1.0f), now);
        EXPECT_EQ(AscensionDayNight::IsNightHour(HourOf(AscensionDayNight::VirtualTime(now, ServerStart, 1.0f))),
                  AscensionDayNight::IsNightHour(HourOf(now)));
    }
    EXPECT_FLOAT_EQ(AscensionDayNight::ClientSpeed(1.0f), AscensionDayNight::RealTimeClientSpeed);
}

TEST(DayNightRules, MissingOrInvalidFactorMeansRealTime)
{
    EXPECT_FLOAT_EQ(AscensionDayNight::ClampFactor(0.0f), 1.0f);
    EXPECT_FLOAT_EQ(AscensionDayNight::ClampFactor(-3.0f), 1.0f);
    EXPECT_FLOAT_EQ(AscensionDayNight::ClampFactor(0.5f), 1.0f);
    EXPECT_FLOAT_EQ(AscensionDayNight::ClampFactor(100.0f), 24.0f);
}

TEST(DayNightRules, FourTimesFasterRunsFourNightsADay)
{
    int nights = 0;
    bool wasNight = AscensionDayNight::IsNightHour(HourOf(AscensionDayNight::VirtualTime(ServerStart, ServerStart, 4.0f)));
    for (int64_t now = ServerStart; now <= ServerStart + 86400; now += 60)
    {
        bool const night = AscensionDayNight::IsNightHour(HourOf(AscensionDayNight::VirtualTime(now, ServerStart, 4.0f)));
        if (night && !wasNight)
            ++nights;
        wasNight = night;
    }
    EXPECT_EQ(nights, 4);
    EXPECT_FLOAT_EQ(AscensionDayNight::ClientSpeed(4.0f), 4.0f * AscensionDayNight::RealTimeClientSpeed);
}

TEST(DayNightRules, ClientSpeedAndServerClockAgree)
{
    int64_t const later = ServerStart + 7200;
    int64_t const virtualLater = AscensionDayNight::VirtualTime(later, ServerStart, 6.0f);
    EXPECT_EQ(virtualLater - ServerStart, 6 * 7200);
}
