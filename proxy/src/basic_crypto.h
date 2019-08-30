//
// Created by Lloyd Brown on 8/30/19.
//

#ifndef PANCAKE_BASIC_CRYPTO_H
#define PANCAKE_BASIC_CRYPTO_H

class Basic_Crypto : public Encryption_Engine {
    std::string Encrypt(const std::string &plaintext) override;
    std::string Encrypt(const std::string &cipher) override;
    std::string HMAC(const std::string &key) override;
};
#endif //PANCAKE_BASIC_CRYPTO_H
