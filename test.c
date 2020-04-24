#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *dirpath = "/home/hexa/Documents/testad.py";
char temp[1000]="";

void getPath(char *path, char *files){
    char string[1000] = "";
    strcpy(string, path);
    char * token = strtok(string, "/");
    // loop through the string to extract all other tokens
    while(strcmp(token, files)) {
        sprintf( temp, "%s/%s", temp, token); //printing each token
        token = strtok(NULL, "/");
    }
}

int main()
{
    FILE *fp1 = fopen("/home/hexa/Documents/testad/env2_w/cmd.py.0", "r"); 
    FILE *fp2 = fopen("/home/hexa/Documents/testad/env2_w/cmd.py.1", "r"); 
    FILE *fp3 = fopen("/home/hexa/Documents/testad/env2_w/cmd.py.2", "r"); 
    FILE *fp4 = fopen("/home/hexa/Documents/testad/env2_w/cmd.py.3", "r"); 
    FILE *fp5 = fopen("/home/hexa/Documents/testad/env2_w/cmd.py.4", "r"); 

    FILE *fp6 = fopen("/home/hexa/Documents/testad/env2_w/cmd.py", "w"); 
    char c;  
    
    // Copy contents of first file to file3.txt 
    while ((c = fgetc(fp1)) != EOF) 
        fputc(c, fp6); 
    
    // Copy contents of second file to file3.txt 
    while ((c = fgetc(fp2)) != EOF) 
        fputc(c, fp6); 

     // Copy contents of second file to file3.txt 
    while ((c = fgetc(fp3)) != EOF) 
        fputc(c, fp6); 
    
    while ((c = fgetc(fp4)) != EOF) 
        fputc(c, fp6); 
    
    while ((c = fgetc(fp5)) != EOF) 
        fputc(c, fp6); 
    
    fclose(fp1); 
    fclose(fp2); 
    fclose(fp3); 
    fclose(fp4); 
    fclose(fp5); 
    fclose(fp6);

    return 0;
}