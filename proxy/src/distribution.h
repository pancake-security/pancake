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
    distribution(const std::vector<std::string> &items, const std::vector<double> &frequencies);
    std::string sample();
    const std::vector<std::string>& get_items() const;
    const std::vector<double>& get_probabilities() const;

private:
    std::discrete_distribution<int> distribution_;
    std::default_random_engine random_;
    std::vector<std::string> items_;
    std::vector<double> probabilities_;
};

#endif //DISTRIBUTION_H