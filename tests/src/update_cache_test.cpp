//
// Created by Lloyd Brown on 9/17/19.
//

#include "update_cache.h"
#include "assert.h"

#include <iostream>

void run_basic_test(){
    std::string key = "1";
    std::string val = "10";
    std::vector<std::pair<std::string, int>> pairs;
    pairs.push_back(std::make_pair("1", 8));
    update_cache cache = update_cache(pairs);

     std::cout << "Check basic sizing" << std::endl;
    //std::cout << "Cache size in bytes: " << cache.size_in_bytes() << std::endl;
    assert(cache.size_in_bytes() == 1);

    std::cout << "Check basic insertion and update logic" << std::endl;
    assert(cache.check_for_update(key, 1) == "");

    std::cout << "Check basic write update" << std::endl;
    assert(cache.populate_replica_updates(key, val, 8));
    assert(cache.check_for_update(key, 1) == val);
    //std::cout << "Cache size in bytes: " << cache.size_in_bytes() << std::endl;
    assert(cache.size_in_bytes() == 3);

    std::cout << "Ensure bit_vector size change does not remove old values" << std::endl;
    cache.edit_bit_vector_size(key, 35);
    assert(cache.check_for_update(key, 4) == val);
    std::cout << "Ensure newly instantiated replicas need to be updated" << std::endl;
    assert(cache.check_for_update(key, 10) == val);

    std::cout << "Performing update cache stress test" << std::endl;

    std::vector<std::pair<std::string, int>> big_pairs;
    // Use this range because update cache requires padded keys
    for (int i = 1000000; i < 2000000; i++){
        big_pairs.push_back(std::make_pair(std::to_string(i), 64));
    }

    update_cache big_cache = update_cache(big_pairs);

    int current_size = 1000000*8;
    for (int i = 1000000; i < 2000000; i++){
        std::string new_key = std::to_string(i);
        assert(big_cache.check_for_update(new_key, 1) == "");\
        assert(big_cache.populate_replica_updates(new_key, val, 64));
        assert(big_cache.check_for_update(new_key, 1) == val);
        current_size += 2;
    }
    assert(big_cache.size_in_bytes() == current_size);

    std::cout << "Passed all tests :)" << std::endl;
}

int main(int argc, char* argv[]) {
    run_basic_test();
}