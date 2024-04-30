#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>





char* hash(char *real)
{
    char *pass = (char*)malloc(sizeof(char)*strlen(real));
    for (int i = 0; i < strlen(real); i++)
    {
        pass[i] = real[i] ^ 4;
    }
    pass[strlen(real)] = '\0'; // add null terminator
    return pass;
}




int main(int argc, char *argv[]) 
{
    if(argc!=3)
    {
        printf("Must be three arguments: ./SeqCode passwords.txt password\n");
        return 0;
    }

    clock_t start, end;
    float time_exec;

    FILE *fptr, *fptw;

    fptr = fopen(argv[1], "r"); // reading all passwords
  //  fptw = fopen("hashed_pass.txt","w");
    if (fptr == NULL) {
        printf("Failed to open passwords file.\n");
        return 0;
    }
    char *p = (char*)malloc(sizeof(char)*20);
    char *hashed_p = (char*)malloc(sizeof(char)*20); 

    while(fscanf(fptr, "%s", p) == 1)
    {
        hashed_p = hash(p);
        fputs(hashed_p, fptw);
        fputs("\n", fptw);
    }
    fclose(fptr);
    fclose(fptw);

    //looking for the password
    start = clock();
    //char *my_pass = argv[2];
    char *my_hashed = argv[2]; 	//hash(my_pass);
    fptr = fopen("hashed_pass.txt","r");
    if (fptr == NULL) {
        printf("Failed to open hashed passwords file.\n");
        return 0;
    }
    char *thispass = (char*)malloc(sizeof(char)*20);
    int found = 0;
   // printf("the hashed pass u entered %s\n",myhashed)
    while(fscanf(fptr, "%s", thispass) == 1)
    {
        if(strcmp(my_hashed, thispass) == 0)
        {
            printf("The cracked password is %s\n", hash(my_hashed));
            found = 1;
            break;
        }
    }
    fclose(fptr);
    if(!found)
    {
        printf("Not Found!!\n");
    }

    end=clock();
    time_exec = ((float)(end-start))/CLOCKS_PER_SEC;
    printf("Time of execution: %.5f\n", time_exec);
    return 0;
}
