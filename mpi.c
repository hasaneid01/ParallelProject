#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PASSWORD_LEN 20
#define HASH_SIZE 20

// Simple XOR hash for demonstration
void hash(char *input, char *output) {
    for (int i = 0; i < strlen(input); i++) {
        output[i] = input[i] ^ 4; // Simple XOR with 255
    }
    output[strlen(input)] = '\0';
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_File fh;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) {
        if (rank == 0) {
            printf("Usage: %s <file> <target>\n", argv[0]);
        }
        MPI_Finalize();
        return 0;
    }

    char *filename = argv[1];
    
    
    char target_hash[HASH_SIZE];
    //hash(argv[2], target_hash); // Hash the target password
strcpy(target_hash, argv[2]);


    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

    // Get the file size
    MPI_Offset filesize;
    MPI_File_get_size(fh, &filesize);


    // Calculate the segment size per processor
    MPI_Offset localsize = filesize / size;
    MPI_Offset start = rank * localsize;
    MPI_Offset end = (rank == size - 1) ? filesize : start + localsize;


    // Allocate buffer for local segment
    char *local_buf = malloc(localsize + 1);
    MPI_File_read_at_all(fh, start, local_buf, localsize, MPI_CHAR, &status);
    local_buf[localsize] = '\0'; // Null-terminate the buffer


    // Process local segment
    char *token;
    char *saveptr = local_buf;
    int found = 0;
   
             double start_time = MPI_Wtime();
            
    while ((token = strtok_r(saveptr, "\n", &saveptr))) {
        char hashed[PASSWORD_LEN];
        hash(token, hashed);
        if (strcmp(hashed, target_hash) == 0) { ///567
            
           double end_time = MPI_Wtime();
         hash(target_hash, target_hash);
            printf("Password found by rank %d: %s\n", rank, target_hash);
            printf("Time taken %f: \n", end_time - start_time);
            
            found = 1;
            break;
        }
    }

    // Reduce to check if any process has found the password
    int global_found;
    MPI_Allreduce(&found, &global_found, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

    if (global_found) {
        if (rank == 0) {
            printf("Password has been found.\n");
        }
    } else {
        if (rank == 0) {
            printf("Password not found.\n");
        }
    }

    // Clean up
    free(local_buf);
    MPI_File_close(&fh);
    MPI_Finalize();
    return 0;
}
