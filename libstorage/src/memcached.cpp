#include "memcached.h"

class memcached {
    void init(const std::string &server, const std::string &port)
    {
        memcached_return rc;
        memcached_server_st *servers = NULL;
        memcached_st *memc;
        memc = memcached_create(NULL);
        servers = memcached_server_list_append(servers, server.c_str(), 11211, &rc);
        rc = memcached_server_push(memc, servers);
        if (rc == MEMCACHED_SUCCESS) {
            this.client = memc;
        }
        else {
            fprintf(stderr, "Couldn't add server: %s\n", memcached_strerror(memc, rc));
            exit(-1);
        }
    }

    std::string get(const std::string &key) {
        size_t return_value_length;
        memcached_return rc;
        auto value = memcached_get(client, key.c_str(), strlen(key.c_str()), &return_value_length, (uint32_t)0, &rc);
        if (rc == MEMCACHED_SUCCESS) {
            return std::string(value);
        }
        else {
            fprintf(stderr, "Couldn't get key: %s\n", memcached_strerror(m_client.back(), rc));
            exit(-1);
        }
    }

    void put(const std::string &key, const std::string &value) {
        memcached_return rc;
        rc = memcached_set(client, key.c_str(), strlen(key.c_str()), value.c_str(), strlen(value.c_str()), (time_t)0, (uint32_t)0);
        if (rc != MEMCACHED_SUCCESS) {
            fprintf(stderr, "Write failed: %s\n", memcached_strerror(m_client.back(), rc));
            exit(-1);
        }
    }

    std::shared_ptr<std::vector<const std::string>> get_batch(std::shared_ptr<std::vector<const std::string>> keys) {
        char return_key[MEMCACHED_MAX_KEY];
        size_t return_key_length;
        char *return_value;
        size_t return_value_length;
        std::shared_ptr<std::vector<const std::string>> return_values;
        char ** char_keys = (char **)malloc(keys.size() * sizeof(char*));
        for (auto key : keys) {
            char_keys.push_back(key.c_str());
        }
        memcached_return_t rc = memcached_mget(memc, keys, key_length, keys.size());
        while ((return_value = memcached_fetch(memc, return_key, &return_key_length, &return_value_length, &flags, &rc))){
            return_values.push_back(std::string(return_value));
        }
        return return_values;
    }
}