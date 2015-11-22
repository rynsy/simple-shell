/* $begin shellmain */
#include "csapp.h"
#define MAXARGS   128
/* my globals */
#define PROMPTSIZE 16
#define MAXCWD 256
#define MAXPLUGGINS 20
char prompt[PROMPTSIZE] = "upsh>";
void handles[MAXPLUGGINS];
int pluggins = 0;

/* function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int p3parseline(char *buf, char **argv); /* new parseline function for cs485 project 3 */
int builtin_command(char **argv);

int main()
{
        char cmdline[MAXLINE]; /* Command line */

        while (1) {
                /* Read */
                printf(prompt); /* TODO change to global string that represents prompt (changed with setprompt)*/
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
        char *argv[MAXARGS]; /* Argument list execve() */
        char buf[MAXLINE]; /* Holds modified command line */
        int bg;          /* Should the job run in bg or fg? */
        pid_t pid;       /* Process id */

        strcpy(buf, cmdline);
        /*    bg = parseline(buf, argv); */
        bg = p3parseline(buf, argv); /* call new parseline function for cs485 project 3 */
        if (argv[0] == NULL)
                return;  /* Ignore empty lines */

        if (!builtin_command(argv)) {
                if ((pid = fork()) == 0) { /* Child runs user job */
                        if (execve(argv[0], argv, environ) < 0) {
                                printf("%s: Command not found.\n", argv[0]);
                                exit(0);
                        }
                }

                /* Parent waits for foreground job to terminate */
                if (!bg) {
                        int status;
                        if (waitpid(pid, &status, 0) < 0)
                                unix_error("waitfg: waitpid error");
                }
                else
                        printf("%d %s", pid, cmdline);
        }
        return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv)
{       //TODO fold all quit commands into one if statement,
        //add clause to call dlclose() as many times as dlopen() succeeds
        if (!strcmp(argv[0], "quit")) /* quit command */
                exit(0);

        if (!strcmp(argv[0], "culater")) /* quit command TODO trigger exit on ctrl-D*/
                exit(0);

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
                    if (!(handles[pluggins++]=dlopen(argv[1], RTLD_LAZY))) {
                        printf(stderr, "%s\n", dlerror());
                    } //TODO check to see if that's the right ++
                }
                return 1;
        } //endif(loadpluggin)

        /* TODO figure out where file redirection fits into all this */
        /* TODO I think for bg you need to return 1, but I have no idea.
           fg will do something similar. I think this plays into bgjobs
           and I need some C functions that give me processes and let
           me switch up control. This is probably the hardest part of the
           whole assignment. Needs to use signal() to communicate between procs
         */
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
