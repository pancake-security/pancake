//
// Created by Lloyd Brown on 8/30/19.
//

#ifndef PANCAKE_STANDARD_DISTRIBUTION_H
#define PANCAKE_STANDARD_DISTRIBUTION_H

#include "distribution.h"

class Standard_Distribution : public Distribution {
public:
    void init(std::shared_ptr<std::vector<const std::string>> items, std::shared_ptr<std::vector<int>> frequencies) override;
    std::string sample() = override;

private:
    std::discrete_distribution<int> distribution;
    std::vector<const string> items;
};

#endif //PANCAKE_STANDARD_DISTRIBUTION_H
