#include "memcached.h"


    void memcached::init(const std::string &server, int &port)
    {
        memcached_return rc;
        memcached_server_st *servers = NULL;
        memcached_st *memc;
        memc = memcached_create(NULL);
        servers = memcached_server_list_append(servers, server.c_str(), port, &rc);
        rc = memcached_server_push(memc, servers);
        if (rc == MEMCACHED_SUCCESS) {
            clients.push_back(memc);
        }
        else {
            throw std::runtime_error("Server connection failed: " + memcached_strerror(memc, rc));
        }
    }

    std::string memcached::get(const std::string &key) {
        size_t return_value_length;
        memcached_return rc;
        auto idx = (std::hash<std::string>{}(std::string(key)) % clients.size());
        auto value = memcached_get(clients[idx], key.c_str(), strlen(key.c_str()), &return_value_length, (uint32_t)0, &rc);
        if (rc == MEMCACHED_SUCCESS) {
            return std::string(value);
        }
        else {
            throw std::runtime_error("Key retrieval failed: " + memcached_strerror(clients[idx], rc));
        }
    }

    void memcached::put(const std::string &key, const std::string &value) {
        memcached_return rc;
        auto idx = (std::hash<std::string>{}(std::string(key)) % clients.size());
        rc = memcached_set(clients[idx], key.c_str(), strlen(key.c_str()), value.c_str(), strlen(value.c_str()), (time_t)0, (uint32_t)0);
        if (rc != MEMCACHED_SUCCESS) {
            throw std::runtime_error("Write failed: " + memcached_strerror(clients[idx], rc));
        }
    }

    std::vector<const std::string> memcached::get_batch(std::vector<const std::string> keys) {
        std::unordered_map<int, std::vector<std::string>> key_vectors;

        // Gather all relevant storage interface's by id and create vector for key batch
        for (auto key: keys) {
            auto id = (std::hash<std::string>{}(std::string(key)) % clients.size());
            key_vectors[id].emplace_back(key);
        }

        std::vector<const std::string> return_values;

        for (auto it = key_vectors.begin(); it != key_vectors.end(); it++) {
            char return_key[MEMCACHED_MAX_KEY];
            size_t return_key_length;
            char *return_value;
            size_t return_value_length;
            char ** char_keys = (char **)malloc(keys.size() * sizeof(char*));
            int i = 0;
            for (auto key : it->second) {
                char_keys[i] = strdup(key.c_str());
                i++;
            }
            memcached_return_t rc = memcached_mget(clients[it->first], it->second, key_length, keys.size());
            if (rc != MEMCACHED_SUCCESS) {
                throw std::runtime_error("Batch read failed: " + memcached_strerror(clients[it->first], rc));
            }
            while ((return_value = memcached_fetch(clients[it->first], return_key, &return_key_length, &return_value_length, &flags, &rc))){
                return_values.push_back(std::string(return_value));
            }
        }

        return return_values;
    }

    void memcached::put_batch(std::vector<const std::string> keys, std::vector<const std::string> values)
        std::unordered_map<int, std::pair<std::vector<std::string>, std::vector<std::string>>> key_value_vector_pairs;

        // Gather all relevant storage interface's by id and create vector for key batch
        int i = 0;
        for (auto key: keys) {
            auto id = (std::hash<std::string>{}(std::string(key)) % clients.size());
            key_value_vector_pairs[id].first.emplace_back(key);
            key_value_vector_pairs[id].second.emplace_back(values[i]);
            i++;
        }

        for (auto it = key_value_vector_pairs.begin(); it != key_value_vector_pairs.end(); it++) {
            char return_key[MEMCACHED_MAX_KEY];
            size_t return_key_length;
            char *return_value;
            size_t return_value_length;
            char ** char_keys = (char **)malloc(keys.size() * sizeof(char*));
            char ** char_values = (char **)malloc(values.size() * sizeof(char*));
            int i = 0;
            for (auto key : it->second) {
                char_keys[i] = strdup(key.c_str());
                i++;
            }
            memcached_return_t rc = memcached_mset(clients[it->first], it->second, key_length, keys.size());
            if (rc != MEMCACHED_SUCCESS) {
                throw std::runtime_error("Batch write failed: " + memcached_strerror(clients[it->first], rc));
            }
        }
    }
