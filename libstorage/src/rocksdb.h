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
    rocksdb(std::string hostname, int port);
    std::string get(std::string &key) override;
    void put(std::string &key, std::string &value) override;
    std::vector<std::string> get_batch(std::vector<std::string> * keys) override;
    void put_batch(std::vector<std::string> * keys, std::vector<std::string> * values) override;

private:
    std::vector<ssdb::Client*> clients;
};


#endif //ROCKSDB
