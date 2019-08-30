//
// Created by Lloyd Brown on 8/30/19.
//

#ifndef PANCAKE_CUCKOO_UPDATE_CACHE_H
#define PANCAKE_CUCKOO_UPDATE_CACHE_H

#include <libcuckoo/cuckoohash_map.hh>

#include "update_cache.h"

class Cuckoo_Update_Cache : public Update_Cache{
    std::string check_for_update(std::string &replica) override;
    std::string populate_replica_updates(std::string &key, std::string &val, int number_replicas) override;
    int size_in_bytes () override;
};

#endif //PANCAKE_CUCKOO_UPDATE_CACHE_H
