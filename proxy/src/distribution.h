//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#endif //DISTRIBUTION_H

class Distribution {
public:
    virtual void init(std::shared_ptr<std::vector<const std::string>> items, std::shared_ptr<std::vector<const double>> frequencies) = 0;
    virtual const std::string sample() = 0;
};