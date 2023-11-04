#pragma once
#include <cstdint>

enum class GameMessageType : uint32_t
{
    // Get server status
    Server_GetStatus,
    // make sure we're still connected
    Server_GetPing,

    // client was accepted
    Client_Accepted,
    // then we give him an id
    Client_AssignID,

    Game_AddPlayer,
    Game_RemovePlayer,
    Game_UpdatePlayer
    
};