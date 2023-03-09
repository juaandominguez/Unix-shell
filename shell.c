//Juan Domínguez Rodríguez & juan.dominguezr@udc.es
#define MAX 1024
#define MAX_ARRAY 4096
#define MAX_RECURSION 10
#define SIZE 2048
#define LINESIZE 25
#define MAXNAME 50
#define MAXVAR 100
#define MAXSIGNAL 20
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/utsname.h>
#include <time.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdint.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/resource.h>
int glob1,glob2,glob3;

extern char ** environ;
void * addr1;
void * addr2;

typedef struct {
    void * address;
    off_t size;
    int descriptor;
    key_t key;
    char name[MAX];
    struct tm timeinfo; 
    char allocType[MAX];
    }memList;

typedef struct {
    pid_t pid;
    struct tm timeinfo;
    char name[MAX];
    char state[MAXNAME];
    int prio;
    char user[MAXNAME];
}jobList;

struct CMD{
    char * name;
    void (*func)(char **);
};

struct SEN{
    char * nombre;
    int senal; 
};


void * cadtop(char * str){
    void *p;
    sscanf(str,"%p",&p);
    return p;
}

int isDirectory(char *name){
    struct stat s;
    if(lstat(name,&s)==-1) return 0;
    return S_ISDIR(s.st_mode);
}

void printDirectory(){
    char dir[MAX];
    printf("%s\n", getcwd (dir,MAX));
}

void Cmd_autores(char * tr[]){
    if(tr[0]==NULL){
        printf("juan.dominguezr@udc.es\n");
        printf("Juan Domínguez Rodríguez\n");
        return;
    }
    if(!strcmp(tr[0],"-l")){
        printf("juan.dominguezr@udc.es\n");}
        else if(!strcmp(tr[0],"-n")){
        printf("Juan Domínguez Rodríguez\n");

    }
}

void Cmd_pid(char * tr[]){
    if(tr[0]!=NULL && !strcmp(tr[0],"-p"))
        printf("Parent process pid: %d\n",getppid());
    else
        printf("Process pid: %d\n",getpid());
}

void Cmd_carpeta(char * tr[]){
    if(tr[0]==NULL)
        printDirectory();
    else{
        if(chdir (tr[0])==-1)
        perror ("Cannot change directory");
    }
}

void Cmd_fecha(char * tr[]){
    time_t mytime;
    struct tm * timeinfo; 
    time(&mytime);
    timeinfo = localtime(&mytime);
    if(tr[0]==NULL)
        printf("%02d:%02d:%02d %02d/%02d/%04d\n",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec,timeinfo->tm_mday,timeinfo->tm_mon+1,timeinfo->tm_year+1900);
    else if(!strcmp(tr[0],"-h")) printf("%02d:%02d:%02d\n",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
    else if(!strcmp(tr[0],"-d")) printf("%02d/%02d/%04d\n",timeinfo->tm_mday,timeinfo->tm_mon+1,timeinfo->tm_year+1900);
}

void Cmd_infosis(){
    struct utsname cmd_uname;
    if(uname(&cmd_uname)<0){
        perror("Failed to uname\n");
    }
    else{
        printf("%s %s %s %s %s\n",cmd_uname.sysname, cmd_uname.nodename, cmd_uname.release, cmd_uname.version, cmd_uname.machine);
    }
}

void Cmd_fin(){
    exit(0);
}

void Cmd_ayuda(char *tr[]){
    if(tr[0]==NULL){
        printf("Available commands:\nautores pid carpeta fecha hist comando N infosis fin salir bye salir exit ayuda create stat list delete deltree allocate deallocate i-o memfill memdump memory recurse priority showvar changevar showenv fork execute listjobs deljobs ***\n");
    }
    else if(!strcmp(tr[0],"autores")){
        printf("%s\t[-n|-l]\tShows the names and/or logins of the authors\n",tr[0]);
    }
    else if (!strcmp(tr[0],"pid")){
        printf("%s\t[-p]\tShows the pid of the shell or of his parent process\n",tr[0]);
    }
    else if (!strcmp(tr[0],"carpeta")){
        printf("%s\t[dir]\tChange (or shows) the actual directory of the shell\n",tr[0]);
    }
    else if (!strcmp(tr[0],"fecha")){
        printf("%s\t[-d|-h]\tShows the current date and/or hour\n",tr[0]);
    }
    else if (!strcmp(tr[0],"hist")){
        printf("%s\t[-c|-N]\tShows(or clears) the historic of commands\n\t-N: muestra los N primeros\n\t-c: borra el historico\n",tr[0]);
    }
    else if (!strcmp(tr[0],"comando")){
        printf("%s\t[-N]\tRepeats command number N (from historic list)\n",tr[0]);
    }
    else if(!strcmp(tr[0],"infosis")){
        printf("%s\tPrints information on the machine running the shell\n",tr[0]);
    }
    else if(((!strcmp(tr[0],"fin")||(!strcmp(tr[0],"exit"))||(!strcmp(tr[0],"salir"))|| (!strcmp(tr[0],"bye"))))){
        printf("%s\tEnds the shell\n",tr[0]);
    }
    else if(!strcmp(tr[0],"ayuda")){
        printf("%s\t[cmd|-lt|-T]\tShows help about the commands\n\t-lt: list of help topics\n\t-T topic: list of commands of that topic\n\tcmd: info about command cmd\n",tr[0]);
    }
    else if(!strcmp(tr[0],"create")){
    printf("%s\t[-f] [name]\tCreates a directory or a file (-f)\n",tr[0]);
    }
    else if(!strcmp(tr[0],"stat")){
    printf("%s\t[-long][-link][-acc]\tname1 name2 ..\tlist files;\n\t-long:long list\n\t-acc: acesstime\n\t-link: if it is symbolic link, the contained path\n",tr[0]);
    }
    else if(!strcmp(tr[0],"list")){
        printf("%s\t [-reca] [-recb] [-hid][-long][-link][-acc]\nn1 n2 ..\tlista contents\n of directories\n\t-hid: include hidden files\n\t-reca: recursive (before)\n\t-recb: recursive (after)\n\trest of parameters as stat\n",tr[0]);
    }
    else if(!strcmp(tr[0],"delete")){
    printf("%s\t[name1 name2 ..]\tClears empty files or directories\n",tr[0]);
    }
    else if(!strcmp(tr[0],"deltree")){
    printf("%s\t[name1 name2 ..]\tClears non-empty files or directories recursively\n",tr[0]);
    }
    else if(!strcmp(tr[0],"allocate")){
    printf("%s [-malloc|-shared|-createshared|-mmap]... Assigns a block of memory\n\t-malloc tam: assigns a malloc block of size tam\n\t-createshared cl tam: assigns (creating) the block of shared memory of key cl and size tam\n\t-shared cl: assigns the shared memory block (already creted) of key cl\n\t-mmap fich perm: maps the file fich, perm are the permissions\n",tr[0]);
    }
    else if(!strcmp(tr[0],"deallocate")){
    printf("%s [-malloc|-shared|-delkey|-mmap|addr]..Designates a memory block\n\t-malloc tam: deasignates a malloc block of size tam\n\t-shared cl: deasignates (unmap) the block of shared memory of key cl\n\t-delkey cl: delete from the system (without unmap) the memory key cl\n\t-mmap fich: unmap the mapped file fich\n\taddr: deasignates the memory block in the address addr\n",tr[0]);
    }
    else if(!strcmp(tr[0],"i-o")){
    printf("%s [read|write] [-o] fiche addr cont\n\tread fich addr cont: rread cont bytes from fich to addr\n\twrite [-o] fich addr cont: Writes cont bytes from addr to fich. -o to overwrite\n\taddr is a memory address\n",tr[0]);
    }
    else if(!strcmp(tr[0],"memdump")){
    printf("%s addr cont\tDumps in the screen the contents (cont bytes) of the memory address addr\n",tr[0]);
    }
    else if(!strcmp(tr[0],"memfill")){
    printf("%s addr cont byte 	Fills the memory from addr with byte\n",tr[0]);
    }
    else if(!strcmp(tr[0],"memory")){
    printf("%s [-blocks|-funcs|-vars|-all|-pmap] ..	Shows details about the memory of the process\n\t-blocks: the assigned memory blocks\n\t-funcs: addresses of the functions\n\t-vars: the addresses of the variables\n\t-all: everything\n\t-pmap: shows the output of the command pmap(or similar)\n",tr[0]);
    }
    else if(!strcmp(tr[0],"recurse")){
    printf("%s [n]\tInvokes the recursive function n times\n",tr[0]);
    }
    else if(!strcmp(tr[0],"priority")){
    printf("%s priority [pid] [value] 	Shows or changes the priority of process pid to value\n",tr[0]);
    }
    else if(!strcmp(tr[0],"showvar")){
    printf("%s showvar var1	Shows the value and directions of enviroment value var\n",tr[0]);
    }
    else if(!strcmp(tr[0],"changevar")){
    printf("%s [-a|-e|-p] var value: Changes enviroment value\n\t-a 3 arg main\n\t-e environ\n\t-p putenv\n",tr[0]);
    }
    else if(!strcmp(tr[0],"showenv")){
    printf("%s [-environ|-addr] Shows process enviroment\n",tr[0]);
    }
    else if(!strcmp(tr[0],"fork")){
    printf("%s Makes a call to create a process\n",tr[0]);
    }
    else if(!strcmp(tr[0],"execute")){
    printf("%s prog args... Executes, without creating a process, prog with arguments\n",tr[0]);
    }
    else if(!strcmp(tr[0],"listjobs")){
    printf("%s Shows background processes\n",tr[0]);
    }
    else if(!strcmp(tr[0],"deljobs")){
    printf("%s [-term|-sig] Deletes job list\n\t-term: finished processes\n\t-sig: signaled processes\n",tr[0]);
    }
    else if(!strcmp(tr[0],"job")){
    printf("%s [-fg] pid Shows information about process pid\n\t-fg: Changes it to foreground\n",tr[0]);
    }
    else if(!strcmp(tr[0],"***")){
    printf("prog args... Executes, creating a process, prog with arguments\n");
    }
    else printf("%s not found\n",tr[0]);
}

void Cmd_hist(char * tr[],char * historico[]){
    int i=0;
    if(tr[0]==NULL){
        while(historico[i]!=NULL){
            printf("%d-->%s\n",i,historico[i]);
            i++;
        }
    }
    else {
        if(!strcmp(tr[0],"-c")){
            for(i=0;historico[i]!=NULL;i++){
                free(historico[i]);
                historico[i]=NULL;
            }
        }

        else if(atoi(tr[0])<=0 && atoi(tr[0])>-MAX_ARRAY){
            for(int k=0;k<=(-atoi(tr[0]));k++){
                if(historico[k]!=NULL) printf("%d-->%s\n",k,historico[k]);
                else break;}}        
    }          
}

void Cmd_comando(char *tr[],char * historico[],int rec, memList *m[], char *env[],jobList *jobs[]);

void Cmd_create(char *tr[]){
    if(tr[0]==NULL) printDirectory();
    else if(!strcmp(tr[0],"-f")){
        if(tr[1]==NULL) printDirectory();
        else if(mknod(tr[1],00666,0)==-1) perror("Could not create file");
    }
    else if(mkdir(tr[0],00777)==-1) perror("Could not create directory");
    
}

char * getUserName(uintmax_t uid){
    struct passwd* pwd;
    if((pwd = getpwuid(uid))==NULL)
    return "NONAME";
    return pwd->pw_name;
}

char * getGroupName(uintmax_t gid){
    struct group* grp;
    if((grp = getgrgid(gid))==NULL)
    return "NOGROUP";
    return grp->gr_name;
}

char LetraTF (mode_t m)
{
     switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case S_IFSOCK: return 's'; /*socket */
        case S_IFLNK: return 'l'; /*symbolic link*/
        case S_IFREG: return '-'; /* fichero normal*/
        case S_IFBLK: return 'b'; /*block device*/
        case S_IFDIR: return 'd'; /*directorio */
        case S_IFCHR: return 'c'; /*char device*/
        case S_IFIFO: return 'p'; /*pipe*/
        default: return '?'; /*desconocido, no deberia aparecer*/
     }
}

char * ConvierteModo2 (mode_t m)
{
    static char permisos[12];
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

void doStat(char * name,bool doLong, bool doLink, bool doAcc){
    struct stat sb;
    char *buf;
    ssize_t nbytes,bufsiz;
    struct tm* timeinfo;
    if (lstat(name,&sb)==-1){
        printf ("Cannot access %s: %s\n",name,strerror(errno));return;}
    if(doLong) {
        if(doAcc) timeinfo = localtime(&(sb.st_atime)); else timeinfo = localtime(&(sb.st_mtime));
        printf("%04d/%02d/%02d-%02d:%02d\t%ju (%ju)\t%s\t%s %s\t%jd ",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,(intmax_t) sb.st_nlink,(uintmax_t) sb.st_ino,
        getUserName(sb.st_uid),getGroupName(sb.st_gid),ConvierteModo2 ( sb.st_mode),(intmax_t) sb.st_size);
    if(doLink) {
    bufsiz = sb.st_size + 1; if(sb.st_size == 0) bufsiz = PATH_MAX;
    buf = malloc(bufsiz); if (buf == NULL) {perror("malloc");free(buf);return;}
    nbytes = readlink(name, buf, bufsiz); if (nbytes == -1) {printf("%s\n",name);free(buf);return;}
    printf("%s -> %.*s\n", name, (int) nbytes, buf);
    free(buf);
    }
    else printf("%s\n",name);
}
else printf("%jd\t%s\n",sb.st_size,name);
}

void Cmd_stat(char *tr[]){
    bool doLong,doLink ,doAcc;
    int i;
    doLong=doLink=doAcc=false;
    for(i=0;tr[i]!=NULL;i++){
        if(!strcmp(tr[i],"-long"))
            doLong=true;
        else if(!strcmp(tr[i],"-link"))
            doLink=true;
        else if(!strcmp(tr[i],"-acc"))
            doAcc=true;
        else break;
    }
    if(tr[i]==NULL){
    printDirectory();
        return;}
    
    for(;tr[i]!=NULL;i++){
        doStat(tr[i], doLong, doLink, doAcc);
}
}

void list_dir(char * name, bool doLong, bool doLink, bool doAcc, bool hid){
    DIR *d;
    if((d = opendir(name))==NULL){printf("Cannot open %s: %s\n",name,(strerror)errno); return;}
    struct dirent *dir;
    char buf[PATH_MAX];
    printf("************\t%s\n",name);
    while((dir = readdir(d))!=NULL){
        sprintf(buf,"%s/%s",name,dir->d_name);
        if(!hid && (dir->d_name)[0]=='.') continue;
        else doStat(buf,doLong,doLink,doAcc);
        }
    closedir(d);
}

void list_reca(char * name, bool doLong, bool doLink, bool doAcc, bool hid){
    DIR *d;
    if((d = opendir(name))==NULL){printf("Cannot open %s: %s\n",name,(strerror)errno); return;}
    struct dirent *dir;
    char buf[PATH_MAX];
    list_dir(name,doLong,doLink,doAcc,hid);
    while((dir = readdir(d))!=NULL){
        sprintf(buf,"%s/%s",name,dir->d_name);
        if(!hid && (dir->d_name)[0]=='.') continue;
        if(isDirectory(buf)&&strcmp(dir->d_name,".")&&strcmp(dir->d_name,"..")) list_reca(buf,doLong,doLink,doAcc,hid);
        }
    closedir(d);
}

void list_recb(char * name, bool doLong, bool doLink, bool doAcc, bool hid){
    DIR *d;
    if((d = opendir(name))==NULL){printf("Cannot open %s: %s\n",name,(strerror)errno); return;}
    struct dirent *dir;
    char buf[PATH_MAX];
    while((dir = readdir(d))!=NULL){
        sprintf(buf,"%s/%s",name,dir->d_name);
        if(!hid && (dir->d_name)[0]=='.') continue;
        if(isDirectory(buf)&&strcmp(dir->d_name,".")&&strcmp(dir->d_name,"..")) list_recb(buf,doLong,doLink,doAcc,hid);
        }
        list_dir(name,doLong,doLink,doAcc,hid);
    closedir(d);
}

void doList(char * name, bool doLong,bool doLink, bool doAcc, bool reca, bool recb, bool hid){
    if(!isDirectory(name)) doStat(name,doLong,doLink,doAcc);
    else{
        if(recb) list_recb(name,doLong,doLink,doAcc,hid);
        else if(reca) list_reca(name,doLong,doLink,doAcc,hid);
        else list_dir(name,doLong,doLink,doAcc,hid);
    }
}

void Cmd_list(char *tr[]){
    bool doLong, doLink ,doAcc, reca, recb, hid;
    int i;
    doLong=doLink=doAcc=reca=recb=hid=false;
    for(i=0;tr[i]!=NULL;i++){
        if(!strcmp(tr[i],"-long"))
            doLong=true;
        else if(!strcmp(tr[i],"-link"))
            doLink=true;
        else if(!strcmp(tr[i],"-acc"))
            doAcc=true;
        else if(!strcmp(tr[i],"-reca"))
            reca=true;
        else if(!strcmp(tr[i],"-recb"))
            recb=true;
        else if(!strcmp(tr[i],"-hid"))
            hid=true;
        else break;
    }
    if(tr[i]==NULL){
    printDirectory();
        return;}
    
    for(;tr[i]!=NULL;i++){
        doList(tr[i], doLong, doLink, doAcc, reca, recb, hid);
}
}

void Cmd_delete(char * tr[]){
    int i=0;
    if (tr[0]==NULL) printDirectory();
    else{
        while (tr[i]!=NULL)
        {
            if(remove(tr[i])==-1) printf("Cannot delete %s: %s\n",tr[i],strerror(errno));
            i++;
        }
        
    } 
}

void deleteItem(char *fpath) {
    struct stat sb;
    if(lstat(fpath, &sb)==-1) {
        printf("Cannot access %s: %s\n",fpath,strerror(errno));
        return;}
    if (isDirectory(fpath)) {
        DIR *d;
        struct dirent *dir;
        if((d = opendir(fpath))==NULL) {
            printf("Cannot open %s: %s\n",fpath,strerror(errno));
            return;}
        while((dir = readdir(d))!=NULL) {
            char aux[PATH_MAX];
            if(!strcmp(dir->d_name,".")||!strcmp(dir->d_name,"..")) continue;
            sprintf(aux, "%s/%s",fpath,dir->d_name);
            deleteItem(aux);
        }
        closedir(d);
    }
    if(remove(fpath)==-1) {
        printf("Cannot delete %s: %s\n",fpath,strerror(errno));
    }
}


void Cmd_deltree(char * tr[]){
    int i=0;
    if (tr[0]==NULL) printDirectory();
    else{
        while (tr[i]!=NULL){
        deleteItem(tr[i]);
        i++;
        }
    }
}


void insertList(memList *m[],void *p, off_t size, int descriptor, char * file,int key, char* allocType){
    time_t mytime;
    struct tm * timeinfo; 
    time(&mytime);
    timeinfo = localtime(&mytime);
    int i;
    for(i=0;m[i]!=NULL;i++);
    if(i==MAX_ARRAY) while(i!=0) { free(m[i]); i--;}
    m[i]=(memList*)malloc(sizeof(memList ));
    m[i]->address = p;
    m[i]->size = size;
    m[i]->descriptor = descriptor;
    if(file!=NULL) strcpy(m[i]->name,file);
    else strcpy(m[i]->name,"NULL");
    m[i]->key = key;
    if(allocType!=NULL) strcpy(m[i]->allocType,allocType);
    else strcpy(m[i]->allocType,"NULL");
    m[i]->timeinfo = *timeinfo;
    i++;
    m[i]=NULL;
}

void printAllLists(memList *m[],char * allocType){
    char buf[50];
    printf("******List of allocated ");
    if(allocType==NULL);
    else if(!strcmp(allocType,"malloc")) printf("malloc ");
    else if(!strcmp(allocType,"shared")) printf("shared ");
    else if(!strcmp(allocType,"mmap")) printf("mmap ");
    printf("blocks for process %d\n",getpid());
    for(int i = 0;m[i]!=NULL;i++){
        if(allocType == NULL);
        else if((!strcmp(allocType,"malloc")&&(strcmp(m[i]->allocType,"malloc")))||(!strcmp(allocType,"shared")&&(strcmp(m[i]->allocType,"shared")))||(!strcmp(allocType,"mmap")&&(strcmp(m[i]->allocType,"mmap")))) continue;
        strftime(buf,50,"%b %d %H:%M",&(m[i]->timeinfo));
        printf("\t%p\t\t%ld %s ",m[i]->address,m[i]->size,buf);
        if(!strcmp(m[i]->allocType,"malloc")) printf("malloc\n");
        if(!strcmp(m[i]->allocType,"shared")) printf("shared (key %d)\n",m[i]->key);
        if(!strcmp(m[i]->allocType,"mmap")) printf("%s (descriptor %d)\n",m[i]->name,m[i]->descriptor);
    }
}


void * ObtenerMemoriaShmget (key_t clave, size_t tam,memList *m[]){
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;

    if (tam)     /*tam distito de 0 indica crear */
        flags=flags | IPC_CREAT | IPC_EXCL;
    if (clave==IPC_PRIVATE)  /*no nos vale*/
        {errno=EINVAL; return NULL;}
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;
        if (tam)
             shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s);
    insertList(m, p, s.shm_segsz, 0,NULL,clave,"shared"); 
    return (p);
}

void do_AllocateCreateshared (char *tr[],memList *m[]){
   key_t cl;
   size_t tam;
   void *p;

   if (tr[0]==NULL || tr[1]==NULL) {
		printAllLists(m,"shared");
		return;
   }
  
   cl=(key_t)  strtoul(tr[0],NULL,10);
   tam=(size_t) strtoul(tr[1],NULL,10);
   if (tam==0) {
	printf ("No se asignan bloques de 0 bytes\n");
	return;
   }
   if ((p=ObtenerMemoriaShmget(cl,tam,m))!=NULL)
		printf ("Asignados %lu bytes en %p\n",(unsigned long) tam, p);
   else
		printf ("Imposible asignar memoria compartida clave %lu:%s\n",(unsigned long) cl,strerror(errno));
}

void * MapearFichero (char * fichero, int protection,memList *m[])
{
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;

    if (protection&PROT_WRITE)
          modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
          return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
           return NULL;
    insertList(m,p,s.st_size,df,fichero,0,"mmap");
    return p;
}

void do_AllocateMmap(char *arg[],memList *m[]){ 
     char *perm;
     void *p;
     int protection=0;
     
     if (arg[0]==NULL)
            {printAllLists(m,"shared"); return;}
     if ((perm=arg[1])!=NULL && strlen(perm)<4) {
            if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
            if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
            if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
     }
     if ((p=MapearFichero(arg[0],protection,m))==NULL)
             perror ("Imposible mapear fichero");
     else
             printf ("fichero %s mapeado en %p\n", arg[0], p);
}

void do_AllocateMalloc(char * bytes,memList *m[]){
    size_t n;
    void *p;
    if (bytes==NULL){printAllLists(m,"malloc");return;}
        else{
            if((n=(size_t)strtoul(bytes,NULL,10))==0) {printf("Cannot assign blocks of 0 bytes\n"); return;}
            p=(void*)malloc(n);
            if(p==NULL) {printf("Impossible to obtain memory with malloc: %s\n",(strerror)errno);return;}
            else printf("Assigned %ld bytes in %p\n",n,p);
            }
            insertList(m,p,n,0,NULL,0,"malloc");
}

void do_AllocateShared(char *tr[],memList*m[]){
    void * p;
    int flags = 0777,sharedId;
    key_t cl;
    struct shmid_ds buf;
    if (tr[0]==NULL){printAllLists(m,"shared");return;}
    cl=(key_t)  strtoul(tr[0],NULL,10);
    if((sharedId = shmget(cl,0,flags))==-1) {perror("shmget"); return;}
    if(shmctl(sharedId, IPC_STAT, &buf)==-1) {perror("shmctl"); return;}
    if ((p=shmat(sharedId,NULL,0))==(void*) -1){
        perror("shmat"); return;
    }
    printf("Shared memory of key %d in %p\n",cl,p);
    insertList(m,p,buf.shm_segsz,buf.shm_perm.mode,NULL,cl,"shared");

}

void Cmd_allocate(char * tr[], memList *m[]){
    if(tr[0]==NULL){printAllLists(m,NULL);}
    else if(!(strcmp(tr[0],"-malloc"))){do_AllocateMalloc(tr[1],m);}
    else if(!strcmp(tr[0],"-shared")){do_AllocateShared(tr+1,m);}
    else if(!strcmp(tr[0],"-createshared")){do_AllocateCreateshared(tr+1,m);}
    else if(!strcmp(tr[0],"-mmap")){do_AllocateMmap(tr+1,m);}
    else printf("usage: allocate [-malloc|-shared|-createshared|-mmap] ....\n");    
}


void memfill (void *p, size_t cont, unsigned char byte)
{
  unsigned char *arr=(unsigned char *) p;
  size_t i;
  printf("Filling %ld bytes of memory with the byte %c(%02x) starting at adress 0x%p\n",cont,byte,(unsigned char) byte,p);

  for (i=0; i<cont;i++)
		arr[i]=byte;
}

void Cmd_memfill(char *tr[]){
    size_t aux1=128;
    unsigned char aux2 = 'A';
    if(tr[0]==NULL) return;
    if(tr[1]!=NULL) sscanf(tr[1], "%zu", &aux1);
    if(tr[2]!=NULL) aux2 = (unsigned char)tr[2][0];
    memfill(cadtop(tr[0]),aux1,aux2);
}

void do_Pmap (void) /*sin argumentos*/
 { pid_t pid;       /*hace el pmap (o equivalente) del proceso actual*/
   char elpid[32];
   char *argv[4]={"pmap",elpid,NULL};
   
   sprintf (elpid,"%d", (int) getpid());
   if ((pid=fork())==-1){
      perror ("Imposible crear proceso");
      return;
      }
   if (pid==0){
      if (execvp(argv[0],argv)==-1)
         perror("cannot execute pmap (linux, solaris)");
         
      argv[0]="procstat"; argv[1]="vm"; argv[2]=elpid; argv[3]=NULL;   
      if (execvp(argv[0],argv)==-1)/*No hay pmap, probamos procstat FreeBSD */
         perror("cannot execute procstat (FreeBSD)");
         
      argv[0]="procmap",argv[1]=elpid;argv[2]=NULL;    
            if (execvp(argv[0],argv)==-1)  /*probamos procmap OpenBSD*/
         perror("cannot execute procmap (OpenBSD)");
         
      argv[0]="vmmap"; argv[1]="-interleave"; argv[2]=elpid;argv[3]=NULL;
      if (execvp(argv[0],argv)==-1) /*probamos vmmap Mac-OS*/
         perror("cannot execute vmmap (Mac-OS)");      
      exit(1);
  }
  waitpid (pid,NULL,0);
}

void Cmd_memory(char *tr[],memList *m[]){
    int i,j,k;
    static char q,w,e;
    if(tr[0]==NULL||!strcmp(tr[0],"-all")) {printf("Local variables \t%p, %p, %p\nGlobal variables \t%p, %p, %p\nStatic variables \t%p, %p, %p\nProgram functions \t%p, %p, %p\nLibrary functions \t%p, %p, %p\n",
    (void*)&i,(void*)&j,(void*)&k,(void*)&glob1,(void*)&glob2,(void*)&glob3,(void*)&q,(void*)&w,(void*)&e,&Cmd_autores,&Cmd_ayuda,&Cmd_allocate,&lstat,&getpwuid,&getgrgid);
    printAllLists(m,NULL);
    }
    else if(!strcmp(tr[0],"-blocks")) printAllLists(m,NULL);
    else if(!strcmp(tr[0],"-funcs")) printf("Program functions \t%p, %p, %p\nLibrary functions \t%p, %p, %p\n",&Cmd_autores,&Cmd_ayuda,&Cmd_allocate,&lstat,&getpwuid,&getgrgid);
    else if(!strcmp(tr[0],"-vars")) printf("Local variables \t%p, %p, %p\nGlobal variables \t%p, %p, %p\nStatic variables \t%p, %p, %p\n",(void*)&i,(void*)&j,(void*)&k,(void*)&glob1,(void*)&glob2,(void*)&glob3,&q,&w,&e);
    else if(!strcmp(tr[0],"-pmap")) do_Pmap();
    else (printf("Option %s not contemplated\n",tr[0]));
}   


void recurse(int n){
  char dynamicArr[SIZE];
  static char staticArr[SIZE];
  printf ("parameter:%3d(%p) array %p, static arr %p\n",n,&n,dynamicArr, staticArr);
  if (n>0) recurse(n-1);
}

void Cmd_recurse(char *tr[]){
    if(tr[0]==NULL) return;
    recurse(atoi(tr[0]));
}

ssize_t LeerFichero (char *f, void *p, size_t cont)
{
   struct stat s;
   ssize_t  n;  
   int df,aux;

   if (stat (f,&s)==-1 || (df=open(f,O_RDONLY))==-1)
	return -1;     
   if (cont==-1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
	cont=s.st_size;
   if ((n=read(df,p,cont))==-1){
	aux=errno;
	close(df);
	errno=aux;
	return -1;
   }
   close (df);
   return n;
}

void do_I_O_read (char *ar[])
{
   void *p;
   size_t cont=-1;
   ssize_t n;
   if (ar[0]==NULL || ar[1]==NULL){
	printf ("faltan parametros\n");
	return;
   }
   p=cadtop(ar[1]);  /*convertimos de cadena a puntero*/
   if (ar[2]!=NULL)
	cont=(size_t) atoll(ar[2]);

   if ((n=LeerFichero(ar[0],p,cont))==-1)
	perror ("Imposible leer fichero");
   else
	printf ("leidos %lld bytes de %s en %p\n",(long long) n,ar[0],p);
}

ssize_t EscribirFichero (char *f, void *p, size_t cont,int overwrite)
{
   ssize_t  n;
   int df,aux, flags=O_CREAT | O_EXCL | O_WRONLY;

   if (overwrite)
	flags=O_CREAT | O_WRONLY | O_TRUNC;

   if ((df=open(f,flags,0777))==-1)
	return -1;

   if ((n=write(df,p,cont))==-1){
	aux=errno;
	close(df);
	errno=aux;
	return -1;
   }
   close (df);
   return n;
}

void Cmd_i_o(char *tr[]){
    if(tr[0]==NULL) {printf("use: i-o [read|write] ......\n"); return;}
    if (!strcmp(tr[0],"read")){
       do_I_O_read(tr+1);
    }
    else if (!strcmp(tr[0],"write")){
        if(!(tr[1]&&!strcmp(tr[1],"-o"))){
            EscribirFichero(tr[1],cadtop(tr[2]),(size_t)tr[3],false);
        }
        else EscribirFichero(tr[2],cadtop(tr[3]),(size_t)tr[4],true);
        
        
    }
    
}
bool isAscii(char a){
    return (a>31&&a<127);
}
char * prettyChar(char a){
    if((unsigned char)a==10) return "\\n";
    else if((unsigned char)a==9) return "\\t";
    else if((unsigned char)a==13) return "\\r";
    if(isAscii(a)){
        char aux[5]={0};
        char * aux2 =aux;
        if(snprintf(aux2,5,"%c ",(unsigned char)a)<0) {perror("snprintf"); return"  ";}
        return aux2;
    }
    else return "  ";
}
void memdump(char *arr, unsigned long cont){
    int i=0,j=0;
    int lines = cont/(unsigned long)LINESIZE;
    int remainder = cont%(unsigned long)LINESIZE;
    for(i=0;i<lines;i++){
    for(j = 0;j<LINESIZE;j++){
        printf(" %s",prettyChar(arr[j+i*LINESIZE]));

    }
    printf("\n");
    for(j=0; j<LINESIZE;j++){
        printf("%02x ",(unsigned char)arr[j+i*LINESIZE]);
    }
    printf("\n");
    }
    if(remainder!=0){
        for(j = 0;j<remainder;j++){
            printf(" %s",prettyChar(arr[j+lines*LINESIZE]));
        }
        printf("\n");
        for( j=0; j<remainder;j++){
        printf("%02x ",(unsigned char)arr[j+lines*LINESIZE]);
        }
        printf("\n");
    }

}

void Cmd_memdump(char *tr[]){
    if(tr[0]==NULL) return;
    unsigned long nbytes = (tr[1]==NULL)? (unsigned long)LINESIZE:strtoul(tr[1],NULL,10);
    printf("Dumping %ld bytes from address %s\n",nbytes,tr[0]);
    memdump(cadtop(tr[0]),nbytes);
}

void reallocateList(int pos,memList *m[]){
    while(m[pos+1]!= NULL){
        m[pos]=m[pos+1];
        pos++;
    }
    m[pos]=NULL;
}

void do_deallocateMalloc(size_t size,memList *m[]){
    int i;
    if(size<=0) return;
    for(i = 0;m[i]!=NULL;i++){
        if(m[i]->size==size) {
            printf("Deallocating malloc memory with size %ld\n",size);
            free(m[i]->address);
            free(m[i]); m[i]=NULL; break;;}
    }
    reallocateList(i,m);
}
void do_deallocateShared(key_t key,memList *m[]){
    int i;
    if(key<=0) return;
    for(i = 0;m[i]!=NULL;i++){
        if(m[i]->key==key) {
            printf("Deallocating shared memory with key %d\n",key);
            if(shmdt(m[i]->address)==-1) {perror("shmdt"); return;}
            free(m[i]); m[i]=NULL; break;}
    }
    reallocateList(i,m);
}
void do_deallocateDelkey(key_t key,memList *m[]){
    int id,flags=0777;
    struct shmid_ds buf;
    if(key<=0) return;
    if((id=shmget(key,0,flags))==-1) {perror("shmget"); return;}
    if(shmctl(id,IPC_RMID,&buf)==-1) {perror("shmdt"); return;}
    printf("Deleted key %d\n",key);
}
void do_deallocateMmap(char *tr,memList *m[]){
    int i;
    if(tr==NULL) return;
    for(i = 0;m[i]!=NULL;i++){
        if(!strcmp(m[i]->name,tr)) {
            printf("Deallocating mapped memory of file %s\n",tr);
            if(munmap(m[i]->address,m[i]->size)==-1) {perror("munmap"); return;}
            free(m[i]); m[i]=NULL; break;}
    }
    reallocateList(i,m);
}
void deleteAdress(memList *m[],void *p){
    int i;
    if(p==NULL) return;
    for(i = 0;m[i]!=NULL;i++){
        if(m[i]->address==p) {
            if(!strcmp(m[i]->allocType,"malloc")) do_deallocateMalloc(m[i]->size,m);
            else if(!strcmp(m[i]->allocType,"shared")) do_deallocateShared(m[i]->key,m);
            else do_deallocateMmap(m[i]->name,m);}
    }
}
void Cmd_deallocate(char *tr[],memList *m[]){
    if(tr[0]==NULL){printAllLists(m,NULL);}
    else if(!(strcmp(tr[0],"-malloc"))){
        if(tr[1]==NULL) {printAllLists(m,"malloc"); return;}
        do_deallocateMalloc(atol(tr[1]),m);
        }
    else if(!strcmp(tr[0],"-shared")){
        if(tr[1]==NULL) {printAllLists(m,"shared"); return;}
        do_deallocateShared(atoi(tr[1]),m);
        }
    else if(!strcmp(tr[0],"-delkey")){
        if(tr[1]==NULL) {printf("Delkey needs a valid key\n"); return;}
        do_deallocateDelkey(atoi(tr[1]),m);}
    else if(!strcmp(tr[0],"-mmap")){
        if(tr[1]==NULL) {printAllLists(m,"mmap"); return;}
        do_deallocateMmap(tr[1],m);}

    else deleteAdress(m,cadtop(tr[0]));
}
//-----------------------------------------P3------------------------------------------------------------------//
static struct SEN sigstrnum[]={
    {"ACTIVE",0},   
	{"HUP", SIGHUP},
	{"INT", SIGINT},
	{"QUIT", SIGQUIT},
	{"ILL", SIGILL}, 
	{"TRAP", SIGTRAP},
	{"ABRT", SIGABRT},
	{"IOT", SIGIOT},
	{"BUS", SIGBUS},
	{"FPE", SIGFPE},
	{"KILL", SIGKILL},
	{"USR1", SIGUSR1},
	{"SEGV", SIGSEGV},
	{"USR2", SIGUSR2}, 
	{"PIPE", SIGPIPE},
	{"ALRM", SIGALRM},
	{"TERM", SIGTERM},
	{"CHLD", SIGCHLD},
	{"CONT", SIGCONT},
	{"STOP", SIGSTOP},
	{"TSTP", SIGTSTP}, 
	{"TTIN", SIGTTIN},
	{"TTOU", SIGTTOU},
	{"URG", SIGURG},
	{"XCPU", SIGXCPU},
	{"XFSZ", SIGXFSZ},
	{"VTALRM", SIGVTALRM},
	{"PROF", SIGPROF},
	{"WINCH", SIGWINCH}, 
	{"IO", SIGIO},
	{"SYS", SIGSYS},
/*senales que no hay en todas partes*/
#ifdef SIGPOLL
	{"POLL", SIGPOLL},
#endif
#ifdef SIGPWR
	{"PWR", SIGPWR},
#endif
#ifdef SIGEMT
	{"EMT", SIGEMT},
#endif
#ifdef SIGINFO
	{"INFO", SIGINFO},
#endif
#ifdef SIGSTKFLT
	{"STKFLT", SIGSTKFLT},
#endif
#ifdef SIGCLD
	{"CLD", SIGCLD},
#endif
#ifdef SIGLOST
	{"LOST", SIGLOST},
#endif
#ifdef SIGCANCEL
	{"CANCEL", SIGCANCEL},
#endif
#ifdef SIGTHAW
	{"THAW", SIGTHAW},
#endif
#ifdef SIGFREEZE
	{"FREEZE", SIGFREEZE},
#endif
#ifdef SIGLWP
	{"LWP", SIGLWP},
#endif
#ifdef SIGWAITING
	{"WAITING", SIGWAITING},
#endif
 	{NULL,-1},
	};    /*fin array sigstrnum */


int ValorSenal(char * sen)  /*devuelve el numero de senial a partir del nombre*/ 
{ 
  int i;
  for (i=0; sigstrnum[i].nombre!=NULL; i++)
  	if (!strcmp(sen, sigstrnum[i].nombre))
		return sigstrnum[i].senal;
  return -1;
}


char *NombreSenal(int sen)  /*devuelve el nombre senal a partir de la senal*/ 
{			/* para sitios donde no hay sig2str*/
 int i;
  for (i=0; sigstrnum[i].nombre!=NULL; i++)
  	if (sen==sigstrnum[i].senal)
		return sigstrnum[i].nombre;
 return ("SIGUNKNOWN");
}

void insertJobList(jobList *jobs[],pid_t pid,char * state, char *tr[], int prio){
    int j = 1;
    char * userName = getUserName(getuid());
    time_t mytime;
    struct tm * timeinfo;
    time(&mytime);
    timeinfo = localtime(&mytime);
    int i;
    for(i=0;jobs[i]!=NULL;i++);
    if(i==MAX_ARRAY) while(i!=0) { free(jobs[i]); i--;}
    jobs[i]=(jobList*)malloc(sizeof(jobList ));
    jobs[i]->pid=pid;
    strcpy(jobs[i]->state,state);
    jobs[i]->prio = prio;
    strcpy(jobs[i]->name,tr[0]);
    while(tr[j]!=NULL){
        strcat(jobs[i]->name," ");
        strcat(jobs[i]->name,tr[j]);
        j++;
    }
    jobs[i]->timeinfo = *timeinfo;
    if(userName != NULL) strcpy(jobs[i]->user,userName);
    else strcpy(jobs[i]->user,"NOUSER");
    i++;
    jobs[i]=NULL;

}
void updateList(jobList *jobs[]){
    int status;
    for(int i = 0; jobs[i]!=NULL;i++){
        if(waitpid(jobs[i]->pid, &status, WNOHANG | WUNTRACED | WCONTINUED )==jobs[i]->pid){
        if (WIFEXITED(status)){
            snprintf(jobs[i]->state,MAXNAME,"FINISHED (%03d)",WEXITSTATUS(status));
        }
        else if (WIFCONTINUED(status)){
            strcpy(jobs[i]->state, "ACTIVE (000)");
        }
        else if (WIFSTOPPED(status)){
            snprintf(jobs[i]->state,MAXNAME, "STOPPED (%03d)",WSTOPSIG(status));
        }
        else if (WTERMSIG(status)){
            snprintf(jobs[i]->state,MAXNAME, "SIGNALED (%s)",NombreSenal(WTERMSIG(status)));
        }
        jobs[i]->prio = getpriority(PRIO_PROCESS,jobs[i]->pid);
        }
    }
}

void Cmd_listJobs(jobList *jobs[],pid_t pid){
    updateList(jobs);
    for (int i = 0; jobs[i]!=NULL; i++){
        if (pid >= 0) 
            printf("%d\t%s p=%d %04d/%02d/%02d %02d:%02d:%02d %s %s\n",jobs[i]->pid,jobs[i]->user,jobs[i]->prio,jobs[i]->timeinfo.tm_year+1900,jobs[i]->timeinfo.tm_mon+1,jobs[i]->timeinfo.tm_mday,jobs[i]->timeinfo.tm_hour,jobs[i]->timeinfo.tm_min,jobs[i]->timeinfo.tm_sec,jobs[i]->state, jobs[i]->name);
        else if( jobs[i]->pid == pid)
            printf("%d\t%s p=%d %04d/%02d/%02d %02d:%02d:%02d %s %s\n",jobs[i]->pid,jobs[i]->user,jobs[i]->prio,jobs[i]->timeinfo.tm_year+1900,jobs[i]->timeinfo.tm_mon+1,jobs[i]->timeinfo.tm_mday,jobs[i]->timeinfo.tm_hour,jobs[i]->timeinfo.tm_min,jobs[i]->timeinfo.tm_sec,jobs[i]->state, jobs[i]->name);
    }
}

void reallocateJobList(int pos,jobList *jobs[]){
    while(jobs[pos+1]!= NULL){
        jobs[pos]=jobs[pos+1];
        pos++;
    }
    jobs[pos]=NULL;
}

void deleteJobs(jobList *jobs[],int option,pid_t pid){
    for (int i = 0; jobs[i]!=NULL; i++){
        if(pid>0){
            if(jobs[i]->pid == pid){
                free(jobs[i]); reallocateJobList(i,jobs);
            i--;
            }
        }
        else if(option == 0){
            free(jobs[i]); reallocateJobList(i,jobs);
            i--;
        }
        else if(option==1){
            char aux[MAXNAME];
            strcpy(aux,jobs[i]->state);
            char *aux2 = strtok(aux, " "); 
            if (aux2!=NULL && !strcmp(aux2,"FINISHED")){
            free(jobs[i]); reallocateJobList(i,jobs);
            i--;
            }
        }
        else if(option==2){
            char aux[MAXNAME];
            strcpy(aux,jobs[i]->state);
            char *aux2 = strtok(aux, " "); 
            if (aux2!=NULL && !strcmp(aux2,"SIGNALED")){
            free(jobs[i]); reallocateJobList(i,jobs);
            i--;}
        }
    }
}

void Cmd_deljobs(char *tr[],jobList *jobs[]){
    Cmd_listJobs(jobs,0);
    if(tr[0]==NULL) {return;} 
    if(!strcmp(tr[0],"-term")) {updateList(jobs);deleteJobs(jobs,1,-1); return;}
    if(!strcmp(tr[0],"-sig")) {updateList(jobs);deleteJobs(jobs,2,-1);}
}

void Cmd_priority(char *tr[]){
    int prio;
    pid_t pid = getpid();
    if(tr[0]==NULL)printf("Priority of proccess %d is %d\n",pid,getpriority(PRIO_PROCESS,pid));
    else if(tr[1]==NULL) {if((prio=getpriority(PRIO_PROCESS,atoi(tr[0])))==-1){ perror("Cannot obtain priority"); return;}
    else printf("Priority of proccess %d is %d\n",atoi(tr[0]),prio);}
    else if(setpriority(PRIO_PROCESS,atoi(tr[0]),atoi(tr[1]))==-1) perror("Cannot change priority"); 
    
}

int BuscarVariable (char * var, char *e[])  /*busca una variable en el entorno que se le pasa como parámetro*/
{
  int pos=0;
  char aux[MAXVAR];
  
  strncpy (aux,var,MAXVAR);
  strcat (aux,"=");
  
  while (e[pos]!=NULL)
    if (!strncmp(e[pos],aux,strlen(aux)))
      return (pos);
    else 
      pos++;
  errno=ENOENT;   /*no hay tal variable*/
  return(-1);
}

int CambiarVariable(char * var, char * valor, char *e[]) /*cambia una variable en el entorno que se le pasa como parámetro*/
{                                                        /*lo hace directamente, no usa putenv*/
  int pos;
  char *aux;
   
  if ((pos=BuscarVariable(var,e))==-1)
    return(-1);
 
  if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
	return -1;
  strcpy(aux,var);
  strcat(aux,"=");
  strcat(aux,valor);
  e[pos]=aux;
  return (pos);
}

void Cmd_changeVar(char *tr[], char *envp[]){
    if(tr[0]==NULL|| tr[1]==NULL|| tr[2]==NULL){ printf("Use: changevar [-a|-e|-p] var value\n"); return;}
    if(!strcmp(tr[0],"-a")){
        if(CambiarVariable(tr[1],tr[2],envp)==-1) printf("Cannot change variable\n");
        return;
    }
    else if(!strcmp(tr[0],"-e")){
        if(CambiarVariable(tr[1],tr[2],environ)==-1) printf("Cannot change variable\n");
        return;
    }
    else if(!strcmp(tr[0],"-p")){
        static char foo[MAXVAR];
        snprintf(foo,MAXVAR,"%s=%s",tr[1],tr[2]);

        if(putenv(foo)==-1) printf("Cannot change variable\n");
        return;
    }
    else printf("Use: changevar [-a|-e|-p] var value\n");
}

void Cmd_showEnv(char *tr[],char * envp[]){
    int i = 0;
    bool mainArg3 = true;
    char **env = envp;
    if(tr[0]!=NULL && (!strcmp(tr[0],"-addr"))){
        printf("environ:\t%p (stored in %p)\nmain arg3:\t%p (stored in %p)\n",environ,&environ,envp,&envp);
        return;
    }
    if(tr[0]!=NULL && (!strcmp(tr[0],"-environ"))){
        env = environ;
        mainArg3 = false;
    }
    while (env[i]!=NULL){
    printf ("%p->",&env[i]);
    if(mainArg3) printf("main arg3"); 
    else printf("environ");
    printf("[%d]=(%p) %s\n",i, env[i],env[i]);
    i++;
    }
}

void Cmd_showVar(char *tr[],char *envp[]){
    int i = 0;
    if(tr[0]==NULL) {Cmd_showEnv(tr,envp); return;}
    const char *value = getenv(tr[0]);
    if(value != NULL){
    while (envp[i]!=NULL){
        char name[MAXVAR];
        strcpy(name,envp[i]);
        char *token = strtok(name, "=");
        if(!strcmp(token,tr[0])){
        printf ("With arg3 main %s (%p) @%p\n",envp[i],envp[i],&envp[i]);
        break;;;
        }
        i++;
    }
    while (environ[i]!=NULL){
        char name[MAXVAR];
        strcpy(name,environ[i]);
        char *token = strtok(name, "=");
        if(!strcmp(token,tr[0])){
        printf ("With environ %s (%p) @%p\n",environ[i],environ[i],&environ[i]);
        break;;;
        }
        i++;
    }
    printf("With getenv %s (%p)\n",value,value);
    }
    
}

void Cmd_fork (char *tr[],jobList *jobs[])
{
	pid_t pid;
	
	if ((pid=fork())==0){
        deleteJobs(jobs,0,-1);
		printf ("ejecutando proceso %d\n", getpid());
	}
	else if (pid!=-1)
		waitpid (pid,NULL,0);
}

char * Ejecutable (char *s)
{
	char path[MAXNAME];
	static char aux2[MAXNAME];
	struct stat st;
	char *p;
	if (s==NULL || (p=getenv("PATH"))==NULL)
		return s;
	if (s[0]=='/' || !strncmp (s,"./",2) || !strncmp (s,"../",3))
        return s;       /*is an absolute pathname*/
	strncpy (path, p, MAXNAME);
	for (p=strtok(path,":"); p!=NULL; p=strtok(NULL,":")){
       sprintf (aux2,"%s/%s",p,s);
	  if (lstat(aux2,&st)!=-1)
		return aux2;
	}
	return s;
}

int OurExecvpe(char *file, char *const argv[], char *const envp[])
{
   return (execve(Ejecutable(file),argv, envp));
}

void Cmd_external(char *tr[],char *env[],jobList *jobs[]){
    bool background= false,priority= false;
    int status;
    int prio;
    if(tr[0]==NULL) {printf("Command not found\n"); return;}
    pid_t pid;
    if ((pid=fork())==-1){
      perror ("Imposible to create a process");
      return;}
    int i = 0;
    char **aux = (char**)malloc(MAXVAR*sizeof(tr[0]));
    if (aux ==NULL){perror ("malloc"); return;}
    while(tr[i]!=NULL && BuscarVariable(tr[i],environ)!=-1){
        aux[i] = malloc((MAX/2) * sizeof(char)+1);
        if(aux[i]==NULL) break;
        snprintf(aux[i],(MAX/2)+1,"%s=%s",tr[i],getenv(tr[i]));
        i++;
    }
    aux[i]=NULL;
    tr = tr + i;
    for(int i = 0; tr[i]!=NULL;i++){
        if(!strcmp(tr[i],"&")){
            background = true;
            tr[i]=NULL;}
    }
    for(int i = 0; tr[i]!=NULL;i++){
        if(tr[i][0]=='@'){
            priority = true;
            sscanf(tr[i],"@%d",&prio);
            tr[i]=NULL;}
    }
    if (pid==0){
        if(aux[0]!=NULL){if (OurExecvpe(tr[0],tr,aux)==-1)
        printf("Cannot execute command %s: %s\n",tr[0],strerror(errno));
        exit(-1);}
        else if (OurExecvpe(tr[0],tr,env)==-1)
        printf("Cannot execute command %s: %s\n",tr[0],strerror(errno));
        exit(-1);}
    if(priority){ 
        if (setpriority(PRIO_PROCESS,pid,prio)==-1)
            perror("setpriority");}
    if(background){
    insertJobList(jobs,pid,"ACTIVE (000)",tr,getpriority(PRIO_PROCESS,pid));}
    else waitpid (pid,&status, WUNTRACED );
    for(int i = 0; aux[i]!=NULL; i++){
        free(aux[i]);
    }
	free (aux);
}

void Cmd_execute(char *tr[],char *env[]){
    int prio;
    bool priority = false;
    for(int i = 0; tr[i]!=NULL;i++){
        if(tr[i][0]=='@'){
            priority = true;
            sscanf(tr[i],"@%d",&prio);
            tr[i]=NULL;}
    }
    if(priority){ 
        if (setpriority(PRIO_PROCESS,getpid(),prio)==-1)
            perror("setpriority");}
    if (OurExecvpe(tr[0],tr,env)==-1)
        printf("Cannot execute command %s: %s\n",tr[0],strerror(errno));  
}
void Cmd_job(char *tr[],jobList *jobs[]){
    int status;
    if(tr[0]==NULL) {Cmd_listJobs(jobs,0); return;}
    if(!strcmp(tr[0],"-fg")){
        if(tr[1]!=NULL){
            pid_t pid = (pid_t)atoi(tr[1]);
        if(waitpid(pid,&status,0)==-1) {perror("waitpid"); return;}
            if (WIFEXITED(status)){
            printf("Process %d exited normally, value returned %d\n",pid,WEXITSTATUS(status));
        }
        else if (WIFCONTINUED(status)){
            printf("Process %d is active\n",pid);
        }
        else if (WIFSTOPPED(status)){
            printf("Process %d was stopped by %s\n",pid,NombreSenal(WSTOPSIG(status)));
        }
        else if (WTERMSIG(status)){
            printf("Process %d was signaled by %s\n",pid,NombreSenal(WTERMSIG(status)));
        }
        deleteJobs(jobs,0,atoi(tr[1]));
        return;
        }
    }
    else Cmd_listJobs(jobs,atoi(tr[0]));
    }



int TrocearCadena(char * cadena, char * trozos[]){
    int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

struct CMD c[]={
    {"autores",Cmd_autores},
    {"carpeta",Cmd_carpeta},
    {"pid",Cmd_pid},
    {"fecha",Cmd_fecha},
    {"infosis",Cmd_infosis},
    {"fin",Cmd_fin},
    {"bye",Cmd_fin},
    {"salir",Cmd_fin},
    {"exit",Cmd_fin},
    {"ayuda",Cmd_ayuda},
    {"create",Cmd_create},
    {"stat",Cmd_stat},
    {"list",Cmd_list},
    {"delete",Cmd_delete},
    {"deltree",Cmd_deltree},
    {"recurse",Cmd_recurse},
    {"memfill",Cmd_memfill},
    {"i-o",Cmd_i_o},
    {"memdump",Cmd_memdump},
    {"priority",Cmd_priority},
    {NULL,NULL}
};

void ProcesarEntrada(char * tr[],char * historico[], memList *m[], char * env[], jobList *jobs[]){
    int i;
    if(tr[0]==NULL) return;
    for(i=0;c[i].name!=NULL;i++){
        if(!strcmp(tr[0],"hist")){
            Cmd_hist(tr+1,historico);
            return;
        }
        else if(!strcmp(tr[0],"comando")){
            Cmd_comando(tr+1,historico,0,m,env,jobs);
            return;
        }
        else if(!strcmp(tr[0],"allocate")){
            Cmd_allocate(tr+1,m);
            return;
        }
        else if(!strcmp(tr[0],"deallocate")){
            Cmd_deallocate(tr+1,m);
            return;
        }
        else if(!strcmp(tr[0],"memory")){
            Cmd_memory(tr+1,m);
            return;
        }
        else if(!strcmp(tr[0],"execute")){
            Cmd_execute(tr+1,env);
            return;
        }
        else if(!strcmp(tr[0],"showenv")){
            Cmd_showEnv(tr+1,env);
            return;
        }
        else if(!strcmp(tr[0],"changevar")){
            Cmd_changeVar(tr+1,env);
            return;
        }
        else if(!strcmp(tr[0],"listjobs")){
            Cmd_listJobs(jobs,0);
            return;
        }
        else if(!strcmp(tr[0],"deljobs")){
            Cmd_deljobs(tr+1,jobs);
            return;
        }
        else if(!strcmp(tr[0],"showvar")){
            Cmd_showVar(tr+1,env);
            return;
        }
        else if(!strcmp(tr[0],"fork")){
            Cmd_fork(tr+1,jobs);
            return;
        }
        else if(!strcmp(tr[0],"job")){
            Cmd_job(tr+1,jobs);
            return;
        }
        else if(!strcmp(c[i].name,tr[0])){
            (*c[i].func)(tr+1);
            return;
        }
    }
    Cmd_external(tr,env,jobs);
}

void GuardarHistorico(char entrada[], char * historico[]){
    static int i=0;
    if(historico[0]==NULL) i=0;
    if(i==MAX_ARRAY-1) while(i!=0) { free(historico[i]); i--;}
    if (entrada[1]!='\0'){
    historico[i]=(char*)malloc(strlen(entrada)+1);
    if(historico[i]==NULL){perror("malloc");return;}
    strcpy(historico[i],entrada);
    historico[i][strlen(historico[i]) - 1] = '\0';
    i++;
    historico[i]=NULL;}

}

int getLastPos(char * historico[]){
    int i=0;
    while (historico[i]!=NULL) i++;
    return i-1;
}

void Cmd_comando(char * tr[],char * historico[],int rec,memList *m[], char *env[], jobList *jobs[]){
    char * option[MAX/2], entrada[MAX];
    if(tr[0]==NULL) {printf("Please specify the number of the comand\n"); return;}
    if(!(atoi(tr[0])<0||atoi(tr[0])>=MAX_ARRAY) && atoi(tr[0])<=getLastPos(historico) && historico[atoi(tr[0])]!=NULL){
        strcpy(entrada,historico[atoi(tr[0])]);
        TrocearCadena(entrada,option);
        printf("Executing hist (%d): %s\n", atoi(tr[0]),historico[atoi(tr[0])]);
        if(!strcmp(option[0],"comando")){
            if(rec<MAX_RECURSION) {Cmd_comando(option+1,historico,rec+1,m,env,jobs); return;}
            else {printf("Too much recursion\n");return;}
        }
        ProcesarEntrada(option,historico,m,env,jobs); return;
    }
    else {printf("Invalid command\n");return;}
}

int main(int arg, char *argv[], char *env[]){
    char entrada[MAX], * trozos[MAX/2], * historico [MAX_ARRAY];
    historico[0]=NULL; memList *m[MAX_ARRAY]; m[0] = NULL;
    jobList *jobs[MAX_ARRAY]; jobs[0] = NULL; 
    addr1 = &argv;
    addr2 = &env;
    
    while(1){
        printf("> ");
        fgets(entrada,MAX,stdin);
        GuardarHistorico(entrada, historico);
        if(TrocearCadena(entrada,trozos)==0)
            continue;
        ProcesarEntrada(trozos, historico, m, env,jobs);
    }
}
