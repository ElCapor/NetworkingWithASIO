#pragma once
#define WIN32_LEAN_AND_MEAN
#if defined(_WIN32)
// To avoid conflicting windows.h symbols with raylib, some flags are defined
// WARNING: Those flags avoid inclusion of some Win32 headers that could be required
// by user at some point and won't be included...
//-------------------------------------------------------------------------------------

// If defined, the following flags inhibit definition of the indicated items.
#define NOGDICAPMASKS     // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES // VK_*
#define NOWINMESSAGES     // WM_*, EM_*, LB_*, CB_*
#define NOWINSTYLES       // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define NOSYSMETRICS      // SM_*
#define NOMENUS           // MF_*
#define NOICONS           // IDI_*
#define NOKEYSTATES       // MK_*
#define NOSYSCOMMANDS     // SC_*
#define NORASTEROPS       // Binary and Tertiary raster ops
#define NOSHOWWINDOW      // SW_*
#define OEMRESOURCE       // OEM Resource values
#define NOATOM            // Atom Manager routines
#define NOCLIPBOARD       // Clipboard routines
#define NOCOLOR           // Screen colors
#define NOCTLMGR          // Control and Dialog routines
#define NODRAWTEXT        // DrawText() and DT_*
#define NOGDI             // All GDI defines and routines
#define NOKERNEL          // All KERNEL defines and routines
#define NOUSER            // All USER defines and routines
//#define NONLS             // All NLS defines and routines
#define NOMB              // MB_* and MessageBox()
#define NOMEMMGR          // GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE        // typedef METAFILEPICT
#define NOMINMAX          // Macros min(a,b) and max(a,b)
#define NOMSG             // typedef MSG and associated routines
#define NOOPENFILE        // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL          // SB_* and scrolling routines
#define NOSERVICE         // All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND           // Sound driver routines
#define NOTEXTMETRIC      // typedef TEXTMETRIC and associated routines
#define NOWH              // SetWindowsHook and WH_*
#define NOWINOFFSETS      // GWL_*, GCL_*, associated routines
#define NOCOMM            // COMM driver routines
#define NOKANJI           // Kanji support stuff.
#define NOHELP            // Help engine interface.
#define NOPROFILER        // Profiler interface.
#define NODEFERWINDOWPOS  // DeferWindowPos routines
#define NOMCX             // Modem Configuration Extensions

// Type required before windows.h inclusion
typedef struct tagMSG *LPMSG;


#endif
#include "NetBase/NetInclude.h"
#include "GameMessages.hpp"
#include "GamePlayer.hpp"

class GameClient : public net::client_interface<GameMessageType>
{
public:
    GameClient()
    {
        
    }

    std::unordered_map<uint32_t, player> players;
    uint32_t localPlayerId{};
    player localPlayer;

    bool bWaitingForConnection = true;

    void PingServer()
    {
        net::message<GameMessageType> msg;
        msg.header.id = GameMessageType::Server_GetPing;

        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
        msg << timeNow;
        Send(msg);
    }
};


void MainClientLoop(GameClient& c)
{
    if (c.IsConnected())
    {
        DrawText("Connected", 300, 200, 30, BLUE);
        if (!c.Incoming().empty())
        {
            auto msg = c.Incoming().pop_front().msg;
            switch (msg.header.id)
            {
            case GameMessageType::Client_Accepted:
                std::cout << "Yay the server accepted us !!!";
                break;
            case GameMessageType::Server_GetPing:
                {
                    // Server has responded to a ping request
                    std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
                    std::chrono::system_clock::time_point timeThen;
                    msg >> timeThen;
                    std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
                }
                break;
            default:
                std::cout << "Unknown message";
                break;
            }
        }

        if (GetKeyPressed() == KEY_B)
        {
            c.PingServer();
        }
            
    }
    else
    {
        DrawText("Waiting for connection...", 350, 200, 30, RED);
        c.bWaitingForConnection = !c.Connect("127.O.0.1", 62689);
    }
}
int main(int argc, char* argv[])
{
    InitWindow(800, 600, "NetBase Demo Mogus");
    SetTargetFPS(60);
    GameClient c;
    c.bWaitingForConnection = !c.Connect("127.0.0.1", 62689);
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);
        MainClientLoop(c);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
