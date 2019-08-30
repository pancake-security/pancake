//
// Created by Lloyd Brown on 8/30/19.
//

#ifndef PANCAKE_UPDATE_CACHE_H
#define PANCAKE_UPDATE_CACHE_H

#include <libcuckoo/cuckoohash_map.hh>

class update_cache {
public:
    std::string check_for_update(std::string &replica);
    std::string populate_replica_updates(std::string &key, std::string &val, int number_replicas);
    int size_in_bytes ();

private:
    cuckoohash_map<std::string, std::pair<std::string, std::vector<bool>>> map;
};

#endif //PANCAKE_CUCKOO_UPDATE_CACHE_H
