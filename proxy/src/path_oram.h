//
// Created by Lloyd Brown on 8/30/19.
//

#ifndef PANCAKE_PATH_ORAM_H
#define PANCAKE_PATH_ORAM_H

#include "proxy.h

class Path_ORAM : public Proxy {
public:
    void init() override;
    void run() override;
    std::string get(const std::string &key, const std::string &value) override;
    void put(const std::string &key, const std::string &value) override;
    std::shared_ptr<std::vector<const std::string>> get_batch(std::shared_ptr<std::vector<const std::string>> keys) override;
    virtual void put_batch(std::shared_ptr<std::vector<const std::string>> keys, std::shared_ptr<std::vector<const std::string>> values) override;
};
#endif //PANCAKE_PATH_ORAM_H
