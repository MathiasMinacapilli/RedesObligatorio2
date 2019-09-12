#include <stdio.h>
#include <sys/types.h>   
#include <regex.h>
#include <string.h>

int main()
{

/*
 * Match string against the extended regular expression in
 * pattern, treating errors as no match.
 *
 * return 1 for match, 0 for no match
 */

char * source = "a001 loGin matias saitam\n";
char * regexString = "(([a-z]|[0-9])+) login ([a-z]+) ([a-z]+)";
int    status;
regex_t  regexCompiled; 
size_t maxGroups = 10;
regmatch_t groupArray[maxGroups];
char * cursor;
char * usuario = "";

  //Compila la expresion regular  
  if (regcomp(&regexCompiled, regexString, REG_EXTENDED|REG_ICASE))
    {
      printf("No se pudo compilar la expresión regular\n");
      return 1;
    };

  cursor = source;
     {
      if (regexec(&regexCompiled, cursor, maxGroups, groupArray, 0))
        return(0);  // No more matches

      unsigned int g = 0;
      unsigned int offset = 0;
      for (g = 0; g < maxGroups; g++)
        {
          if (groupArray[g].rm_so == (size_t)-1){
            printf(usuario);
            regfree(&regexCompiled);
            return(0);  // No more groups
          }
          
          if (g == 0)
            offset = groupArray[g].rm_eo;
          
          char cursorCopy[strlen(cursor) + 1];
          strcpy(cursorCopy, cursor);
          cursorCopy[groupArray[g].rm_eo] = 0;
          printf("Group %u: [%2u-%2u]: %s\n",
                 g, groupArray[g].rm_so, groupArray[g].rm_eo,
                 cursorCopy + groupArray[g].rm_so);
        }
      cursor += offset;
    }
  return(1);


return 0;
}
