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
    redis(std::string hostname, int port);
    std::string get(std::string &key) override;
    void put(std::string &key,  std::string &value) override;
    std::vector< std::string> get_batch(std::vector<std::string> * keys) override;
    void put_batch(std::vector< std::string> * keys, std::vector<std::string> * values) override;

private:
    std::vector<std::shared_ptr<cpp_redis::client>> clients;
};

#endif //REDIS_H
