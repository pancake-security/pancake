//
// Created by Lloyd Brown on 9/17/19.
//

#include "distribution.h"

    distribution::distribution(){};

    distribution::distribution(std::vector<std::string> items, std::vector<double> frequencies) {
        std::discrete_distribution<double> new_distribution_(frequencies.begin(), frequencies.end());
        distribution_ = new_distribution_;
        items_ = items;
        frequencies_ = frequencies;
    };

    std::string distribution::sample() {
        return items_[distribution_(random_)];
    };

    std::pair<std::vector<std::string> *, std::vector<double>*> distribution::get_items_and_frequencies(){
        return std::make_pair(&items_, &frequencies_);
    }
