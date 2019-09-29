//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include <random>

class distribution {
public:
    distribution(std::shared_ptr<std::vector<std::string> > items, std::shared_ptr<std::vector<int>> frequencies);
    std::string sample();

private:
    std::discrete_distribution<int> distribution_;
    std::default_random_engine random_;
    std::shared_ptr<std::vector<std::string>> items_;
};

#endif //DISTRIBUTION_H