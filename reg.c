#include <stdio.h>
#include <sys/types.h>   
#include <regex.h>

int main()
{

/*
 * Match string against the extended regular expression in
 * pattern, treating errors as no match.
 *
 * return 1 for match, 0 for no match
 */

char string[] = "a001 loGin matias saitam\n";
int    status;
regex_t  re; 

    if (regcomp(&re, "([a-z]|[A-Z]|[0-9])+ (login) ([a-z]|[A-Z])+ ([a-z]|[A-Z])+", REG_EXTENDED|REG_NOSUB|REG_ICASE) != 0) {//devuelve 0 si compila la re
        printf("No compilo");
        return(0);      /* report error */
    }
    status = regexec(&re, string, (size_t) 0, NULL, 0); //devuelve 0 si matchea
    regfree(&re);
    if (status != 0) {
        printf("No matcheo");
        return(0);      /* report error */
    }
    
    
    
    printf("matcheo");
     return(1);


return 0;
}
