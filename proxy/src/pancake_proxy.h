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
#include "queue.h"
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
    void insert_replicas(std::string key, int num_replicas, std::shared_ptr<std::vector<std::string>>> replicas,
                         std::shared_ptr<std::vector<double>>> frequencies,
                         std::shared_ptr<std::vector<double>>> fake_frequencies, bool is_dummy_key);
    void compute_fake_distribution();
    void prepare_for_swapping(std::string key, int r_new, int r_old, bool is_dummy_key,
                              std::shared_ptr<std::vector<std::string>> unassigned_labels,
                              std::shared_ptr<std::vector<std::pair<std::string, int>>> needs_labels)
    int perform_swapping(std::shared_ptr<std::vector<std::string>> unassigned_labels,
                         std::shared_ptr<std::vector<std::pair<std::string, int>>> needs_labels)
    void update_distribution();
    bool is_true_distribution();
    void execute_batch(const std::vector<operation> & operations, std::vector<std::string> & _returns,
                       std::vector<bool> & is_trues);
    void service_thread(int id);
    std::shared_ptr<std::vector<operation> create_security_batch(Operation op, queue op_queue);

    std::vector<storage_interface *> storage_interfaces_;
    std::queue <Operation> operation_queue_;
    update_cache update_cache_;
    update_cache  missing_new_replicas_;
    std::unordered_map<std::string, int> key_to_frequency_;
    std::unordered_map<std::string, int> new_key_to_frequency_;
    int frequency_sum_ = 0;
    int label_count_ = 0;
    int dummy_keys_ = 0;
    double p_max;
    std::string dummy_key_ = gen_random(16);
    std::unordered_map<std::string, int> key_to_number_of_replicas_;
    std::unordered_map<std::string, int> replica_to_label_;
    distribution fake_distribution_;
    distribution real_distribution_;
    encryption_engine encryption_engine_;
    std::string output_location_ = "log";
    std::string server_hostname_ = "127.0.0.1";
    int server_port_ = 50054;
    std::string workload_file_ = "keysets/Uniform/xtrace1";
    int security_batch_size_ = 4;
    int object_size_ = 1000;
    int key_size_ = 16;
    int server_count_ = 1;
    std::string server_type_ = "redis";
    int pthreads_ = 1;
    int storage_batch_size_ = 50;
    int core_ = 0;
    bool is_static_ = true;

};

#endif //PANCAKE_PROXY_H
