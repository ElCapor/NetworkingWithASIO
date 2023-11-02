#include <NetIncludes.h>

enum class CustomMsgTypes :uint32_t
{
    FireBullet,
    MovePlayer
};
int main(int argc, char* argv[])
{
    net::message<CustomMsgTypes> msg;

    msg.header.id = CustomMsgTypes::FireBullet;

    int a = 1;
    bool b = true;
    float c = 3.1415f;

    

    msg << a << b << c;

    a = 99;
    b = false;
    c = 99.0f;
    
    msg >> c >> b >> a;
    return 0;
}
