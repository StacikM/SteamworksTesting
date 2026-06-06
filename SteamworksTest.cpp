
// Dear programmer, you are about to experience the worst c++ code of your life, that still works (somehow!)
// If you want to contribute, please also contribute your eyes to help this code look better, thanks!

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <limits>
#include <steam_api.h>

std::string nameButGlobal;
CSteamID steamIdButGlobalToo;
uint64 steamId64ButGlobalToo;

class LobbyPrinter // i hate steam
{
public:
    void PrintLobbies();

private:
    void OnLobbyMatchList(LobbyMatchList_t* pResult, bool bIOFailure);

    CCallResult<LobbyPrinter, LobbyMatchList_t> m_CallResult;
};

void LobbyPrinter::PrintLobbies()
{
    SteamAPICall_t handle =
        SteamMatchmaking()->RequestLobbyList();

    m_CallResult.Set(handle, this,
        &LobbyPrinter::OnLobbyMatchList);
}
void LobbyPrinter::OnLobbyMatchList(LobbyMatchList_t* pResult, bool bIOFailure)
{
    if (bIOFailure || !pResult)
        return;

    for (int i = 0; i < pResult->m_nLobbiesMatching; i++)
    {
        CSteamID lobby =
            SteamMatchmaking()->GetLobbyByIndex(i);

        printf("%llu\n", lobby.ConvertToUint64());
    }
}

class LobbyManager
{
public:
    CCallResult<LobbyManager, LobbyCreated_t> m_CreateCall;

    void Create();
private:
    void OnLobbyCreated(LobbyCreated_t* pResult, bool bIOFailure);
};
void LobbyManager::Create()
{
    SteamAPICall_t call =
        SteamMatchmaking()->CreateLobby(k_ELobbyTypePublic, 8);

    m_CreateCall.Set(call, this, &LobbyManager::OnLobbyCreated);
}
void LobbyManager::OnLobbyCreated(LobbyCreated_t* pResult, bool bIOFailure)
{
    if (bIOFailure || pResult->m_eResult != k_EResultOK)
        return;

    CSteamID lobbyID = pResult->m_ulSteamIDLobby;

    printf("Created lobby: %llu\n", lobbyID.ConvertToUint64());

    SteamMatchmaking()->SetLobbyData(lobbyID, "name", "Test Lobby");
}

void SetAppID(std::string what)
{
    std::ofstream file("steam_appid.txt");
    file << what;
    return;
}

std::string GetAppID()
{
    std::ifstream file("steam_appid.txt");
    if (!file.is_open()) {
        std::cout << "creating file";
        SetAppID("480");
    }

    std::string line;
    std::getline(file, line);

    return line;
}

void TheChoice() {
    std::string thechoice;
    std::cout <<
        "\n\nType a number:\n\n"
        "1 - Your Player Info        2 - Game Info\n"
        "3 - Change App ID           4 - Your Friends\n"
        "5 - Achievement List        6 - Unlock Achievement\n"
        "7 - Remove Achievement      8 - Current App ID info\n"
        "9 - List lobbies           10 - Create a test lobby (deleted on exit)\n"
        "11 - Set Rich Presence\n\n"
        "exit - Exit\n\n"
        "Pick Your Choice: ";    std::cin >> thechoice;
    LobbyManager lobbymanager;
    if (thechoice == "1") {
        std::cout << "\n\n---- Player Info ----" << std::endl;
        std::cout << "Name: ";
        std::cout << nameButGlobal;
        std::cout << "\nSteam ID 64: ";
        std::cout << std::to_string(steamId64ButGlobalToo);
        std::cout << "\nIs account phone verified?: ";
        std::cout << SteamUser()->BIsPhoneVerified();
        std::cout << "\nIs twofactor verified? ";
        std::cout << SteamUser()->BIsTwoFactorEnabled();
        std::cout << "\nSteam Level: ";
        std::cout << SteamUser()->GetPlayerSteamLevel();
    }
    else if (thechoice == "2") {
        int appid;
        std::cout << "Enter App ID:\n\n";
        std::cin >> appid;
        bool isinstalled = SteamApps()->BIsAppInstalled(appid);
        bool isOwned = SteamApps()->GetAppOwner() == steamIdButGlobalToo;
        //todo: implement steam store api but im too lazy to do it rn
        std::cout << "\n\nYour relation with app id: ";
        std::cout << appid;
        std::cout << "\nOwned? \n";
        std::cout << isOwned;
        std::cout << "\nInstalled? \n";
        std::cout << isinstalled;
    }
    else if (thechoice == "3") {
        std::string appID;
        std::cout << "What App ID? ";
        std::cin >> appID;
        SetAppID(appID);
        std::cout << "Done, please restart the tool";
    }
    else if (thechoice == "4") //so far cleanest thing somehow
    {
        int friendCount = SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);

        for (int i = 0; i < friendCount; i++)
        {
            CSteamID friendId = SteamFriends()->GetFriendByIndex(i, k_EFriendFlagImmediate);
            const char* name = SteamFriends()->GetFriendPersonaName(friendId);
            EPersonaState state = SteamFriends()->GetFriendPersonaState(friendId);
            FriendGameInfo_t gameInfo;
            bool isInGame = SteamFriends()->GetFriendGamePlayed(friendId, &gameInfo);

            std::cout << "-----------------------------\n";
            std::cout << "Name: " << name << "\n";
            std::cout << "SteamID: " << friendId.ConvertToUint64() << "\n";

            // status/game info
            if (isInGame)
            {
                std::cout << "Playing AppID: " << gameInfo.m_gameID.AppID() << "\n";
            }
            else
            {
                switch (state)
                {
                case k_EPersonaStateOnline:
                    std::cout << "Status: Online\n";
                    break;
                case k_EPersonaStateAway:
                    std::cout << "Status: Away\n";
                    break;
                case k_EPersonaStateBusy:
                    std::cout << "Status: Busy\n";
                    break;
                case k_EPersonaStateOffline:
                    std::cout << "Status: Offline\n";
                    break;
                default:
                    std::cout << "Status: Unknown\n";
                    break;
                }
            }
        }
    }
    else if (thechoice == "5") {; //yes ai generated (i couldnt get it to work my way)
        int count = SteamUserStats()->GetNumAchievements();

        for (int i = 0; i < count; i++)
        {
            const char* key = SteamUserStats()->GetAchievementName(i);

            const char* name = SteamUserStats()->GetAchievementDisplayAttribute(key, "name");
            const char* desc = SteamUserStats()->GetAchievementDisplayAttribute(key, "desc");

            bool unlocked = false;
            SteamUserStats()->GetAchievement(key, &unlocked);

            printf(
                "name: %s | desc: %s | unlocked: %d | key: %s\n",
                name ? name : "",
                desc ? desc : "",
                unlocked ? 1 : 0,
                key ? key : ""
            );
        }
    }
    else if (thechoice == "6") {
        std::string key;
        std::cout << "Enter Achievement Key (make sure you have set the AppID to the same game that has this achievement, and has checked the achievements and copied the key):\nAchievement Key: ";
        std::cin >> key;
        SteamUserStats()->SetAchievement(key.c_str());
        SteamUserStats()->StoreStats();
        std::cout << "\nDone! You will get a achievement notif, if the game dev has set that only the backend can apply the achievement, nothing will happen.";
    }
    else if (thechoice == "7") {
        std::string key;
        std::cout << "Enter Achievement Key to Reset (make sure you have set the AppID to the same game that has this achievement, and has checked the achievements and copied the key):\nAchievement Key: ";
        std::cin >> key;
        SteamUserStats()->ClearAchievement(key.c_str());
        SteamUserStats()->StoreStats();
        std::cout << "\nRemoved! if the game dev has set that only the backend can remove the achievement, nothing will happen.";
    }
    else if (thechoice == "exit") {
        std::exit(0);
    }
    else if (thechoice == "8") {
        int buildid = SteamApps()->GetAppBuildId();
        char installPath[1024];

        uint32 len = SteamApps()->GetAppInstallDir(
            SteamUtils()->GetAppID(),
            installPath,
            sizeof(installPath)
        );
        std::string languages = SteamApps()->GetAvailableGameLanguages();
        std::string currentLanguage = SteamApps()->GetCurrentGameLanguage();

        std::cout << "Game Info for ";
        std::cout << GetAppID();
        std::cout << "\n------------\n"; 
        std::cout << "App Build ID: ";
        std::cout << buildid;
        std::cout << "\nInstall path: " << installPath;
        std::cout << "\nLanguages: ";
        std::cout << languages;
        std::cout << "\nCurrent language: ";
        std::cout << currentLanguage;

    }
    else if (thechoice == "9") {
        LobbyPrinter lobbyPrinter;
        lobbyPrinter.PrintLobbies();
    }
    else if (thechoice == "10") {
        lobbymanager.Create();
    }
    else if (thechoice == "11") {
        std::unordered_set<std::string> validKeys = {
            "state", "location", "mode", "details"
        };

        auto getKey = [&](const std::string& prompt) -> std::string {
            std::string key;
            while (true)
            {
                std::cout << prompt;
                std::getline(std::cin, key);

                if (validKeys.count(key))
                    return key;

                std::cout << "Invalid key. Use: state, location, mode, details\n";
            }
            };

        auto getValue = [&](const std::string& prompt) -> std::string {
            std::string value;
            std::cout << prompt;
            std::getline(std::cin, value);
            return value;
            };

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::string key1 = getKey("Key1 (state, location, mode, details): ");
        std::string value1 = getValue("Value1: ");

        std::string key2 = getKey("Key2 (state, location, mode, details): ");
        std::string value2 = getValue("Value2: ");

        std::string key3 = getKey("Key3 (state, location, mode, details): ");
        std::string value3 = getValue("Value3: ");

        auto set = [](const std::string& k, const std::string& v) {
            if (!k.empty() && !v.empty())
                SteamFriends()->SetRichPresence(k.c_str(), v.c_str());
            };

        set(key1, value1);
        set(key2, value2);
        set(key3, value3);

        std::cout << "\nDone! Check your rich presence:\nhttps://steamcommunity.com/dev/testrichpresence\n";
    }
    else {
        std::cout << "\nEnter a valid number";
    }
}

int main()
{
    const char* name;
    if (!SteamAPI_Init()) {
        std::string response;
        std::cerr << "Failed to initialize Steam API, launch steam pls (press enter to quit, enter PANIC to reset app id to 480)\nWarning: first time installation needs PANIC" << std::endl;
        std::cin >> response;
        if (response == "PANIC") {
            SetAppID("480");
            std::cout << "Reset app id to 480";
        }
        else if (response == "panic") {
            SetAppID("480");
            std::cout << "Reset app id to 480";
        }
        std::cout << "Quitting";
        return 1;
	}
    name = SteamFriends()->GetPersonaName();
    CSteamID id = SteamUser()->GetSteamID();
    uint64 steamID64 = id.ConvertToUint64();
    nameButGlobal = name;
    steamIdButGlobalToo = id;
    steamId64ButGlobalToo = steamID64;
    //wow crazy code i have no idea what im doing

    std::cout << "\n\n\n\n\n\nLoaded Steamworks successfully!\n\n\n";
    std::cout << "Logged in as: ";
    std::cout << name;
    std::cout << " (";
    std::cout << steamID64;
    std::cout << ")\n\n";
    std::cout << "Current App ID: ";
    std::cout << GetAppID() << std::endl;
    while (true) {
        TheChoice();
        SteamAPI_RunCallbacks();
    }
}