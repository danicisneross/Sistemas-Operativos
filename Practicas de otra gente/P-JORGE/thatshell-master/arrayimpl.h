/**
 * @author Jorge Teixeira Crespo (jorge.teixeira)
 * @author Miguel Juncal Paz (miguel.juncalp)
 *
 */


typedef struct historic_struct *historic;

historic create_historic();
void remove_historic(historic);
void insert_in_historic(historic, char*);
void remove_from_historic(historic, int);
char* read_from_historic(historic, int);
int n_elements_in_historic(historic);


