//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef MEMCACHED_H
#define MEMCACHED_H

#include "storage_interface.h"
#include <libmemcached/memcached.h>

class memcached : public storage_interface {
public:
    void init(const std::string hostname, int port) override;
    std::string get(const std::string &key) override;
    void put(const std::string &key, const std::string &value) override;
    std::vector<const std::string> get_batch(std::vector<const std::string> keys) override;
    void put_batch(std::vector<const std::string> keys, std::vector<const std::string> values) override;

private:
    std::vector<std::shared_ptr<memcached_st>> clients;
};

#endif //MEMCACHED_H
