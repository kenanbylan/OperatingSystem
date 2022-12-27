#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

sem_t sem;

void swap(int *arr, int i, int j) {
    int tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

int findTopK(FILE *fp, int k){
    int *arr = malloc(k * sizeof(int));
    int temp;
    for (int i = 0; i < k; i++) {
        arr[i] = 0;
    }

    while (fscanf(fp, "%d", &temp) != EOF){
        if (temp > arr[0]){
            arr[0] = temp;
            for (int i = 0; i < k-1; i++){
                if (arr[i] < arr[i+1]){
                    break;
                }
                swap(arr, i, i+1);
            }
        }
    }
    temp = arr[0];
    free(arr);
    return temp;
}

int main() {
    int n;
    sem_open(&sem, 0, 1);
    printf("Enter a number: ");
    scanf("%d", &n);

    pid_t pid[n];
    // Create n child processes and open text[n] files
    for (int i = 0; i < n; i++) {
        pid[i] = fork();
        if (pid[i] == 0) {
            char filename[]="textx.txt";
            filename[4] = i + 48;
            FILE *fp = fopen(filename, "r");
            if (fp == NULL) {
                printf("Error opening file\n");
                exit(1);
            }
            int result = findTopK(fp, 3);
            fclose(fp);
            sem_wait(&sem);
            fp = fopen("intermediate.txt", "a");
            fprintf(fp, "%d\n", result);
            fclose(fp);
            sem_post(&sem);
            exit(0);
        }
    }
    //wait for all child processes to finish, open intermediate.txt, sort the numbers and delete intermediate.txt
    for (int i = 0; i < n; i++) {
        waitpid(pid[i], NULL, 0);
    }
    FILE *fp = fopen("intermediate.txt", "r");
    int *arr = malloc(n * sizeof(int));
    int temp;
    for (int i = 0; i < n; i++) {
        fscanf(fp, "%d", &temp);
        arr[i] = temp;
    }
    fclose(fp);
    remove("intermediate.txt");
    //bubble sort
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j] < arr[j+1]) {
                swap(arr, j, j+1);
            }
        }
    }
    //create result.txt and write the sorted numbers
    fp = fopen("result.txt", "w");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d\n", arr[i]);
    }
    fclose(fp);
    free(arr);
    return 0;
}
