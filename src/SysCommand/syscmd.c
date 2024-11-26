#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

int sys_cmd(const char *cmd) {
    int ret = system(cmd);

    if (ret != 0) {
        fprintf(stderr, "Error while performing %s\n", cmd);
        return -1;
    }

    return 0;
}