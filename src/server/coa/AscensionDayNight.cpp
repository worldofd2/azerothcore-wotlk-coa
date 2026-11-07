/* Copyright (C) 2016+ AzerothCore, GNU AGPL v3. */

#include "AscensionDayNight.h"
#include "Config.h"
#include "GameTime.h"
#include "Log.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "WorldSessionMgr.h"
#include <atomic>

namespace
{
std::atomic<float> g_factor{AscensionDayNight::MinFactor};
constexpr uint32 ClientClockResendMs = 10 * MINUTE * IN_MILLISECONDS;
}

float AscensionDayNight::Factor()
{
    return g_factor.load(std::memory_order_relaxed);
}

time_t AscensionDayNight::Now()
{
    return time_t(VirtualTime(GameTime::GetGameTime().count(), GameTime::GetStartTime().count(), Factor()));
}

void AscensionDayNight::SendClientClock(Player* player, time_t shownTime)
{
    if (!player || !player->GetSession())
        return;
    WorldPacket data(SMSG_LOGIN_SETTIMESPEED, 4 + 4 + 4);
    data.AppendPackedTime(shownTime);
    data << ClientSpeed(Factor());
    data << uint32(0);
    player->SendDirectMessage(&data);
}

void AscensionDayNight::SendClientClock(Player* player)
{
    SendClientClock(player, Now());
}

class ascension_day_night_world : public WorldScript
{
public:
    ascension_day_night_world()
        : WorldScript("ascension_day_night_world", { WORLDHOOK_ON_AFTER_CONFIG_LOAD, WORLDHOOK_ON_UPDATE }) { }

    void OnAfterConfigLoad(bool reload) override
    {
        float const factor = AscensionDayNight::ClampFactor(sConfigMgr->GetOption<float>("CoA.DayNightSpeed", 1.0f));
        float const before = g_factor.exchange(factor, std::memory_order_relaxed);
        if (factor != 1.0f || before != factor)
            LOG_INFO("server.loading", "CoA.DayNightSpeed: the day/night cycle runs {:.1f} times real time", factor);
        if (reload && before != factor)
            SendToAll();
    }

    void OnUpdate(uint32 diff) override
    {
        if (Factor() == 1.0f)
            return;
        _elapsed += diff;
        if (_elapsed < ClientClockResendMs)
            return;
        _elapsed = 0;
        SendToAll();
    }

private:
    static float Factor() { return AscensionDayNight::Factor(); }

    static void SendToAll()
    {
        for (auto const& [id, session] : sWorldSessionMgr->GetAllSessions())
            if (Player* player = session ? session->GetPlayer() : nullptr; player && player->IsInWorld())
                AscensionDayNight::SendClientClock(player);
    }

    uint32 _elapsed = 0;
};

class ascension_day_night_player : public PlayerScript
{
public:
    ascension_day_night_player() : PlayerScript("ascension_day_night_player", { PLAYERHOOK_ON_LOGIN, PLAYERHOOK_ON_MAP_CHANGED }) { }

    void OnPlayerLogin(Player* player) override
    {
        if (AscensionDayNight::Factor() != 1.0f)
            AscensionDayNight::SendClientClock(player);
    }

    void OnPlayerMapChanged(Player* player) override
    {
        if (AscensionDayNight::Factor() != 1.0f)
            AscensionDayNight::SendClientClock(player);
    }
};

void AddSC_AscensionDayNight()
{
    new ascension_day_night_world();
    new ascension_day_night_player();
}
