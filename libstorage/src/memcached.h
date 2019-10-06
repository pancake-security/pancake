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
    memcached(const std::string &host_name, int port);
    void add_server(const std::string &host_name, int port);
    std::string get(const std::string &key) override;
    void put(const std::string &key, const std::string &value) override;
    std::vector<std::string> get_batch(const std::vector<std::string> &keys) override;
    void put_batch(const std::vector<std::string> &keys, const std::vector<std::string> &values) override;

private:
    std::vector<std::shared_ptr<memcached_st>> clients;
};

#endif //MEMCACHED_H
