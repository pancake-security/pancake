//
// Created by Lloyd Brown on 9/19/19.
//

#ifndef PANCAKE_UTIL_H
#define PANCAKE_UTIL_H

#include <stdint.h>
#include <string>
#include <random>

uint32_t rand_uint32(const uint32_t &min, const uint32_t &max);

std::string rand_str(const int len);

bool prob(double p);

#endif //PANCAKE_UTIL_H
