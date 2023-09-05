/**
 * @author Jorge Teixeira Crespo (jorge.teixeira)
 * @author Miguel Juncal Paz (miguel.juncalp)
 *
 */


typedef struct memory_struct * mem_list;
typedef struct struct_command command;

mem_list create_memlist();
int pos_in_mem_list(mem_list,char*,char*);
void remove_memlist(mem_list);
int get_size_of_key(mem_list,char*);
void insert_in_memlist(mem_list, void*, unsigned long , char*, char*);
void remove_from_memlist(mem_list, int, int);
void unmap_from_memlist(mem_list ,int);
command* read_from_memlist(mem_list, int);
int n_elements_in_memlist(mem_list);
void print_memlist(mem_list, char*);
void print_sharedmem_key_memlist(mem_list,char*);
void detachShared(mem_list ,int);
void deallocAddr(mem_list ,int);
int findAddr(mem_list, void*);

