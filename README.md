#User-Pluggin Shell

An extensible shell written for CS 485 (Systems Programming) at UKy

##Built-in Functions
* 'loadpluggin' command for loading your own commands from a shared object file
* Running commands in the background by putting a '&' at the end of a command
* Bring a job to the foreground with 'fg', or view background jobs with 'bgjobs'
* Supports commented commands, ignoring text following a '%'
* 'setprompt' to set a custom prompt for your session.
* 'quit' or 'culater' to exit.

##Writing Pluggins
Pluggins need to have the following structure to work properly:
'''C

struct NewBuiltIn {
    char CommandName[64];
    char FunctionName[64];
    char AnalyzerName[64];
};

struct NewBuiltIn pluggin_method = {"command", "funcName", "analyzerName"};

void funcName( char** argv ) { }
'''

Pluggins need to be compiled to be position-independent in order for the shell to load them properly. Use the following compiler flags when compiling a pluggin:

'''make
gcc -Wall -shared -fPIC -I. {Dependencies} -o fileName.so
'''
##TODO

* Fix file redirection - Can't direct in and out at the same time
* Fix job handling - Doesn't handle jobs correctly. Need to replace job struct with something that can be updated after a job has finished. 
