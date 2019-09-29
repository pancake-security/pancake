//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef PROXY_H
#define PROXY_H

#include <string>
#include <vector>

class proxy {
public:

    virtual void init() = 0;
    virtual void run() = 0;
    virtual std::string get(const std::string &key, const std::string &value) = 0;
    virtual void put(const std::string &key, const std::string &value) = 0;
    virtual std::vector<const std::string> get_batch(std::vector<const std::string> * keys) = 0;
    virtual void put_batch(std::vector<const std::string> * keys, std::vector<const std::string> * values) = 0;

};
#endif //PANCAKE_PROXY_H
