//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include <random>
#include <vector>

class distribution {
public:
    distribution();
    distribution(std::vector<std::string> items, std::vector<double> frequencies);
    std::string sample();
    std::pair<std::vector<std::string> *, std::vector<double> *> get_items_and_frequencies();

private:
    std::discrete_distribution<double> distribution_;
    std::default_random_engine random_;
    std::vector<std::string> items_;
    std::vector<double> frequencies_;
};

#endif //DISTRIBUTION_H