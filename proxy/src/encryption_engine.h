//
// Created by Lloyd Brown on 8/30/19.
//

#ifndef PANCAKE_ENCRYPTION_ENGINE_H
#define PANCAKE_ENCRYPTION_ENGINE_H

#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <assert.h>

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>

class encryption_engine {
public:
    void init();
    std::string Encrypt(const std::string &plaintext);
    std::string Encrypt(const std::string &cipher);
    std::string HMAC(const std::string &key);

private:
    void handleErrors();
    int sign_it(const byte* msg, size_t mlen, byte** sig, size_t* slen, EVP_PKEY* pkey);
    int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *aad,
                int aad_len, unsigned char *key, unsigned char *iv, int iv_len,
                unsigned char *ciphertext, unsigned char *tag);
    int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
                unsigned char *iv, unsigned char *ciphertext);
    int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *aad,
                int aad_len, unsigned char *tag, unsigned char *key, unsigned char *iv,
                int iv_len, unsigned char *plaintext);
    int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
                unsigned char *iv, unsigned char *plaintext);
    int verify_it(const byte* msg, size_t mlen, const byte* sig, size_t slen, EVP_PKEY* pkey);
    void print_it(const char* label, const byte* buff, size_t len);
    int make_keys(EVP_PKEY** skey, EVP_PKEY** vkey);
    int hmac_it(const byte* msg, size_t mlen, byte** val, size_t* vlen, EVP_PKEY* pkey);

    unsigned char *encryption_key_;
    unsigned char *iv_;
    EVP_PKEY *skey_ ,*vkey_;
};
#endif //PANCAKE_BASIC_CRYPTO_H
