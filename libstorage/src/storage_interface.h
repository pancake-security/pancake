//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef STORAGE_INTERFACE_H
#define STORAGE_INTERFACE_H
#include <string>
#include <vector>

class storage_interface {
public:

    virtual void init(const std::string hostname, const std::string port) = 0;
    virtual std::string get(const std::string &key, const std::string &value) = 0;
    virtual void put(const std::string &key, const std::string &value) = 0;
    virtual std::shared_ptr<std::vector<const std::string>> get_batch(std::shared_ptr<std::vector<const std::string>> keys) = 0;
    virtual void put_batch(std::shared_ptr<std::vector<const std::string>> keys, std::shared_ptr<std::vector<const std::string>> values) = 0;

private:
    static std::vector<std::shared_ptr<storage_interface>> s_ifs;
};
#endif //STORAGE_INTERFACE_H
