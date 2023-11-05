#include <unordered_map>
#include <NetBase/NetInclude.h>
#include <NetCustom/NetCustom.h>


class SimpleServer : public net::server_interface<CustomMsgTypes>
{
public:
    SimpleServer(uint16_t nPort) : server_interface<CustomMsgTypes>(nPort)
    {
        
    }
protected:
    virtual bool OnClientConnect(std::shared_ptr<net::connection<CustomMsgTypes>> client)
    {
        net::message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::ServerAuthenticate;
        client->Send(msg);
        return true;
    }

    // Called when a client appears to have disconnected
    virtual void OnClientDisconnect(std::shared_ptr<net::connection<CustomMsgTypes>> client)
    {
        std::cout << "Removing client [" << client->GetID() << "]\n";
    }

    // Called when a message arrives
    virtual void OnMessage(std::shared_ptr<net::connection<CustomMsgTypes>> client, net::message<CustomMsgTypes>& msg)
    {
        switch (msg.header.id)
        {
        case CustomMsgTypes::MessagePackTest:
            {
                std::cout << "Message pack test" << std::endl;
                std::vector<uint8_t> vectorData;
                msg >> vectorData;
                // Create a vector and use the constructor with two iterators
                auto auth = msgpack::unpack<AuthRequest>(vectorData);
                std::cout << "Auth : " << auth.user << auth.pass << std::endl;

            }
            break;
        }
    }

public:
};

int main(int argc, char* argv[])
{
    SimpleServer server(62899);
    server.Start();

    while (1)
    {
        server.Update(-1, true);
    }
    return 0;
}
