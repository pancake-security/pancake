//
// Created by Lloyd Brown on 8/30/19.
//

#include "encryption_engine.h"
#include <iostream>

    encryption_engine::encryption_engine() {
        OpenSSL_add_all_algorithms();
	    encryption_string_ = rand_str(32);
	    iv_string_= rand_str(16);
        encryption_key_ = (unsigned char *)encryption_string_.c_str();
        iv_ = (unsigned char *)iv_string_.c_str();

        skey_ = NULL;
        vkey_ = NULL;

        int rc = make_keys(&skey_, &vkey_);
        if (rc != 0)
            exit(1);
        assert(skey_ != NULL);
        if (skey_ == NULL)
            exit(1);
        assert(vkey_ != NULL);
        if (vkey_ == NULL)
            exit(1);
    };

    encryption_engine::encryption_engine(const encryption_engine& enc_engine) {
        encryption_key_ = (unsigned char *)enc_engine.encryption_string_.c_str();
        iv_ = (unsigned char *)enc_engine.iv_string_.c_str();
	skey_ = NULL;
        vkey_ = NULL;

        int rc = make_keys(&skey_, &vkey_);
        if (rc != 0)
            exit(1);
        assert(skey_ != NULL);
        if (skey_ == NULL)
            exit(1);
        assert(vkey_ != NULL);
        if (vkey_ == NULL)
            exit(1);

    };

    void encryption_engine::handle_errors(void) {
        ERR_print_errors_fp(stderr);
        abort();
    };

    int encryption_engine::sign_it(const byte* msg, size_t mlen, byte** sig, size_t* slen, EVP_PKEY* pkey) {
        /* Returned to caller */
        int result = -1;

        if(!msg || !mlen || !sig || !pkey) {
            assert(0);
            return -1;
        }

        if(*sig)
            OPENSSL_free(*sig);

        *sig = NULL;
        *slen = 0;

        EVP_MD_CTX* ctx = NULL;

        do
        {
            ctx = EVP_MD_CTX_create();
            assert(ctx != NULL);
            if(ctx == NULL) {
                printf("EVP_MD_CTX_create failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            const EVP_MD* md = EVP_get_digestbyname(hn);
            assert(md != NULL);
            if(md == NULL) {
                printf("EVP_get_digestbyname failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            int rc = EVP_DigestInit_ex(ctx, md, NULL);
            assert(rc == 1);
            if(rc != 1) {
                printf("EVP_DigestInit_ex failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            rc = EVP_DigestSignInit(ctx, NULL, md, NULL, pkey);
            assert(rc == 1);
            if(rc != 1) {
                printf("EVP_DigestSignInit failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            rc = EVP_DigestSignUpdate(ctx, msg, mlen);
            assert(rc == 1);
            if(rc != 1) {
                printf("EVP_DigestSignUpdate failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            size_t req = 0;
            rc = EVP_DigestSignFinal(ctx, NULL, &req);
            assert(rc == 1);
            if(rc != 1) {
                printf("EVP_DigestSignFinal failed (1), error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            assert(req > 0);
            if(!(req > 0)) {
                printf("EVP_DigestSignFinal failed (2), error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            *sig = (byte *)OPENSSL_malloc(req);
            assert(*sig != NULL);
            if(*sig == NULL) {
                printf("OPENSSL_malloc failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            *slen = req;
            rc = EVP_DigestSignFinal(ctx, *sig, slen);
            assert(rc == 1);
            if(rc != 1) {
                printf("EVP_DigestSignFinal failed (3), return code %d, error 0x%lx\n", rc, ERR_get_error());
                break; /* failed */
            }

            assert(req == *slen);
            if(rc != 1) {
                printf("EVP_DigestSignFinal failed, mismatched signature sizes %ld, %ld", req, *slen);
                break; /* failed */
            }

            result = 0;

        } while(0);

        if(ctx) {
            EVP_MD_CTX_destroy(ctx);
            ctx = NULL;
        }

        /* Convert to 0/1 result */
        return !!result;
    };

    int encryption_engine::encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
                unsigned char *iv, unsigned char *ciphertext) {
        EVP_CIPHER_CTX *ctx;

        int len;

        int ciphertext_len;

        /* Create and initialise the context */
        if(!(ctx = EVP_CIPHER_CTX_new())) handle_errors();

        /* Initialise the encryption operation. IMPORTANT - ensure you use a key
         * and IV size appropriate for your cipher
         * In this example we are using 256 bit AES (i.e. a 256 bit key). The
         * IV size for *most* modes is the same as the block size. For AES this
         * is 128 bits */
        if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
            handle_errors();

        /* Provide the message to be encrypted, and obtain the encrypted output.
         * EVP_EncryptUpdate can be called multiple times if necessary
         */
        if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
            handle_errors();
        ciphertext_len = len;

        /* Finalise the encryption. Further ciphertext bytes may be written at
         * this stage.
         */
        if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handle_errors();
        ciphertext_len += len;

        /* Clean up */
        EVP_CIPHER_CTX_free(ctx);

        return ciphertext_len;
    };

    int encryption_engine::decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
                unsigned char *iv, unsigned char *plaintext) {
        EVP_CIPHER_CTX *ctx;

        int len;

        int plaintext_len;

        /* Create and initialise the context */
        if(!(ctx = EVP_CIPHER_CTX_new())) handle_errors();

        /* Initialise the decryption operation. IMPORTANT - ensure you use a key
         * and IV size appropriate for your cipher
         * In this example we are using 256 bit AES (i.e. a 256 bit key). The
         * IV size for *most* modes is the same as the block size. For AES this
         * is 128 bits */
        if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
            handle_errors();

        /* Provide the message to be decrypted, and obtain the plaintext output.
         * EVP_DecryptUpdate can be called multiple times if necessary
         */
        if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
            handle_errors();
        plaintext_len = len;

        /* Finalise the decryption. Further plaintext bytes may be written at
         * this stage.
         */
        if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handle_errors();
        plaintext_len += len;

        /* Clean up */
        EVP_CIPHER_CTX_free(ctx);

        return plaintext_len;
    };

    int encryption_engine::verify_it(const byte* msg, size_t mlen, const byte* sig, size_t slen, EVP_PKEY* pkey) {
        /* Returned to caller */
        int result = -1;

        if(!msg || !mlen || !sig || !slen || !pkey) {
            assert(0);
            return -1;
        }

        EVP_MD_CTX* ctx = NULL;

        do
        {
            ctx = EVP_MD_CTX_create();
            assert(ctx != NULL);
            if(ctx == NULL) {
                printf("EVP_MD_CTX_create failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            const EVP_MD* md = EVP_get_digestbyname(hn);
            assert(md != NULL);
            if(md == NULL) {
                printf("EVP_get_digestbyname failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            int rc = EVP_DigestInit_ex(ctx, md, NULL);
            assert(rc == 1);
            if(rc != 1) {
                printf("EVP_DigestInit_ex failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            rc = EVP_DigestSignInit(ctx, NULL, md, NULL, pkey);
            assert(rc == 1);
            if(rc != 1) {
                printf("EVP_DigestSignInit failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            rc = EVP_DigestSignUpdate(ctx, msg, mlen);
            assert(rc == 1);
            if(rc != 1) {
                printf("EVP_DigestSignUpdate failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            byte buff[EVP_MAX_MD_SIZE];
            size_t size = sizeof(buff);

            rc = EVP_DigestSignFinal(ctx, buff, &size);
            assert(rc == 1);
            if(rc != 1) {
                printf("EVP_DigestVerifyFinal failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            assert(size > 0);
            if(!(size > 0)) {
                printf("EVP_DigestSignFinal failed (2)\n");
                break; /* failed */
            }

            const size_t m = (slen < size ? slen : size);
            result = !!CRYPTO_memcmp(sig, buff, m);

            OPENSSL_cleanse(buff, sizeof(buff));

        } while(0);

        if(ctx) {
            EVP_MD_CTX_destroy(ctx);
            ctx = NULL;
        }

        /* Convert to 0/1 result */
        return !!result;
    };

    void encryption_engine::print_it(const char* label, const byte* buff, size_t len) {
        if(!buff || !len)
            return;

        if(label)
            printf("%s: ", label);

        for(size_t i=0; i < len; ++i)
            printf("%02X", buff[i]);

        printf("\n");
    };

    int encryption_engine::make_keys(EVP_PKEY** skey, EVP_PKEY** vkey) {
        /* HMAC key */
        byte hkey[EVP_MAX_MD_SIZE];

        int result = -1;

        if(!skey || !vkey)
            return -1;

        if(*skey != NULL) {
            EVP_PKEY_free(*skey);
            *skey = NULL;
        }

        if(*vkey != NULL) {
            EVP_PKEY_free(*vkey);
            *vkey = NULL;
        }

        do
        {
            const EVP_MD* md = EVP_get_digestbyname(hn);
            assert(md != NULL);
            if(md == NULL) {
                printf("EVP_get_digestbyname failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            int size = EVP_MD_size(md);
            assert(size >= 16);
            if(!(size >= 16)) {
                printf("EVP_MD_size failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            assert(size <= sizeof(hkey));
            if(!(size <= sizeof(hkey))) {
                printf("EVP_MD_size is too large\n");
                break; /* failed */
            }

            /* Generate bytes */
            int rc = RAND_bytes(hkey, size);
            assert(rc == 1);
            if(rc != 1) {
                printf("RAND_bytes failed, error 0x%lx\n", ERR_get_error());
                break;
            }

            //print_it("HMAC key", hkey, size);

            *skey = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, hkey, size);
            assert(*skey != NULL);
            if(*skey == NULL) {
                printf("EVP_PKEY_new_mac_key failed, error 0x%lx\n", ERR_get_error());
                break;
            }

            *vkey = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, hkey, size);
            assert(*vkey != NULL);
            if(*vkey == NULL) {
                printf("EVP_PKEY_new_mac_key failed, error 0x%lx\n", ERR_get_error());
                break;
            }

            result = 0;

        } while(0);

        OPENSSL_cleanse(hkey, sizeof(hkey));

        /* Convert to 0/1 result */
        return !!result;
    };

    int encryption_engine::hmac_it(const byte* msg, size_t mlen, byte** val, size_t* vlen, EVP_PKEY* pkey) {
        /* Returned to caller */
        int result = -1;

        if(!msg || !mlen || !val || !pkey) {
            assert(0);
            return -1;
        }

        if(*val)
            OPENSSL_free(*val);

        *val = NULL;
        *vlen = 0;

        EVP_MD_CTX* ctx = NULL;

        do
        {
            ctx = EVP_MD_CTX_create();
            assert(ctx != NULL);
            if(ctx == NULL) {
                printf("EVP_MD_CTX_create failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            const EVP_MD* md = EVP_get_digestbyname("SHA256");
            assert(md != NULL);
            if(md == NULL) {
                printf("EVP_get_digestbyname failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            int rc = EVP_DigestInit_ex(ctx, md, NULL);
            assert(rc == 1);
            if(rc != 1) {
                printf("EVP_DigestInit_ex failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            rc = EVP_DigestSignInit(ctx, NULL, md, NULL, pkey);
            assert(rc == 1);
            if(rc != 1) {
                printf("EVP_DigestSignInit failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            rc = EVP_DigestSignUpdate(ctx, msg, mlen);
            assert(rc == 1);
            if(rc != 1) {
                printf("EVP_DigestSignUpdate failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            size_t req = 0;
            rc = EVP_DigestSignFinal(ctx, NULL, &req);
            assert(rc == 1);
            if(rc != 1) {
                printf("EVP_DigestSignFinal failed (1), error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            assert(req > 0);
            if(!(req > 0)) {
                printf("EVP_DigestSignFinal failed (2), error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            *val = (byte *)OPENSSL_malloc(req);
            assert(*val != NULL);
            if(*val == NULL) {
                printf("OPENSSL_malloc failed, error 0x%lx\n", ERR_get_error());
                break; /* failed */
            }

            *vlen = req;
            rc = EVP_DigestSignFinal(ctx, *val, vlen);
            assert(rc == 1);
            if(rc != 1) {
                printf("EVP_DigestSignFinal failed (3), return code %d, error 0x%lx\n", rc, ERR_get_error());
                break; /* failed */
            }

            assert(req == *vlen);
            if(req != *vlen) {
                printf("EVP_DigestSignFinal failed, mismatched signature sizes %ld, %ld", req, *vlen);
                break; /* failed */
            }

            result = 0;

        } while(0);

        if(ctx) {
            EVP_MD_CTX_destroy(ctx);
            ctx = NULL;
        }

        /* Convert to 0/1 result */
        return !!result;
    };

    std::string encryption_engine::encrypt(const std::string &plain_text) {
        unsigned char cipher_text[4096];
        int text_len = encrypt((unsigned char *)plain_text.c_str(), plain_text.length(), encryption_key_, iv_, cipher_text);
        assert(text_len > 0);
        std::string str((const char *)cipher_text, text_len);
        return str;
    };

    std::string encryption_engine::decrypt(const std::string &cipher_text) {
        unsigned char text[4096];
        int text_len = decrypt((unsigned char *)cipher_text.c_str(), cipher_text.length(), encryption_key_, iv_, text);
        assert(text_len > 0);
        return std::string(text, std::find(text, text + text_len, '\0'));
    };

    std::string encryption_engine::hmac(const std::string &key) {
        byte *val = NULL;
        size_t val_len = 256;
        int res = hmac_it((byte *)key.c_str(), key.size(), &val, &val_len, skey_);
        std::string str = std::string((const char *)val, val_len);
        return str;
    };