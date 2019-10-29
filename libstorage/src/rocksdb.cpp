//
// Created by Lloyd Brown on 9/2/19.
//

#include "rocksdb.h"


    rocksdb::rocksdb(const std::string &host_name, int port){
        this->clients.push_back(ssdb::Client::connect(host_name.c_str(), port));
        if (this->clients.back() == NULL){
            std::cerr << "Failed to connect" << std::endl;
            exit(-1);
        }
    }

    void rocksdb::add_server(const std::string &host_name, int port){
        this->clients.push_back(ssdb::Client::connect(host_name.c_str(), port));
        if (this->clients.back() == NULL){
            std::cerr << "Failed to connect" << std::endl;
            exit(-1);
        }
    }

    std::string rocksdb::get(const std::string &key){
        std::string val;
        auto idx = (std::hash<std::string>{}(std::string(key)) % clients.size());
        ssdb::Status s = clients[idx]->get(key, &val);
        if (!s.ok())
            throw std::runtime_error((s.code()));
        return val;
    }

    void rocksdb::put(const std::string &key, const std::string &value){
        auto idx = (std::hash<std::string>{}(std::string(key)) % clients.size());
        ssdb::Status s = clients[idx]->set(key, value);
        if (!s.ok())
            throw std::runtime_error((s.code()));
        assert(s.ok());
    }

    std::vector<std::string> rocksdb::get_batch(const std::vector<std::string> &keys){
        std::vector<std::string> return_vector;
        for (const auto &key: keys){
            return_vector.push_back(get(key));
        }
        return return_vector;
    }

    void rocksdb::put_batch(const std::vector<std::string> &keys, const std::vector<std::string> &values){
        int i = 0;
        for (const auto &key: keys){
            put(key, values[i]);
            i++;
        }
    }