/**
 * TITLE: Lab Assignment 1
 * @author Daniela Cisneros Sande @login: d.cisneross
 * @author Graciela Méndez Olmos @login: graciela.mendez.olmos@udc.es
 * GROUP: 4.1
 * DATE: 21 / 10 / 2022
 */

#include "p1.h"

/*Variables Globales*/

char *trozos[MAXTROZOS]; //Array para guardar y luego leer lo cada uno de los trozos escritos por terminal 
int numtrozos;           //Lleva la cuenta del numero de palabras introducidas
char linea[MAXLINEA];    //Guarda absolutamente todo lo escrito por terminal 
char ruta[PATH_MAX];     //Array para guardar el path 


/**
 * Function: funAutores
 * ----------------------
 * Prints the names and logins
 * of the program autors. 
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
 *            usage of comand cmd.
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
                        
                        if (i >= 13){
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
        } 
}

/**
 * Function: funFin
 * ----------------------
 * Function that serves to end 
 * the execution of the shell.
 *            
 * @return void.
 */
void funFin(tList *listhistorial){
        removeElement(listhistorial);
        exit(0);
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
        char *fileORdir[NAME_MAX];
        int i = 0;

        for (int k = 1; k < numtrozos; k++){
                
                if(strcmp(trozos[k], "-long") == 0){
                        flagLong = 1;
                } else if (strcmp(trozos[k], "-link") == 0) {
                        flagLink = 1;
                } else if (strcmp(trozos[k], "-acc") == 0){
                        flagAcc = 1;
                } else { //no es ninguna de las opciones del comando 
                        fileORdir[i] = trozos[k];  //ingresamos en el array el nombre del archivo o directorio
                        i++; //vamos ingresando por orden en el array por lo que vamos avanzando
                }                  
        }
        
        for(int j = 0; j < i ; j++){
                funStatAux(fileORdir[j], flagAcc, flagLink, flagLong); //llamamos la funcion con cada elemento del array 
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
        char *fileORdir[NAME_MAX];
        struct stat buf;
        int k = 0 ;

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
                        fileORdir[k] = trozos[i]; //ingresamos en el array el nombre del directorio
                        k++; //vamos ingresando por orden en el array por lo que vamos avanzando
                }   
        } 
        
        opciones_trozos = flaglong + flaglink + flagacc + flaghid + flagreca +  flagrecb;
        if(numtrozos == opciones_trozos + 1){ 
                funCarpeta();
        }

        for(int j = 0; j < k ; j++){  
                 //comprobamos que es un directorio
                if(LetraTF(buf.st_mode) == 'd'){       
                        if(!(flagreca) && !(flagrecb)){
                                funListFiles(fileORdir[j], flagacc, flaglink, flaglong, flaghid);
                        }
                        
                        if(flagrecb || flagreca){ 
                                funListRecAux(fileORdir[j], flagacc, flaglink, flaglong, flagreca, flagrecb, flaghid);
                        }

                } else { 
                        //si es un archivo, entonces llamamos a Stat
                        funStatAux(fileORdir[j], flagacc, flaglink, flaglong);
                }                   
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

/**
 * Function: trocearCadena
 * ----------------------
 * Copies in trozos all the tokens found
 * in the string separated by spaces.
 *
 * @param cadena string with the command
 * @param trozos array of strings
 * @return number of strings copied in trozos
 */
int TrocearCadena(char * cadena, char * trozos[]){
        int i=1;
        if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
                return 0;
        while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
                i++;
        return i;
}


//Main
int main(){
        tList listhistorial;
        createList(&listhistorial);
        int i;
        tItemL d;

        while(1){
                printf("-> ");
                if(fgets(linea, MAXLINEA, stdin) == NULL) 
                        exit(1);
                //copio en d a cadena de entrada para insetar los comandos o historial
                strcpy(*d.comando, linea); 
                numtrozos = TrocearCadena(linea, trozos);
                if (numtrozos == 0)
                        continue;
                for (i = 0; ; i++){
                        if(comandos[i].nombre == NULL){
                                if (strcmp(trozos[0], "hist") == 0){
                                        insertElement(d, &listhistorial);
                                        funHist(&listhistorial);
                                        break;
                                }
                                else if(strcmp(trozos[0], "fin") == 0 || strcmp(trozos[0], "bye") == 0 || strcmp(trozos[0], "salir") == 0){
                                        insertElement(d, &listhistorial);
                                        funFin(&listhistorial);
                                }
                                else{
                                        fprintf(stderr, "%s '%s'\n", strerror(3), trozos[0]);
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