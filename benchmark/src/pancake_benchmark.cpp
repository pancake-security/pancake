//
// Created by Lloyd Brown on 10/3/19.
//

#include "pancake_benchmark.h"

distribution pancake_benchmark::load_frequencies_from_trace(const std::string &trace_location) {
    std::vector<std::string> get_keys;
    std::vector<std::string> put_keys;
    std::vector<std::string> put_values;

    std::unordered_map<std::string, int> key_to_frequency;
    int frequency_sum = 0;
    std::string op, key, val;
    std::ifstream in_workload_file;
    in_workload_file.open(trace_location);
    if(!in_workload_file){
        std::perror("Unable to find workload file");
    }
    std::string line;
    while (std::getline(in_workload_file, line)) {
        op = line.substr(0, line.find(" "));
        key = line.substr(line.find(" ")+1);
        val = "";
        if (key.find(" ") != -1) {
            val = key.substr(key.find(" ")+1);
            key = key.substr(0, key.find(" "));
        }
        if(val == ""){
            get_keys.push_back(key);
            if (get_keys.size() == client_batch_size_){
                trace_.push_back(std::make_pair(get_keys, std::vector<std::string>()));
                get_keys.clear();
            }
        }
        else {
            put_keys.push_back(key);
            put_values.push_back(val);
            if (put_keys.size() == client_batch_size_){
                trace_.push_back(std::make_pair(put_keys, put_values));
                put_keys.clear();
                put_values.clear();
            }
        }
        assert (key != "PUT");
        assert (key != "GET");
        assert (key.length() < object_size_);
        if (key_to_frequency.count(key) == 0){
            key_to_frequency[key] = 1;
            frequency_sum += 1;
        }
        else {
            key_to_frequency[key] += 1;
            frequency_sum += 1;
        }
    }
    if (get_keys.size() > 0){
        trace_.push_back(std::make_pair(get_keys, std::vector<std::string>()));
        get_keys.clear();
    }
    if (put_keys.size() > 0){
        trace_.push_back(std::make_pair(put_keys, put_values));
        put_keys.clear();
        put_values.clear();
    }
    std::vector<std::string> keys;
    std::vector<double> frequencies;
    for (auto pair: key_to_frequency){
        keys.push_back(pair.first);
        frequencies.push_back(pair.second/(double)frequency_sum);
    }
    in_workload_file.close();
    distribution = distribution(keys, frequencies);
    return distribution;
};

void pancake_benchmark::run_benchmark(int run_time, bool stats, std::vector<int> * latencies) {
    std::string dummy(std::stoi(object_size), '0');s
    int numerrors = 0;
    int ops = 0;
    size_t chunknum = 0;
    uint64_t start, end;
    std::pair<std::string, std::string> res;
    auto ticks_per_ns = static_cast<double>(rdtscuhz()) / 1000;
    auto s = chrono::high_resolution_clock::now();
    auto e = chrono::high_resolution_clock::now();
    int elapsed = 0;
    std::vector<std::string> results;
    int i = 0;
    std::vector
    while (elapsed*1000000 < run_time) {
        if (stats) {
            rdtscll(start);
        }
        auto keys_values_pair = trace_[i];
        auto keys = keys_vals_pair.first;
        auto values = keys_vals_pair.second;
        if (values.size() == 0){
            client_.get_batch(keys);
        }
        else {
            client_.put_batch(keys, values);
        }
        if (stats) {
            rdtscll(end);
            double cycles = static_cast<double>(end - start);
            latencies->push_back((cycles / ticks_per_ns) / client_batch_size_);
            rdtscll(start);
            ops += keys.size();
        }
        e = chrono::high_resolution_clock::now();
        elapsed = static_cast<int>(chrono::duration_cast<chrono::milliseconds>(e - s).count());
        i = (i+1)%keys.size();
    }
    if (stats)
        xput_ = ((static_cast<double>(ops) / elapsed));
    return ops;
}

void pancake_benchmark::warmup(std::vector<int> *latencies) {
    run_benchmark(15, false, latencies);
}

void pancake_benchmark::cooldown(std::vector<int> *latencies) {
    run_benchmark(15, false, latencies);
}

int pancake_benchmark::main(int argc, char ** argv){
    auto dist = load_frequencies_from_trace(argv[1]);
    pancake_ = pancake_thriftHandler(dist);
    pancake_.main()
    std::vector<int> latencies;
    warmup(&latencies);
    run_benchmark(20, true, &latencies);
    cooldown(&latencies);
    for (auto latency: latencies){
        std::cout << latency << std::endl;
    }
    std::cout << "Xput: " << xput_ << std::endl;
}