#pragma once
#include "NetCommon.h"
#include "NetMessage.h"
#include "NetTsQueue.h"

namespace net
{
    
    // Abstract connection class
    template <typename T>
    class connection : public std::enable_shared_from_this<connection<T>>
    {
    public:
        connection()
        {
            
        }

        virtual ~connection()
        {
            
        }

    public:
        bool ConnectToServer();
        bool Disconnect();
        bool IsConnected() const;

    public:
        bool Send(const message<T>& msg);

    protected:
        asio::ip::tcp::socket m_Socket;
    };
}