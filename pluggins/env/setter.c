#include <stdio.h> //Do I even need this?

struct setvar {
    char CommandName[64]; //Name of the command
    char FunctionName[64]; //Name of the function (in .so) to invoke when using this command
    char AnalyzerName[64]; //returns a pointer to a string for p3parseline to analyze
};

char* Analyzer(char* input)
{
    return input;
}

void SetVar(char* argv) //This is the argv returned by p3parseline
{
    //pretty much going to use putenv
    //and have a commandline parameter for unsetting a variable
}
