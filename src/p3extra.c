#include "csapp.h"
#include <string.h>

/* Uncomment the following line to see the arguments to the command */
/* #define P3PARSELINE_DEBUG */

char buf[MAXLINE];   /* A global buffer to store argv tokens in */
                     /* IMPORTANT: Pointers to this buffer are returned to the caller */
                     /* !!NOTE!! buf will be overwritten on each call to p3parseline */
                     /* so the caller cannot continue using values returned by a previous call */

#define IsSpecialChar(x) ((x == '<') || (x == '>') || (x == '%') || (x == '&'))
#define IsWhiteSpace(x) ((x == ' ') || (x == '\t'))

#define HandleError(x) { printf(x); argv[0] = NULL; return 1; }  /* print msg and pretend nothing typed */

/*--------------------------------------------------------------------------------------*/
/* p3parseline - Parse the command line and build the argv array for CS 485 project 3   */
/*               CS 485 project 3 support strings, and special characters < > % and & */
/*--------------------------------------------------------------------------------------*/
int p3parseline(char *line, char **argv) 
{
  char *bptr;        /* Pointer into the buffer buf */
  char *tptr;        /* Temp pointer */
  int intoken;       /* Are we in the middle of reading a token in? */
  int argc;          /* Number of args */
  int bg;            /* Background job? */

  if ((strlen(line)*2) > MAXLINE) {
    HandleError("Line too long for p3parseline\n");
  }

  argc = 0;
  intoken = 0;
  
  /* Copy line into buf character-by-character to create the argv strings */
  bptr = buf;
  tptr = line;
  while (*tptr != '\n') {
    
    if (IsWhiteSpace(*tptr)) {
      *bptr++ = '\0';                  /* insert \0 to terminate current token */
      intoken = 0;
    }
    else if (IsSpecialChar(*tptr)) {   /* stand-alone token -- add room for \0 */
      if (intoken) {  
	*bptr++ = '\0';
	intoken = 0;
      }
      argv[argc++] = bptr;
      *bptr++ = *tptr;
      *bptr++ = '\0';
    }
    else if (*tptr == '"') {            /* starting a string -- copy until matching \" */
      if (intoken) {  
	*bptr++ = '\0';
	intoken = 0;
      }
      argv[argc++] = bptr;
      tptr++;
      while ((*tptr != '\"') && (*tptr != '\0')) *bptr++ = *tptr++;
      if (*tptr == '\0') {
	HandleError("String not terminated -- missing double quote\n");
      }
      else {
        *bptr++ = '\0';
      }
    }
    else {                             /* non-special character */
      if (intoken) *bptr++ = *tptr;
      else {
	intoken = 1;
	argv[argc++] = bptr;
	*bptr++ = *tptr;
      }
    }
    
    tptr++;                             /* move to the next character */
    
  }
  *bptr = '\0';                         /* terminate the last token */

  argv[argc] = NULL;

  if (argc == 0)  /* Ignore blank line */
    return 1;

  /* Should the job run in the background? */
  if ((bg = (*argv[argc-1] == '&')) != 0)
    argv[--argc] = NULL;

#ifdef P3PARSELINE_DEBUG
  int i;
  for (i = 0; i < argc; i++) printf("argv[%d] = '%s'\n", i, argv[i]);
#endif
  

  return bg;
}

