//
// Created by Lloyd Brown on 10/24/19.
//

// for windows mkdir
#ifdef _WIN32
#include <direct.h>
#endif

#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <thread>

#include "timer.h"
#include "distribution.h"
#include "pancake_proxy.h"
#include "thrift_server.h"
#include "proxy_client.h"
#include "thrift_utils.h"

typedef std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> trace_vector;

void load_trace(const std::string &trace_location, trace_vector &trace, int client_batch_size) {
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
            if (get_keys.size() == client_batch_size){
                trace.push_back(std::make_pair(get_keys, std::vector<std::string>()));
                get_keys.clear();
            }
        }
        else {
            put_keys.push_back(key);
            put_values.push_back(val);
            if (put_keys.size() == client_batch_size){
                trace.push_back(std::make_pair(put_keys, put_values));
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
        trace.push_back(std::make_pair(get_keys, std::vector<std::string>()));
        get_keys.clear();
    }
    if (put_keys.size() > 0){
        trace.push_back(std::make_pair(put_keys, put_values));
        put_keys.clear();
        put_values.clear();
    }
    in_workload_file.close();
};

void run_benchmark(int run_time, bool stats, std::vector<int> &latencies, int client_batch_size,
                   int object_size, trace_vector &trace, std::atomic<int> &xput, proxy_client client) {
    std::string dummy(object_size, '0');
    int ops = 0;
    uint64_t start, end;
    auto ticks_per_ns = static_cast<double>(rdtscuhz()) / 1000;
    auto s = std::chrono::high_resolution_clock::now();
    auto e = std::chrono::high_resolution_clock::now();
    int elapsed = 0;
    std::vector<std::string> results;
    int i = 0;
    while (elapsed < run_time*1000000) {
        if (stats) {
            rdtscll(start);
        }
        auto keys_values_pair = trace[i];
        if (keys_values_pair.second.empty()){
            client.get_batch(keys_values_pair.first);
        }
        else {
            client.put_batch(keys_values_pair.first, keys_values_pair.second);
        }
        if (stats) {
            rdtscll(end);
            double cycles = static_cast<double>(end - start);
            latencies.push_back((cycles / ticks_per_ns) / client_batch_size);
            rdtscll(start);
            ops += keys_values_pair.first.size();
        }
        e = std::chrono::high_resolution_clock::now();
        elapsed = static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(e - s).count());
        i = (i+1)%keys_values_pair.first.size();
    }
    if (stats)
        xput += (int)(static_cast<double>(ops) * 1000000 / elapsed);
}

void warmup(std::vector<int> &latencies, int client_batch_size,
            int object_size, trace_vector &trace, std::atomic<int> &xput, proxy_client client) {
    run_benchmark(15, false, latencies, client_batch_size, object_size, trace, xput, client);
}

void cooldown(std::vector<int> &latencies, int client_batch_size,
              int object_size, trace_vector &trace, std::atomic<int> &xput, proxy_client client) {
    run_benchmark(15, false, latencies, client_batch_size, object_size, trace, xput, client);
}

void client(int idx, int client_batch_size, int object_size, trace_vector &trace, std::string &output_directory, std::string &host, int proxy_port, std::atomic<int> &xput) {
    proxy_client client;
    client.init(host, proxy_port);
    std::atomic<int> indiv_xput;
    std::atomic_init(&indiv_xput, 0);
    std::vector<int> latencies;
    warmup(latencies, client_batch_size, object_size, trace, indiv_xput, client);
    run_benchmark(20, true, latencies, client_batch_size, object_size, trace, indiv_xput, client);
    std::string location = output_directory + "/" + std::to_string(idx);
    std::ofstream out(location);
    std::string line("");
    for (auto lat : latencies) {
        line.append(std::to_string(lat) + "\n");
        out << line;
        line.clear();
    }
    line.append("Xput: " + std::to_string(indiv_xput) + "\n");
    out << line;
    xput += indiv_xput;
    cooldown(latencies, client_batch_size, object_size, trace, indiv_xput, client);
}

void usage() {
    std::cout << "Proxy client\n";
    std::cout << "\t -h: Proxy host name\n";
    std::cout << "\t -p: Proxy port\n";
    std::cout << "\t -t: Trace Location\n";
    std::cout << "\t -n: Number of threads to spawn\n";
    std::cout << "\t -s: Object Size\n";
    std::cout << "\t -o: Output Directory\n";
};

int _mkdir(const char *path) {
    #ifdef _WIN32
        return ::_mkdir(path);
    #else
        #if _POSIX_C_SOURCE
            return ::mkdir(path, 0755);
        #else
            return ::mkdir(path, 0755); // not sure if this works on mac
        #endif
    #endif
}

int main(int argc, char *argv[]) {
    std::string proxy_host = "127.0.0.1";
    int proxy_port = 9090;
    std::string trace_location = "";
    int client_batch_size = 50;
    int object_size = 1000;
    int num_clients = 1;

    std::time_t end_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto date_string = std::string(std::ctime(&end_time));
    date_string = date_string.substr(0, date_string.rfind(":"));
    date_string.erase(remove(date_string.begin(), date_string.end(), ' '), date_string.end());
    std::string output_directory = "data/"+date_string;

    int o;
    std::string proxy_type_ = "pancake";
    while ((o = getopt(argc, argv, "h:p:t:s:b:n:o:")) != -1) {
        switch (o) {
            case 'h':
                proxy_host = std::string(optarg);
                break;
            case 'p':
                proxy_port = std::atoi(optarg);
                break;
            case 't':
                trace_location = std::string(optarg);
                break;
            case 's':
                object_size = std::atoi(optarg);
                break;
            case 'n':
                num_clients = std::atoi(optarg);
                break;
            case 'o':
                output_directory = std::string(optarg);
                break;
            default:
                usage();
                exit(-1);
        }
    }

    _mkdir((output_directory).c_str());
    std::atomic<int> xput;
    std::atomic_init(&xput, 0);

    trace_vector trace;
    load_trace(trace_location, trace, client_batch_size);

    std::vector<std::thread> threads;
    for (int i = 0; i < num_clients; i++) {
        threads.push_back(std::thread(client, std::ref(i), std::ref(client_batch_size), std::ref(object_size), std::ref(trace),
                          std::ref(output_directory), std::ref(proxy_host), std::ref(proxy_port), std::ref(xput)));
    }
    for (int i = 0; i < num_clients; i++)
        threads[i].join();
    std::cout << "Xput was: " << xput << std::endl;
}