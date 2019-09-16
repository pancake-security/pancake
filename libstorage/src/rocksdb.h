//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef ROCKSDB_H
#define ROCKSDB_H

#include "storage_interface.h"
#include "SSDB.h"

#include <iostream>

class rocksdb : public storage_interface {
public:
    void init(const std::string hostname, int port) override;
    std::string get(const std::string &key) override;
    void put(const std::string &key, const std::string &value) override;
    std::vector<const std::string> get_batch(std::vector<const std::string> keys) override;
    void put_batch(std::vector<const std::string> keys, std::vector<const std::string> values) override;

private:
    std::vector<ssdb::Client*> clients;
};


#endif //ROCKSDB
