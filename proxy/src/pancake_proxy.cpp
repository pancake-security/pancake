//
// Created by Lloyd Brown on 9/24/19.
//

#include "pancake_proxy.h"

void pancake_proxy::init(){
    encryption_engine_ = encryption_engine();
    load_frequencies_from_trace(output_location_, &key_to_frequency_);
    create_replicas();
}

void pancake_proxy::load_frequencies_from_trace(const std::string &trace_location, std::unordered_map<std::string, int> * key_to_frequency) {
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
        assert (key.length() < object_size_);
        if (key_to_frequency->count(key) == 0){
            (*key_to_frequency)[key] = 1;
            frequency_sum_ += 1;
        }
        else {
            (*key_to_frequency)[key] += 1;
            frequency_sum_ += 1;
        }
    }
    in_workload_file.close();
};


void pancake_proxy::insert_replicas(std::string key, int num_replicas){
    std::string value_cipher = encryption_engine_.encrypt(rand_str(object_size_));
    for (int i = 0; i < num_replicas; i++){
        std::string replica = key+std::to_string(i);
        std::string replica_cipher = encryption_engine_.hmac(std::to_string(label_count_));
        replica_to_label_[replica] = label_count_;
        //TODO add initial writes
        //storage_interfaces_[label_count_ % s_ifs.size()]->write(encryption_engine_.hmac(std::to_string(label_count_)), value_cipher);
        label_count_++;
    }
}

void pancake_proxy::create_replicas() {
    int n = key_to_frequency_.size();
    int n_prime = 2*n;
    int keys_created = 0;
    std::vector<std::pair<std::string, int>> update_cache_pairs;
    std::vector<std::string> keys;
    std::vector<double> frequencies;
    std::vector<double> fake_frequencies;
    for (auto iterator = key_to_frequency_.begin(); iterator != key_to_frequency_.end(); iterator++) {
        std::string key = iterator->first;
        double pi = key_to_frequency_[key]/(double)frequency_sum_;
        int r_i = ceil((key_to_frequency_[key]/(double)frequency_sum_)*n);
        keys.push_back(key);
        frequencies.push_back(key_to_frequency_[key]/(double)frequency_sum_);
        fake_frequencies.push_back(((1-pi/r_i)/(2*n-1))*r_i);
        key_to_number_of_replicas_[key] = r_i;
        insert_replicas(key, r_i);
        update_cache_pairs.push_back(std::make_pair(key, r_i));
        keys_created += r_i;
    }
    real_distribution_ = distribution(keys, frequencies);
    int dummy_r_i = std::max(0,n_prime-keys_created);
    dummy_keys_ = dummy_r_i;
    if(dummy_r_i != 0) {
        keys.push_back(dummy_key_);
        fake_frequencies.push_back((1/(2*n-1))*dummy_r_i);
        key_to_number_of_replicas_[dummy_key_] = dummy_r_i;
        insert_replicas(dummy_key_, dummy_r_i);
        update_cache_pairs.push_back(std::make_pair(dummy_key_, dummy_r_i));
    }
    update_cache_ = update_cache(&update_cache_pairs);
    fake_distribution_ = distribution(keys, fake_frequencies);
};

void pancake_proxy::compute_fake_distribution() {
    std::vector<std::string> keys;
    std::vector<double> frequencies;
    std::vector<double> fake_frequencies;
    int n = key_to_frequency_.size();
    double curr_max_real = (double)1/n;
    auto locked = missing_new_replicas_.get_locked_table());
    for (auto iterator = locked.begin(); iterator != locked.end(); iterator++){
        std::string bit_vec = iterator->second;
        std::string key = iterator->first;
        int r = key_to_number_of_replicas_[key];
        assert(bit_vec.length() == r);
        curr_max_real = std::max(new_key_to_frequency_[key]/(double)frequency_sum_), curr_max_real);
    }
    locked.unlock();
    p_max = curr_max_real;
    int num_real_replicas = 0;
    for (auto iterator = key_to_frequency_.begin(); iterator != key_to_frequency_.end(); iterator++){
        std::string key = iterator->first;
        int r = key_to_number_of_replicas_[key];
        num_real_replicas += r;
        keys.push_back(key);
        frequencies.push_back(iterator->second/frequency_sum_);
        fake_frequencies.push_back((r*p_max-new_key_to_frequency_[key]/(double)frequency_sum_));
    }
    real_distribution_ = distribution(keys, frequencies);
    keys.push_back(dummy_key_);
    fake_frequencies.push_back(dummy_keys_*p_max);
    fake_distribution_ = distribution(keys, fake_frequencies);
};

void pancake_proxy::prepare_for_swapping(std::string key, int r_new, int r_old,
                                         std::vector<std::string> * unassigned_labels,
                                         std::vector<std::pair<std::string, int>> * needs_labels){
    if(r_new > r_old){
        for (int i = r_old; i < r_new; i++)
            needs_labels->push_back(std::make_pair(key, i));
        if (key != dummy_key_)
            missing_new_replicas_.edit_bit_vector_size(key, r_new);
    }
    else if(r_new < r_old) {
        for (int i = r_new; i < r_old; i++) {
            std::string label;
            assert(replica_to_label_.find(key+std::to_string(i)) != replica_to_label_.end());
            label = replica_to_label_[key+std::to_string(i)];
            replica_to_label_.erase(key+std::to_string(i));
            unassigned_labels->push_back(label);
        }
    }
}

int pancake_proxy::perform_swapping(std::vector<std::string> * unassigned_labels,
                                    std::vector<std::pair<std::string, int>> * needs_labels){
    for (int i = 0; i < needs_labels->size(); i++){
        auto key_rep_pair = (*needs_labels)[i];
        std::string replica = key_rep_pair.first + std::to_string(key_rep_pair.second);
        auto label = (*unassigned_labels)[i];
        replica_to_label_[replica] = std::stoi(label);
    }
    return needs_labels->size();
}

void pancake_proxy::update_distribution() {
    auto update_s = std::chrono::high_resolution_clock::now();
    auto update_e = std::chrono::high_resolution_clock::now();
    std::vector<std::string> unassigned_labels;
    std::vector<std::pair<std::string, int>> needs_labels;
    int new_num_real_replicas = 0;
    int n = key_to_frequency_.size();
    int num_swaps = 0;
    for (auto iterator = new_key_to_frequency_.begin(); iterator != new_key_to_frequency_.end(); iterator++) {
        auto key = iterator->first;
        int r_old = (key_to_number_of_replicas_.find(key) != key_to_number_of_replicas_.end()) ? key_to_number_of_replicas_[key] : 0;
        int r_new = ceil(new_key_to_frequency_[key]/(double)frequency_sum_*n);
        key_to_number_of_replicas_[key] = r_new;
        assert(r_new > 0);
        new_num_real_replicas += r_new;
        prepare_for_swapping(key, r_new, r_old, &unassigned_labels, &needs_labels);
    }
    assert (new_num_real_replicas < 2*n);
    int r_old = dummy_keys_;
    dummy_keys_ = 2*n - new_num_real_replicas;
    prepare_for_swapping(dummy_key_, dummy_keys_, r_old, &unassigned_labels, &needs_labels);
    compute_fake_distribution();
    key_to_frequency_ = new_key_to_frequency_;
    assert(needs_labels.size() == unassigned_labels.size());
    num_swaps += perform_swapping(&unassigned_labels, &needs_labels);
    update_e = std::chrono::high_resolution_clock::now();
    auto elapsed = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(update_e - update_s).count());
    std::cout << "Time to update dist: " << elapsed << " Num swaps: " << num_swaps << std::endl;
};

bool pancake_proxy::is_true_distribution() {
    return rand()/(RAND_MAX+1.0) < prob(1.0*(2.0 * p_max * key_to_frequency_.size()));
};

void pancake_proxy::create_security_batch(operation op, queue <operation> * op_queue,
                                                       std::vector<operation> * storage_batch,
                                                       std::vector<bool> * is_trues) {
    for (int i = 0; i < security_batch_size_; i++) {
        if (is_true_distribution()) {
            if (op_queue->size() == 0) {
                struct operation operat;
                operat.key = real_distribution_.sample();
                operat.value = "";
                storage_batch->push_back(operat);
                is_trues->push_back(false);
            } else {
                storage_batch->push_back(op_queue->pop());
                is_trues->push_back(true);
            }
        } else {
            struct operation operat;
            operat.key = fake_distribution_.sample();
            operat.value = "";
            storage_batch->push_back(operat);
            is_trues->push_back(false);
        }
    }
};

void pancake_proxy::execute_batch(const std::vector<operation> * operations, std::vector<std::string> * _returns,
                               std::vector<bool> * is_trues, std::vector<storage_interface*>* storage_interfaces) {

    // Store which are real queries so we can return the values
    std::vector<int> replica_ids;
    std::vector<std::string> labels;
    std::vector<int> ids;

    std::vector<std::string> storage_keys;
    for(int i = 0; i < operations->size(); i++){
        replica_ids.push_back(missing_new_replicas_.sample_a_replica((*operations)[i].key, key_to_number_of_replicas_[(*operations)[i].key], (*is_trues)[i], key_to_frequency_[(*operations)[i].key], p_max));
        labels.push_back(std::to_string(replica_to_label_[(*operations)[i].key+std::to_string(replica_ids[i])]));
        int id = std::stoi(labels[i]) % storage_interfaces->size();
        ids.push_back(id);
        storage_keys.push_back(encryption_engine_.hmac(labels[i]));
    }
    auto responses = (*storage_interfaces)[0]->get_batch(&storage_keys);
    std::vector<std::string> storage_values;
    for(int i = 0; i < operations->size(); i++){
        auto cipher = responses[i];
        auto plaintext = encryption_engine_.decrypt(cipher);
        auto plaintext_update = update_cache_.check_for_update((*operations)[i].key, replica_ids[i]);
        plaintext = plaintext_update == "" ? plaintext : plaintext_update;
        missing_new_replicas_.check_if_missing((*operations)[i].key, plaintext, &update_cache_);
        if ((*is_trues)[i])
            _returns->push_back(plaintext);
        storage_values.push_back(encryption_engine_.encrypt(plaintext));
    }
    (*storage_interfaces)[0]->put_batch(&storage_keys, &storage_values);
}

std::string pancake_proxy::get(std::string &key, std::string &value) {
    return "";
};

void pancake_proxy::put(std::string &key, std::string &value) {

};
std::vector<std::string> pancake_proxy::get_batch(std::vector<std::string> * keys) {
    std::vector<std::string> _return;
    return _return;
};

void pancake_proxy::put_batch(std::vector<std::string> * keys, std::vector<std::string> * values) {

};

void pancake_proxy::service_thread(queue <operation> operation_queue){
    while (true) {
        std::vector<bool> is_trues;
        std::vector <operation> storage_batch;
        while (storage_batch.size() < storage_batch_size_) {
            operation operat = operation_queue.pop();
            create_security_batch(operat, &operation_queue, &storage_batch, &is_trues);
        }
        std::vector <std::string> _returns;
        execute_batch(&storage_batch, &_returns, &is_trues, *storage_interfaces);
    }
};

bool pancake_proxy::distribution_changed(){
    return false;
}

void pancake_proxy::run() {
    if (distribution_changed()){
        update_distribution();
        sleep(1);
    }
};

void pancake_proxy::usage() {
    std::cout << "Pancake proxy: frequency flattening kvs\n";
    // Network Parameters
    std::cout << "\t -h: Storage server host name\n";
    std::cout << "\t -p: Storage server port\n";
    std::cout << "\t -s: Storage server type (redis, rocksdb, memcached)\n";
    std::cout << "\t -n: Storage server count\n";
    // Workload parameters
    std::cout << "\t -w: Clients' first workload file\n";
    std::cout << "\t -l: key size\n";
    std::cout << "\t -v: Value size\n";
    std::cout << "\t -b: Security batch size\n";
    std::cout << "\t -c: Storage batch size\n";
    std::cout << "\t -t: Number of worker threads for cpp_redis\n";
    // Other parameters
    std::cout << "\t -o: Output location for sizing thread\n";
    std::cout << "\t -d: Core to run on\n";
};

int main(int argc, char* argv[]) {
    pancake_proxy proxy;
    int o;
    while ((o = getopt(argc, argv, "a:p:s:n:w:v:b:c:p:o:d:t:x:f:")) != -1) {
        switch (o) {
            case 'h':
                proxy.server_hostname_ = std::string(optarg);
                break;
            case 'p':
                proxy.server_port_ = std::atoi(optarg);
                break;
            case 's':
                proxy.server_type_ = std::string(optarg);
                break;
            case 'n':
                proxy.server_count_ = std::atoi(optarg);
                break;
            case 'w':
                proxy.workload_file_ = std::string(optarg);
                break;
            case 'l':
                proxy.key_size_ = std::atoi(optarg);
                break;
            case 'v':
                proxy.object_size_ = std::atoi(optarg);
                break;
            case 'b':
                proxy.security_batch_size_ = std::atoi(optarg);
                break;
            case 'c':
                proxy.storage_batch_size_ = std::atoi(optarg);
                break;
            case 't':
                proxy.pthreads_ = std::atoi(optarg);
                break;
            case 'o':
                proxy.output_location_ = std::string(optarg);
                break;
            case 'd':
                proxy.core_ = std::atoi(optarg) - 1;
                break;
            default:
                proxy.usage();
                exit(-1);
        }
    }
    proxy.init();
    // TODO: Create various threads
    while (true){
        proxy.run();
    }
}