//
// Created by Lloyd Brown on 9/20/19.
//

#include "util.h"

uint32_t rand_uint32(const uint32_t &min, const uint32_t &max) {
    static thread_local std::mt19937 generator;
    std::uniform_int_distribution<uint32_t> distribution(min, max);
    return distribution(generator);
};

std::string rand_str(const int len) {
    static const char alphanum[] = "0123456789";
    std::string ret;
    ret.resize(len);
    for (int i = 0; i < len; ++i) {
        ret[i] = alphanum[rand_uint32(0, 500) % (sizeof(alphanum) - 1)];
    }
    return ret;
}