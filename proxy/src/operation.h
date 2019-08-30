//
// Created by Lloyd Brown on 8/29/19.
//

#ifndef PANCAKE_OPERATION_H
#define PANCAKE_OPERATION_H

#include <string>

class Operation {
public:
    Operation(const Operation&);
    Operation& operator=(const Operation&);
    Operation() : key(), value() {
    }

    virtual ~Operation() throw();
    std::string key;
    std::string value;

    void __set_key(const std::string& val);

    void __set_value(const std::string& val);

    bool operator == (const Operation & rhs) const
    {
        if (!(key == rhs.key))
            return false;
        if (!(value == rhs.value))
            return false;
        return true;
    }
    bool operator != (const Operation &rhs) const {
        return !(*this == rhs);
    }
};

#endif //PANCAKE_OPERATION_H
