#pragma once

#include "includes.h"


// XOR encryption macro
#define xor_enc(str) ({ \
    char* enc_str = strdup(str); \
    xor_encrypt_decrypt(enc_str); \
    enc_str; \
})

void xor_encrypt_decrypt(char *str);
