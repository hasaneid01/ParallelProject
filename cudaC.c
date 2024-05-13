// include %%cu in case running this code on Google Colab
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <cuda_runtime.h>


#define BLOCK_SIZE 256
__global__ void hash_kernel(char *dev_pass, int length)
{
    __shared__ char s_pass[BLOCK_SIZE];
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int tid = threadIdx.x;

    while(i < length)
    {
        s_pass[tid] = dev_pass[i];
        __syncthreads();

        s_pass[tid] ^= 4;
        __syncthreads();

        dev_pass[i] = s_pass[tid];
        i += blockDim.x * gridDim.x;
    }
}

char* hash(char *real)
{
    int length = strlen(real);
    char *dev_real, *dev_pass;
    char *pass = (char*)malloc(sizeof(char) * length);

    cudaMalloc(&dev_real, sizeof(char) * length);
    cudaMemcpy(dev_real, real, sizeof(char) * length, cudaMemcpyHostToDevice);

    cudaMalloc(&dev_pass, sizeof(char) * length);

    int block_size = 256;
    int grid_size = (length + block_size - 1) / block_size;
    hash_kernel<<<grid_size, block_size>>>(dev_real, length);
    cudaMemcpy(pass, dev_real, sizeof(char) * length, cudaMemcpyDeviceToHost);

    pass[length] = '\0'; // add null terminator
    cudaFree(dev_real);
    cudaFree(dev_pass);

    return pass;
}

int main()
{
    clock_t start, end;
    float time_exec;

    FILE *fptr, *fptw;

    fptr = fopen("passwords.txt", "r"); // reading all passwords
    fptw = fopen("hashed_pass.txt","a");
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
    char *my_pass = "Yaakoub";
    char *my_hashed= hash(my_pass);
    fptr = fopen("hashed_pass.txt","r");
    if (fptr == NULL) {
        printf("Failed to open hashed passwords file.\n");
        return 0;
    }
    char *thispass = (char*)malloc(sizeof(char)*20);
    int found = 0;
    while(fscanf(fptr, "%s", thispass) == 1)
    {
        if(strcmp(my_hashed, thispass) == 0)
        {
            printf("The cracked password is %s\n", my_pass);
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
