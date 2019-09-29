//
// Created by Lloyd Brown on 9/18/19.
//

#include "encryption_engine.h"
#include <iostream>

void run_basic_test(){
    encryption_engine encryption = encryption_engine();

    std::cout << "Performing encryption sanity test" << std::endl;
    assert("12345675757" == encryption.decrypt(encryption.encrypt("12345675757")));
    std::cout << "Sanity check passed" << std::endl;

    std::cout << "Performing encryption stress test" << std::endl;
    for (int i = 0; i < 100000; i++){
        std::string random_string = encryption.gen_random(1000);
        assert(encryption.encrypt(random_string) != random_string);
        assert(random_string == encryption.decrypt(encryption.encrypt(random_string)));
    }
    std::cout << "Passed stress test" << std::endl;

    std::cout << "Passed all tests :)" << std::endl;
}

int main(int argc, char* argv[]) {
    run_basic_test();
}