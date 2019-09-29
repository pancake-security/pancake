//
// Created by Lloyd Brown on 9/18/19.
//

#include "update_cache.h"

    update_cache::update_cache(std::shared_ptr<std::vector <std::pair<std::string, int>>> keys_replicas_pairs) {
        for (auto key_replica_pair = keys_replicas_pairs->begin(); key_replica_pair != keys_replicas_pairs->end(); key_replica_pair++) {
            this->map.insert_or_assign(key_replica_pair->first, std::make_pair("", std::vector<bool>(key_replica_pair->second, false)));
        }
    };

    std::string update_cache::check_for_update(std::string &key, int replica_id) {
        std::string to_send = "";
        auto update_fn = [&](std::pair<std::string, std::vector<bool>> &cache_entry) {
            auto &replica_string = cache_entry.second;
            if (replica_string[replica_id - 1])
            {
                replica_string[replica_id - 1] = false;
                to_send = cache_entry.first;
            }
        };
        map.update_fn(key, update_fn);
        return to_send;
    };

    void update_cache::edit_bit_vector_size(std::string &key, int size) {
        if (map.contains(key))  {
            auto update_fn = [&](std::pair<std::string, std::vector<bool>> &cache_entry) {
                auto new_bit_vector = std::vector<bool>(size, true);
                for(int i = 0; i < cache_entry.second.size() && i < new_bit_vector.size(); i++)
                    new_bit_vector[i] = cache_entry.second[i];
                cache_entry.second = new_bit_vector;
            };
            map.update_fn(key, update_fn);
        }
        else {
            map.insert_or_assign(key, std::make_pair("", std::vector<bool>(size, false)));
        }
    };

    bool update_cache::populate_replica_updates(std::string &key, std::string &val){
        auto clear_fn = [&](std::pair<std::string, std::vector<bool>> &cache_entry) {
            cache_entry.first = val;
            cache_entry.second = std::vector<bool>(cache_entry.second.size(), true);
        };
        return map.update_fn(key, clear_fn);
    };

    int update_cache::size_in_bytes (){
        double count = 0.0;
        auto locked_table = map.lock_table();
        for(auto it = locked_table.begin(); it != locked_table.end(); it++){
            count += it->second.first.size();
            count += (it->second.second.size())/8.0;
        }
        return (int)count;
    };