#include <stdio.h> 
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "csapp.h"

extern char **environ; /* Defined by libc */

void setter(char** argv);
void unsetter(char** argv);
void getter(char** argv);

struct NewBuiltIn {
    char CommandName[64]; //Name of the command
    char FunctionName[64]; //Name of the function (in .so) to invoke when using this command
    char AnalyzerName[64]; //returns a pointer to a string for p3parseline to analyze
};

struct NewBuiltIn pluggin_method = {"env","env",""};
//Not sure if I need an Analyzer or not. 

int env(char** argv) //This is the argv returned by p3parseline
{
    //going to use setenv, and unsetenv
    //Need to check that argv[1] and argv[2] are set. 
    //Should the analyzer be the one to throw a fit if these aren't set?
    if( argv[1] != NULL )
    {
        if( strcmp(argv[1],"-s") == 0 )
            setter(argv);
        else if( strcmp( argv[1],"-u") == 0 )
            unsetter(argv);
        else if( strcmp( argv[1],"-g") == 0 )
            getter(argv);
        else {
            printf("Invalid usage. Expected -[sug] -[n] VARNAME -[v] VALUE\n");
        }
    }
    return(0);
}

void setter(char** argv) 
{
    int err = 0;
    if( argv[2] != NULL && argv[3] != NULL && argv[4] != NULL)
    {
        if( (strcmp(argv[2],"-n") == 0) ) 
        {
            if( (strcmp(argv[4],"-v") == 0) ) 
            {
                if(setenv(argv[3],argv[5],1) != 0) 
                    unix_error("error setting environment variable");            
            }
            else 
            {
                err = 1;
            }
        } 
        else
        {
            err = 1;
        }
    }
    else
    {
        err = 1;
    }
    if (err)
        printf("Invalid usage. Expected -s -n VARNAME -v VALUE\n");
}

void unsetter(char** argv) 
{
    if( argv[2] != NULL && argv[3] != NULL )
    { 
        if( (strcmp(argv[2],"-n") == 0) )
        {
            if( unsetenv(argv[3]) != 0 )
                unix_error("error unsetting variable");
        }
        else
        {
            printf("Invalid usage. Expected -n VARNAME\n");
        }
    }
    else
    {
        printf("Invalid usage. Expected -n VARNAME\n");
    }
}

void getter(char** argv) 
{
    char* value;
    if( argv[2] != NULL && argv[3] != NULL )
    {
        if( (strcmp(argv[2],"-n") == 0) && argv[3] != NULL ) 
        {
            value = getenv(argv[3]);
            if( value == NULL )
                printf("%s is not defined.\n", argv[3]);
            else
                printf("%s = %s \n", argv[3], value);
        } 
        else 
        {
            printf("Invalid usage. Expected -g -n VARNAME\n");
        }
    }
    else
    {
        printf("Invalid usage. Expected -g -n VARNAME\n");
    }
}

