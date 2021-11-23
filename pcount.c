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
        printf("　");
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
        //printf("%d\n", byteCount % aThread);
    }else{
        dat.counter = (byteCount - byteCount % aThread)/aThread;
        //printf("%d\n", (byteCount - byteCount % aThread)/aThread);
    }
    //printf("%d\n",dat.counter);
    memset(dat.charCount, 0, sizeof(dat.charCount)); //Initialize with all 0, otherwise random number will appear
    return dat;
}

int main(int argc, char *argv[]){
    char *sorted[argc-1];
    char fileCount = argc - 1;
    if(fileCount == 0){
        //printf("0\n");
        printf("pzip: file1 [file2 ...]\n");
        return 1;
    }else if(fileCount == 1){
        //printf("1\n");
        sorted[0] = argv[1];
    }else if(fileCount > 1){
        //printf(">1\n");
        for(int i = 0; i < argc - 1; i++){
            sorted[i] = argv[i + 1];
        }
        char *cmp;
        for(int i = 0;i <fileCount;i++) 
        {
            for(int j = i + 1;j < fileCount;j++)
            {           
                if(strcmp(sorted[i], sorted[j]) > 0)
                {   
                    cmp = sorted[i];
                    sorted[i] = sorted[j];   
                    sorted[j] = cmp;              
                }    
            } 
        }
    }

    int availableThread = 80;
    int aThread = availableThread - 1;
    char *buffer;
    long byteCount;
    for (int i = 0; i < fileCount; i++){
        int mainCountList[26];
        memset(mainCountList, 0, sizeof(mainCountList));
        FILE *f;
        f = fopen(sorted[i], "r");
        fseek(f, 0L, SEEK_END);
        byteCount = ftell(f);
        fseek(f, 0L, SEEK_SET);
        buffer = (char*)calloc(byteCount, sizeof(char));
        byteCount = fread(buffer, sizeof(char), byteCount, f); //fread return value needs to be stored and used
        //printf("fread: %d\n", readCheck);
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
        printf("%s\n", sorted[i]);
        printResult(mainCountList);   
    }


    return 0;
}