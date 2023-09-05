/*
Alumna 1 : Daniela Cisneros Sande
Alumna 2: Graciela Méndez Olmos

login 1: d.cisneross@udc.es
login 2: graciela.mendez.olmos@udc.es

*/

#include "p0.h"

//Variables GLOBAIs para 
//cadea recibida:
char *trozos[MAXTROZOS];
int numtrozos;
char linea[MAXLINEA];
//path 
char ruta[MAXPATHLEN];


//Función para comando autores [-l|-n]
void funAutores(){

        char* nomes[4] = {"Graciela Méndez Olmos", "Daniela Cisneros Sande", "graciela.mendez.olmos@udc.es", "d.cisneross@udc.es"};

        int flaglogin=1, flagnome=1;
        
        if (numtrozos>1){
                if(strcmp(trozos[1], "-l")==0)
                        flagnome = 0;
                if(strcmp(trozos[1], "-n")==0)
                        flaglogin = 0;
        }
        //Se só ponme autores
        if(numtrozos == 1)
                printf("%s:%s\n%s:%s\n",nomes[1],nomes[3],nomes[0],nomes[2]); 
        else{
                if(flaglogin)
                        printf("%s\n%s\n", nomes[2], nomes[3]);
                if(flagnome)
                        printf("%s\n%s\n", nomes[0], nomes[1]);                               
        }
}
    
void funPid(){
	if (numtrozos == 1)
		printf("Pid de shell: %d\n", getpid());
	else if (strcmp(trozos[1],"-p")==0)  
		printf("Pid del padre shell: %d\n", getppid());
}

void funCarpeta(){
        if(getcwd(ruta, MAXPATHLEN)==NULL){ perror("getcwd"); exit(0);}
        if (numtrozos == 1)
                printf("El directorio actual es: %s\n", ruta);
        if(numtrozos > 1){
                if(chdir(trozos[1])==-1)
                        fprintf(stderr, " %s '%s'\n", strerror(2), trozos[1]);
                else
                        chdir(trozos[1]);
        }
} 

void funFecha(){
	time_t tiempo = time(0);
	struct tm *tlocal = localtime(&tiempo);
	char fecha[MAXFyH];
	char hora[MAXFyH];

	strftime(hora, MAXFyH, "%H:%M:%S", tlocal);
	strftime(fecha, MAXFyH, "%d/%m/%y", tlocal);

	if (numtrozos == 1){
		printf("%s\n%s\n", hora, fecha);
	} else {
		if(strcmp(trozos[1], "-h")==0)
		printf("%s\n", hora);
		else if(strcmp(trozos[1], "-d")==0)
		printf("%s\n", fecha);
	}
}

void funHist(tList *listhistorial){
        int i=0, flagbal=0, n;
        tItemL d;
        tPosL p = first(*listhistorial);
        char* token;
        
        if(numtrozos == 1){
                while(p!=NULL){
                        d = getItem(p,listhistorial);
                        i++;
                        p = next(p,*listhistorial);
                } 
        }
        if(numtrozos>1){
                if(strcmp(trozos[1],"-c")==0)
                        flagbal=1;
                if(flagbal){
                        removeElement(listhistorial);
                        }
                else{
                        token = strtok(trozos[1], "-"); //consego unha cadea que so conteña o número
                        n = (int) strtol(token,NULL,10); // Converto a cadea a un enteiro numérico
                        
                        if(n<0)
                                fprintf(stderr, "%s\n", strerror(3));
                        else{
                                while(i!=n){
                                        d = getItem(p,listhistorial);
                                        printf("%d->%s\n", i, *d.comando);
                                        i++;
                                        p = next(p,*listhistorial);
                                }
                        }
                }       
        }        
}


void funInfosis(){
	struct utsname infosisp;
	uname(&infosisp);
	
	printf("%s (%s), OS: %s-%s-%s\n", infosisp.nodename, infosisp.machine, infosisp.sysname, infosisp.release, infosisp.version);
}


void funAyuda(){
        int i, flagatopar = 0;
        if (numtrozos>1){
                for (i = 0; ; i++){
                        if (strcmp(tabla[i].nombre, trozos[1])==0){
                                printf("%s %s\n",trozos[1], tabla[i].info);
                        break;}
                        if (i >= 9){ flagatopar = 1; break;}
                }
                if (flagatopar)  printf(" '%s' no encontrado\n", trozos[1]);
        }  
        else{
                printf("'ayuda cmd' donde cmd es uno de los siguientes comandos:\n");
                printf("1.autores\n2.pid\n3.carpeta\n4.fecha\n5.hist\n");
                printf("6.infosis\n7.fin\n8.salir\n9.bye\n10.ayuda\n");
        }        
};

void funFin(tList *listhistorial){
        removeElement(listhistorial);
        exit(0);
}


//Función para trocear a cadea de entrada
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
                printf("~€ ");
                if(fgets(linea, MAXLINEA,stdin)==NULL) {
                        exit(1);
                }
                //copio en d a cadea de entrada para insetar os comandos ó historial
                strcpy(*d.comando, linea); 
                numtrozos= TrocearCadena(linea,trozos);
        
                if (numtrozos==0)
                        continue;
                for (i = 0; ; i++){

                        if(comandos[i].nombre==NULL){
                                if (strcmp(trozos[0],"hist")==0){
                                        insertElement(d, &listhistorial);
                                        funHist(&listhistorial);
                                        break;
                                }
                                else if(strcmp(trozos[0],"fin")==0||strcmp(trozos[0],"bye")==0||strcmp(trozos[0],"salir")==0){
                                        insertElement(d, &listhistorial);
                                        funFin(&listhistorial);
                                }
                                else{
                                        fprintf(stderr, "%s '%s'\n", strerror(3), trozos[0] );
                                        insertElement(d, &listhistorial);
                                        break;
                                }
                        }
                        if (strcmp(comandos[i].nombre, trozos[0])==0){  
                                comandos[i].pfun();
                                insertElement(d, &listhistorial);
                                break;
                        }
                }
        }
}    
