#include <stdio.h>
#include <string.h>
#include <stdlib.h>



int main()
{
	char *aa = "/home/hexa/Documents/test.py";
	char str[100];
    strcpy(str, aa);
    int init_size = strlen(str);
	char delim[] = "/";
    char result[1000] = "";
    char *files = "test.py";

	char *ptr = strtok(str, delim);

	while(strcmp(ptr, files))
	{
		sprintf(result, "%s/%s", result, ptr);
		ptr = strtok(NULL, delim);
    }   

    char *aaa="";
    strcpy(aaa, result);
    printf("%s\n", aaa);
	
    return 0;
}