//
// Created by Lloyd Brown on 9/2/19.
//

#include "redis.h"

class redis {
    void init(const std::string hostname, const std::string port){
        client = std::make_shared<cpp_redis::client>();
        client->connect(server, std::stoull(port),
                [](const std::string &host, std::size_t port, cpp_redis::client::connect_state status) {
                    if (status == cpp_redis::client::connect_state::dropped || status == cpp_redis::client::connect_state::failed || status == cpp_redis::client::connect_state::lookup_failed){
                        std::cerr << "Redis client disconnected from " << host << ":" << port << std::endl;
                        exit(-1);
                    }
                });
    }

    std::string get(const std::string &key, const std::string &value){
        auto fut = client->get(key);
        client->commit();
        auto reply = fut.get();
        if (reply.is_error()){
            throw std::runtime_error(reply.error());
        }
        return relpy.as_string();
    }

    void put(const std::string &key, const std::string &value){
        auto fut = client->set(key, value);
        client->commit();
        auto reply = fut.get();
        if (reply.is_error()){
            throw std::runtime_error(reply.error());
        }
    }

    std::shared_ptr<std::vector<const std::string>> get_batch(std::shared_ptr<std::vector<const std::string>> keys){
        auto fut = client->mget(keys);
        client->commit();
        auto reply = fut.get();
        if (reply.is_error()){
            throw std::runtime_error(reply.error());
        }
        auto reply_array = reply.as_array();
        std::shared_ptr<std::vector<const std::string>> return_vector;
        for (auto nested_reply: reply_array){
            if (nested_reply.is_error()){
                throw std::runtime_error(nested_reply.error());
            }
            return_vector.push_back(nested_reply.as_string());
        }
        return return_vector;
    }

    void put_batch(std::shared_ptr<std::vector<const std::string>> keys, std::shared_ptr<std::vector<const std::string>> values){
        auto fut = client->mset(keys, values);
        client->commit();
        auto reply = fut.get();
        if (reply.is_error()){
            throw std::runtime_error(reply.error());
        }
        auto reply_array = reply.as_array();
        for (auto nested_reply: reply_array){
            if (nested_reply.is_error()){
                throw std::runtime_error(nested_reply.error());
            }
        }
    }
};