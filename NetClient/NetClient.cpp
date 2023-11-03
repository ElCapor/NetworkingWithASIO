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
};

int main(int argc, char* argv[])
{
    SimpleClient c;
    c.Connect("127.0.0.1", 62899);
    bool key[3] = { false, false, false };
    bool old_key[3] = { false, false, false };

    bool bQuit = false;
    while (!bQuit)
    {
        if (GetForegroundWindow() == GetConsoleWindow())
        {
            key[0] = GetAsyncKeyState(VK_LEFT) & 0x8000;
            key[1] = GetAsyncKeyState(VK_UP) & 0x8000;
            key[2] = GetAsyncKeyState(VK_DOWN) & 0x8000;
        }

        if (key[0] && !old_key[0]) c.PingServer();
        if (key[1] && !old_key[1]) c.MessageAll();
        if (key[2] && !old_key[2]) bQuit = true;

        for (int i = 0; i < 3; i++) old_key[i] = key[i];

        if (c.IsConnected())
        {
            if (!c.Incoming().empty())
            {


                auto msg = c.Incoming().pop_front().msg;

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
                }
            }
        }
        else
        {
            std::cout << "Server Down\n";
            bQuit = true;
        }

    }
    return 0;
}
