/**
 * @author Jorge Teixeira Crespo (jorge.teixeira)
 * @author Miguel Juncal Paz (miguel.juncalp)
 *
 */

#include <stdio.h>
#include <memory.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <stdlib.h>
#include <libgen.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include "listimpl.h"


/**
 * Function: LetraTF
 * -----------------
 * Function copied from the exercise assesment.
 * From a file, depending on the file type, returns the char related
 * to the file type.
 *
 * @param m the inode's mode data of a file.
 * @return the caracter to show in long listing, relative to file type.
 */
char LetraTF(mode_t m) {
    switch (m & S_IFMT) { /* and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK:
            return 's';  /* socket */
        case S_IFLNK:
            return 'l';    /* symbolic link */
        case S_IFREG:
            return '-';   /* fichero normal */
        case S_IFBLK:
            return 'b';   /* block device */
        case S_IFDIR:
            return 'd';   /* directorio */
        case S_IFCHR:
            return 'c';   /* char device */
        case S_IFIFO:
            return 'p';   /* pipe */
        default:
            return '?';   /* desconocido, no deberia aparecer */
    }
}

/**
 * Function: ConvierteModo
 * ------------------------
 * Function copied from the exercise assesment.
 * Reads from de inode's mode data, the permissions of  the file, and
 * coverts it to readable string.
 *
 * @param m the inode's mode data of a file.
 * @param permisos the pointer where to write the permissions string formatted
 */
void ConvierteModo(mode_t m, char *permisos) {
    strcpy(permisos, "----------");
    permisos[0] = LetraTF(m);
    if (m & S_IRUSR) permisos[1] = 'r';  /* propietario */
    if (m & S_IWUSR) permisos[2] = 'w';
    if (m & S_IXUSR) permisos[3] = 'x';
    if (m & S_IRGRP) permisos[4] = 'r';   /* grupo */
    if (m & S_IWGRP) permisos[5] = 'w';
    if (m & S_IXGRP) permisos[6] = 'x';
    if (m & S_IROTH) permisos[7] = 'r';   /* resto */
    if (m & S_IWOTH) permisos[8] = 'w';
    if (m & S_IXOTH) permisos[9] = 'x';
    if (m & S_ISUID) permisos[3] = 's';  /*setuid, setgid y stickybit*/
    if (m & S_ISGID) permisos[6] = 's';
    if (m & S_ISVTX) permisos[9] = 't';
}

/**
 * Function: file_date
 * -------------------
 * From a time data type, prints formatted the time in stdin.
 *
 * @param mtim the inode's time data.
 */
void file_date(time_t mtim) {
    char buffer[13];
    struct tm *tc;

    tc = localtime(&mtim);

    strftime(buffer, 13, "%b %d %R", tc);
    printf("%s ", buffer);
}

/**
 * Function: list_file
 * --------------------
 * List the file received, according to the flags.
 *
 * @param filename The file name to print
 * @param rec_flag 1 If recursive listing.
 * @param hid_flag 1 If should show hidden files.
 * @param long_flag 1 If should show with long properties.
 */
void list_file(char *filename, int rec_flag, int hid_flag, int long_flag) {
    struct stat st;
    if (lstat(filename, &st) == -1) {
        printf("Could not list %s: %s\n", filename, strerror(errno));
        return;
    }

    if (filename[0] == '.' && !hid_flag) return;

    if (long_flag) {
        file_date(st.st_mtim.tv_sec);
        printf("%lu ", st.st_ino);
        struct passwd *pwd = getpwuid(st.st_uid);
        struct group *grp = getgrgid(st.st_gid);
        printf("%s ", pwd->pw_name);
        printf("%s ", grp->gr_name);
        char *permisos = malloc(11);
        ConvierteModo(st.st_mode, permisos);
        printf("%s ", permisos);
        free(permisos);
        printf("%6lu ", st.st_size);
        printf("(%3lu) ", st.st_nlink);
    }
    printf("%s", basename(filename));
    if (S_ISLNK(st.st_mode)) {
        char lnk[st.st_size + 1];
        readlink(filename, lnk, st.st_size + 1);
        lnk[st.st_size] = '\0';
        printf(" -> %s", lnk);
    }
    printf("\n");

    if (S_ISDIR(st.st_mode) && rec_flag) {
        DIR *d;
        d = opendir(filename);
        if (d == NULL) return;
        printf("\n%s/ content:\n", filename);
        struct dirent *ent;
        while ((ent = readdir(d)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            char *path = malloc(PATH_MAX);
            strcpy(path, filename);
            strcat(path, "/");
            strcat(path, ent->d_name);
            list_file(path, rec_flag, hid_flag, long_flag);
            free(path);
        }
        closedir(d);
        printf("\n");
    }

}

/**
 * Function: list_cmd
 * ------------------
 * Contains the logic to use the list command.
 *
 * @param tokens The array of arguments.
 * @param ntokens The number of arguments in tokens.
 * @return 0 when normal, -1 when error.
 */
int list_cmd(char **tokens, int ntokens) {

    int rec_flag = 0;
    int hid_flag = 0;
    int long_flag = 0;
    int dir_flag = 0;

    int flags = 0;
    for (int i = 0; i < ntokens; ++i) {
        if (tokens[i][0] == '-') {
            if (strcmp(tokens[i], "-rec") == 0) {
                rec_flag = 1;
                flags++;
            }
            if (strcmp(tokens[i], "-hid") == 0) {
                hid_flag = 1;
                flags++;
            }
            if (strcmp(tokens[i], "-dir") == 0) {
                dir_flag = 1;
                flags++;
            }
            if (strcmp(tokens[i], "-long") == 0) {
                long_flag = 1;
                flags++;
            }
        } else break;
    }

    if (ntokens - flags == 0) {
        DIR *d;
        d = opendir(".");
        if (d == NULL) return -1;
        struct dirent *ent;
        while ((ent = readdir(d)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            list_file(ent->d_name, rec_flag, hid_flag, long_flag);
        }
        closedir(d);
    } else
        for (int i = flags; i < ntokens; ++i) {
            struct stat st;
            lstat(tokens[i], &st);
            if (S_ISDIR(st.st_mode)) {
                if (dir_flag) {
                    DIR *d;
                    d = opendir(tokens[i]);
                    if (d == NULL) return -1;
                    struct dirent *ent;
                    while ((ent = readdir(d)) != NULL) {
                        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
                        char *path = malloc(PATH_MAX);
                        strcpy(path, tokens[i]);
                        strcat(path, "/");
                        strcat(path, ent->d_name);
                        list_file(path, rec_flag, hid_flag, long_flag);
                        free(path);
                    }
                    closedir(d);
                } else {
                    list_file(tokens[i], rec_flag, hid_flag, long_flag);
                }
            } else {
                list_file(tokens[i], rec_flag, hid_flag, long_flag);
            }
        }


    return 0;
}