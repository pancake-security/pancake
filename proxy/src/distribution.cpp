//
// Created by Lloyd Brown on 9/17/19.
//

#include "distribution.h"

    distribution::distribution() = default;

    distribution::distribution(const std::vector<std::string> &items, const std::vector<double> &probabilities) {
        std::discrete_distribution<int> new_distribution_(probabilities.begin(), probabilities.end());
        distribution_ = new_distribution_;
        items_ = items;
        probabilities_ = probabilities;
    }

    std::string distribution::sample() {
        return items_[distribution_(random_)];
    }

    const std::vector<std::string>& distribution::get_items() const{
        return items_;
    }

    const std::vector<double>& distribution::get_probabilities() const{
        return probabilities_;
    }
