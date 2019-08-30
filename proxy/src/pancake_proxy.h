//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef PANCAKE_PROXY_H
#define PANCAKE_PROXY_H

#include <set>
#include <vector>

#include "proxy.h"
#include "operation.h"
#include "update_cache.h"
#include "distribution.h"
#include "encryption_engine.h"
#include "storage_interface.h"
#include "redis.h"
#include "roksdb.h"
#include "memcached.h"


class pancake_proxy : public proxy {
public:
    void init() override;
    void run() override;
    std::string get(const std::string &key, const std::string &value) override;
    void put(const std::string &key, const std::string &value) override;
    std::shared_ptr<std::vector<const std::string>> get_batch(std::shared_ptr<std::vector<const std::string>> keys) override;
    void put_batch(std::shared_ptr<std::vector<const std::string>> keys, std::shared_ptr<std::vector<const std::string>> values) override;

private:
    void usage();
    std::shared_ptr<std::vector<operation> create_security_batch(Operation op);
    void create_replicas();
    void load_frequencies_from_trace(const std::string &trace_location);
    void compute_fake_distribution();
    void update_distribution();
    bool is_true_distribution();
    void execute_batch(const std::vector<operation> & operations, std::vector<std::string> & _returns,
                       std::vector<bool> & is_trues);
    void service_thread(int id);

    std::vector<std::shared_ptr<storage_interface>> storage_interfaces_;
    std::queue <Operation> operation_queue_;
    update_cache update_cache_;
    update_cache  missing_new_replicas_;
    std::unordered_map<std::string, int> key_to_frequency_;
    std::unordered_map<std::string, int> key_to_number_of_replicas_;
    std::unordered_map<std::string, int> replica_to_label_;
    distribution fake_distribution_;
    distribution real_distribution_;
    encryption_engine encryption_engine_;
    bool is_static_;

};

#endif //PANCAKE_PROXY_H
