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
    distribution dist(keys, frequencies);
    return dist;
};

void pancake_benchmark::run_benchmark(int run_time, bool stats, std::vector<int> &latencies) {
    std::string dummy(object_size_, '0');
    int numerrors = 0;
    int ops = 0;
    size_t chunknum = 0;
    uint64_t start, end;
    std::pair<std::string, std::string> res;
    auto ticks_per_ns = static_cast<double>(rdtscuhz()) / 1000;
    auto s = std::chrono::high_resolution_clock::now();
    auto e = std::chrono::high_resolution_clock::now();
    int elapsed = 0;
    std::vector<std::string> results;
    int i = 0;
    while (elapsed*1000000 < run_time) {
        if (stats) {
            rdtscll(start);
        }
        auto keys_values_pair = trace_[i];
        auto keys = keys_values_pair.first;
        auto values = keys_values_pair.second;
        if (values.size() == 0){
            client_.get_batch(keys);
        }
        else {
            client_.put_batch(keys, values);
        }
        if (stats) {
            rdtscll(end);
            double cycles = static_cast<double>(end - start);
            latencies.push_back((cycles / ticks_per_ns) / client_batch_size_);
            rdtscll(start);
            ops += keys.size();
        }
        e = std::chrono::high_resolution_clock::now();
        elapsed = static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(e - s).count());
        i = (i+1)%keys.size();
    }
    if (stats)
        xput_ += ((static_cast<double>(ops) / elapsed));
}

void pancake_benchmark::warmup(std::vector<int> &latencies) {
    run_benchmark(15, false, latencies);
}

void pancake_benchmark::cooldown(std::vector<int> &latencies) {
    run_benchmark(15, false, latencies);
}

void pancake_benchmark::usage() {
    std::cout << "Pancake proxy: frequency flattening kvs\n";
    // Network Parameters
    std::cout << "\t -h: Storage server host name\n";
    std::cout << "\t -p: Storage server port\n";
    std::cout << "\t -s: Storage server type (redis, rocksdb, memcached)\n";
    std::cout << "\t -n: Storage server count\n";
    std::cout << "\t -z: Proxy server type\n";
    // Workload parameters
    std::cout << "\t -w: Clients' workload file\n";
    std::cout << "\t -l: key size\n";
    std::cout << "\t -v: Value size\n";
    std::cout << "\t -b: Security batch size\n";
    std::cout << "\t -c: Storage batch size\n";
    std::cout << "\t -t: Number of worker threads for cpp_redis\n";
    // Other parameters
    std::cout << "\t -o: Output location for sizing thread\n";
    std::cout << "\t -d: Core to run on\n";
};

int pancake_benchmark::main(int argc, char **argv) {
    auto proxy = std::make_shared<pancake_proxy>(new pancake_proxy());

    int port = 9090;
    int o;
    std::string proxy_type = "pancake";
    while ((o = getopt(argc, argv, "a:p:s:n:w:v:b:c:p:o:d:t:x:f:")) != -1) {
        switch (o) {
            case 'h':
                proxy->server_host_name_ = std::string(optarg);
                break;
            case 'p':
                proxy->server_port_ = std::atoi(optarg);
                break;
            case 's':
                proxy->server_type_ = std::string(optarg);
                break;
            case 'n':
                proxy->server_count_ = std::atoi(optarg);
                break;
            case 'w':
                proxy->workload_file_ = std::string(optarg);
                break;
            case 'l':
                proxy->key_size_ = std::atoi(optarg);
                break;
            case 'v':
                proxy->object_size_ = std::atoi(optarg);
                break;
            case 'b':
                proxy->security_batch_size_ = std::atoi(optarg);
                break;
            case 'c':
                proxy->storage_batch_size_ = std::atoi(optarg);
                break;
            case 't':
                proxy->p_threads_ = std::atoi(optarg);
                break;
            case 'o':
                proxy->output_location_ = std::string(optarg);
                break;
            case 'd':
                proxy->core_ = std::atoi(optarg) - 1;
                break;
            case 'z':
                proxy_type = std::string(optarg);
            default:
                usage();
                exit(-1);
        }
    }

    void *arguments[3];
    arguments[0] = malloc(sizeof(distribution * ));
    arguments[1] = malloc(sizeof(double *));
    arguments[2] = malloc(sizeof(double *));

    auto dist = load_frequencies_from_trace(argv[1]);
    arguments[0] = &dist;
    auto items = dist.get_items();
    double alpha = 1.0 / items.size();
    double delta = 1.0 / (2 * items.size()) * 1 / alpha;
    arguments[1] = &alpha;
    arguments[2] = &delta;
    std::string dummy(object_size_, '0');
    proxy->init(items, std::vector<std::string>(items.size(), dummy), arguments);

    auto server = thrift_server::create(proxy, proxy_type);
    server->serve()
    std::vector<int> latencies;
    warmup(latencies);
    run_benchmark(20, true, latencies);
    cooldown(latencies);
    return 0;
}