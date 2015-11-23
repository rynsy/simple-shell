/* $begin shellmain */
#include "csapp.h"
#define MAXARGS   128

/* my globals */
#define PROMPTSIZE 16
char prompt[PROMPTSIZE] = "upsh>";

#define MAXPLUGGINS 20
void* handles[MAXPLUGGINS];
void (*method) (char**);
void (*analyzer) (char**);
int pluggins = 0;

struct NewBuiltIn {
    char CommandName[64];   
    char FunctionName[64]; 
    char AnalyzerName[64];
} *pluggin_methods[MAXPLUGGINS];


#define MAXJOBS 10
int jobnum = 0;

struct JobList {
    pid_t pid;
    char cmd[MAXLINE];
    int running;
    int bg;
} jobs[MAXJOBS];

/* function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int p3parseline(char *buf, char **argv); /* new parseline function for cs485 project 3 */
int builtin_command(char **argv);
int loaded_command(char **argv);
void child_handler(int sig);


int main()
{
        char cmdline[MAXLINE]; /* Command line */

        while (1) {
                /* Read */
                printf("%s",prompt); 
                fgets(cmdline, MAXLINE, stdin);
                if (feof(stdin))
                        exit(0);

                /* Evaluate */
                eval(cmdline);
        }
}
/* $end shellmain */

/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline)
{
        char *argv[MAXARGS];    /* Argument list execve() */
        char buf[MAXLINE];      /* Holds modified command line */
        char cmd[MAXLINE];      /* A temporary buffer for making a pretty command for the jobslist */
        int bg, i;            /* Should the job run in bg or fg? */
        int inFile, outFile;    /* file descriptors for redirection */
        int directIn, directOut; /* flags used to indicate what needs to be redirected */
        signal(SIGCHLD, child_handler); 
        i = inFile = outFile = directIn = directOut = 0;

        while( i < pluggins) {
            pluggin_methods[i] = dlsym(handles[i], "pluggin_method");
            if( strcmp(pluggin_methods[i]->AnalyzerName,"") > 0 ) {
                analyzer = dlsym(handles[i], pluggin_methods[i]->AnalyzerName);
                analyzer(argv);     //Assuming analyzers are void functions
            }
            //printf("Just checked analyzer for command: %s\n", pluggin_methods[i]->CommandName);
            i++;
        }

        strcpy(buf, cmdline);
        /*    bg = parseline(buf, argv); */
        bg = p3parseline(buf, argv); /* call new parseline function for cs485 project 3 */
        if (argv[0] == NULL)
                return;  /* Ignore empty lines */

        if (!builtin_command(argv)) {
           if (!loaded_command(argv)) {
                
                i = 0;
                strcpy(cmd, "");
                while( argv[i] != NULL && i < MAXARGS ) {
                   strcat(cmd, argv[i]);
                   strcat(cmd, " ");
                   i++; 
                }   // make a pretty job title -- job titles are important
                strcpy(jobs[jobnum].cmd, cmd);

                //check for redirection
                i = 0;
                while( argv[i] != NULL && i < MAXARGS ) {
                    if (!strcmp(argv[i], "<")) {
                        if(argv[i+1] != NULL) {
                            directIn = 1;
                            inFile = Open(argv[i++], O_RDONLY, S_IRUSR);
                        } else {
                            unix_error("syntax error: expected '< infilename'");
                        }

                    } else if (!strcmp(argv[i], ">")) {
                        if(argv[i+1] != NULL) {
                            directOut = 1;
                            outFile = Open(argv[i++], O_CREAT|O_TRUNC|O_RDWR, S_IRUSR|S_IWUSR);
                        } else {
                            unix_error("syntax error: expected '> outfilename'");
                        }
                    } else {
                        i++;
                    }
                }       //Check for file redirection, set flags, open files

                if ((jobs[jobnum].pid = fork()) == 0) { /* Child runs user job */
                    
                       if(directIn > 0)
                           Dup2(inFile, 0);

                       if(directOut > 0)
                           Dup2(outFile, 1);
                       if (execvp(argv[0], argv) < 0) {
                                printf("%s: Command not found.\n", argv[0]);
                                exit(0);
                       }
                } else {
                    jobs[jobnum].running = 1;
                    jobs[jobnum].bg = bg;
                }

                /* Parent waits for foreground job to terminate */
                if (!bg) {
                        int status;
                        if (waitpid(jobs[jobnum].pid, &status, 0) < 0) {
                                unix_error("waitfg: waitpid error");
                        } 
                } 
           
                if(directIn > 0) Close(inFile);
                if(directOut > 0) Close(outFile); 
           }
        }
        return;
}

/*  If the first arg is a loaded command, run it and return true. */
int loaded_command(char **argv)
{
    int i = 0;

    while( i < pluggins ) {
        if(!strcmp(pluggin_methods[i]->CommandName,argv[0])) {
            method = dlsym(handles[i], pluggin_methods[i]->FunctionName);
            method(argv);
            return 1;
        }
        i++;
    }
    return 0;
}

void child_handler(int sig) 
{
    int child_status, i, acc;
    char buf[128];
    pid_t pid = wait(&child_status);
    i = acc = 0;

    while( i <= jobnum && acc <= MAXJOBS) {
        if(jobs[acc].running > 0)
            i++;
        if(jobs[acc].pid == pid) {
           if(jobs[acc].bg) {
               sprintf(buf, "\n[%d] %s - Finished\n", i, jobs[acc].cmd); 
               Write(1, buf, strlen(buf));
           } 
           jobs[acc].running = 0;
           jobnum--;
        }
        acc++;
    }
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv)
{   
   
        if (!strcmp(argv[0], "quit") || !strcmp(argv[0], "culater") || (atoi(argv[0]) == EOF)) {
            int i = 0;

            while( pluggins > 0 ) {
                if(dlclose(handles[--pluggins]) < 0){
                    fprintf(stderr,"%s\n",dlerror());
                    exit(1);
                }
                printf("Closed pluggin: %d\n", pluggins);
            }
            while( i < MAXJOBS && 0 < jobnum ) {
                if(jobs[i].running > 0) {
                    //send kill signal to each ps
                    kill(jobs[i].pid, SIGINT);
                    //printf("Killed job no: %d\n", i);
                    jobnum--;
                }
                i++;
            }
            exit(0);
        }

        if (!strcmp(argv[0], "&")) /* Ignore singleton & */
                return 1;

        if (!strcmp(argv[0], "\%")) /* Ignore everything after '% ' */
                return 1;

        if (!strcmp(argv[0], "setprompt")) {
                if (argv[1] != NULL) {
                        printf("assigning the prompt: %s\n", argv[1]);
                        strcpy(prompt,argv[1]);
                }
                return 1;
        } //endif(setprompt)

        if (!strcmp(argv[0], "cd")) {
                if (argv[1] != NULL) {
                        if (chdir(argv[1]) != 0) {
                                printf("Unknown path: %s\n", argv[1]);
                        }
                }
                return 1;
        } //endif(cd)

        if (!strcmp(argv[0], "loadpluggin")) { //I think you export the path to the environment. or something.
            if (argv[1] != NULL) {
                handles[pluggins] = dlopen(argv[1], RTLD_LAZY);
                if(!handles[pluggins]){
                    fprintf(stderr,"%s\n",dlerror());
                    exit(1);
                } else {
                    printf("Opened pluggin no. %d\n", pluggins);
                    pluggins++;
                }
                return 1;
            }
        } //endif(loadpluggin)

        if (!strcmp(argv[0], "bgjobs")) {
            int i, acc;
            i = acc = 0;
            while( acc <= jobnum ) {
                if(jobs[acc].running > 0) {
                    printf("[%d] %s\n", ++i, jobs[acc].cmd);
                }
                acc++;
            }
            return 1;
        } //endif(bgjobs)
        
        if (!strcmp(argv[0], "fg")) {
            if (argv[1] != NULL) {
                int status, num;
                num = atoi(argv[1]);
                if ( 0 <= num && num <= jobnum ) {
                    if (waitpid(jobs[num].pid, &status, 0) < 0) 
                        unix_error("waitfg: waitpid error\n");
                }
                jobs[num].running = 0;
            }
            return 1;
        } //endif(fg)

        return 0;                 /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv)
{
        char *delim;     /* Points to first space delimiter */
        int argc;        /* Number of args */
        int bg;          /* Background job? */

        buf[strlen(buf)-1] = ' '; /* Replace trailing '\n' with space */
        while (*buf && (*buf == ' ')) /* Ignore leading spaces */
                buf++;

        /* Build the argv list */
        argc = 0;
        while ((delim = strchr(buf, ' '))) {
                argv[argc++] = buf;
                *delim = '\0';
                buf = delim + 1;
                while (*buf && (*buf == ' ')) /* Ignore spaces */
                        buf++;
        }
        argv[argc] = NULL;

        if (argc == 0) /* Ignore blank line */
                return 1;

        /* Should the job run in the background? */
        if ((bg = (*argv[argc-1] == '&')) != 0)
                argv[--argc] = NULL;

        return bg;
}
/* $end parseline */
