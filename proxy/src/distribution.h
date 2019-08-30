//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#endif //DISTRIBUTION_H

class distribution {
public:
    void init(std::shared_ptr<std::vector<const std::string>> items, std::shared_ptr<std::vector<int>> frequencies);
    std::string sample();

private:
    std::discrete_distribution<int> distribution_;
    std::vector<const string> items_;
};