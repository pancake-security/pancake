//
// Created by Lloyd Brown on 8/30/19.
//

#ifndef PANCAKE_PATH_ORAM_H
#define PANCAKE_PATH_ORAM_H

#include <cryptopp/config.h>
#include "util.h"

#include "proxy.h

class path_oram : public proxy {
public:
    void init(const std::vector<std::string> &keys, const std::vector<std::string> &values, void ** args) override;
    void run() override;
    std::string get(const std::string &key, const std::string &value) override;
    void put(const std::string &key, const std::string &value) override;
    std::vector<std::string> get_batch(const std::vector<std::string> &keys) override;
    void put_batch(const std::vector<std::string> &keys, const std::vector<std::string> &values) override;

    std::string get(int queue_id, const std::string &key) override;
    void put(int queue_id, const std::string &key, const std::string &value) override;
    std::vector<std::string> get_batch(int queue_id, const std::vector<std::string> &keys) override;
    void put_batch(int queue_id, const std::vector<std::string> &keys, const std::vector<std::string> &values) override;

private:
    std::string get(const uint32_t & key);
    void put(const uint32_t & key, const std::string & value);
    void access(const char& op, const uint32_t& block_id, std::string& data);
    bool check(int x, int y, int l);
    void fetchAlongPath(const uint32_t& x, std::string* sbuffer, size_t& length);
    void loadAlongPath(const uint32_t& x, const std::string* sbuffer, const size_t& length);

    std::unordered_map<uint32_t, std::string> stash_;
    uint32_t *pos_map_;
    std::vector< std::pair<uint32_t, std::string> > insert_buffer_;

    byte* key_;
    std::string* sbuffer_;
    uint32_t n_blocks_;
    uint32_t height_;
    std::vector<std::shared_ptr<storage_interface>> storage_interfaces_;
};
#endif //PANCAKE_PATH_ORAM_H
