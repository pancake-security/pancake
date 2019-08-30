//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef UPDATE_CACHE_H
#define UPDATE_CACHE_H

class Update_Cache {
public:
    virtual std::string check_for_update(std::string &replica) = 0;
    virtual std::string populate_replica_updates(std::string &key, std::string &val, int number_replicas) = 0;
    virtual int size_in_bytes () = 0;

private:
    cuckoohash_map<std::string, std::pair<std::string, std::vector<bool>>> map;
};
#endif //UPDATE_CACHE_H
