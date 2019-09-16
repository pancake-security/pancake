//
// Created by Lloyd Brown on 9/2/19.
//

#include "redis.h"

    void redis::init(const std::string hostname, int port){
        clients.push_back(std::move(std::make_shared<cpp_redis::client>()));
        clients.back()->connect(hostname, port,
                [](const std::string &host, std::size_t port, cpp_redis::client::connect_state status) {
                    if (status == cpp_redis::client::connect_state::dropped || status == cpp_redis::client::connect_state::failed || status == cpp_redis::client::connect_state::lookup_failed){
                        std::cerr << "Redis client disconnected from " << host << ":" << port << std::endl;
                        exit(-1);
                    }
                });
    }

    std::string redis::get(const std::string &key){
        auto idx = (std::hash<std::string>{}(std::string(key)) % clients.size());
        auto fut = clients[idx]->get(key);
        clients[idx]->commit();
        auto reply = fut.get();
        if (reply.is_error()){
            throw std::runtime_error(reply.error());
        }
        return reply.as_string();
    }

    void redis::put(const std::string &key, const std::string &value){
        auto idx = (std::hash<std::string>{}(std::string(key)) % clients.size());
        auto fut = clients[idx]->set(key, value);
        clients[idx]->commit();
        auto reply = fut.get();
        if (reply.is_error()){
            throw std::runtime_error(reply.error());
        }
    }

    std::vector<const std::string> redis::get_batch(std::vector<const std::string> keys){
        std::queue<std::future<cpp_redis::reply>> futures;
        std::unordered_map<int, std::vector<std::string>> key_vectors;

        // Gather all relevant storage interface's by id and create vector for key batch
        for (auto key: keys) {
            auto id = (std::hash<std::string>{}(std::string(key)) % clients.size());
            key_vectors[id].emplace_back(key);
        }

        for (auto it = key_vectors.begin(); it != key_vectors.end(); it++) {
            auto future = clients[it->first]->mget(it->second);
            futures.push(std::move(future));

        }
        // Issue requests to each storage server
        for (auto it = key_vectors.begin(); it != key_vectors.end(); it++)
            clients[it->first]->commit();

        std::vector<const std::string> return_vector;

        for (int i = 0; i < futures.size(); i++) {
            auto reply = futures.front().get();
            futures.pop();
            if (reply.is_error()){
                throw std::runtime_error(reply.error());
            }
            auto reply_array = reply.as_array();
            for (auto nested_reply: reply_array){
                if (nested_reply.is_error()){
                    throw std::runtime_error(nested_reply.error());
                }
                return_vector.push_back(nested_reply.as_string());
            }
        }
        return return_vector;
    }

    void redis::put_batch(std::vector<const std::string> keys, std::vector<const std::string> values){
        std::queue<std::future<cpp_redis::reply>> futures;
        std::unordered_map<int, std::vector<std::pair<std::string, std::string>>> key_value_vector_pairs;

        // Gather all relevant storage interface's by id and create vector for key batch
        int i = 0;
        for (auto key: keys) {
            auto id = (std::hash<std::string>{}(std::string(key)) % clients.size());
            key_value_vector_pairs[id].emplace_back(std::make_pair(key, values[i]));
            i++;
        }

        for (auto it = key_value_vector_pairs.begin(); it != key_value_vector_pairs.end(); it++) {
            auto future = clients[it->first]->mset(it->second);
            futures.push(std::move(future));
        }

        // Issue requests to each storage server
        for (auto it = key_value_vector_pairs.begin(); it != key_value_vector_pairs.end(); it++)
            clients[it->first]->commit();

        std::shared_ptr<std::vector<const std::string>> return_vector;

        for (int i = 0; i < futures.size(); i++){
            auto reply = futures.front().get();
            futures.pop();
            if (reply.is_error()){
                throw std::runtime_error(reply.error());
            }
        }
    }