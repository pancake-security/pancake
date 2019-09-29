//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef STORAGE_INTERFACE_H
#define STORAGE_INTERFACE_H
#include <string>
#include <vector>

class storage_interface {
public:
    virtual std::string get(std::string &key) = 0;
    virtual void put(std::string &key, std::string &value) = 0;
    virtual std::vector<std::string> get_batch(std::vector<std::string> * keys) = 0;
    virtual void put_batch(std::vector<std::string> * keys, std::vector<std::string> * values) = 0;
};
#endif //STORAGE_INTERFACE_H
