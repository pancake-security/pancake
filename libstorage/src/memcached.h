//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef MEMCACHED_H
#define MEMCACHED_H

#include "storage_interface.h"
#include <unordered_map>
#include <libmemcached/memcached.h>

class memcached : public storage_interface {
public:
    memcached(std::string hostname, int port);
    std::string get(std::string &key) override;
    void put(std::string &key, std::string &value) override;
    std::vector<std::string> get_batch(std::vector<std::string> * keys) override;
    void put_batch(std::vector<std::string> * keys, std::vector<std::string> * values) override;

private:
    std::vector<std::shared_ptr<memcached_st>> clients;
};

#endif //MEMCACHED_H
