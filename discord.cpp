#include "discord.h"
#include <chrono>
#include <string.h>
#include <iostream>

#include "DiscordSDK/include/discord_rpc.h"
#include "DiscordSDK/include/discord_register.h"

#include <QString>
#include "mainwindow.h"

bool Discord::rpc;

void Discord::SetupDiscord()
{
    if (rpc)
    {
        DiscordEventHandlers handlers;
        memset(&handlers, 0, sizeof(handlers));
        Discord_Initialize("842434378209951808", &handlers, 1, "");
        UpdateDiscord();
    }
    else
    {
        Discord_Shutdown();
    }
}


void Discord::UpdateDiscord()
{
    static int64_t StartTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    if (rpc)
    {
        DiscordRichPresence discordPresence;
        memset(&discordPresence, 0, sizeof(discordPresence));

        discordPresence.startTimestamp = StartTime;
        discordPresence.endTimestamp = NULL;
        discordPresence.largeImageKey = "image_large";
        discordPresence.largeImageText = "3rc6";
        discordPresence.instance = 1;

        Discord_UpdatePresence(&discordPresence);
    }
    else
    {
        Discord_ClearPresence();
    }
}

void Discord::Toggle()
{
    rpc = !rpc;
}

void Discord::Toggle(bool nrpc)
{
    rpc = nrpc;
}

void Discord::Shutdown()
{
    Discord_Shutdown();
}
