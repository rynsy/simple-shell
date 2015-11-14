#include <stdio.h> //Do I even need this?
#include <stdlib.h>

void printlogo();

struct NewBuiltIn {
    char CommandName[64]; //Name of the command
    char FunctionName[64]; //Name of the function (in .so) to invoke when using this command
    char AnalyzerName[64]; //returns a pointer to a string for p3parseline to analyze
};

struct NewBuiltIn pluggin_method = {"batman","batman",""};

int batman(char** argv) //This is the argv returned by p3parseline
{
    //Should take "batman N" from cmd, N being a number
    //Should print "Na" * N, then ", Batman!"
  int i, count;
  
  i = 0;
  if( argv[1] == NULL ) {
    printlogo();
  }
  else { //Need to check if argv is int
    count = atoi(argv[1]);

    if( count <= 0 ) {
        printf("Is this some kind of riddle?\n");
    } 
    else {
        while( i++ < count ) {printf("Na");}
        printf(", Batman!\n");
    }
  }
  return(0); 
}

void printlogo()
{
    printf("\n       _==/          i     i          \\==_ \n");
    printf("     /XX/            |\\___/|            \\XX\\ \n");
    printf("   /XXXX\\            |XXXXX|            /XXXX\\ \n");
    printf("  |XXXXXX\\_         _XXXXXXX_         _/XXXXXX| \n");
    printf(" XXXXXXXXXXXxxxxxxxXXXXXXXXXXXxxxxxxxXXXXXXXXXXX \n");
    printf("|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX| \n");
    printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX \n");
    printf("|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX| \n");
    printf(" XXXXXX/^^^^\\XXXXXXXXXXXXXXXXXXXXX/^^^^^\\XXXXXX \n");
    printf("  |XXX|       \\XXX/^^\\XXXXX/^^\\XXX/       |XXX| \n");
    printf("    \\XX\\       \\X/    \\XXX/    \\X/       /XX/ \n");
    printf("       \"\\       \"      \\X/      \"      /\" \n\n");
}
