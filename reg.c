#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h> 
 
int main(int argc, char const *argv[])
{ 
    char *source = "a001 login matias saitam";
    int length = (int)strlen(source); //sizeof(source)=sizeof(char *) = 4 on a 32 bit implementation
    int i = 0;
    int encontre_usuario = 0;
    int cant_espacios = 0;
    while (i < length && encontre_usuario == 0) 
    {
        if(source[i] == ' '){
            cant_espacios++;
        }
        
        if(source[i] == ' ' && cant_espacios == 2){
            encontre_usuario = 1;
        }
//         printf("%c", source[i]);
        i++;
    }
    
    int j = i;
    while(source[j] != ' '){
        /*char * caracter = &source[i];
        strcpy(usuario, caracter);
        printf("%c", source[i]);*/
        j++;
    }
    int largo = j-i;
    char usuario[largo];
    for(int k=i; k<j; k++){
        usuario[k-i]= source[k];
    } 
    printf("%s", usuario);
return 0;
}
