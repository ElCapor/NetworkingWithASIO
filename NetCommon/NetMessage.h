#pragma once
#include "NetCommon.h"

namespace net
{
    
    // Message header , used for all messages, we use a template to be able to use an ENUM
    // and since the enum is checked at compile time we can make sure that the messages are valid
    template <typename T>
    struct message_header
    {
        T id{};
        uint32_t size = 0;
    };

    // treating the message as a stack
    template <typename T>
    struct message
    {
        message_header<T> header{};
        std::vector<uint8_t> body;

        size_t size() const
        {
            return body.size();
        }

        friend std::ostream& operator << (std::ostream& os, const message<T>& msg)
        {
            os << "ID: " << int(msg.header.id) << " Size: " << msg.header.size;
            return os;
        }

        // send data into the message
        template <typename DataType>
        friend message<T>& operator << (message<T>& msg, const DataType& data)
        {
            // some data types are too complicated to be serialized by c++ , so we have to perform checks
            static_assert(std::is_standard_layout<DataType>::value, "DataType is too complex to be serialized");

            // we can't just start from 0 if we added things before
            size_t base_size = msg.body.size();

            // resize the vector to fit our data
            msg.body.resize(msg.body.size() + sizeof(DataType));

            // we start the copy at the base size
            std::memcpy(msg.body.data() + base_size, &data, sizeof(DataType));

            // update the header to reflect our new size
            msg.header.size = msg.size();

            return msg;
        }

        // extract data out of the message
        template <typename DataType>
        friend message<T>& operator >> (message<T>& msg, DataType& data)
        {
            static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be extracted");

            // this is the base size at which we will start extraction
            size_t base_size = msg.body.size() - sizeof(DataType);

            std::memcpy(&data, msg.body.data() + base_size, sizeof(DataType));

            msg.body.resize(base_size);

            msg.header.size = msg.size();

            return msg;
        }
         
    };

    template <typename T>
    class connection;
    /*
     * The server will have to manage multiple connections at the same time , so how will he know where did
     * each message come from ? A solution is to make an owned message type , whose owner is a client connection
     */
    template <typename T>
    struct owned_message
    {
        std::shared_ptr<connection<T>> remote = nullptr;
        message<T> msg;

        friend std::ostream& operator <<(std::ostream& os, const owner_message<T>& msg)
        {
            os << msg.msg;
            return os;
        }
        
    };
    
}
