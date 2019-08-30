//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef PANCAKE_PROXY_H
#define PANCAKE_PROXY_H

#include <set>
#include <vector>

#include "proxy.h"
#include "operation.h"
#include "update_cache.h"
#include "cuckoo_update_cache.h"
#include "distribution.h"
#include "standard_distribution.h"
#include "encryption_engine.h"
#include "basic_crypto.h"
#include "storage_interface.h"
#include "redis.h"
#include "roksdb.h"
#include "memcached.h"


class Pancake_Proxy : public Proxy {
public:
    void init() override;
    void run() override;
    std::string get(const std::string &key, const std::string &value) override;
    void put(const std::string &key, const std::string &value) override;
    std::shared_ptr<std::vector<const std::string>> get_batch(std::shared_ptr<std::vector<const std::string>> keys) override;
    void put_batch(std::shared_ptr<std::vector<const std::string>> keys, std::shared_ptr<std::vector<const std::string>> values) override;

private:
    std::shared_ptr<std::vector<Operation> create_security_batch(Operation op);

    std::vector<std::shared_ptr<storage_interface>> storage_interfaces;
    std::queue <Operation> operation_queue;
    Update_Cache update_cache;
    std::unordered_map<std::string, int> key_to_frequency;
    Distribution fake_distribution;
    Distribution real_distribution;



    Encryption_Engine encryption_engine;

};

#endif //PANCAKE_PROXY_H
