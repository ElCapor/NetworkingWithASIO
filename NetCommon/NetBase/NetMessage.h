#pragma once
#include "MessagePack.h"
#include "NetCommon.h"
/*
 Changes made by myself : ElCapor

 Added support for vectors
 Using MessagePack as a backend for complicated data types
 (Maybe i should use it for trivial too ?)
 */
namespace net
{
    // Message Header is sent at start of all messages. The template allows us
    // to use "enum class" to ensure that the messages are valid at compile time
    template <typename T>
    struct message_header
    {
        T id{};
        uint32_t size = 0;
    };

    // Message Body contains a header and a std::vector, containing raw bytes
    // of infomation. This way the message can be variable length, but the size
    // in the header must be updated.
    template <typename T>
    struct message
    {
        // Header & Body vector
        message_header<T> header{};
        std::vector<uint8_t> body;

        // returns size of entire message packet in bytes
        size_t size() const
        {
            return body.size();
        }

        // Override for std::cout compatibility - produces friendly description of message
        friend std::ostream& operator <<(std::ostream& os, const message<T>& msg)
        {
            os << "ID:" << int(msg.header.id) << " Size:" << msg.header.size;
            return os;
        }

        // Convenience Operator overloads - These allow us to add and remove stuff from
        // the body vector as if it were a stack, so First in, Last Out. These are a 
        // template in itself, because we dont know what data type the user is pushing or 
        // popping, so lets allow them all. NOTE: It assumes the data type is fundamentally
        // Plain Old Data (POD). TLDR: Serialise & Deserialise into/from a vector

        // Pushes any POD-like data into the message buffer
        template <typename DataType>
        friend message<T>& operator <<(message<T>& msg, const DataType& data)
        {
            if (std::is_standard_layout_v<DataType>)
            {
                // Cache current size of vector, as this will be the point we insert the data
                size_t i = msg.body.size();

                // Resize the vector by the size of the data being pushed
                msg.body.resize(msg.body.size() + sizeof(DataType));

                // Physically copy the data into the newly allocated vector space
                std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

                // Recalculate the message size
                msg.header.size = msg.size();

                // Return the target message so it can be "chained"
                return msg;
            }
            else
            {
                std::cout << "[Warning] Complex data type used , resolving with MessagePack" << std::endl;
                try
                {
                    auto packed_data = msgpack::pack(data);
                    msg << data; // send the bytes of vector inside
                    return msg;
                } catch (const std::exception& e)
                {
                    std::cout << "[ERROR] packing complex data type " << e.what() << std::endl;
                    return msg; // return the unchanged msg
                }
                
            }
            
        }

        // extension to push vectors
        // it pushes the vector and it's size in the last
        template <typename VectorType>
        friend message<T>& operator << (message<T>& msg, const std::vector<VectorType>& vector)
        {
            static_assert(std::is_standard_layout_v<VectorType> && std::is_trivial_v<VectorType> , "VectorType is too complex to be pushed into vector");
            assert(vector.size() > 0, "Can't push an empty vector");
            for (int i=0; i < vector.size(); i++)
            {
                msg << vector[i];
            }
            msg << vector.size(); // push the size

            return msg;
        }
        
        // Pulls any POD-like data form the message buffer
        template <typename DataType>
        friend message<T>& operator >>(message<T>& msg, DataType& data)
        {
            if (std::is_standard_layout_v<DataType>)
            {
                // Cache the location towards the end of the vector where the pulled data starts
                size_t i = msg.body.size() - sizeof(DataType);

                // Physically copy the data from the vector into the user variable
                std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

                // Shrink the vector to remove read bytes, and reset end position
                msg.body.resize(i);

                // Recalculate the message size
                msg.header.size = msg.size();

                // Return the target message so it can be "chained"
                return msg;
            }
            else
            {
                try
                {
                    std::error_code ec;
                    std::vector<uint8_t> complexBytes;
                    msg >> complexBytes; // this should trigger vector operator
                    auto data_unpacked = msgpack::unpack<DataType>(complexBytes,ec);
                    data = data_unpacked;
                    return msg;
                }
                catch (std::exception& e)
                {
                    std::cout << "[ERROR] Unpacking complex data type failed " << e.what() << std::endl;
                }
            }
            
        }

        // extension to retrieve vectors
        // it retrieve the vector it's size and then reverse it
        template <typename VectorType>
        friend message<T>& operator >> (message<T>& msg, std::vector<VectorType>& vector)
        {
            static_assert(std::is_standard_layout_v<VectorType> && std::is_trivial_v<VectorType> , "VectorType is too complex to be retrieved from vector");
            //assert(vector.size() > 0, "Can't retrieve an empty vector");
            size_t arraySize;
            msg >> arraySize; // retrieve the size of the vector
            for (int i=0; i < arraySize; i++)
            {
                VectorType temp;
                msg >> temp;
                vector.push_back(temp);
            }
            std::reverse(vector.begin(), vector.end()); // since it's a stack you have to do this
            return msg;
        }
    };


    // An "owned" message is identical to a regular message, but it is associated with
    // a connection. On a server, the owner would be the client that sent the message, 
    // on a client the owner would be the server.

    // Forward declare the connection
    template <typename T>
    class connection;

    template <typename T>
    struct owned_message
    {
        std::shared_ptr<connection<T>> remote = nullptr;
        message<T> msg;

        // Again, a friendly string maker
        friend std::ostream& operator<<(std::ostream& os, const owned_message<T>& msg)
        {
            os << msg.msg;
            return os;
        }
    };
}
