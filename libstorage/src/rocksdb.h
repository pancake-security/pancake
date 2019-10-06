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
    rocksdb(const std::string &host_name, int port);
    void add_server(const std::string &host_name, int port) override;
    std::string get(const std::string &key) override;
    void put(const std::string &key, const std::string &value) override;
    std::vector<std::string> get_batch(const std::vector<std::string> &keys) override;
    void put_batch(const std::vector<std::string> &keys, const std::vector<std::string> &values) override;

private:
    std::vector<ssdb::Client*> clients;
};


#endif //ROCKSDB
