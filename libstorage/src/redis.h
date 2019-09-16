//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef REDIS_H
#define REDIS_H

#include "storage_interface.h"
#include <queue>
#include <cpp_redis/cpp_redis>

class redis : public storage_interface
{
public:
    void init(const std::string hostname, int port) override;
    std::string get(const std::string &key) override;
    void put(const std::string &key, const std::string &value) override;
    std::vector<const std::string> get_batch(std::vector<const std::string> keys) override;
    void put_batch(std::vector<const std::string> keys, std::vector<const std::string> values) override;

private:
    std::vector<std::shared_ptr<cpp_redis::client>> clients;
};

#endif //REDIS_H
