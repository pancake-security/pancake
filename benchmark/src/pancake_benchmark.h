//
// Created by Lloyd Brown on 8/30/19.
//

#ifndef PANCAKE_PANCAKE_BENCHMARK_H
#define PANCAKE_PANCAKE_BENCHMARK_H

#include <unordered_map>

#include "timer.h"
#include "distribution.h"
#include "client.h"
#include "proxy_client.h"
#include "pancake_thrift_server.h"
class pancake_benchmark {
public:
    distribution load_frequencies_from_trace(const std::string &trace_location);
    void run_benchmark(int run_time, bool stats, std::vector<int> * latencies);
    void warmup(std::vector<int> * latencies);
    void cooldown(std::vector<int> * latencies);
    int main(int argc, char ** argv);

private:

    std::vector<std::pair<std::vector<std::string>, std::vector<std::string>> trace_;
    int client_batch_size_ = 50;
    client client_;
    pancake_thriftHandler pancake_
    double xput_;
};

#endif //PANCAKE_PANCAKE_BENCHMARK_H
