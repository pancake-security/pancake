//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef ROCKSDB_H
#define ROCKSDB_H

#include "storage_interface.h"
#include "SSDB.h"

class Rocksdb : public storage_interface {
public:
    void init(const std::string hostname, const std::string port) override;
    std::string get(const std::string &key, const std::string &value) override;
    void put(const std::string &key, const std::string &value) override;
    std::shared_ptr<std::vector<const std::string>> get_batch(std::shared_ptr<std::vector<const std::string>> keys) override;
    void put_batch(std::shared_ptr<std::vector<const std::string>> keys, std::shared_ptr<std::vector<const std::string>> values) override;

private:
    std::shared_ptr<ssdb::Client> client;
};


#endif //ROCKSDB
