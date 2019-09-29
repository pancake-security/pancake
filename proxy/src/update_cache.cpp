//
// Created by Lloyd Brown on 9/18/19.
//

#include "update_cache.h"

    update_cache::update_cache(std::vector <std::pair<std::string, int>> * keys_replicas_pairs) {
        for (auto key_replica_pair = keys_replicas_pairs->begin(); key_replica_pair != keys_replicas_pairs->end(); key_replica_pair++) {
            this->map.insert_or_assign(key_replica_pair->first, std::make_pair("", std::vector<bool>(key_replica_pair->second, false)));
        }
    };

    std::string update_cache::check_for_update(std::string key, int replica_id) {
        std::string to_send = "";
        auto update_fn = [&](std::pair<std::string, std::vector<bool>> &cache_entry) {
            auto &bit_vec = cache_entry.second;
            if (bit_vec[replica_id - 1])
            {
                bit_vec[replica_id - 1] = false;
                to_send = cache_entry.first;
            }
            if (hamming_weight(bit_vec) == 0)
                return true;
            return false;
        };
        map.erase_fn(key, update_fn);
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

    bool update_cache::populate_replica_updates(std::string &key, std::string &val, int num_replicas){
        return map.insert_or_assign(key, std::make_pair(val, std::vector<bool>(num_replicas, true)));
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

    int update_cache::hamming_weight(std::vector<bool> bit_vec){
        int count = 0;
        for (int i=0;i<bit_vec.size();i++){
            if (bit_vec[i] == true)
                count++;
        }
        return count;
    }

    int update_cache::num_leading_zeros(std::vector<bool> bit_vec){
        int i = 0;
        for (;i<bit_vec.size();i++){
            if (bit_vec[i] == true)
                break;
        }
        return i;
    }

    bool update_cache::mirror_update_cache(std::string key, std::string value, std::vector<bool> bit_vec){
        auto update_the_cache = [&](std::pair<std::string, std::vector<bool>> &cache_entry) {
            for(int i = num_leading_zeros(bit_vec); i < bit_vec.size(); i++)
                cache_entry.second[i] = true;
            return false;
        };
        map.uprase_fn(key, update_the_cache, std::make_pair(value, bit_vec));
        return true;
    }

    void update_cache::check_if_missing(std::string key, std::string value, update_cache * cache){
        auto check_missing = [&](std::pair<std::string, std::vector<bool>> & cache_entry) {
            auto bit_vec = cache_entry.second;
            return cache->mirror_update_cache(key, value, bit_vec);
        };
        map.erase_fn(key, check_missing);
    }

    int update_cache::sample_a_replica(std::string key, int num_replicas, bool access_is_real, int frequency, double p_max){
        int ret;
        auto sample_from_missing = [&](std::pair<std::string, std::vector<bool>> &cache_entry) {
            auto bit_vec = cache_entry.second;
            int r_initialized = num_leading_zeros(bit_vec);
            assert(r_initialized <= num_replicas);
            double p_initialized = (double)(r_initialized * p_max - frequency) / (double)(num_replicas * p_max - frequency);
            bool rep_exists = prob(p_initialized);
            if (rep_exists || access_is_real)
                ret = (rand()%r_initialized);
            else
                ret = (r_initialized+rand()%(num_replicas-r_initialized));
        };
        bool updated = map.find_fn(key, sample_from_missing);
        if (!updated){
            return (rand()%num_replicas);
        }
        return ret;
    }

    locked_table get_locked_table(){
        return map.lock_table();
    };