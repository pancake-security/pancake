//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef STORAGE_INTERFACE_H
#define STORAGE_INTERFACE_H
#include <string>
#include <vector>

class storage_interface {
public:
    virtual std::string get(const std::string &key) = 0;
    virtual void put(const std::string &key, const std::string &value) = 0;
    virtual std::vector<const std::string> get_batch(std::vector<const std::string> keys) = 0;
    virtual void put_batch(std::vector<const std::string> keys, std::vector<const std::string> values) = 0;
};
#endif //STORAGE_INTERFACE_H
