//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef CLIENT_H
#define CLIENT_H
#include <string>
#include <vector>

class client {
public:

    virtual void init(const std::string host_name, int port) = 0;
    virtual std::string get(const std::string &key) = 0;
    virtual void put(const std::string &key, const std::string &value) = 0;
    virtual std::vector<std::string> get_batch(const std::vector<std::string> keys) = 0;
    virtual void put_batch(const std::vector<std::string> keys, const std::vector<std::string> values) = 0;

};
#endif //CLIENT_H
