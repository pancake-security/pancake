//
// Created by Lloyd Brown on 8/30/19.
//

#ifndef PANCAKE_UPDATE_CACHE_H
#define PANCAKE_UPDATE_CACHE_H

#include <libcuckoo/cuckoohash_map.hh>
#include "util.h"


class update_cache {
public:
    update_cache();
    update_cache(std::vector <std::pair<std::string, int>> * keys_replicas_pairs);
    int hamming_weight(std::vector<bool> bit_vec);
    int num_leading_zeros(std::vector<bool> bit_vec);
    std::string check_for_update(std::string key, int replica_id);
    bool populate_replica_updates(std::string &key, std::string &val, int num_replicas);
    void edit_bit_vector_size(std::string &key, int size);
    bool mirror_update_cache(std::string key, std::string value, std::vector<bool> bit_vec);
    void check_if_missing(std::string key, std::string value, update_cache * cache);
    int sample_a_replica(std::string key, int num_replicas, bool access_is_real, int frequency, double p_max);
    int size_in_bytes ();
    locked_table get_locked_table();

private:
    cuckoohash_map<std::string, std::pair<std::string, std::vector<bool>>> map;
};

#endif //PANCAKE_CUCKOO_UPDATE_CACHE_H
