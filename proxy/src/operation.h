//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef PANCAKE_OPERATION_H
#define PANCAKE_OPERATION_H

#include <string>

class operation {
public:
    operation(const operation&);
    operation& operator=(const operation&);
    operation() : key(), value() {
    }

    virtual ~operation() throw();
    std::string key;
    std::string value;

    void __set_key(const std::string& val);

    void __set_value(const std::string& val);

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
