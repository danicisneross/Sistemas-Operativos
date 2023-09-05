/**
 * @author Jorge Teixeira Crespo (jorge.teixeira)
 * @author Miguel Juncal Paz (miguel.juncalp)
 *
 */

#include <stdio.h>
#include <memory.h>
#include <fcntl.h>
#include <bits/fcntl-linux.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include "create.h"

/**
 * Function: create_cmd
 * ----------------------
 * Creates a file or a directory.
 * If the first argument is "-dir" then creates a directory.
 * Else creates a file.
 *
 * @param tokens array of char* with the arguments received
 * @param ntokens number of tokens received
 * @return 0 when normal, -1 when error.
 */
int create_cmd(char **tokens, int ntokens) {
    int dir_flag = 0;
    if (strcmp(tokens[0], "-dir") == 0) {
        dir_flag = 1;
    } else if (tokens[0][0] == '-') {
        printf("Argument not correct.\n");
        return -1;
    }
    for (int i = dir_flag; i < ntokens; ++i) {
        if (!dir_flag) {
            int fd;
            fd = open(tokens[i], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
            if (fd == -1) {
                printf("Could not create file %s: %s\n", tokens[i], strerror(errno));
                return -1;
            }
            close(fd);
        } else {
            if (mkdir(tokens[i], S_IRUSR | S_IWUSR | S_IXUSR) == -1) {
                printf("Could not create directory %s: %s\n", tokens[i], strerror(errno));
                return -1;
            }
        }

        printf("Created %s succesfully.\n", tokens[i]);
    }
    return 0;
}