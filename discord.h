#ifndef DISCORD_H
#define DISCORD_H

class QString;
class MainWindow;

class Discord
{
public:
    Discord();
    void SetupDiscord();
    static void UpdateDiscord();
    void Toggle();
    void Shutdown();
    void Toggle(bool rpc);
    static bool rpc;
};
#endif // DISCORD_H
