//
// Created by Lloyd Brown on 8/30/19.
//

#ifndef PANCAKE_ENCRYPTION_ENGINE_H
#define PANCAKE_ENCRYPTION_ENGINE_H

class Encryption_Engine {
public:
    virtual std::string Encrypt(const std::string &plaintext) = 0;
    virtual std::string Encrypt(const std::string &cipher) = 0;
    virtual std::string HMAC(const std::string &key) = 0;
};

#endif //PANCAKE_ENCRYPTION_ENGINE_H
