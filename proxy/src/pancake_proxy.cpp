//
// Created by Lloyd Brown on 9/24/19.
//

#include "pancake_proxy.h"

pancake_proxy::pancake_proxy(){
    load_frequencies_from_trace();
    this.encryption_engine_ = encryption_engine();
    create_replicas();

    update_cache update_cache_ = update_cache();
    update_cache  missing_new_replicas_ = update_cache();
    this.fake_distribution_ = distribution();
    this.real_distribution_ = distribution();
}

void pancake_proxy::load_frequencies_from_trace(const std::string &trace_location, std::map<std::string, int> * key_to_frequency) {
    frequency_sum_ = 0;
    std::string op, key;
    std::ifstream in_workload_file;
    in_workload_file.open(trace_location);
    if(!in_workload_file){
        std::perror("Unable to find workload file");
    }
    std::string line;
    while (std::getline(in_workload_file, line)) {
        key = line.substr(line.find(" ")+1);
        if (key.find(" ") != -1)
            key = key.substr(0,key.find(" "));
        assert (key != "PUT");
        assert (key != "GET");
        assert (key.length() < object_size);
        if (key_to_frequency.count(key) == 0){
            key_to_frequency[key] = 1;
            frequency_sum_ += 1;
        }
        else {
            key_to_frequency[key] += 1;
            frequency_sum_ += 1;
        }
    }
    in_workload_file.close();
};


void pancake_proxy::insert_replicas(std::string key, int num_replicas, std::shared_ptr<std::vector<std::string>>> replicas,
                                    std::shared_ptr<std::vector<double>>> frequencies,
                                    std::shared_ptr<std::vector<double>>> fake_frequencies, bool is_dummy_key){
    std::string value_cipher = encryption_engine_.encrypt(rand_str(object_size));
    for (int i = 0; i < num_replicas; i++){
        std::string replica = key+std::to_string(i);
        std::string replica_cipher = encryption_engine.hmackey(std::to_string(label_count_));
        replica_to_label_[replica] = std::to_string(label_count_);
        replicas->push_back(replica);
        double pi = key_to_frequency_[key]/(double)frequencysum;
        double r = ceil(n*pi)
        fake_frequencies->push_back()
        if (!is_dummy_key){
            frequencies->push_back(key_to_frequency_[key]/(double)num_replicas);
        }
        storage_interfaces_[label_count_ % s_ifs.size()]->write(std::to_string(label_count_), value_cipher);
        label_count_++;
    }
}

void pancake_proxy::create_replicas() {
    int n = key_to_frequency_.size();
    int n_prime = 2*n;
    int keys_created = 0;
    auto update_cache_pairs = std::make_shared<std::vector<std::pair<std::string, int>>>();
    c
    for (auto iterator = key_to_frequency_.begin(); iterator != key_to_frequency_.end(); iterator++) {
        std::string key = iterator->first;
        int r_i = ceil((key_to_frequency_[key]/(double)frequency_sum_)*n);
        key_to_replica_num_[key] = r_i;
        insert_replicas(key, r_i, replicas, frequencies, fake_frequencies, false);
        update_cache_pairs->push_back(std::make_pair(key, r_i));
        keys_created += r_i;
    }
    int dummy_r_i = MAX(0,n_prime-keys_created);
    dummy_keys_ = dummy_r_i;
    if(dummy_r_i != 0) {
        key_to_replica_num_[dummy_key_] = dummy_r_i;
        insert_replicas(dummy_key_, dummy_r_i, replicas, frequencies, fake_frequencies, true);
        update_cache_pairs->push_back(std::make_pair(dummy_key_, dummy_r_i));
    }
    update_cache_ = update_cache(update_cache_pairs);
    real_distribution = distribution(replicas, frequencies);
    fake_distribution = distribution(replicas, fake_frequencies);
};

void pancake_proxy::compute_fake_distribution() {
    auto replicas = std::make_shared<std::vector<std::string>>();
    auto frequencies = std::make_shared<std::vector<int>>();
    auto fake_frequencies = std::make_shared<std::vector<int>>();
    int n = key_to_frequency_.size();
    double curr_max_real = (double)1/n;
    auto locked = missing_new_replicas.lock_table();
    for (auto iterator = locked.begin(); iterator != locked.end(); iterator++){
        std::string bit_vec = iterator->second;
        std::string key = iterator->first;
        int r = get_num_replicas(key);
        assert(bit_vec.length() == r);
        curr_max_real = MAX(new_key_to_frequency_[key]/(double)frequency_sum_), curr_max_real);
    }
    locked.unlock();
    p_max = curr_max_real;
    int num_real_replicas = 0;
    for (auto iterator = key_to_frequency_->begin(); iterator != key_to_frequency_->end(); iterator++){
        std::string key = iterator->first;
        int r = get_num_replicas(key);
        num_real_replicas += r;
        for (int i = 0; i < r; i++){
            replicas->push_back(key+std::to_string(i));
            fake_frequencies->push_back(((r*p_max-new_key_to_frequency_[key]/(double)frequency_sum_))/r);
            frequencies->push_back((iterator->second/frequencysum)/r);
        }
    }
    for (int i = 0; i < dummy_keys_; i++){
        replicas->push_back(dummy_key_+std::to_string(i));
        fake_frequencies->push_back((dummy_keys_*p_max)/r);
    }
    real_distribution_ = distribution(replicas, frequencies);
    fake_distribution_ = distribution(replicas, fake_frequencies);
};

void pancake_proxy::prepare_for_swapping(std::string key, int r_new, int r_old, bool is_dummy_key,
                                         std::shared_ptr<std::vector<std::string>> unassigned_labels,
                                         std::shared_ptr<std::vector<std::pair<std::string, int>>> needs_labels){
    if(r_new > r_old){
        for (int i = r_old; i < r_new; i++)
            needs_labels.push_back(std::make_pair(key, i));
        if (!is_dummy_key)
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

int pancake_proxy::perform_swapping(std::shared_ptr<std::vector<std::string>> unassigned_labels,
                                     std::shared_ptr<std::vector<std::pair<std::string, int>>> needs_labels){
    for (int i = 0; i < needy_replicas->size(); i++){
        auto key_rep_pair = (*needy_replicas)[i];
        std::string replica = key_rep_pair.first + std::to_string(key_rep_pair.second);
        auto label = (*unassigned_labels)[i];
        replica_to_label_[replica] = label;
    }
    return needy_replicas->size();
}

void pancake_proxy::update_distribution() {
    auto update_s = std::chrono::high_resolution_clock::now();
    auto update_e = std::chrono::high_resolution_clock::now();
    auto unassigned_labels = std::make_shared<std::vector<std::string>>();
    auto needs_labels = std::make_shared<std::vector<std::pair<std::string, int>>>();
    int new_num_real_replicas = 0;
    int n = key_to_frequency_.size();
    int num_swaps = 0;
    for (auto iterator = new_key_to_frequency_.begin(); iterator != new_key_to_frequency_.end(); iterator++) {
        auto key = iterator->first;
        int r_old = (key_to_replica_num_.find(key) != key_to_replica_num_.end()) ? key_to_replica_num_[key] : 0;
        int r_new = ceil(new_key_to_replica_num[key]/(double)frequency_sum_*n);
        key_to_replica_num_[key] = r_new;
        assert(r_new > 0);
        new_num_real_replicas += r_new;
        num_swaps += (key, r_new, r_old, false, unassigned_labels, needs_labels);
    }
    assert (new_num_real_replicas < 2*n);
    int r_old = dummy_keys_;
    dummy_keys_ = 2*n - new_num_real_replicas;
    prepare_for_swapping(dummy_key_, dummy_keys_, r_old, true, unassigned_labels, needs_labels);
    key_to_frequency_ = new_key_to_frequency_;
    compute_fake_dist();
    assert(needy_replicas.size() == unassigned_labels.size());
    num_swaps += perform_swapping(unassigned_labels, needs_labels);
    update_e = std::chrono::high_resolution_clock::now();
    auto elapsed = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(update_e - update_s).count());
    std::cout << "Time to update dist: " << elapsed << " Num swaps: " << num_swaps << std::endl;
};

bool pancake_proxy::is_true_distribution() {
    return rand()/(RAND_MAX+1.0) < prob(1.0*(2.0 * p_max * key_to_frequency_.size()));
};

std::shared_ptr<std::vector<operation> pancake_proxy::create_security_batch(Operation op, queue * op_queue) {
    auto security_batch = std::make_shared<std::vector<operation>>();
    for (int i = 0; i < security_batch_size_; i++) {
        if (is_true_distribution()) {
            if (op_queue->size() == 0) {
                struct operation operat;
                operat.key = real_distribution_.sample();
                operat.value = "";
                security_batch->push_back(operat);
            } else {
                security_batch->push_back(op_queue->pop());
            }
        } else {
            struct operation operat;
            operat.key = fake_distribution_.sample();
            operat.value = "";
            securitybatch.push_back(operat);
        }
    }
};

void pancake_proxy::execute_batch(const std::vector<operation> & operations, std::vector<std::string> & _returns,
                   std::vector<bool> & is_trues) {

};

std::string pancake_proxy::get(const std::string &key, const std::string &value) {

};

void pancake_proxy::put(const std::string &key, const std::string &value) {

};
std::shared_ptr<std::vector<const std::string>> pancake_proxy::get_batch(std::shared_ptr<std::vector<const std::string>> keys) {

};

void pancake_proxy::put_batch(std::shared_ptr<std::vector<const std::string>> keys, std::shared_ptr<std::vector<const std::string>> values) {

};

void pancake_proxy::service_thread(int id){

};

void pancake_proxy::run() {

};

void pancake_proxy::usage() {

};

int main() {

}