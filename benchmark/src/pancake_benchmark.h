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

private:
    Pancake_Proxy pancake_proxy;
};

#endif //PANCAKE_PANCAKE_BENCHMARK_H
