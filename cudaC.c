
// compile using: gcc -fopenmp omp_code -o omp_code.c
// run using: ./omp_code passwords.txt password_to_crack
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#define MAX_PASSWORD_LENGTH 20
#define NUM_PASSWORDS 100000

char* hash(const char *real)
{
    size_t len = strlen(real);
    char *pass = malloc(len + 1);
    for (size_t i = 0; i < len; i++)
    {
        pass[i] = real[i] ^ 4;
    }
    pass[len] = '\0'; // add null terminator
    return pass;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: ./OpenMPCode passwords.txt password\n");
        return 1;
    }

    clock_t start, end;
    float time_exec;

    FILE *fptr, *fptw;

    fptr = fopen(argv[1], "r"); // reading all passwords
    fptw = fopen("hashed_pass.txt", "w");
    if (fptr == NULL || fptw == NULL) {
        printf("Failed to open files.\n");
        return 1;
    }

    start = clock();

    #pragma omp parallel shared(fptr, fptw)
    {
        char p[MAX_PASSWORD_LENGTH + 1];
        char *hashed_p;

        #pragma omp for schedule(dynamic)
        for (int i = 0; i < NUM_PASSWORDS; i++)
        {
            if (fscanf(fptr, "%s", p) == 1)
            {
                hashed_p = hash(p);
			
			if(hashed_p == argv[2]){
			printf("found %s", hash(argv[2]));
			}


                #pragma omp critical
                {
                    fputs(hashed_p, fptw);
                    fputs("\n", fptw);
                }

                free(hashed_p);
            }
        }
    }

    fclose(fptr);
    fclose(fptw);

    //looking for the password
    start = clock();

    char *my_pass = argv[2];
    char *my_hashed = hash(my_pass);

    fptr = fopen("hashed_pass.txt", "r");
    if (fptr == NULL) {
        printf("Failed to open hashed passwords file.\n");
        return 1;
    }

    int found = 0;

    #pragma omp parallel shared(fptr, found)
    {
        char thispass[MAX_PASSWORD_LENGTH + 1];

        #pragma omp for schedule(dynamic)
        for (int i = 0; i < NUM_PASSWORDS; i++)
        {
            if (fscanf(fptr, "%s", thispass) == 1)
            {
                if (strcmp(my_hashed, thispass) == 0)
                {
                    printf("The cracked password is %s\n", my_pass);
                    #pragma omp atomic write
                    found = 1;
                    #pragma omp cancel for
                }
            }
        }
    }

    fclose(fptr);

    if (!found)
    {
        printf("Not Found!!\n");
    }

    end = clock();
    time_exec = ((float)(end - start)) / CLOCKS_PER_SEC;
    printf("Time of execution: %.5f\n", time_exec);

    free(my_hashed);

    return 0;
}
