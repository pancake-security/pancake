//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef PANCAKE_PROXY_H
#define PANCAKE_PROXY_H

#include <unordered_map>
#include <vector>
#include <unistd.h>
#include <fstream>
#include <algorithm>

#include "proxy.h"
#include "operation.h"
#include "update_cache.h"
#include "distribution.h"
#include "util.h"
#include "encryption_engine.h"
#include "queue.h"
#include "storage_interface.h"
#include "redis.h"
#include "rocksdb.h"
//#include "memcached.h"


class pancake_proxy : public proxy {
public:

    void init(const std::vector<std::string> &keys, const std::vector<std::string> &values, void ** args) override;
    void run() override;
    std::string get(const std::string &key) override;
    void put(const std::string &key, const std::string &value) override;
    std::vector<std::string> get_batch(const std::vector<std::string> &keys) override;
    void put_batch(const std::vector<std::string> &keys, const std::vector<std::string> &values) override;

    std::string get(int queue_id, const std::string &key) override;
    void put(int queue_id, const std::string &key, const std::string &value) override;
    std::vector<std::string> get_batch(int queue_id, const std::vector<std::string> &keys) override;
    void put_batch(int queue_id, const std::vector<std::string> &keys, const std::vector<std::string> &values) override;

    std::string get(int queue_id, const std::string &key, std::string& _return);
    void put(int queue_id, const std::string &key, const std::string &value, std::string& _return);
    std::vector<std::string> get_batch(int queue_id, const std::vector<std::string> &keys, std::vector<std::string> & _return);
    void put_batch(int queue_id, const std::vector<std::string> &keys, const std::vector<std::string> &values, std::string& _return);

    std::string output_location_ = "log";
    std::string server_host_name_ = "127.0.0.1";
    int server_port_ = 50054;
    std::string workload_file_ = "keysets/Uniform/xtrace1";
    int security_batch_size_ = 4;
    int object_size_ = 1000;
    int key_size_ = 16;
    int server_count_ = 1;
    std::string server_type_ = "redis";
    int p_threads_ = 1;
    int storage_batch_size_ = 50;
    int core_ = 0;
    bool is_static_ = true;

private:
    void create_security_batch(std::shared_ptr<queue <std::pair<operation, void *>>> op_queue,
                               std::vector<operation> &storage_batch,
                               std::vector<void *> &is_trues);
    void create_replicas();
    void insert_replicas(const std::string &key, int num_replicas);
    void recompute_fake_distribution(const distribution &new_distribution);
    void prepare_for_swapping(const std::string &key, int r_new, int r_old,
                              std::vector<std::string> &unassigned_labels,
                              std::vector<std::pair<std::string, int>> &needs_labels);
    int perform_swapping(const std::vector<std::string> &unassigned_labels,
                         const std::vector<std::pair<std::string, int>> &needs_labels);
    void update_distribution(const distribution &new_distribution);
    bool distribution_changed();
    bool is_true_distribution();
    void execute_batch(const std::vector<operation> &operations, std::vector<std::string> &_returns,
                       std::vector<void *> &is_trues, storage_interface &storage_interface);
    void service_thread(int id);

    storage_interface * storage_interface_;
    std::vector<std::shared_ptr<queue<std::pair<operation, void *>>>> operation_queues_;
    update_cache update_cache_;
    update_cache  missing_new_replicas_;
    std::unordered_map<std::string, int> key_to_frequency_;
    std::unordered_map<std::string, int> new_key_to_frequency_;
    int frequency_sum_ = 0;
    int label_count_ = 0;
    int dummy_keys_ = 0;
    double p_max_;
    double alpha_;
    double delta_;
    std::string dummy_key_ = rand_str(16);
    std::unordered_map<std::string, int> key_to_number_of_replicas_;
    std::unordered_map<std::string, int> replica_to_label_;
    distribution * fake_distribution_;
    distribution * real_distribution_;
    encryption_engine encryption_engine_;
};

#endif //PANCAKE_PROXY_H
