//
// Created by Lloyd Brown on 8/30/19.
//

#ifndef PANCAKE_UPDATE_CACHE_H
#define PANCAKE_UPDATE_CACHE_H

#include <libcuckoo/cuckoohash_map.hh>

class update_cache {
public:
    update_cache(std::shared_ptr<std::vector <std::pair<std::string, int>>> keys_replicas_pairs);
    std::string check_for_update(std::string& key, int replica_id);
    bool populate_replica_updates(std::string &key, std::string &val);
    void edit_bit_vector_size(std::string &key, int size);

    int size_in_bytes ();

private:
    cuckoohash_map<std::string, std::pair<std::string, std::vector<bool>>> map;
};

#endif //PANCAKE_CUCKOO_UPDATE_CACHE_H
