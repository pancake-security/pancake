//
// Created by Lloyd Brown on 9/24/19.
//

#include "pancake_proxy.h"

void pancake_proxy::init(const std::vector<std::string> &keys, const std::vector<std::string> &values, void ** args){
    real_distribution_ = (distribution *)args[0];
    alpha_ = *((double *)args[1]);
    delta_ = *((double *)args[2]);
    encryption_engine_ = encryption_engine();
    if (server_type_ == "redis") {
        storage_interface_ = new redis(server_host_name_, server_port_);
        cpp_redis::network::set_default_nb_workers(std::min(10, p_threads_));
    }
    else if (server_type_ == "rocksdb") {
        storage_interface_ = new rocksdb(server_host_name_, server_port_);
    }
    //else if (server_type_ == "memcached")
    //    storage_interface_ new memcached(server_host_name_, server_port_+i);
    for (int i = 1; i < server_count_; i++) {
        storage_interface_->add_server(server_host_name_, server_port_+i);
    }
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);;
    for (int i = 0; i < num_cores; i++) {
        operation_queues_.emplace_back();
    }

    create_replicas();

    std::vector<std::thread> threads;
    for (int i = 0; i < num_cores; i++){
        threads_.push_back(std::thread(&pancake_proxy::service_thread, this, i));
    }
    threads_.push_back(std::thread(&pancake_proxy::distribution_thread, this));
}

void pancake_proxy::insert_replicas(const std::string &key, int num_replicas){
    std::string value_cipher = encryption_engine_.encrypt(rand_str(object_size_));
    std::vector<std::string> labels;
    for (int i = 0; i < num_replicas; i++){
        std::string replica = key+std::to_string(i);
        std::string replica_cipher = std::to_string(label_count_);
        replica_to_label_[replica] = label_count_;
        labels.push_back(std::to_string(label_count_));
        if (labels.size() >= 50){
            storage_interface_->put_batch(labels, std::vector<std::string>(labels.size(), value_cipher));
            labels.clear();
        }
        label_count_++;
    }
    if (!labels.empty()){
        storage_interface_->put_batch(labels, std::vector<std::string>(labels.size(), value_cipher));
    }
}

void pancake_proxy::create_replicas() {
    //int n = key_to_frequency_.size();
    //int n_prime = 2*n;
    int keys_created = 0;
    std::vector<std::pair<std::string, int>> update_cache_pairs;
    
    auto keys = real_distribution_->get_items();
    auto probabilities = real_distribution_->get_probabilities();

    std::vector<double> fake_probabilities;
    int index = 0;
    for (const auto &key: keys) {
        double pi = probabilities[index];
        key_to_frequency_[key] = pi;
        int r_i = ceil(pi/alpha_);
        double pi_f = r_i * (alpha_-pi/r_i)/(1/delta_ - 1);
        fake_probabilities.push_back(pi_f);
        key_to_number_of_replicas_[key] = r_i;
        insert_replicas(key, r_i);
        update_cache_pairs.push_back(std::make_pair(key, r_i));
        keys_created += r_i;
        index++;
    }
    int dummy_r_i = 0 > 2*keys.size()-keys_created ? 0 : 2*keys.size()-keys_created;
    dummy_keys_ = dummy_r_i;
    if(dummy_r_i != 0) {
        fake_probabilities.push_back((alpha_/(1/delta_-1))*dummy_r_i);
        key_to_number_of_replicas_[dummy_key_] = dummy_r_i;
        insert_replicas(dummy_key_, dummy_r_i);
        update_cache_pairs.push_back(std::make_pair(dummy_key_, dummy_r_i));
    }
    update_cache_ = update_cache(update_cache_pairs);
    auto full_keys = keys;
    full_keys.push_back(dummy_key_);
    fake_distribution_ = new distribution(full_keys, fake_probabilities);
};

void pancake_proxy::recompute_fake_distribution(const distribution &new_distribution) {
    auto new_keys = new_distribution.get_items();
    std::vector<double> new_probabilities = new_distribution.get_probabilities();
    std::vector<std::string> keys;
    std::vector<double> fake_probabilities;
    int n = new_keys.size();
    double curr_max_real = (double)1/n;
    auto locked = missing_new_replicas_.get_locked_table();
    for (auto iterator = locked.begin(); iterator != locked.end(); iterator++){
        std::vector<bool> bit_vec = iterator->second.second;
        std::string key = iterator->first;
        int r = key_to_number_of_replicas_[key];
        assert(bit_vec.size() == r);
        double prob = new_key_to_frequency_[key];
        curr_max_real = curr_max_real > prob ? curr_max_real : prob;
    }
    locked.unlock();
    p_max_ = curr_max_real;
    int num_real_replicas = 0;
    int index = 0;
    for (const auto &new_key: new_keys){
        int r = key_to_number_of_replicas_[new_key];
        num_real_replicas += r;
        fake_probabilities.push_back((r*p_max_-new_probabilities[index]));
        index++;
    }
    keys = new_keys;
    keys.push_back(dummy_key_);
    fake_probabilities.push_back(dummy_keys_*p_max_);
    fake_distribution_ = new distribution(keys, fake_probabilities);
};

void pancake_proxy::prepare_for_swapping(const std::string &key, int r_new, int r_old,
                                         std::vector<std::string> &unassigned_labels,
                                         std::vector<std::pair<std::string, int>> &needs_labels){
    if(r_new > r_old){
        for (int i = r_old; i < r_new; i++)
            needs_labels.push_back(std::make_pair(key, i));
        if (key != dummy_key_)
            missing_new_replicas_.edit_bit_vector_size(key, r_new);
    }
    else if(r_new < r_old) {
        for (int i = r_new; i < r_old; i++) {
            std::string label;
            assert(replica_to_label_.find(key+std::to_string(i)) != replica_to_label_.end());
            label = replica_to_label_[key+std::to_string(i)];
            replica_to_label_.erase(key+std::to_string(i));
            unassigned_labels.push_back(label);
        }
    }
}

int pancake_proxy::perform_swapping(const std::vector<std::string> &unassigned_labels,
                                    const std::vector<std::pair<std::string, int>> &needs_labels){
    for (int i = 0; i < needs_labels.size(); i++){
        auto key_rep_pair = needs_labels[i];
        std::string replica = key_rep_pair.first + std::to_string(key_rep_pair.second);
        auto label = unassigned_labels[i];
        replica_to_label_[replica] = std::stoi(label);
    }
    return needs_labels.size();
}

void pancake_proxy::update_distribution(const distribution &new_distribution) {
    auto update_s = std::chrono::high_resolution_clock::now();
    auto update_e = std::chrono::high_resolution_clock::now();
    std::vector<std::string> unassigned_labels;
    std::vector<std::pair<std::string, int>> needs_labels;
    int new_num_real_replicas = 0;
    auto new_keys = new_distribution.get_items();
    std::vector<double> new_probabilities = new_distribution.get_probabilities();
    int n = new_keys.size();
    int num_swaps = 0;
    int index = 0;
    for (const auto &new_key: new_keys) {
        //auto key = iterator->first;
        double pi = new_probabilities[index];
        new_key_to_frequency_[new_key] = new_probabilities[index];
        int r_old = (key_to_number_of_replicas_.find(new_key) != key_to_number_of_replicas_.end()) ? key_to_number_of_replicas_[new_key] : 0;
        int r_new = ceil(pi/alpha_);
        key_to_number_of_replicas_[new_key] = r_new;
        assert(r_new > 0);
        new_num_real_replicas += r_new;
        prepare_for_swapping(new_key, r_new, r_old, unassigned_labels, needs_labels);
        index++;
    }
    assert (new_num_real_replicas < 2*n);
    int r_old = dummy_keys_;
    dummy_keys_ = 2*n - new_num_real_replicas;
    prepare_for_swapping(dummy_key_, dummy_keys_, r_old, unassigned_labels, needs_labels);
    recompute_fake_distribution(new_distribution);
    *real_distribution_ = new_distribution;
    key_to_frequency_ = new_key_to_frequency_;
    assert(needs_labels.size() == unassigned_labels.size());
    num_swaps += perform_swapping(unassigned_labels, needs_labels);
    update_e = std::chrono::high_resolution_clock::now();
    auto elapsed = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(update_e - update_s).count());
    std::cout << "Time to update dist: " << elapsed << " Num swaps: " << num_swaps << std::endl;
};

bool pancake_proxy::is_true_distribution() {
    return rand()/(RAND_MAX+1.0) < prob(1.0*(2.0 * p_max_ * key_to_frequency_.size()));
};

void pancake_proxy::create_security_batch(std::shared_ptr<queue <std::pair<operation, void *>>> op_queue,
                                          std::vector<operation> &storage_batch,
                                          std::vector<void *> &is_trues) {
    for (int i = 0; i < security_batch_size_; i++) {
        if (is_true_distribution()) {
            if (op_queue->size() == 0) {
                struct operation operat;
                operat.key = real_distribution_->sample();
                operat.value = "";
                storage_batch.push_back(operat);
                is_trues.push_back(nullptr);
            } else {
                auto operation_return_pair = op_queue->pop();
                storage_batch.push_back(operation_return_pair.first);
                is_trues.push_back((void *)operation_return_pair.second);
            }
        } else {
            struct operation operat;
            operat.key = fake_distribution_->sample();
            operat.value = "";
            storage_batch.push_back(operat);
            is_trues.push_back(nullptr);
        }
    }
};

void pancake_proxy::execute_batch(const std::vector<operation> &operations, std::vector<std::string> &_returns,
                                  std::vector<void *> &is_trues, storage_interface &storage_interface) {

    // Store which are real queries so we can return the values
    std::vector<int> replica_ids;
    std::vector<std::string> labels;

    std::vector<std::string> storage_keys;
    for(int i = 0; i < operations.size(); i++){
        std::string key = operations[i].key;
        replica_ids.push_back(missing_new_replicas_.sample_a_replica(key, key_to_number_of_replicas_[key], is_trues[i], key_to_frequency_[key], p_max_));
        labels.push_back(std::to_string(replica_to_label_[operations[i].key+std::to_string(replica_ids[i])]));
        storage_keys.push_back(labels[i]);
    }
    auto responses = storage_interface.get_batch(storage_keys);
    std::vector<std::string> storage_values;
    auto is_trues_iterator = is_trues.end();
    is_trues_iterator--;
    for(int i = operations.size(); i >= 0; i--){
        auto cipher = responses[i];
        auto plaintext = encryption_engine_.decrypt(cipher);
        auto plaintext_update = update_cache_.check_for_update(operations[i].key, replica_ids[i]);
        plaintext = plaintext_update == "" ? plaintext : plaintext_update;
        missing_new_replicas_.check_if_missing(operations[i].key, plaintext, update_cache_);
        if (is_trues[i] == nullptr) {
            _returns.push_back(plaintext);
            is_trues_iterator--;
        }
        else {
            auto old_iterator = is_trues_iterator;
            is_trues_iterator--;
            is_trues.erase(old_iterator);
        }

        storage_values.push_back(encryption_engine_.encrypt(plaintext));
    }
    storage_interface.put_batch(storage_keys, storage_values);
}

std::string pancake_proxy::get(const std::string &key) {
    return get(rand_uint32(0, RAND_MAX), key);
};

void pancake_proxy::put(const std::string &key, const std::string &value) {
    return put(rand_uint32(0, RAND_MAX), key, value);
};

std::vector<std::string> pancake_proxy::get_batch(const std::vector<std::string> &keys) {
    return get_batch(rand_uint32(0, RAND_MAX), keys);
};

void pancake_proxy::put_batch(const std::vector<std::string> &keys, const std::vector<std::string> &values) {
    return put_batch(rand_uint32(0, RAND_MAX), keys, values);
};

std::string pancake_proxy::get(int queue_id, const std::string &key) {
    std::string _return;
    return get(queue_id, key, _return);
};

void pancake_proxy::put(int queue_id, const std::string &key, const std::string &value) {
    std::string _return;
    return put(queue_id, key, value, _return);
};
std::vector<std::string> pancake_proxy::get_batch(int queue_id, const std::vector<std::string> &keys) {
    std::vector<std::string> _return;
    return get_batch(queue_id, keys, _return);
};

void pancake_proxy::put_batch(int queue_id, const std::vector<std::string> &keys, const std::vector<std::string> &values) {
    std::string _return;
    return put_batch(queue_id, keys, values, _return);
};

std::string pancake_proxy::get(int queue_id, const std::string &key, std::string& _return) {
    struct operation operat;
    operat.key = key;
    operat.value = "";
    operation_queues_[queue_id % operation_queues_.size()]->push(std::make_pair(operat, (void *)&_return));
    return _return;
};

void pancake_proxy::put(int queue_id, const std::string &key, const std::string &value, std::string& _return) {
    struct operation operat;
    operat.key = key;
    operat.value = value;
    operation_queues_[queue_id % operation_queues_.size()]->push(std::make_pair(operat, (void *)&_return));
};
std::vector<std::string> pancake_proxy::get_batch(int queue_id, const std::vector<std::string> &keys, std::vector<std::string> & _return) {
    for (const auto &key: keys) {
        struct operation operat;
        operat.key = key;
        operat.value = "";
        operation_queues_[queue_id % operation_queues_.size()]->push(std::make_pair(operat, (void *)&_return));
    }
    return _return;
};

void pancake_proxy::put_batch(int queue_id, const std::vector<std::string> &keys, const std::vector<std::string> &values, std::string& _return) {
    int i = 0;
    for (const auto &key: keys) {
        struct operation operat;
        operat.key = key;
        operat.value = values[i];
        operation_queues_[queue_id % operation_queues_.size()]->push(std::make_pair(operat, (void *)&_return));
        i++;
    }
};

void pancake_proxy::service_thread(int id){
    auto operation_queue = operation_queues_[id];
    storage_interface *storage_interface;
    if (server_type_ == "redis") {
        storage_interface = new redis(server_host_name_, server_port_);
    }
    else if (server_type_ == "rocksdb") {
        storage_interface = new rocksdb(server_host_name_, server_port_);
    }
    //else if (server_type_ == "memcached")
    //    storage_interface_ new memcached(server_host_name_, server_port_+i);
    for (int i = 1; i < server_count_; i++) {
        storage_interface->add_server(server_host_name_, server_port_+i);
    }
    std::vector<void *> is_trues;
    std::vector <std::string> _returns;
    while (!distribution_changed()) {
        std::vector <operation> storage_batch;
        while (storage_batch.size() < storage_batch_size_) {
            create_security_batch(operation_queue, storage_batch, is_trues);
        }
        execute_batch(storage_batch, _returns, is_trues, *storage_interface);
        if (!is_trues.empty()) {
            auto value_iterator = _returns.begin();
            auto return_iterator = is_trues.begin();
            void *previous_return = *return_iterator;

            auto erase_value = _returns.begin();
            auto erase_return = is_trues.begin();

            std::vector<std::string> return_values;
            bool should_erase = false;
            for (; return_iterator != is_trues.end(); return_iterator++, value_iterator++) {
                auto current_return = *return_iterator;
                // Return current values
                if (current_return != previous_return) {
                    if (return_values.size() > 1) {
                        auto real_return = (std::vector<std::string> *) previous_return;
                        *real_return = std::move(return_values);
                    }
                    else {
                        auto real_return = (std::string *) previous_return;
                        *real_return = return_values[0];
                    }
                    return_values.clear();
                    should_erase = true;
                    auto erase_value = value_iterator;
                    auto erase_return = return_iterator;
                }
                return_values.push_back(*value_iterator);
                previous_return = current_return;
            }
            if (should_erase){
                _returns.erase(_returns.begin(), erase_value);
                is_trues.erase(is_trues.begin(), erase_return);
            }
        }
    }
};

bool pancake_proxy::distribution_changed(){
    return false;
}

distribution pancake_proxy::load_new_distribution(){
    distribution dist;
    return dist;
}

void pancake_proxy::distribution_thread() {
    // TODO: Pause other threads?
    if (distribution_changed()){
        update_distribution(load_new_distribution());
        sleep(1);
    }
}

void pancake_proxy::close() {
    for (int i = 0; i < threads_.size(); i++)
        threads_[i].join();
};
