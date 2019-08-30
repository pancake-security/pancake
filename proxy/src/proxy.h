//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef PROXY_H
#define PROXY_H

#include <string>
#include <vector>

#include "storage_interface.h"

class Proxy {
public:

    virtual void init() = 0;
    virtual void run() = 0;
    virtual std::string get(const std::string &key, const std::string &value) = 0;
    virtual void put(const std::string &key, const std::string &value) = 0;
    virtual std::shared_ptr<std::vector<const std::string>> get_batch(std::shared_ptr<std::vector<const std::string>> keys) = 0;
    virtual void put_batch(std::shared_ptr<std::vector<const std::string>> keys, std::shared_ptr<std::vector<const std::string>> values) = 0;

private:
    std::vector<std::shared_ptr<storage_interface>> storage_interfaces;
};
#endif //PANCAKE_PROXY_H
