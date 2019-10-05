//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef PANCAKE_OPERATION_H
#define PANCAKE_OPERATION_H

#include <string>

class operation {
public:
    operation(const operation&){};
    operation& operator=(const operation&){
        return *this;
    };
    operation() : key(), value(), keys(), values(){
    }

    ~operation(){};
    std::string key;
    std::vector<std::string> keys;
    std::string value;
    std::vector<std::string> values;

    bool operator == (const operation & rhs) const
    {
        if (!(key == rhs.key))
            return false;
        if (!(value == rhs.value))
            return false;
        return true;
    }
    bool operator != (const operation &rhs) const {
        return !(*this == rhs);
    }
};

#endif //PANCAKE_OPERATION_H
