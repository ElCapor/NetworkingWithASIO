#include <iostream>

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <chrono>
int main(int argc, char* argv[])
{
    asio::error_code ec;

    asio::io_context context;

    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec), 80);

    asio::ip::tcp::socket socket(context);

    socket.connect(endpoint, ec);

    if (!ec)
    {
        std::cout << "Connected" << std::endl;
    }
    else
    {
        std::cout << "Error" << std::endl;
    }

    if (socket.is_open())
    {
        std::string sRequest =
            "GET /index.html HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Connection: close\r\n\r\n";

        socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

        socket.wait(socket.wait_read);
        size_t bytes = socket.available();
        std::cout << "Free bytes " << bytes << std::endl;
        if (bytes > 0)
        {
            std::vector<char> vBuffer(bytes);
            socket.read_some(asio::buffer(vBuffer.data(), vBuffer.size()), ec);
            for (auto c: vBuffer)
                std::cout << c;
        }
            
    }
    
    return 0;
}
