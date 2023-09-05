/**
 * @author Jorge Teixeira Crespo (jorge.teixeira)
 * @author Miguel Juncal Paz (miguel.juncalp)
 *
 */

#include "deleteimpl.h"

int main(int argc, char *argv[]) {
    delete_cmd(argv + 1, argc - 1);
    return 0;
}