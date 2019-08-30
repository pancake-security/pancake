//
// Created by Lloyd Brown on 8/30/19.
//

#ifndef PANCAKE_PANCAKE_BENCHMARK_H
#define PANCAKE_PANCAKE_BENCHMARK_H

#include "timer.h"
#include "pancake_proxy.h"

class pancake_benchmark {
public:
    void run_benchmark();
    void warmup(const std::string key_file, const std::string& workload_file);
    void cooldown(const std::string key_file, const std::string& workload_file);

private:
    pancake_proxy pancake_proxy_;
};

#endif //PANCAKE_PANCAKE_BENCHMARK_H
