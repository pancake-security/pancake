//
// Created by Lloyd Brown on 9/17/19.
//

#include "distribution.h"
#include <iostream>

void run_basic_test(){
    std::cout << "Performing distribution stress test" << std::endl;

    std::vector<std::string> items;
    std::vector<double> frequencies;
    for (int i = 0; i < 10000000; i++){
        items.push_back(std::to_string(i));
        frequencies.push_back(1.0);
    }


    // Initialize distribution
    distribution dist(std::move(items), std::move(frequencies));


    for (int i = 0; i < 10000000; i++){
        std::string sample = dist.sample();
        assert(std::stoi(sample) >= 0 && std::stoi(sample) <= 10000000);
    }

    std::cout << "Passed all tests :)" << std::endl;
}

int main(int argc, char* argv[]) {
    run_basic_test();
}