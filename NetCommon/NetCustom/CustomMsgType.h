#pragma once
#include <cstdint>

enum class CustomMsgTypes : uint32_t
{
    ServerAccept,
    ServerDeny,
    ServerAuthenticate, // the server asks to client to auth himself
    ClientAuthRequest, // the client sends an auth request
    ServerPing,
    MessageAll,
    ServerMessage,
    MessagePackTest
};

