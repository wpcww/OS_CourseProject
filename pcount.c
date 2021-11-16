/*
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your group information here.
 *
 * Group No.: 10 (Join a project group in Canvas)
 * First member's full name: Marcus Owen LEE
 * First member's email address: 
 * Second member's full name: Sze Man FUNG
 * Second member's email address: 
 * Third member's full name: Wai Pong CHEUNG
 * Third member's email address: wpcheung33-c@cityu.edu.hk
 */

#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
//#include <sys/mman.h>                           
#include <sys/stat.h>                            
#include <sys/types.h>
//#include <sys/sysinfo.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int mainCountList[26];

typedef struct sharedMemory {
    int counter;        //Record sorted number of chars
    int charCount[26];
    int startIndex;
    char *content;
} sharedMemory;

void *thread(void *arg){
    sharedMemory *datL = (sharedMemory *)arg;
    int indL = datL->startIndex;
    int endL = datL->counter;
    for(int i = 0; i < endL; i++){
        //printf("%d\n", datL->content[i] - 97);
        datL->charCount[datL->content[indL + i] - 97] += 1;
        //printf("Count %c: %d\n", datL->content[indL + i], datL->charCount[datL->content[indL + i] - 97]);
    }
    //printf("%d\n", endL);
    //printf("%d\n", datL->charCount[17]);
    pthread_exit(NULL);
}

void printResult(int arr[26]){
    for(int i = 0; i < 26; i++){
        if(arr[i] > 0){
            printf("%c %d\n", 97 + i, arr[i]);
        }
    }
}

sharedMemory datInit(int j, int byteCount, int aThread, char *buffer){
    sharedMemory dat;
    dat.content = buffer;
    dat.startIndex = 0 + j * (byteCount - byteCount%aThread)/aThread;
    //printf("%c\n", dat.content[dat.startIndex]);
    if(j == aThread){
        dat.counter = byteCount % aThread;
    }else{
        dat.counter = (byteCount - byteCount % aThread)/aThread;
    }
    //printf("%d\n",dat.counter);
    memset(dat.charCount, 0, sizeof(dat.charCount)); //Initialize with all 0, otherwise random number will appear
    return dat;
}

int main(int argc, char *argv[]){
    int availableThread = 3;
    int aThread = availableThread - 1;
    char *buffer;
    long byteCount;
    for (int i = 1; i < argc; i++){
        FILE *f;
        f = fopen(argv[i], "r");
        int readCheck;
        fseek(f, 0L, SEEK_END);
        byteCount = ftell(f);
        fseek(f, 0L, SEEK_SET);
        buffer = (char*)calloc(byteCount, sizeof(char));
        readCheck = fread(buffer, sizeof(char), byteCount, f); //fread return value needs to be stored and used
        //printf("fread: %d\n", readCheck);
        while(readCheck != byteCount){
            free(buffer);
            buffer = (char*)calloc(byteCount, sizeof(char));
            readCheck = fread(buffer, sizeof(char), byteCount, f);
        }
        //printf("%d %d\n", readCheck, byteCount);
        fclose(f);

        pthread_t *threadStack;
        threadStack = (pthread_t *)malloc(availableThread * sizeof(pthread_t));
        sharedMemory datStack[availableThread];
        for(int j = 0; j < availableThread; j++){
            datStack[j] = datInit(j, byteCount, aThread, buffer);
            //printf("%d\n", datStack[j].startIndex);
            pthread_create(&threadStack[j], NULL, thread, (void *)&datStack[j]);
            //printf("Created\n");
            //printf("here\n");
        }
        for (int j = 0; j < availableThread; j++){
            pthread_join(threadStack[j], NULL);
            //printf("%d\n", datStack[j].charCount[1]);
            for(int k = 0; k < 26; k++){
                mainCountList[k] += datStack[j].charCount[k];
            }
        }
        //printf("here\n");
        free(buffer);
        free(threadStack); 
        printf("%s\n", argv[i]);
        printResult(mainCountList);   
    }


    return 0;
}