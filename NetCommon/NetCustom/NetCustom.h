#pragma once
#include "CustomMsgType.h"
#include "../NetBase/MessagePack.h"
struct AuthRequest
{
    std::string user;
    std::string pass;
    template<class T>
    void pack(T &pack) {
      pack(user, pass);
    }
};