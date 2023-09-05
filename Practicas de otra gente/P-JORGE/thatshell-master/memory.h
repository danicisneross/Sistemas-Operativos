/**
 * @author Jorge Teixeira Crespo (jorge.teixeira)
 * @author Miguel Juncal Paz (miguel.juncalp)
 *
 */
#include "listmem.h"

int memory_cmd(char **tokens, int ntokens, mem_list ml);
int memdump_cmd(char **tokens, int ntokens);
int memfill_cmd(char **tokens, int ntokens);
int recurse_cmd(char **tokens, int ntokens);
int readfile_cmd(char **tokens, int ntokens);
int writefile_cmd(char **tokens, int ntokens);
int mem_show(char**tokens, int ntokens,mem_list ml);