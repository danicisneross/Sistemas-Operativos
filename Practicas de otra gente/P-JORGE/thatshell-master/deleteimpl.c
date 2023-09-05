/**
 * @author Jorge Teixeira Crespo (jorge.teixeira)
 * @author Miguel Juncal Paz (miguel.juncalp)
 *
 */
#include <stdio.h>
#include <memory.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/unistd.h>

int isempty(char *file) {
    DIR *dir = opendir(file);
    int n = 0;

    if (dir == NULL)
        return 1;
    while (readdir(dir) != NULL) {
        if(++n > 2)
            break;
    }
    closedir(dir);
    if (n <= 2)
        return 1;
    else
        return 0;
}


void delete_rec(char *file) {
    struct stat buf;
    DIR *dir;
    struct dirent *entry;

    if (lstat(file, &buf)) {
        printf("Error formato invalido: %s\n", file);
        perror("");
        return;
    }

    if ((buf.st_mode & S_IFMT) == S_IFDIR) {
        if(isempty(file)){
            if(rmdir(file)==-1){  //-1 si hay error borrando, 0 exito
                printf("Error no se puedo eliminar el directorio: %s\n", file);
                perror("");
            }
        }else{
            if (!(dir = opendir(file)))
                return;

            while ((entry = readdir(dir)) != NULL) {
                char path[1024];
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                    continue;
                snprintf(path, sizeof(path), "%s/%s", file, entry->d_name);
                delete_rec(path);
            }
            closedir(dir);
            if(rmdir(file)==-1){
                printf("Error no se puedo eliminar el directorio: %s\n", file);
                perror("");
            }
        }
    }else{
        if(unlink(file)==-1){
            printf("Error no se puedo eliminar el fichero: %s\n", file);
            perror("");
        }
    }
}

void delete_n(char *file) {
    struct stat buf;

    if (lstat(file, &buf)) {
        printf("Error formato invalido: %s\n", file);
        perror("");
        return;
    }

    if ((buf.st_mode & S_IFMT) == S_IFDIR) {
        if(isempty(file)){
            if(rmdir(file)==-1){
                printf("Error no se puedo eliminar el directorio: %s\n", file);
                perror("");
            }else{
                printf("El fichero (%s) fue eliminado",file);
            }
        }else{
            printf("Error no se puedo eliminar el directorio: '%s' por que no esta vacio\n", file);
        }
    }else{
        if(unlink(file)==-1){
            printf("Error no se puedo eliminar el fichero: %s\n", file);
            perror("");
        }else{
            printf("El archivo (%s) fue eliminado",file);
        }
    }
}


void delete_cmd(char **tokens, int ntokens) {
    int i;

    if (ntokens > 0) {
        if (strcmp("-rec", tokens[0]) == 0) {
            printf("%s\n",tokens[1]);
            for (i = 1; i < ntokens; i++) {
                delete_rec(tokens[i]);
            }
        } else {
            for (i = 0; i < ntokens; i++) {
                delete_n(tokens[i]);
            }
        }

    }else{
        printf("Error en el comando");
    }
}

