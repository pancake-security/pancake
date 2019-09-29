//
// Created by Lloyd Brown on 9/17/19.
//

#include "distribution.h"


    distribution::distribution(std::vector<std::string> items, std::vector<double> frequencies) {
        std::discrete_distribution<double> new_distribution_(frequencies.begin(), frequencies.end());
        this->distribution_ = new_distribution_;
        this->items_ = items;
    };

    std::string distribution::sample() {
        return *items_[distribution_(random_)];
    };
