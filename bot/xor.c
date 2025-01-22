#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "headers/xor.h"

// XOR encryption key
const char encryption_key[] = "VVagner";

void xor_encrypt_decrypt(char *str) {
    size_t key_length = strlen(encryption_key);
    size_t str_length = strlen(str);
    for (size_t i = 0; i < str_length; i++) {
        if (str[i] != '\0') {
            str[i] ^= encryption_key[i % key_length];
        }
    }
}
