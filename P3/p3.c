/**
 * TITLE: Lab Assignment 1
 * @author Daniela Cisneros Sande @login: d.cisneross
 * @author Graciela Méndez Olmos @login: graciela.mendez.olmos@udc.es
 * GROUP: 4.1
 * DATE: 21 / 10 / 2022
 */

#include "p3.h"

/*Variables Globales*/

char *trozos[MAXTROZOS]; //Array para guardar y luego leer lo cada uno de los trozos escritos por terminal 
int numtrozos;           //Lleva la cuenta del numero de palabras introducidas
char linea[MAXLINEA];    //Guarda absolutamente todo lo escrito por terminal 
char ruta[PATH_MAX];     //Array para guardar el path 
char memory;             //utilizada como variable global en el memory 


extern char **environ;


/**
 * Function: funAutores
 * ----------------------
 * Prints the names and logins
 * of the program au malloc 
tors. 
 *
 * Opcions: 
 *      [-l] Prints only the logins. 
 *      [-n] Prints only the names. 
 *
 * @return void.
 */
void funAutores(){
        char* nomes[4] = {"Graciela Méndez Olmos", "Daniela Cisneros Sande", "graciela.mendez.olmos@udc.es", "d.cisneross@udc.es"}; 
        int flaglogin=1, flagnome=1; 
        
        //Verificamos si nos introducen alguna opcion del comando
        if (numtrozos>1){ 
                if(strcmp(trozos[1], "-l")==0){
                        flagnome = 0;
                }
                if(strcmp(trozos[1], "-n")==0){
                        flaglogin = 0;
                }
        }
        //Si solo llama al comando "autores" mostramos los nombres y los logins
        if(numtrozos == 1){
                printf("%s:%s\n%s:%s\n",nomes[1],nomes[3],nomes[0],nomes[2]);
        } else {
                if(flaglogin){
                        printf("%s\n%s\n", nomes[2], nomes[3]); //Opcion -> -l
                } if(flagnome){
                        printf("%s\n%s\n", nomes[0], nomes[1]); //Opcion -> -n
                }                                          
        }
}

/**
 * Function: funPid
 * ----------------------
 * Prints the pid of the process 
 * executing the shell. 
 *
 * Opcions: 
 *      [-p] Prints the pid of the 
 *           shell’s parent process.
 *
 * @return void.
 */
void funPid(){
	if (numtrozos == 1){
                printf("Pid de shell: %d\n", getpid());
        } else if (strcmp(trozos[1],"-p")==0){
                printf("Pid del padre shell: %d\n", getppid());
        } 		
}

/**
 * Function: funCarpeta
 * ----------------------
 * Prints the current working 
 * directory.
 *
 * Opcions: 
 *      [direct] Changes the current 
 *               working directory of 
 *               the shell to direct.                
 *
 * @return void.
 */
void funCarpeta(){       
        if(getcwd(ruta, PATH_MAX)==NULL){ //envia NULL si el nombre del directorio supera el tamaño de la memoria 
                fprintf(stderr, "%s '%s'\n", strerror(errno),trozos[1]);   
        }                                           
        
        if (numtrozos == 1){
                printf("El directorio actual es: %s\n", ruta);
        } else if(numtrozos > 1){ //si se le manda mas de una palabra al comando
                if(chdir(trozos[1]) != 0){ //si no logra hacer el cambio del directorio muestra un error
                        fprintf(stderr, " %s '%s'\n", strerror(2), trozos[1]);
                } 
        }
} 

/**
 * Function: funFecha
 * ----------------------
 * Prints the current date and 
 * the current time.
 *
 * Opcions: 
 *      [-d] Prints only the current date. 
 *      [-h] Prints only the current time.                
 *
 * @return void.
 */
void funFecha(){
	time_t tiempo = time(0);
	struct tm *tlocal = localtime(&tiempo);
	char fecha[MAXFyH];
	char hora[MAXFyH];

        //Ponemos el formato que queremos
	strftime(hora, MAXFyH, "%H:%M:%S", tlocal);  
	strftime(fecha, MAXFyH, "%d/%m/%y", tlocal);

	if (numtrozos == 1){
		printf("%s\n%s\n", hora, fecha);
	} else {
		if(strcmp(trozos[1], "-h")==0){
		        printf("%s\n", hora);
                } else if(strcmp(trozos[1], "-d")==0){
                        printf("%s\n", fecha);
                }
	}
}

/**
 * Function: funHist
 * ----------------------
 * Shows/clears the historic of 
 * commands executed by this shell.
 *
 * Opcions: 
 *      [-c] Clears (empties) the list 
             of historic commands. 
 *      [-N] Prints the first N comands.                
 *
 * @return void.
 */
void funHist(tList *listhistorial){
        int numLista=0, flagbal=0, n;
        tItemL d;
        tPosL pos = first(*listhistorial);
        char* token;
        
        if(numtrozos == 1){
                while(pos != NULL){ //mientras la lista no llegue al final y no este vacia
                        d = getItem(pos,listhistorial);
                        printf("%d->%s\n", numLista, *d.comando);
                        numLista++;
                        pos = next(pos,*listhistorial);
                } 
        }
        if(numtrozos > 1){
                if(strcmp(trozos[1],"-c")==0){
                        flagbal=1;
                }

                if(flagbal){
                        removeElement(listhistorial);
                } else {
                        token = strtok(trozos[1], "-");  //borramos el "-" para solo tener el numero.
                        n = (int) strtol(token, NULL, 10); //convertimos el numero obtenido como string a un entero numerico
                        if(n < 0){
                                fprintf(stderr, "%s\n", strerror(3));
                        } else {
                                while(numLista != n){
                                        d = getItem(pos,listhistorial);
                                        printf("%d->%s\n", numLista, *d.comando);
                                        numLista++;
                                        pos = next(pos,*listhistorial);
                                }
                        }
                }       
        }        
}


/**
 * Function: funInfosis
 * ----------------------
 * Prints information on the machine 
 * running the shell.
 *
 * @return void.
 */
void funInfosis(){
	struct utsname infosisp;
	uname(&infosisp);
	
	printf("%s (%s), OS: %s-%s-%s\n", infosisp.nodename, infosisp.machine, infosisp.sysname, infosisp.release, infosisp.version);
        //nodename: nombre del nodo dentro de la red           machine: identificador del hardware
        //sysname: nombre del sistema operativo                release: lanzamiento del sistema operativo
}

/**
 * Function: funAyuda
 * ----------------------
 * Displays a list of available commands.
 *
 * Opcions: 
 *      [cmd] Gives a brief help on the 
              usage of comand cmd.
 *            
 * @return void.
 */
void funAyuda(){
        int i, flagatopar = 0;
        if (numtrozos > 1){
                for (i = 0; ; i++){
                        if (strcmp(tabla[i].nombre, trozos[1])==0){
                                printf("%s %s\n",trozos[1], tabla[i].info);
                                break;
                        }
                        
                        if (i >= 40){
                                 flagatopar = 1; 
                                 break;
                        }
                }
                
                if (flagatopar){
                        printf(" '%s' no encontrado\n", trozos[1]);
                } 
        } else {
                printf("'ayuda cmd' donde cmd es uno de los siguientes comandos:\n");
                printf("1.autores\n2.pid\n3.carpeta\n4.fecha\n5.hist\n");
                printf("6.infosis\n7.fin\n8.salir\n9.bye\n10.ayuda\n");
                printf("11.create\n12.stat\n13.list\n14.delete\n15.deltree\n");
                printf("16.allocate\n17.deallocate\n18.i-o, i/o, e-s, e/s\n19.memdump\n20.memfill\n21.memory\n22.recurse\n");
        } 
}

//////////////////////////////////// P1 ///////////////////////////////////////////

/**
 * Function: LetraTF
 * ----------------------
 * Checks mode of the file and
 * returns the corresponding letter.
 *
 * @return Mode letter.
 */
char LetraTF (mode_t m)
{
     switch (m&S_IFMT) {           /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK: return 's'; /*socket */
        case S_IFLNK: return 'l';  /*symbolic link*/
        case S_IFREG: return '-';  /*normal file*/
        case S_IFBLK: return 'b';  /*block device*/
        case S_IFDIR: return 'd';  /*directorio */ 
        case S_IFCHR: return 'c';  /*char device*/
        case S_IFIFO: return 'p';  /*pipe*/
        default: return '?';       /*desconocido, no deberia aparecer*/
     }
}

/**
 * Function: ConvierteModo
 * ----------------------
 * Checks permissions of files and returns 
 * the corresponding string.
 *
 * @param m File mode.
 * @param permisos String.
 * @return Updated string with the
 *         corresponding permissions.
 */
char * ConvierteModo (mode_t m, char *permisos){
    strcpy (permisos,"---------- ");
    
    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';    /*resto*/
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&S_ISVTX) permisos[9]='t'; 
    return permisos;
}  

/**
 * Function: funCreate
 * ----------------------
 * Creates files or directories.
 *
 * Opcions: 
 *      [-f] Create a file.             
 *            
 * @return void.
 */
void funCreate(){
        int fich, dir;
        
        if(strcmp(trozos[1], "-f") != 0){ //si no se le introduce -f
                if((dir = mkdir(trozos[1], 0775)) == -1){ //se crea el directorio con los permisos comunes, si falla dara -1
                        perror("No se ha podido crear el directorio.\n");
                }       
        } else {
                if((fich = creat(trozos[2], 0775)) == -1) //se crea el fichero con los permisos comunes, si falla dara -1
                        perror("No se ha podido crear el archivo.\n");
        }
}

/**
 * Function: funStatAux
 * ----------------------
 * Gives information on files 
 * or directories.
 *
 * @param name File or directory name,
 *             type string. 
 * @param Acc options indicator 
 *            type int.
 * @param Link options indicator 
 *             type int.
 * @param Long options indicator 
 *             type int.    
 * @return void.
 */        
void funStatAux(char *name, int Acc, int Link, int Long){
        char *apunta_a;                                    
        char *permisos = malloc(sizeof(char)*PATH_MAX);    
        char buffer[PATH_MAX];                             
        struct tm *time;
        struct stat buf; 
        struct passwd *p;
        struct group *g;
        ssize_t bufsiz, nbytes;

        //examinamos el fichero al que apunta name y si es un enlace simbolico examina el enlace y llena el buf
        if(lstat(name, &buf) == 0){
                time = localtime(&buf.st_mtime); //ultima modificacion 

                if(Acc){
                        time = localtime(&buf.st_atime);//ultimo acceso
                        strftime(buffer, sizeof(buffer), "%Y/%m/%d-%H:%M", time); //indicamos el formato que queremos

                        if(!(Long) && !(Link)){ //si solo ponemos como opcion -acc 
                                printf("%s" , buffer); //mostrara unicamente la ultima hora de acceso y el nombre con el tamaño de dicho fichero
                        }
                }
                strftime(buffer, sizeof(buffer), "%Y/%m/%d-%H:%M", time); //indicamos el formato que queremos

                p = getpwuid(buf.st_uid); // obtenemos el id del usuario propietario
                g = getgrgid(buf.st_gid); //obtenemos el id del grupo propietario
                
                if(p == NULL ||  g == NULL){
                        perror("Error: pxd"); 
                }

                if ((Long && Acc) || Long){ 
                        ConvierteModo(buf.st_mode, permisos); //ponemos los permisos correspondientes en el string
                        printf("%s" , buffer); //imprimimos la fecha y hora 
                        printf("   %ld (%ld)     %s     %s   %s", buf.st_nlink, buf.st_ino, p->pw_name, g->gr_name, permisos);
                        //st_nlink: numero de enlaces fisicos.                 st_ino: inodo  
                        //p->pw_name: user name                                gr_name: group name
                        free(permisos);
                }
                printf("       %ld %s\n", buf.st_size, name); //imprimimos el tamaño y el nombre
                
                if(Link){ 
                        bufsiz = buf.st_size;
                        apunta_a = malloc(bufsiz);
                        nbytes = readlink(name, apunta_a, bufsiz); //coloca el contenido de la ruta del enlace simbolico en apunta_a que tiene tamaño bufsiz.
                        if (nbytes == -1){
                                fprintf(stderr, "%s '%s' for option -link\n", strerror(errno), name);
                        } else {
                                 printf(" -> %s\n", apunta_a); //imprimimos a donde apunta el enlace simbolico   
                        } 
                        free(apunta_a);
                }
        } else {
                fprintf(stderr, "%s '%s'\n", strerror(errno), name);
        }
}

/**
 * Function: funStat
 * ----------------------
 * The options entered by the shells 
 * are evaluated and each of the 
 * files or directories is sent to 
 * the funStatAux function.
 *
 * Opcions: 
 *      [-long] Shows the long  
 *              list of information.             
 *      [-acc] Shows the acesstime.
 *      [-link] Si es enlace simbolico, 
 *              el path contenido.
 * 
 * @return void.
 */
void funStat(){
        int flagAcc = 0, flagLink = 0, flagLong = 0;
        tList ficheiros;
        createList(&ficheiros);
        tItemL d;
        tPosL p ;
        int i = 0;

        for (int k = 1; k < numtrozos; k++){
                
                if(strcmp(trozos[k], "-long") == 0){
                        flagLong = 1;
                } else if (strcmp(trozos[k], "-link") == 0) {
                        flagLink = 1;
                } else if (strcmp(trozos[k], "-acc") == 0){
                        flagAcc = 1;
                } else {
					strcpy(*d.comando, trozos[k]);
                	insertElement(d, &ficheiros);
                }                  
        }
        
        while (p!=NULL){
                funStatAux(*d.comando, flagAcc, flagLink, flagLong);
        }
}

/**
 * Function: funListFiles
 * ----------------------
 * List directories contens.
 *
 * @param directorio directory name,
 *                   type string. 
 * @param Acc options indicator 
 *            type int.
 * @param Link options indicator 
 *             type int.
 * @param Long options indicator 
 *             type int. 
 * @param Hid options indicator 
 *             type int.
 * @return void.
 */
 void funListFiles(char *directorio, int Acc, int Link, int Long, int Hid){
        DIR *dir; 
        struct dirent *d;

        dir = opendir(directorio); //accedemos al directorio
        if (dir) {
                chdir(directorio); //cambia el directorio 
                printf("************%s\n", directorio);
                while ((d = readdir(dir)) != NULL) { //leemos cada entrada del directorio y cuando llega al final muestra NULL
                        if (d->d_name[0] == '.' && !Hid) { //si el nombre del archivo es un punto y hid no esta activo seguimos  
                                continue;
                        }
                        funStatAux(d->d_name, Acc, Link, Long); //llamamos stat para mostrar lo que queremos archivo por archivo.
                }
                //si no es el propio directorio o el principal 
                if (strcmp(directorio, ".") != 0 && strcmp(directorio, "..") != 0) {
                chdir(".."); //abrimos el padre
                }
                closedir(dir); //cerramos el directorio
        }       
}

/**
 * Function: funListRecAux
 * ----------------------
 * List directories contens recursively.
 *
 * @param directorio directory name,
 *                   type string. 
 * @param Acc options indicator 
 *            type int.
 * @param Link options indicator 
 *             type int.
 * @param Long options indicator 
 *             type int. 
 * @param Reca options indicator 
 *             type int.
 * @param Recb options indicator 
 *             type int.
 * @param Hid options indicator 
 *             type int.
 * @return void.
 */
void funListRecAux(char *directorio, int Acc, int Link, int Long, int Reca, int Recb ,int Hid){
        DIR *dir;               //cabecera que contiene operaciones con directorios
        struct dirent *d;
        struct stat buf;

        if (Reca){ //si la opcion es -reca
               funListFiles(directorio, Acc, Link, Long, Hid); //primero listamos los archivos del directorio
        }
        
        dir = opendir(directorio); //accedemos al directorio
        if(dir){
                chdir(directorio); //cambia el directorio
                while((d = readdir(dir)) != NULL){ //leemos cada entrada
                        //controlamos los ocultos
                        if(d->d_name[0] == '.' && !(Hid)){
                                continue;
                        }

                        //no mostramos "." y ".." para evitar una recursividad infinita
                        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0){
                                continue;
                        }
                                        
                        stat(d->d_name, &buf); 
                        if(LetraTF(buf.st_mode) == 'd') { //si encontramos un directorio 
                                funListRecAux(d->d_name, Acc, Link, Long, Reca, Recb, Hid); //volvemos a entrar en la funcion
                        }
                }
                //si no es el propio directorio o el principal 
                if (strcmp(directorio, ".") != 0 && strcmp(directorio, "..") != 0) {
                        chdir(".."); //abrimos el directorio padre     
                }                           
                closedir(dir); 
        }

        if(Recb){ //si la opcion es -recb
                funListFiles(directorio, Acc, Link, Long, Hid); //listamos los archivos del directorio al final
        }
}

/**
 * Function: funList
 * ----------------------
 * The options that were introduced 
 * by the shell are evaluated and the 
 * directories are sent one by one to 
 * funtions to list them.
 *
 * Opcions: 
 *      [-reca] The list is displayed 
 *              recursively (before).   
 *      [-recb] The list is displayed 
 *              recursively (after).
 *      [-hid] It is shown in the 
 *              hidden files list.
 *      [-long] Shows the long  
 *              list of information.             
 *      [-acc] Shows the acesstime.
 *      [-link] Si es enlace simbolico, 
 *              el path contenido.
 * 
 * @return void.
 */
void funList(){
    int flagreca=0, flagrecb=0, flaghid=0, flaglong=0, flaglink=0, flagacc=0, opciones_trozos = 0;
    //char *fileORdir[NAME_MAX];
    struct stat buf;
    int k = 0 ;
	tList ficheiros;
    createList(&ficheiros);
    tItemL d;
    tPosL p ;

        for (int i = 1; i < numtrozos ; i++){
                if(strcmp(trozos[i], "-long") == 0){
                        flaglong = 1;
                } else if (strcmp(trozos[i], "-link") == 0){
                        flaglink = 1;
                } else if (strcmp(trozos[i], "-acc") == 0){
                        flagacc = 1;
                } else if (strcmp(trozos[i], "-hid") == 0){
                        flaghid = 1;
                } else if (strcmp(trozos[i], "-reca") == 0){ 
                        flagreca = 1;
                } else if (strcmp(trozos[i], "-recb") == 0){
                        flagrecb = 1;
                } else { 
                        if (stat(trozos[i], &buf) == -1){
                        perror("Error: ");
                        continue;
                		}
                        strcpy(*d.comando, trozos[i]); //sobreescribimos
             			insertElement(d, &ficheiros);  //insertanos novo ou o primeiro ficheiro
                }   
        } 
        
        opciones_trozos = flaglong + flaglink + flagacc + flaghid + flagreca +  flagrecb;
        if(numtrozos == opciones_trozos + 1){ 
                funCarpeta();
        }

		p=first(ficheiros);

        while(p!=NULL){  
				d = getItem(p,&ficheiros);
				printf("%s\n",*d.comando);
				
				if(lstat(*d.comando, &buf) == 0){
						
                 //comprobamos que es un directorio
                if(LetraTF(buf.st_mode) == 'd'){       
                        if(!(flagreca) && !(flagrecb)){
                                funListFiles(*d.comando, flagacc, flaglink, flaglong, flaghid);
                        }
                        
                        if(flagrecb || flagreca){ 
                                funListRecAux(*d.comando, flagacc, flaglink, flaglong, flagreca, flagrecb, flaghid);
                        }

                } else { 
                        //si es un archivo, entonces llamamos a Stat
                        funStatAux(*d.comando, flagacc, flaglink, flaglong);
                }
				}
				p= next(p,ficheiros);
        }
}

/**
 * Function: funDelete
 * ----------------------
 * Deletes files and/or empty directories.
 *
 * @return void.
 */
void funDelete(){
        int i;
        for(i = 1; i < numtrozos; i++){
                if(remove(trozos[i]) != 0)
                        fprintf(stderr, "%s '%s'\n", strerror(errno), trozos[i]); //devolve error se o directorio esta baleiro ou non tes permiso.
        }
        if (numtrozos == 1)
                funCarpeta();     
}

/**
 * Function: funAuxDelRec
 * ----------------------
 * Auxiliary function used to 
 * recursively delete a non-empty 
 * directory.
 *
 * @param directorio directory name,
 *                   type string.
 * @return void.
 */
void funAuxDelRec(char* directorio){                                        
        DIR * dir;
        struct dirent *d;
        char direccion[PATH_MAX];
                
        if((dir = opendir(directorio)) != NULL){ //Si podemos abrirlo
                while((d = readdir(dir)) != NULL){ //leemos cada entrada del directorio
                        //cojemos la direccion de cada entrada
                        strcpy(direccion, directorio);
                        strcat(direccion,"/");
                        strcat(direccion, d->d_name);
                        //si no es el propio directorio o el principal 
                        if ((strcmp(d->d_name,"..") != 0) && (strcmp(d->d_name,".") != 0)){
                                if(d->d_type == DT_DIR)   //se es un directorio                                 
                                        funAuxDelRec(direccion); //volvemos recursivamente a ingresar en ese directorio            
                                else {                                   
                                        if(remove(direccion) != 0) //borramos el contenido
                                                fprintf(stderr, "%s '%s'\n", strerror(errno), direccion );
                                }                                        
                        }
                } //para borrar o directorio actual
                if(remove(directorio) != 0) 
                        fprintf(stderr, "%s '%s'\n", strerror(errno), direccion );
        } else {
                fprintf(stderr, "%s '%s'\n", strerror(errno), direccion );    
        }           
}

/**
 * Function: funDeltree
 * ----------------------
 * Deletes files and/or non empty 
 * directories recursively.
 *
 * @return void.
 */
void funDeltree(){
        for(int i = 1; i < numtrozos; i++){
                if(remove(trozos[i]) != 0){
                        funAuxDelRec(trozos[i]);
                }
        }
        if(numtrozos == 1){
                funCarpeta();
        }
}

//////////////////////////////////////////////////////P2///////////////////////////////////////////////////////////////////////

/**
 * Function: Recursiva
 * ----------------------
 * Calls the recursive function 
 * n times.
 *
 * @param n number of times to run.
 *
 * @return void.
 */
void Recursiva (int n){
        char automatico[TAMANO];
        static char estatico[TAMANO];

        printf ("parametro:%3d(%p) array %p, arr estatico %p\n", n, &n, automatico, estatico);

        if (n > 0){
                Recursiva(n-1); 
        }
}

/**
 * Function: LlenarMemoria
 * -----------------------
 * Fills the memory at the address 
 * and with the indicated size (bytes) 
 * with the indicated character. 
 *
 * @param dir direction from which 
 *            it will be filled.
 * @param tam size with which to fill 
 *            the memory.
 * @param character with which we fill 
 *                  the memory.
 *
 * @return void.
 */
void LlenarMemoria(char *dir, int tam, char character){
        int i;

        printf("Llenando %d bytes de memoria con el char '%c' a partir de la direccion %p\n", tam, character, dir);

        for (i = 0; i < tam; i++){
                dir[i] = character; //LLena con el caracter cada espacio de la memoria en dicha direccion
        }	            
}

/**
 * Function: Do_pmap
 * -----------------------
 * Map the current process.
 *
 * @return void.
 */
void Do_pmap(){ 
        pid_t pid;       /*hace el pmap (o equivalente) del proceso actual*/
        char elpid[32];
        char *argv[3] = {"pmap", elpid, NULL};
        
        sprintf (elpid, "%d", (int) getpid());          //Imprime el pid con el formato de entero metiendolo en elpid
        if ((pid = fork()) == -1){                      //FORK: crea un clon idéntico del proceso que la ejecutó.
                perror ("Imposible crear proceso");
                return;
        }
        if (pid == 0){
                if (execvp(argv[0], argv) == -1){
                        perror("Cannot execute pmap (linux, solaris)");
                }
                exit(1);
        }
        waitpid(pid, NULL, 0); //Espera por el final del proceso
 }

/**
 * Function: printListMm
 * -----------------------
 * Displays the necessary list 
 * depending on the function that 
 * calls it.
 *
 * @param L list of memory information. 
 * @param tipo option to execute of the  
 *             function that calls it. 
 *
 * @return void.
 */
void printListMm(tListMem L, char *tipo){
        int all = 1;                                    //flag para ver si imprimo la lista completa [-All]
        
        printf("******Lista de bloques asignados "); 
        if (strcmp("all", tipo) != 0){                  //si el tipo no es -all
                printf("%s ", tipo);                    //imprimo el tipo de lista que se quiere
                all = 0;
        }
        printf("para el proceso %d\n\n", getpid());
         
        if(!isEmptyListm(L)){                           //si la lista no esta vacia
                tPosMem p = firstm(L);                  
                tItemMem d;
                while (p != NULL){                      //mientras la lista tenga elementos
                        d = getItemm(p, L);             
                        if(strcmp(d.tipo, tipo) == 0 || all){  
                                printf("   %p%10d  %s ", d.direc, d.tam, d.tempo);  //tempo: fecha y hora de creacion
                                if(strcmp(d.tipo,"mmap") == 0 )
				        printf("%s (descriptor %d)",d.nomefich, d.df); //df: descriptor -> num que asigna el kernel indicando desde donde se lee y envia bytes en la pila
			        if(strcmp(d.tipo, "shared") == 0 || strcmp(d.tipo, "malloc") == 0)
				        printf("%s", d.tipo);
                                if (strcmp(d.tipo, "shared") == 0)
                                        printf("(key %d)", d.chave);  
                                printf("\n");           
                        }
                        p = nextm(p, L);
		}
        } 
}

/**
 * Function: AuxInsertElememMem
 * -----------------------
 * Helper function to insert 
 * elements to the memory list 
 * according to the parameters.
 *
 * @param dire memory address.
 * @param tam frame size.
 * @param tipo option to execute of the  
 *             function that calls it. 
 * @param chave key if it is a shared 
 *              or mapped block.  
 * @param nfich file name.
 * @param df file descriptor.
 * @param L list of memory information. 
 *
 * @return void.
 */
void AuxInsertElememMem(void *dire, int tam, char *tipo, int chave, char *nfich, int df, tListMem *L){
        tItemMem d;
        time_t data_t;
        time(&data_t);
        struct tm *data;
        int mmap = 0, shared = 0;

        if(strcmp(tipo, "shared") == 0){
                shared = 1;
        }

        if(strcmp(tipo, "mmap") == 0){
                mmap = 1;
        }

        if ((data = localtime(&data_t)) == NULL){
                perror("time\n");
        }     

        strcpy(d.tempo, asctime(data));         //convertimos la fecha y hora a string y lo colocamos en d.tempo 
        d.direc = dire;                         //ponemos el valor de la direccion
        d.tam = tam;                            //ponemos el valor del tamaño 
        strcpy(d.tipo, tipo);                   //copiamos en d.tipo la opcion
        d.chave = chave;                        //en shared o mmap añadimos la clave, si no es 0
        d.df = df;                              //descriptor de fich, que se non es mmap é -1
        if (mmap){
                strcpy(d.nomefich, nfich);      //en mmap ponemos tambien el nombre del fichero
        }
        //Insertamos el elemento
        insertElementm(d, L);
}


/**
 * Function: ObtenerMemoriaShmget
 * ------------------------------
 * Helper function to get 
 * memory segment.
 *
 * @param clave key of a shared 
 *              block.
 * @param tam frame size.   
 * @param flag indicator that allows 
 *             us to know if the key is shared.
 * @param L list of memory information. 
 *
 * @return void.
 */
void * ObtenerMemoriaShmget (key_t clave, size_t tam, int flag, tListMem *L){ //flag para insertar na lista o tamaño segun se é created = 0 ou shared = 1
    void * p;                                           //sera la memoria 
    int aux, id, flags = 0777;
    struct shmid_ds s;
    if (tam){                                           /*tam distinto de 0 indica crear */
        flags = flags | IPC_CREAT | IPC_EXCL;
    }                                            
        
    if (clave == IPC_PRIVATE){                           /*no nos vale*/
        errno = EINVAL; 
        return NULL;
    }

    if ((id = shmget(clave, tam, flags)) == -1){         //shmget devuelve el id de la clve que se le pasa, si no es creada devuelve -1
        return (NULL);
    }    

    if ((p = shmat(id, NULL, 0)) == (void*) -1){        //unha vez que sei o identificador, miro a ver cal é a súa dir de memoria
        aux = errno;
        if (tam || s.shm_segsz){                         //se o tam non é cero : (Nota: cando é shared só teño a chave, por ende para obter o tamaña uso s.shm_segz)
             shmctl(id, IPC_RMID, NULL);                //permite que poda recibir a info so segmento
        }     
        errno = aux;
        return (NULL);
    }

    shmctl (id,IPC_STAT, &s);                            //permite que poda recibir a info so segmento
    if(flag){
        tam = s.shm_segsz;
    }
    AuxInsertElememMem(p, tam, "shared", clave, NULL, -1, L);
    return (p);
}

/**
 * Function: do_AllocateCreateshared
 * ---------------------------------
 * Helper function that we use to 
 * allocate with shared keys 
 * (createshared and shared).
 *
 * @param L list of memory information. 
 *
 * @return void.
 */
void do_AllocateCreateshared (tListMem *L){
        key_t cl;
        size_t tam;
        tItemMem d;
        tPosMem p = firstm(*L);
        int flagCreate = 0, flagShared = 0;                             //flags para distinguir cuando crear clave y asignar segmento de memoria compartida o solo asignar

        if(strcmp(trozos[1], "-shared") == 0){
                flagShared = 1;
        }

        if(strcmp(trozos[1], "-createshared") == 0){
                flagCreate = 1;
        }     

        if (((numtrozos == 2) && flagShared) || ((numtrozos < 3) && flagCreate)){
                printListMm(*L, "shared");  
        } else {
                cl = (key_t)  strtoul(trozos[2], NULL, 10);                                     //convertimos la clave en un tipo unsigned en base 10
                
                if(trozos[3] != NULL){
                        tam = (size_t) strtoul(trozos[3], NULL, 10);                            //para convertilo en un unsigned long
                } else {
                        tam = 0;
                }

                if ((tam == 0) && flagCreate) {                                                 //si se me pasan 0 bytes
                        perror("No se asignan bloques de 0 bytes\n");
                        return;
                }

                if (flagCreate){                                                                //si tengo que crear una clave
                        if ((p = ObtenerMemoriaShmget(cl, tam, 0, L)) != NULL){
                                printf("Asignados %lu bytes en %p\n", (unsigned long) tam, p);
                        } else {
                                printf("Imposible asignar memoria compartida clave %lu:%s\n", (unsigned long) cl, strerror(errno));
                        }
                }

                if (flagShared){                                                                //Si no
                        if ((p = ObtenerMemoriaShmget(cl, tam, 1, L)) != NULL){
                                printf("Memoria compartida de clave %lu en %p\n", (unsigned long) cl,p);
                        } else {
                                perror("Imposible obtener memoria shmget\n");
                        }
                } 
        }
}

/**
 * Function: MapearFichero
 * -----------------------
 * Helper function that maps files.
 *
 * @param fichero file.
 * @param protection protection.
 * @param L list of memory information. 
 *
 * @return void.
 */
void * MapearFichero (char *fichero, int protection, tListMem *L){
    int df, map = MAP_PRIVATE, modo = O_RDONLY;
    struct stat s;
    void *p;
    int tam;

    if (protection&PROT_WRITE){
          modo = O_RDWR;                                                   //puedo leer y escribir si tengo permisos
    }

    if (stat(fichero, &s) == -1 || (df = open(fichero, modo)) == -1){
          return NULL;
    }

    if ((p = mmap(NULL, s.st_size, protection, map, df, 0)) == MAP_FAILED){ //si puedo 
           return NULL;
    }

    tam = s.st_size;                                                       //guardo el tamaño del fichero en la variable tam
    AuxInsertElememMem(p, tam, "mmap", 0, fichero, df, L);
    return p;
}

/**
 * Function: do_AllocateMmap
 * -------------------------
 * Helper function in which 
 * we allocate a mapped 
 * memory block.
 *
 * @param L list of memory information. 
 *
 * @return void.
 */
void do_AllocateMmap(tListMem *L){ 
        char *perm;
        void *p;
        int protection = 0;
        tItemMem d;
        tPosMem pos = firstm(*L);
        
        if((numtrozos == 2 && strcmp(trozos[1], "-mmap") == 0)){
                printListMm(*L,"mmap");
        } else {
                if (((perm = trozos[3]) != NULL) && (strlen(perm) < 4)) {           //strlen: devuelve la longitud de una cadena de texto
                        if (strchr(perm,'r') != NULL) protection|=PROT_READ;      
                        if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
                        if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;      
                }

                if ((p = MapearFichero(trozos[2], protection, L)) == NULL){      //Se non podo mapear
                        perror ("Imposible mapear fichero");
                } else {                                                         // Se si
                        printf ("fichero %s mapeado en %p\n", trozos[2], p);
                }
        }   
}

/**
 * Function: do_AllocateMalloc
 * ---------------------------
 * Allocate a block of 
 * memory with malloc.
 *
 * @param L list of memory information. 
 *
 * @return void.
 */
void do_AllocateMalloc(tListMem *L){
        void *dire;     //direcion de memoria donde se va a asignar el bloque
        int tama;       //tamaño del bloque 

        if (trozos[2] != NULL){
                tama = atoi(trozos[2]);         // para pasar el string a entero
                if (tama > 0){
                        if ((dire = malloc(tama)) != NULL){ // se puedo asignar memoria
                                AuxInsertElememMem(dire, tama, "malloc", 0, NULL, -1, L);
                                printf("Asignados %d bytes en %p\n", tama, dire);
                        } else {
                                perror("malloc");
                        }
                } else if (trozos[2] < 0) {
                        printf("uso: allocate [-malloc|-shared|-createshared|-mmap] ....\n");
                } else {
                        printf("No se asignan bloques de 0 bytes\n");
                }
        } else {
                printListMm(*L, "malloc");       
        }
}

/**
 * Function: funAlloc
 * ---------------------
 * Determine how we are 
 * going to allocate the 
 * memory block.
 *
 * @param L list of memory information. 
 *
 * @return void.
 */
void funAlloc(tListMem *L){      
        if (numtrozos > 1){
                
                if(strcmp("-malloc", trozos[1]) == 0){
                        do_AllocateMalloc(L);
                } else if(strcmp("-mmap", trozos[1]) == 0){
                        do_AllocateMmap(L);
                } else if((strcmp("-shared", trozos[1]) == 0) || (strcmp("-createshared", trozos[1]) == 0)){
                        do_AllocateCreateshared(L);
                } else {
                        printf("uso: allocate [-malloc|-shared|-createshared|-mmap] ....\n");
                }
        } else {
                printListMm(*L, "all");         //si solo se pone "allocate"
        }
}

/**
 * Function: do_DeallocateDelkey
 * -----------------------------
 * Helper function that deletes 
 * the keys created with createshared.
 *
 * @return void.
 */
void do_DeallocateDelkey(){
        key_t clave;
        int id;
        char *key = trozos[2];

        if (key == NULL || (clave = (key_t) strtoul(key, NULL, 10)) == IPC_PRIVATE){
                printf ("      delkey necesita clave_valida\n");
                return;
        }

        if ((id = shmget(clave, 0, 0666)) == -1){
                perror ("shmget: imposible obtener memoria compartida");
                return;
        }

        if (shmctl(id, IPC_RMID, NULL) == -1){ //establezco con shmctl que a chave que teña ese identificador sexa borrada ( con ipc_rmid) 
                perror ("shmctl: imposible eliminar memoria compartida\n");
        }        
}

/**
 * Function: do_Deallocate
 * ------------------------
 * Designates the block of 
 * memory that has the address 
 * passed by parameter.
 *
 * @param L list of memory information. 
 *
 * @return void.
 */
void do_Deallocate(tListMem *L, int flag){      //flag = 0 si es para hacer deallocate con direccion, flag = 1 se es para salir y hacer free a toda la lista
        char *dir;
        tPosMem p;
        tItemMem d;

        if(!flag && sscanf(trozos[1], "0x%p", &dir) == 0){                  //comprobo se a dir que me dan é valida
                perror("Dirección no válida\n"); 
                return;
        }

        if(!isEmptyListm(*L)){
                p = firstm(*L);
                if(!flag){
                        while (p != NULL){                                        //recorro a lista para ver se teño esa dirección
                                d = getItemm(p, *L);
                                if (strcmp(dir, d.direc) == 0){                    // salgo se a encontro
                                        break;
                                }
                                p = nextm(p,*L);            
                        }
                
                        if(p != NULL){                                            //se p non é null porque encontrouse a dirección e existe na miña lista
                                if(strcmp("malloc", d.tipo) == 0){
                                        free(d.direc);
                                }else if(strcmp("shared", d.tipo) == 0){
                                        shmdt(d.direc);
                                }else if( strcmp("mmap", d.tipo) == 0){
                                        munmap(d.direc, d.tam);
                                }else{
                                        printf("\n");
                                }
                                removeElementm(L, p);
                        }else{
                                printf("Dirección %s no asignada con malloc, shared o mmap\n", trozos[1]);
                        }
                }else{
                        while (p != NULL){                                      // recorro a lista para liberar mem
                                d = getItemm(p,*L);
                                if( (strcmp("malloc", d.tipo) == 0)){
                                        free(d.direc);
                                }else if(strcmp("shared", d.tipo) == 0){
                                        shmdt(d.direc);
                                }else if( strcmp("mmap", d.tipo) == 0){
                                        munmap(d.direc, d.tam);
                                }else{
                                        printf("\n");
                                }
                                removeElementm(L,p); 
                                p = nextm(p,*L);         
                        }
                
                }
                
        }else{
                if(!flag){
                        printf("Dirección %s no asignada con malloc, shared o mmap\n", trozos[1]);
                }
        }
}

/**
 * Function: do_DeallocateMalloc
 * -----------------------------
 * Designate the block of memory 
 * with a given size created by malloc.
 *
 * @param L list of memory information. 
 *
 * @return void.
 */
void do_DeallocateMalloc(tListMem *L){
        tPosMem p;
        tItemMem d;

        if (numtrozos == 2){
                printListMm(*L, "malloc");
        }else{
                int tama = atoi(trozos[2]);
                p = firstm(*L);
                while (p != NULL){
                        d = getItemm(p,*L);
                        if (strcmp(d.tipo,"malloc") == 0){       // como recorro toda a lista teño que indicar que me mire se é malloc
                                if (d.tam == tama ){             // Se encontra un tamaño igual, colleme ese para borralo
                                        break;
                                }        
                        }                
                        p = nextm(p,*L);
                }
                
                if (p != NULL){
                        free(d.direc);
                        removeElementm(L,p);
                } else {
                        printListMm(*L, "malloc");
                }
        }
}

/**
 * Function: do_DeallocateShared
 * -----------------------------
 * Designate the blocks in which 
 * they have been assigned with 
 * the shared option.
 *
 * @param L list of memory information. 
 *
 * @return void.
 */
void do_DeallocateShared(tListMem *L){
        tPosMem p;
        tItemMem d;
        int s;
        if (numtrozos == 2){
                printListMm(*L,"shared");
        } else {
                int chave = atoi(trozos[2]);
                p = firstm(*L);
                
                //recoro a lista para ver se atopo a chave 
                while (p != NULL){
                        d = getItemm(p,*L);
                        if (strcmp(d.tipo,"shared") == 0){
                                if (d.chave == chave)
                                        break;
                        }
                        p = nextm(p,*L);
                }
                //si la encuentro 
                if (p != NULL){
                        s = shmdt(d.direc);
                        if (s != -1){
                                removeElementm(L,p);
                        }else{
                                perror("shmdt: Imposible desmapear el bloque de memoria compartida\n");
                        }
                }else{ //si no existe 
                        printf("No hay bloque de esa clave mapeado en el proceso\n");
                }        
        }
}

/**
 * Function: do_DeallocateMmap
 * ---------------------------
 * Designate the allocated memory 
 * block with the mmap option 
 * with the given file.
 *
 * @param L list of memory information. 
 *
 * @return void.
 */
void do_DeallocateMmap(tListMem *L){
        tPosMem p;
        tItemMem d;

        if (numtrozos == 2){
                printListMm(*L,"mmap");
        } else {
                p = firstm(*L);
                while (p != NULL){                                              // recorro la lista para ver si alguna direccion tiene ese fichero
                        d = getItemm(p,*L);
                        if(strcmp(d.tipo,"mmap") == 0){
                                if(strcmp(trozos[2], d.nomefich) == 0)
                                        break;
                        }
                        p = nextm(p,*L);
                }

                if (p != NULL){                                                 //se encuentro el fichero en la lista
                        munmap(d.direc, d.tam);                                 //munmap: borra las ubicaciones para el rango de direcciones especificado; produce referencias a las direcciones dentro del rango a fin de generar referencias a memoria inválidas.
                        removeElementm(L, p);
                } else {
                        printf("Fichero %s no mapeado\n", trozos[2]);        
                }
        }
}

/**
 * Function: funDealloc
 * ---------------------------
 * Determine how we are 
 * going to designate the 
 * memory block.
 *
 * @param L list of memory information. 
 *
 * @return void.
 */
void funDealloc(tListMem *L, int flag){
        if (numtrozos>1){
                
                if(strcmp("-malloc", trozos[1]) == 0){
                        do_DeallocateMalloc(L);
                } else if(strcmp("-mmap", trozos[1]) == 0){
                        do_DeallocateMmap(L);
                } else if((strcmp("-shared", trozos[1]) == 0)){
                        do_DeallocateShared(L);
                } else if((strcmp("-delkey", trozos[1]) == 0)){
                        do_DeallocateDelkey();
                } else {
                        do_Deallocate(L, flag);
                }
        }else{
                if(flag){
                        do_Deallocate(L, flag);
                }else{
                        printListMm(*L, "all");         //si solo le pasamos "deallocate"
                }
        }        
}

/**
 * Function: LeerFichero
 * -----------------------
 * Read files.
 *
 * @param f file name. 
 * @param p memory adreess. 
 * @param cont file size. 
 *
 * @return ssize_t.
 */
ssize_t LeerFichero (char *f, void *p, size_t cont){
        struct stat s;
        ssize_t  n;  
        int df, aux;

        if (stat (f, &s) == -1 || (df = open(f, O_RDONLY)) == -1){       //se non se pode abrir 
                return -1;
        }

        if (cont == -1){                                                   /* si pasamos -1 como bytes a leer lo leemos entero*/
                cont = s.st_size;
        }

        if ((n = read(df, p, cont)) == -1){                             // se non se pode ler
                aux = errno;
                close(df);
                errno = aux;
                return -1;
        }
        close (df);
        return n;    
}

/**
 * Function: do_I_O_read
 * ----------------------
 * Read files with command i-o.
 *
 * @return void.
 */
void do_I_O_read (){
   void *p;
   size_t cont = -1;
   ssize_t n;

   if (numtrozos < 5){
	printf ("faltan parametros\n");
	return;
   }
   
   p = (void*) strtoul(trozos[3], NULL, 16);            /*convertimos de cadena a puntero*/

   if (trozos[4] != NULL){                               //Comprobación para que non dei erro atoll
	cont = (size_t) atoll(trozos[4]);               //atoll pasa de string a long
   }     

   if ((n = LeerFichero(trozos[2], p, cont)) == -1){     //Se temos algún erro ao ler 
	perror ("Imposible leer fichero");
   } else {                                                 //Se lemos 
	printf ("leidos %lld bytes de %s en %p\n", (long long) n, trozos[2], p);
   }     
}

/**
 * Function: EscribirFichero
 * --------------------------
 * Write and overwrite files.
 *
 * @param f file name. 
 * @param p memory adreess. 
 * @param cont file size. 
 * @param overwrite flag if are 
 *                  overwrite.
 * 
 * @return void.
 */
ssize_t EscribirFichero (char *f, void *p, size_t cont, int overwrite){
        ssize_t  n;
        int df, aux, flags=O_CREAT | O_EXCL | O_WRONLY; 

        if (overwrite)                                       // se teño a opción -o de sobrescribir 
                flags=O_CREAT | O_WRONLY | O_TRUNC;          //doulle estes permisos, porque O_TRUNC permite que un ficheiro existente trunque a lonxitude
        //se non, quedome con O_EXCL que combinado co de creación solta unn erro se xa existe o ficheiro
        if ((df = open(f, flags, 0777)) == -1)
                return -1;

        if ((n = write(df, p, cont)) == -1){                 //escribo fich
                aux = errno;
                close(df);
                errno = aux;
                return -1;
        }
        close (df);
        return n;
}

/**
 * Function: do_I_O_write
 * -----------------------
 * Write files with command i-o.
 *
 * @return void.
 */
void do_I_O_write(){
        void *p;
        size_t cont = -1;
        ssize_t n;

        if ((numtrozos < 5 && strcmp("-o", trozos[2]) != 0) || (numtrozos < 6 && strcmp("-o", trozos[2]) == 0)){
                printf("Faltan parametros\n");
                return;
        }else{  
                if((strcmp(trozos[2],"-o") == 0)){                        // Se teño a opción de sobrescribir

                        p = (void*) strtoul(trozos[4], NULL, 16);         // paso a cadea a punteiro
                
                        if (trozos[5] != NULL){
	                        cont = (size_t) atoll(trozos[5]);         //paso o número de bytes a ler de cadea a long
                        }                          
    		
                        if((n = EscribirFichero(trozos[3], p, cont, 1)) == -1){ // Se falla a escritura:
    		        	perror("Imposible escribir el fichero\n");
                        }else{                                                   // Se non, escribo
    		                printf("escritos %s bytes en %s desde %p\n", trozos[5], trozos[3], p);
                        }        
  	        }else{ //Se non sobreescribo fago o mesmo pero con trozos nunha posición menos porque non teño -o
    		        p = (void*) strtoul(trozos[3], NULL, 16);
                        if (trozos[4] != NULL){
                                cont = (size_t) atoll(trozos[4]);
                        }

    	        	if((n = EscribirFichero(trozos[2], p, cont, 0)) == -1){
    		        	perror("Imposible escribir el fichero\n");
    			        return;
    	        	}else{
    		        	printf("escritos %s bytes en %s desde %p\n",trozos[4],trozos[2],p);
                        }
                }
        }
}

/**
 * Function: funIo
 * -----------------------
 * Write or read from an address 
 * to a file and viceversa.
 *
 * @return void.
 */
void funIo(){
        if (numtrozos == 1){
                printf("uso: e-s [read|write] ......\n");
        }else{
                if(strcmp(trozos[1],"read") == 0){ //para leer
                        do_I_O_read();
                }        
                else if (strcmp(trozos[1],"write")==0){ // para escribir
                        do_I_O_write();
                }else{
                        printf("uso: e-s [read|write] ......\n");
                }
        }
}

/**
 * Function: funMemDump
 * -----------------------
 * Dumps the contents of 
 * memory to the screen.
 *
 * @return void.
 */
void funMemDump(){
    int bytes, cont = 0, cont1 = 0, cont2 = 0;
    char *dir;

    if (trozos[2] == NULL){
        bytes = 25;              //si no especifican cont, mostramos los 25 primeros bytes
    } else {
        bytes = atoi(trozos[2]); //convertimos el string de los bytes a volcar en int
    }

    dir = (char *) strtoul(trozos[1], NULL, 16); //guardamos en dir la conversion a base 16 de la direccion que recibimos
    
    printf("Volcando %d bytes desde la direccion %p\n", bytes, dir);

    while (cont < bytes) {

        if (isprint(dir[cont])){                //comprueba si el carácter es un carácter imprimible o no             
                printf("%2c ", dir[cont]);      //imprimos los caracteres
        }
        else{
                printf("   ");                  //si no es un caracter imprimible, un espacio
        } 
        cont++;

        if (cont % 25 == 0 || cont >= bytes) {  //cada vez que llega a 24 comenzando en 0 se pasa a la siguiente linea
            printf("\n");
            cont1 = 0;
            while (cont1 < 25 && cont2 < bytes) {
                printf("%02x ", (unsigned char) dir[cont2]);    //imprimimos el char en hexadecimal
                cont2++;
                cont1++;
            }
            printf("\n");
        }

    }
    printf("\n");
}

/**
 * Function: funMemFill
 * -----------------------
 * Fills the memory with 
 * one character.
 *
 * @return void.
 */
void funMemFill(){
        int tam = 128;
        char *dir;
        char word = 'A';        //por defecto si no se le pasa ninguna letra es A 
        int character;          //variable que indica en que posicion se encuentra el char

        if(numtrozos != 1 && numtrozos < 5){                  
                dir = (char*) strtol(trozos[1], NULL, 16);                      //guardamos en dir la conversion a base 16 de la direccion que recibimos
        
                if(numtrozos == 2){
                        LlenarMemoria(dir, tam, word);                          //solo se le pasa la direccion 
                        return;
                }
                
                if(numtrozos == 3){
                        tam = atoi(trozos[2]);                                  //convierte el string en un entero y lo metemos en tam 
                        if(tam != 0){                        
                                LlenarMemoria(dir, tam, word);                  //se le pasa direccion y numero de bytes con los que llenamos 
                                return;
                        }
                        character = 2;                                  
                        tam = 128;
                }         
                        
                if(numtrozos == 4){
                        tam = atoi(trozos[2]);                                  //convierte el tam en int 
                        character = 3;
                } 

                if(trozos[character][1] == 'x') {                               //miramos si esta pasado en hexadecimal
                        word = (char) strtol(trozos[character], NULL, 16);      //guardamos en word la conversion del num a hexadecinal (tabla assci) que recibimos
                } else {
                        word = (char) trozos[character][0];                     //pasamos el char 
                }

                LlenarMemoria(dir, tam, word);                                  //pasamos todo
        } else {
                printf("Numero de argumentos invalidados\n");
        }
}

/**
 * Function: funMemory
 * -----------------------
 * Shows information on the 
 * memory of the process.
 *
 * @param L list of memory information.
 *  
 * @return void.
 */
void funMemory(tListMem *L){

        // Variables locales
        int numero = 18;
        char character = 'd';
        double decimales = 8.1;

        // Variables estáticas
        static int num = 6;
        static char characters = 'b';
        static double decimal = 1.;


        if(numtrozos == 1){             //solo si pasamos "memory" 
               printListMm(*L, "all");
               printf("Funciones programa       %p,    %p,     %p\n"
                      "Funciones libreria       %p,    %p,     %p\n", &funDelete, &funMemDump, &funAlloc, &printf, &malloc, &strcmp); 
               printf("Variables locales        %p,    %p,     %p\n"
                      "Variables globales       %p,    %p,     %p\n"
                      "Variables estaticas      %p,    %p,     %p\n", &numero, &character, &decimales, &L, &numtrozos, &memory, &num, &characters, &decimal);           
        } else if (numtrozos > 1){
                if(strcmp (trozos[1], "-blocks") == 0 || strcmp(trozos[1], "-all") == 0){
                        printListMm(*L, "all");      
                } 

                if(strcmp(trozos[1], "-funcs") == 0 || strcmp(trozos[1], "-all") == 0){
                        printf("Funciones programa      %p,    %p,     %p\n"
                               "Funciones libreria      %p,    %p,     %p\n", &funDelete, &funMemDump, &funAlloc, &printf, &malloc, &strcmp);        
                }

                if(strcmp(trozos[1], "-vars") == 0 || (strcmp(trozos[1], "-all") == 0)){
                        printf("Variables locales        %p,    %p,     %p\n"
                               "Variables globales       %p,    %p,     %p\n"
                               "Variables estaticas      %p,    %p,     %p\n", &numero, &character, &decimales, &L, &numtrozos, &memory, &num, &characters, &decimal);
                }
                if(strcmp(trozos[1], "-pmap") == 0){
                        Do_pmap();
                }        
        }
}

/**
 * Function: funRecursiva
 * -----------------------
 * Calls the recursive parameter 
 * function n times.
 *  
 * @return void.
 */
void funRecursiva(){
        int n ; //parametro para invocar a función recursiva n veces 
        if (numtrozos > 1){
                n = atoi(trozos[1]);
                (n);
        }
}


/////////////////////////////////////////////////////////////////// P3 ////////////////////////////////////////////////////////////////////

/**
 * Function: BuscarVariable
 * --------------------------
 * Looks for a variable in the 
 * environment that is passed 
 * to it as a parameter.
 *
 * @param var variable.
 * @param e array where the 
 *          variable will be 
 *          searched.
 *                   
 * @return int.
 */
int BuscarVariable (char *var, char *e[]){ 
        int pos = 0;
        char aux[MAXVAR];
        strcpy(aux, var);
        strcat(aux, "=");       

        while (e[pos] != NULL) {
                if (!strncmp(e[pos], aux, strlen(aux))){        //compara hasta n=(strlen(aux)) numeros de caracteres de la cadena e[pos] y la cadena aux
                return(pos);
                } else {
                        pos++;
                }
        }
        errno = ENOENT; /* no hay tal variable */
        return (-1);      
}

/**
 * Function: CambiarVariable
 * --------------------------
 * Changes a variable in the 
 * environment that is passed 
 * to it as a parameter. Without 
 * using putenv. 
 *
 * @param var variable.
 * @param valor new value.
 * @param e array where the 
 *          variable will be 
 *          searched.
 *                   
 * @return int.
 */
int CambiarVariable(char *var, char *valor, char *e[]){                                                   
        int pos;                                              
        char *aux;
        
        if ((pos = BuscarVariable(var, e)) == -1){
                return(-1);
        }
        
        if ((aux = (char *)malloc(strlen(var) + strlen(valor) + 2)) == NULL){ //el +2 es por "=" y el caracter de fin del string 
                return -1;
                free(aux);
        }
        strcpy(aux, var);
        strcat(aux, "=");
        strcat(aux, valor);
        e[pos] = aux;
        return (pos);
}

/**
 * Function: Ejecutable
 * ----------------------
 * Helper function for 
 * systems where there is 
 * no (or we don't want to use) 
 * execvpe0. 
 *
 * @param var variable.
 * @param valor new value.
 * @param e array where the 
 *          variable will be 
 *          searched.
 *                   
 * @return char.
 */
char * Ejecutable (char *s){
 	char path[MAXPATHLEN];
 	static char aux2[MAXPATHLEN];
 	struct stat st;
 	char *p;

 	if (s == NULL || (p = getenv("PATH")) == NULL)
 		return s;
 	if (s[0] == '/' || !strncmp (s, "./", 2) || !strncmp (s, "../", 3))
                return s;       /*is an absolute pathname*/
 	strncpy (path, p, MAXPATHLEN);
 	for (p = strtok(path, ":"); p != NULL; p = strtok(NULL, ":")){
        sprintf (aux2, "%s/%s", p, s);
 	   if (lstat(aux2, &st) != -1)
 		return aux2;
 	}
 	return s;
}

/**
 * Function: OurExecpv
 * ----------------------
 * 
 *
 * @param file variable.
 * @param argv new value.
 * @param envp array where the 
 *          variable will be 
 *          searched.
 *                   
 * @return int.
 */
int OurExecvpe(const char *file, char *const argv[], char *const envp[])
{
    return (execve(Ejecutable(file),argv, envp));
}


/**
 * Function: ValorSenal
 * ----------------------
 * Returns the number of 
 * the signal from the name.
 *
 * @param sen signal name.
 *                   
 * @return int.
 */
int ValorSenal(char * sen){ 
        int i;
        for (i = 0; sigstrnum[i].nome != NULL; i++)
                if (!strcmp(sen, sigstrnum[i].nome))
                        return sigstrnum[i].sinal;
        return -1;
}

/**
 * Function: NombreSenal
 * ----------------------
 * Returns the name of 
 * the signal from the signal 
 * for sites where are not sig2str.
 *
 * @param sen signal.
 *                   
 * @return char.
 */
char *NombreSenal(int sen){			
        int i;
        for (i=0; sigstrnum[i].nome!=NULL; i++)
                if (sen==sigstrnum[i].sinal)
                        return sigstrnum[i].nome;
        return ("SIGUNKNOWN");
}

/**
 * Function: liberarEnv
 * ----------------------
 * Release the reserved 
 * space in the function 
 * funChangeVar.
 *
 * @param EnviromentL signal name.
 *                   
 * @return void.
 */
void liberarEnv(tListE EnvironmentL) {
    tPosE pos;

    if(!isEmptyListE(EnvironmentL)){
        for (pos = firstE(EnvironmentL); pos != LNULL; pos = nextE(pos, EnvironmentL)) {
                free(getItemE(pos, EnvironmentL).name);
        }
    }    
}


/**
 * Function: funAuxgetPriority
 * ----------------------------
 * Auxiliary function to take the 
 * priority of a process, if such 
 * a process does not exist, it 
 * returns its corresponding error.
 *
 * @param pid number of process.
 *                   
 * @return int.
 */ 
int funAuxgetPriority(int pid){
        errno = 0; // Si cuando hacemos get priority hay algún error, errno pasa a ser -1 y se imprime la salída del error
        int prioridade = getpriority(PRIO_PROCESS, pid);
        if(errno == 0)
                return prioridade;
        else
                perror("Imposible obtener la prioridad del proceso:");         
        
}

/**
 * Function: funPriority
 * ------------------------ 
 * Show or change the priority 
 * of process pid at valor.
 *                   
 * @return void.
 */
void funPriority(){
        
        pid_t p;        //pid do proceso a coller
        int pri;        //prioridade do proceso a cambiar ou mostrar
        int valor;      // valor polo cal se quere cambiar a prioridade do proceso
        if(numtrozos==1)
                p = getpid();
        else
             p = atoi(trozos[1]);

        if (numtrozos>=3){
                valor = atoi(trozos[2]);
                printf (" o pid que colle %d", p);
                if(setpriority(PRIO_PROCESS, p, valor)!=0) //cambio a prioridade, devolve -1 se da erro
                        perror("Imposible cambiar la prioridad del proceso:");
        }
        else{
                pri = funAuxgetPriority(p);
                printf("Prioridad del proceso %d es %d\n", p, pri);  
        }
}

/**
 * Function: printVar
 * -------------------- 
 * Helper function to 
 * show variables.
 *                   
 * @return void.
 */
void printVar(char *env[], char *nom_env){
        int i = 0;
        while(env[i] != NULL){
                printf("%p->%s[%d]=(%p) %s\n", &env[i], nom_env, i, env[i], env[i]);
                i++;        
        }
}

/**
 * Function: funShowVar
 * ------------------------
 * Shows the value of 
 * an environment variable.
 *
 * @param arg3 array of the 
 *             third argument
 *             of main.
 *                   
 * @return void.
 */
void funShowVar(char *arg3[]){
        int i = 0, j = 0;
        char *value;

        if(numtrozos == 1){                     //solo se escribe "showvar" 
                printVar(arg3, "main arg3");    //entonces se printea el tercer argumento del main
        } else if (numtrozos == 2){            //se le pone una variable de entorno
                if((value = getenv(trozos[1])) != NULL){   //se obtiene el valor de la variable de entorno     
                        j = BuscarVariable(trozos[1], environ); 
                        if((i = BuscarVariable(trozos[1], arg3)) != -1){      //se busca la variable de entorno en arg3[] si se consigue se muestra en todo 
                               
                                printf("Con main arg3 %s (%p) @%p\n"            
                                       "Con environ   %s (%p) @%p\n"
                                       "Con getenv    %s (%p)\n", arg3[i], arg3[i], &arg3[i], environ[j], environ[j], &environ[j], value, &value);        
                        }else if(j != 1){       //aqui entra cuando la variable solo se encuentra en el entorno es decir fue creada por putenv
                                printf("Con environ   %s (%p) @%p\n"
                                       "Con getenv    %s (%p)\n", environ[j], environ[j], &environ[j], value, &value);
                        }else{
                                printf("Error: No existe esta varible\n");
                        }
                }else{
                        printf("La variable \"%s\" no existe\n", trozos[1]);
                }
        }else{
                printf("El formato no es el correcto\n");
        }
}

/**
 * Function: funChangeVar
 * ---------------------
 * Shows the process environment.
 *
 * @param arg3 array of the 
 *             third argument
 *             of main.
 * @param lEnviron environmet list. 
 *                   
 * @return void.
 */
void funChangeVar(char *arg3[], tListE *lEnviron){
        int pos;
        char *var;
        tItemE variable;

        if(numtrozos == 4){
                if(strcmp(trozos[1], "-a") == 0){       //se busca la variable en el array arg3[] del main
                        pos = CambiarVariable(trozos[2], trozos[3], arg3);
                        if(pos == -1){
                                perror("Error"); 
                        }
                }else if(strcmp(trozos[1], "-e") == 0){        //se busca la variable en environ 
                        pos = CambiarVariable(trozos[2], trozos[3], environ);
                        if(pos == -1){
                                perror("Error"); 
                        }
                }else if(strcmp(trozos[1], "-p") == 0){         
                        var = malloc(strlen(trozos[2]) + strlen(trozos[3]) + 2);        //strlen: calcula el tamaño de los string (+2 es por "=" y por el caracter de fin de string)
                        strcpy(var, trozos[2]);
                        strcat(var, "=");
                        strcat(var, trozos[3]);

                        if(putenv(var) != 0){       //si la variable existe se cambia el valor si no existe se crea
                                perror("Error"); 
                                free(var);
                                return;
                        }
                        variable.name = var;
                        insertItemE(variable, lEnviron);        //añadimos la direccion del string en la lista para luego liberar la memoria reservada
                }else{
                        printf("Uso: changevar [-a|-e|-p] var valor\n");
                        return;        
                }

                if(pos != -1){          //si no se produjo ningun error
                        printf("La variable de entorno \"%s\" cambio su valor a \"%s\"\n", trozos[2], trozos[3]);
                }
        }else{
                printf("Uso: changevar [-a|-e|-p] var valor\n");          
        }
}

void printAdd(char *arg3[]){
        printf("environ:   %p (almacenado en %p)\n"
               "main arg3: %p (almacenado en %p)\n", arg3, &arg3, environ, &environ);  
}

/**
 * Function: funShowEnv
 * ---------------------
 * Shows the process 
 * environment.
 * 
 * @param arg3 array of the 
 *             third argument
 *             of main.
 *
 * @return void.
 */
void funShowEnv(char *arg3[]){

        if(numtrozos == 1){
                printVar(arg3, "main arg3"); //se printea por el tercer argumento del main
        }else if(numtrozos == 2){
                if(strcmp(trozos[1], "-environ") == 0){
                        printVar(environ, "environ");    //se imprime la variable externa (environ) 
                }else if(strcmp(trozos[1], "-addr") == 0){
                        printAdd(arg3);         //se imprimen las direcciones de arg3[] y environ       
                }
        }else{
                printf("Uso: showenv [-environ|-addr]\n");
        }
}

/**
 * Function: funfork
 * ---------------------
 * The shell does the fork 
 * system call and waits for 
 * its child to end.
 *
 * @param arg3 array of the 
 *             third argument
 *             of main.
 *                   
 * @return void.
 */
void funFork(tListP *LP){
	pid_t pid;
        tPosP posP;
	
        if(numtrozos == 1){
                if ((pid = fork()) == 0){       //se crea el nuevo proceso 
                        if(!isEmptyListp(*LP)){  
                                for(posP = firstp(*LP); posP != NULL; posP = nextp(posP, *LP)){
                                removeElementp(LP, posP); //se borran los procesos asociados al padre ya que al ser el hijo hereda todo pero el hijo realmente no tiene ningun proceso aun 
                                }
                        }
		        printf ("Ejecutando proceso %d\n", getpid()); //el proceso hijo continua ejecutandose
                }else if (pid != -1){
                        waitpid (pid, NULL, 0);         //el proceso padre espera a que termine el hijo ç
                }
        }	
}

/**
 * Function: funExecute
 * ---------------------
 * Executa, sen crear proceso, prog con argumentos
 * en un entorno que contén só as variables VAR1,
 * VAR2,...
 * @pri para establecer unha prioridade 
 *                   
 * @return void.
 */
void funExecute(){
        pid_t pid = getpid();
        int pri, flagpri=0;
        int i,j,s,k=1, l;
        char priori[5];
        char *varenv[MAXPATHLEN];
        char *var[MAXPATHLEN];

        //para coller a prioridade
        for(i = 1; i<numtrozos; i++){             
                if(trozos[i][0]=='@'){
                flagpri=1; // para saber se o usuario quere establecer prioridade 
                s = strlen(trozos[i]);
                for (j = 1; j < s; j++){
                        priori[j-1]=trozos[i][j];
                }
                pri = atoi(priori); 
                break; 
                }  
        }
        if(flagpri)
                setpriority(PRIO_PROCESS,pid,pri);

        
        //para coller as variables de entorno
        while ( trozos[k]!=NULL && getenv(trozos[k])!=NULL){
                varenv[k-1]=trozos[k];
                k++;
        }
        
        //para coller parametros para o programa
        l=k;
        while ((l+1)<numtrozos){
                if(trozos[l+1][0]!='@')
                        var[l-k] = trozos[l+1];
                l++;
        }
        //execución
        if(k==1){//se k é igual a 1 significa que o usuario non escribiu variables de entorno, enton collo environ
                if(OurExecvpe(trozos[k],var,environ)==-1)
                perror("Imposible ejecutar");
        }else{
                if(OurExecvpe(trozos[k],var,varenv)==-1)
                perror("Imposible ejecutar");
        }
        
}

/**
 * Function: funEstado
 * ---------------------
 * Helper function to check the state.
 * Execute, without creating 
 * the process, program with 
 * arguments in an environment 
 * containing only the variables 
 * VAR1, VAR2...
 * 
 * @param listaprocesos process list.
 * @param p position.
 *                   
 * @return void.
 */
void funEstado(tListP *listaproc, tPosP p){
        int estado;
        tItemP d = getItemp(p, *listaproc);
        //strcpy(d.estado,"TERMINADO");
        //d.sinal=0;
        int pri; // hai que revisar se cambia a prioridade
        //Revisamos se hai algún cambio de estado
        if(waitpid(d.pid, &estado, WNOHANG | WUNTRACED | WCONTINUED) == d.pid){
                if(WIFEXITED(estado)){
                        strcpy(d.estado,"TERMINADO");
                        d.sinal=WEXITSTATUS(estado);       
                }else if(WIFCONTINUED(estado)){
                        strcpy(d.estado,"ACTIVO");
                        d.sinal=SIGCONT;
                }else if (WIFSIGNALED(estado)){
                        strcpy(d.estado,"SENALADO");
                        d.sinal=WTERMSIG(estado);
                }
                else if(WIFSTOPPED(estado)){
                        strcpy(d.estado,"PARADO");
                        d.sinal=WSTOPSIG(estado);
                }else{
                        strcpy(d.estado,"ERROR");
                        d.sinal = -1;
                }
                pri = getpriority(PRIO_PROCESS, d.pid);
                d.prioridade=pri; // tamén actualizamos a prioridade
                updateListp(d, p, listaproc);
        }
}

/**
 * Function: FunListJobs
 * ---------------------
 * Amosa a lista dos procesos que executan
 * ou executaron en segundo plano.
 *
 * @param listaproc
 *  lista de procesos
 *                   
 * @return void.
 */
void funListJobs(tListP *listaproc){
        tPosP p = firstp(*listaproc);
        tItemP i;
        while (p!=NULL){
                funEstado(listaproc,p);
                i = getItemp(p, *listaproc);
                printf("%d %8s p=%d %s %s (%d) %s\n", i.pid, i.usuario, i.prioridade, i.tempo, i.estado, i.sinal, i.comando);
                p = nextp(p,*listaproc);
        }
}

/**
 * Function: funJob
 * ---------------------
 * Amosa información dun proceso que
 * execute en segundo plano.
 * Con -fg para pasalo a primer plano.
 *
 * @param listaproc
 *  lista de procesos
 *                   
 * @return void.
 */
void funJob(tListP *listaproc){
        pid_t pid;
        tPosP p = firstp(*listaproc);
        tPosP q;
        tItemP d;
        int estado;
        struct stat buff;

        if(!(numtrozos==1 || ((numtrozos>1) && (strcmp(trozos[1],"-fg")!=0)))){
                
                if(strcmp(trozos[1],"-fg")==0){
                        pid = atoi(trozos[2]);
                        //busco o pid na lista
                        while (p!=NULL){
                                funEstado(listaproc,p);
                                d = getItemp(p,*listaproc);
                                if(d.pid==pid){
                                        q = p;
                                        break;
                                }
                                p = nextp(p,*listaproc);
                        }

                        if (q!= NULL){   
                                estado = atoi(d.estado); 

                                if(waitpid(pid,&estado,0) == -1)
                                        perror("Imposible pasar proceso a primer plano"); 
                                printf("Proceso %d pid ya está finalizado\n", pid);
                                removeElementp(listaproc,p);

                        }else
                                funListJobs(listaproc);      
                }else{
                        pid = atoi(trozos[1]);
                        while (p!=NULL){
                                d = getItemp(p,*listaproc);
                                if(d.pid==pid){
                                        q=p;
                                        break;
                                }
                                p = nextp(p,*listaproc);
                        }
                        if(q!=NULL){
                                funEstado(listaproc,q);
                                d = getItemp(q,*listaproc);
                                printf("%d  %s p=%d %s %s (%d) %s\n",d.pid,d.usuario, d.prioridade,d.tempo, d.estado,d.sinal,d.comando);
                        }
                }
                
        }else{ 
                if(!isEmptyListp(*listaproc))
                        funListJobs(listaproc);
        }
        
}

void funAuxDelJob(tListP *listaproc, char *opc){ 
        tPosP p = firstp(*listaproc),q;
        tItemP d;
        while (p != NULL){   
                funEstado(listaproc,p);
                q=nextp(p,*listaproc); 
                d = getItemp(p,*listaproc);
                if(strcmp(opc,d.estado)==0)
                        removeElementp(listaproc, p);
                p=q;  
        }
}

/**
 * Function: funDelJobs
 * ---------------------
 * Elimina os procesos de segundo
 * plano según estado: terminado ou sinalado.
 *
 * @param listaprocesos process list.
 *                   
 * @return void.
 */
void funDelJobs(tListP *listaprocesos){

        char * estado;
        
        if(numtrozos>1) {
                
                if(strcmp(trozos[1],"-term") == 0)
                        estado = "TERMINADO";
                else if(strcmp(trozos[1],"-sig") == 0)
                        estado = "SENALADO";
                else{
                        estado = "-";
                        funListJobs(listaprocesos);

                } 

                if(strcmp(estado,"-")!=0)
                        funAuxDelJob(listaprocesos, estado);

        }else{
                if (!isEmptyListp(*listaprocesos))
                        funListJobs(listaprocesos);
        }
}

//Auxiliar para coller o usuario
char* funAuxGetUser(uid_t u){
        struct passwd *p;
        if((p=getpwuid(u))==NULL){
		return("unkown");
	}
	return p->pw_name;
}

void funAuxExec(tListP *L, int flagSegundo, int p){ 
        tItemP d;       
        int i = 0;

        //para coller o usuario
        uid_t usuario = getuid();
        char *user = funAuxGetUser(usuario);

        //pid do proceso
        pid_t pid = fork();

        //prioridade do proceso
        int pri;
        if(p<(-20))
                pri = getpriority(PRIO_PROCESS,pid);
        else
                pri = setpriority(PRIO_PROCESS,pid,p);
        
        //comando con opcións
        char comando[MAXPATHLEN];
        while (i<numtrozos){
                strcpy(comando,trozos[i]);  
                i++;
        }

        //tempo de creación
        time_t data;
        time(&data);
        struct tm *dat;
        if((dat = localtime(&data))==NULL)
                perror("Error de data");

        if(pid==0){
                if (execvp(trozos[0],&trozos[0]) == -1)                {
                        fprintf(stderr, "No se puede ejecutar el comando:%s\n", strerror(errno));
                }
                exit(255);// por se falla o execvp   
        }else{  
                if(pid==-1){
                        fprintf(stderr, "No se puede ejecutar el comando:%s\n", strerror(errno));
                        exit(255);
                }

                if(flagSegundo){ //se querese executar en segundo plano
                        printf("%d\n",pid);
                        d.pid = pid;
                        d.prioridade = pri;
                        strcpy(d.usuario,user);
                        strcpy(d.comando, comando);
                        strcpy(d.tempo, asctime(dat));
                        strcpy(d.estado, "ACTIVO");
                        d.sinal=0;
                        insertElementp(d,L);
                }
        }
}


/**
 * Function: otrosComandos
 * ----------------------
 * Executa calquer comando linux
 *            
 * @return void.
 */
void otrosComandos(tListP *listaProcesos){
        int pri,i,s,j, flagpri=0;
        char *priori;

        for(i = 0; i<numtrozos; i++){             
                if(trozos[i][0]=='@'){
                flagpri=1; // para saber se o usuario quere establecer prioridade 
                s = strlen(trozos[i]);
                for (j = 1; j < s; j++){
                        priori[j-1]=trozos[i][j];
                }
                pri = atoi(priori); 
                break; 
                }  
        }
        if(!flagpri)
                pri=-21;
        if(strcmp(trozos[numtrozos - 1], "&") == 0){
                trozos[--numtrozos] = NULL;
                funAuxExec(listaProcesos, 1, pri);
        }else
                funAuxExec(listaProcesos, 0, pri);
}


/**
 * Function: trocearCadena
 * ----------------------
 * Copies in trozos all the tokens found
 * in the string separated by spaces.
 *
 * @param cadena string with the command
 * @param trozos array of strings
 * 
 * @return number of strings copied in trozos
 */
int TrocearCadena(char * cadena, char * trozos[]){
        int i = 1;

        if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
                return 0;
        while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
                i++;
        return i;
}

/**
 * Function: funFin
 * ----------------------
 * Function that serves to end 
 * the execution of the shell.
 *            
 * @return void.
 */
void funFin(tList *listhistorial, tListMem *listmemoria, tListP *listaprocesos, tListE *listaEnvironment){
        removeElement(listhistorial);
        funDealloc(listmemoria,1); 
        tPosP p = firstp(*listaprocesos);
        while (p!=NULL){
                removeElementp(listaprocesos,p); 
                p = nextp(p,*listaprocesos);
        }
        liberarEnv(*listaEnvironment);
        exit(0);
}

//Main
int main(int argc, char *argv[], char *env[]){
        tList listhistorial;
        createList(&listhistorial);
        tItemL d;
        tListMem listamemoria;
        createListm(&listamemoria);
        tListP listaProcesos;
        createListp(&listaProcesos);
        tListE listaEnvironment;
        createEmptyListE(&listaEnvironment);

        while(1){
                printf("-> ");
                if(fgets(linea, MAXLINEA, stdin) == NULL) 
                        exit(1);
                //copio en d a cadena de entrada para insetar los comandos o historial
                strcpy(*d.comando, linea); 
                numtrozos = TrocearCadena(linea, trozos);
                if (numtrozos == 0)
                        continue;
                for (int i = 0; ; i++){
                        if(comandos[i].nombre == NULL){
                                if (strcmp(trozos[0], "hist") == 0){
                                        insertElement(d, &listhistorial);
                                        funHist(&listhistorial);
                                        break;
                                }
                                else if(strcmp(trozos[0], "fin") == 0 || strcmp(trozos[0], "bye") == 0 || strcmp(trozos[0], "salir") == 0){
                                        insertElement(d, &listhistorial);
                                        funFin(&listhistorial, &listamemoria, &listaProcesos, &listaEnvironment);
                                }
                                else if(strcmp(trozos[0], "allocate") == 0){
                                        insertElement(d, &listhistorial);
                                        funAlloc(&listamemoria);
                                        break;
                                }else if(strcmp(trozos[0], "deallocate") == 0){
                                        insertElement(d, &listhistorial);
                                        funDealloc(&listamemoria, 0);
                                        break;
                                }else if (strcmp(trozos[0], "memory") == 0){
                                        insertElement(d, &listhistorial);
                                        funMemory(&listamemoria);
                                        break;
                                }else if(strcmp(trozos[0], "showvar") == 0){
                                        insertElement(d,&listhistorial);
                                        funShowVar(env);
                                        break;
                                }else if(strcmp(trozos[0], "changevar") == 0){
                                        insertElement(d,&listhistorial);
                                        funChangeVar(env, &listaEnvironment);
                                        break;
                                }else if(strcmp(trozos[0], "showenv") == 0){
                                        insertElement(d,&listhistorial);
                                        funShowEnv(env);
                                        break;       
                                }else if (strcmp(trozos[0], "fork") == 0){
                                        insertElement(d, &listhistorial);
                                        funFork(&listaProcesos);
                                        break;        
                                } else if(strcmp(trozos[0], "job") == 0){
                                        insertElement(d,&listhistorial);
                                        funJob(&listaProcesos);
                                        break;
                                }else if (strcmp(trozos[0], "listjobs") == 0){
                                        insertElement(d,&listhistorial);
                                        funListJobs(&listaProcesos);
                                        break;
                                }else if(strcmp(trozos[0], "deljobs") == 0){
                                        insertElement(d,&listhistorial);
                                        funDelJobs(&listaProcesos);
                                        break;
                                }else {
                                        otrosComandos(&listaProcesos);
                                        insertElement(d, &listhistorial);
                                        break;
                                }
                        }
                        if (strcmp(comandos[i].nombre, trozos[0]) == 0){  
                                comandos[i].pfun();
                                insertElement(d, &listhistorial);
                                break;
                        }
                }
        }
}     
