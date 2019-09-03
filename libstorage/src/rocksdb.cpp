//
// Created by Lloyd Brown on 9/2/19.
//

#include "rocksdb.h"

class rocksdb {
    void init(const std::string hostname, const std::string port){
        ssdb::Client * new_client = ssdb::Client::connect(server.c_str(), std::stoi(port));
        if (new_client == NULL){
            std::cerr << "Failed to connect" << std::endl;
            exit(-1);
        }
        client = new_client;
    }

    std::string get(const std::string &key, const std::string &value){
        std::string val;
        ssdb::Status s = client->get(key, &val);
        assert(s.ok());
        return val;
    }

    void put(const std::string &key, const std::string &value){
        ssdb::Status s = client->set(key, value);
        if (!s.ok())
            std::cout << s.code() << std::endl;
        assert(s.ok());
    }

    std::shared_ptr<std::vector<const std::string>> get_batch(std::shared_ptr<std::vector<const std::string>> keys){
        std::shared_ptr<std::vector<const std::string>> return_vector;
        for (auto key: keys){
            return_vector.push_back(get(key));
        }
        return return_vector;
    }

    void put_batch(std::shared_ptr<std::vector<const std::string>> keys, std::shared_ptr<std::vector<const std::string>> values){
        int i = 0;
        for (auto key: keys){
            put(key, values[i]);
            i++;
        }
    }
};