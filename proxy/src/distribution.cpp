//
// Created by Lloyd Brown on 9/17/19.
//

#include "distribution.h"


    void distribution::init(std::shared_ptr<std::vector<std::string>> items, std::shared_ptr<std::vector<int> > frequencies) {
        std::discrete_distribution<int> new_distribution_(frequencies->begin(), frequencies->end());
        this->distribution_ = new_distribution_;
        this->items_ = items;
    };

    std::string distribution::sample() {
        return (*items_)[distribution_(random_)];
    };
