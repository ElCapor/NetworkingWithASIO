#include <NetBase/NetInclude.h>
#include <NetCustom/NetCustom.h>

class SimpleClient : public net::client_interface<CustomMsgTypes>
{
public:
    void PingServer()
    {
        net::message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::ServerPing;

        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
        msg << timeNow;
        Send(msg);
    }
    void MessageAll()
    {
        net::message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::MessageAll;
        Send(msg);
    }

    void ProcessPackets()
    {
        if (IsConnected())
        {
            if (!Incoming().empty())
            {


                auto msg = Incoming().pop_front().msg;

                switch (msg.header.id)
                {
                case CustomMsgTypes::ServerAccept:
                    {
                        // Server has responded to a ping request				
                        std::cout << "Server Accepted Connection\n";
                    }
                    break;


                case CustomMsgTypes::ServerPing:
                    {
                        // Server has responded to a ping request
                        std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
                        std::chrono::system_clock::time_point timeThen;
                        msg >> timeThen;
                        std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
                    }
                    break;

                case CustomMsgTypes::ServerMessage:
                    {
                        // Server has responded to a ping request	
                        uint32_t clientID;
                        msg >> clientID;
                        std::cout << "Hello from [" << clientID << "]\n";
                    }
                    break;
                case CustomMsgTypes::ServerAuthenticate:
                    {
                        std::cout << "We must auth with server !" << std::endl;
                        AskForUserPass();
                    }
                    break;
                }
            }
        }
    }

    void AskForUserPass()
    {
        std::string user;
        std::string pass;
        std::cout << "User : ";
        std::cin >> user;
        std::cout << std::endl;
        std::cout << "Pass : ";
        std::cin >> pass;
        std::cout << std::endl;

        net::message<CustomMsgTypes> msg;
        msg.header.id = CustomMsgTypes::MessagePackTest;
        AuthRequest req;
        req.user =user;
        req.pass = pass;
        auto data = msgpack::pack(req);
        
        msg << data;
        Send(msg);
    }


};

int main(int argc, char* argv[])
{
    SimpleClient c;
    c.Connect("127.0.0.1", 62899);
    while (true)
    {
        c.ProcessPackets();
    }
    return 0;
}
