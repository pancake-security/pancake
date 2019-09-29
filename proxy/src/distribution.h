//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include <random>

class distribution {
public:
    distribution();
    distribution(std::vector<std::string> items, std::vector<double> frequencies);
    std::string sample();

private:
    std::discrete_distribution<double> distribution_;
    std::default_random_engine random_;
    std::vector<std::string> items_;
};

#endif //DISTRIBUTION_H