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

typedef struct sharedMemory {   //create a type of sharedMemory for splitting file 
    int counter;                // number of chars needed to be counted
    int charCount[26];          // to store the count of a~z in the specific part of file
    int startIndex;             // to store the location of where a specific thread started to count
    char *content;              // pointer to buffer of file content at line #104
} sharedMemory;

void *thread(void *arg){
    sharedMemory *datL = (sharedMemory *)arg;
    int indL = datL->startIndex;    //begin of the partition
    int endL = datL->counter;       //end of the partition
    for(int i = 0; i < endL; i++){
        datL->charCount[datL->content[indL + i] - 97] += 1; //Convert the character to the index of the counting list, increment the count value
    }
    pthread_exit(NULL);             //terminate the thread
}

void printResult(int arr[26]){  //void print the characters of occurences in the file
    for(int i = 0; i < 26; i++){
        if(arr[i] > 0){
            printf("%c %d\n", 97 + i, arr[i]);  //print the characters that it occurs in file in alphabetical  order, arr[i] means number of occurences
        }
    }
}

sharedMemory datInit(int j, int byteCount, int aThread, char *buffer){
    sharedMemory dat;
    dat.content = buffer;
    dat.startIndex = 0 + j * (byteCount - byteCount%aThread)/aThread;
    if(j == aThread){   //check if the partition is the last partition of file, to determine the partition size 
        dat.counter = byteCount % aThread;
    }else{
        dat.counter = (byteCount - byteCount % aThread)/aThread;
    }
    memset(dat.charCount, 0, sizeof(dat.charCount)); //Initialize with all 0, otherwise random number will appear
    return dat;
}

int main(int argc, char *argv[]){       //argc: how many files, argv: the array of arguments
    char *sorted[argc-1];               //to store a sorted order of file names
    char fileCount = argc - 1;
    if(fileCount == 0){                 //for case of no file, output required text
        printf("pzip: file1 [file2 ...]\n");
        return 1;
    }else if(fileCount == 1){           //Single file, no need to do sorting
        sorted[0] = argv[1];
    }else if(fileCount > 1){            //method to sort by file name
        for(int i = 0; i < argc - 1; i++){
            sorted[i] = argv[i + 1];    //putting file names into sorted [] for sorting
        }
        char *cmp;                      //act as a temporary variable that store the character of old sorted position 
        for(int i = 0;i <fileCount;i++) 
        {
            for(int j = i + 1;j < fileCount;j++)
            {           
                if(strcmp(sorted[i], sorted[j]) > 0)    //sort the file name positions if the latter one is smaller than former one
                {   
                    cmp = sorted[i];
                    sorted[i] = sorted[j];   
                    sorted[j] = cmp;              
                }    
            } 
        }
    }

    int availableThread = 3;            //Set the number of threads that will be used
    int aThread = availableThread - 1;  //base on availableThread
    char *buffer;                       //file content
    long byteCount;                     //count of characters in one file
    for (int i = 0; i < fileCount; i++){
        int mainCountList[26];          //26 characters of a~z
        memset(mainCountList, 0, sizeof(mainCountList));
        FILE *f;
        f = fopen(sorted[i], "r");
        int readCheck;
        fseek(f, 0L, SEEK_END);
        byteCount = ftell(f);
        fseek(f, 0L, SEEK_SET);
        buffer = (char*)calloc(byteCount, sizeof(char));        //allocate memory for file content
        byteCount = fread(buffer, sizeof(char), byteCount, f);  //fread return value needs to be stored and used, required during compilation
        fclose(f);

        pthread_t *threadStack; // pointer to threads that will be created
        threadStack = (pthread_t *)malloc(availableThread * sizeof(pthread_t)); //allocate memory to the threads base on value of availableThread
        sharedMemory datStack[availableThread]; //prepare array of shared memory for pthread_create method base on value of availableThread
        for(int j = 0; j < availableThread; j++){
            datStack[j] = datInit(j, byteCount, aThread, buffer);   // Initiate shared memory for thread creation
            pthread_create(&threadStack[j], NULL, thread, (void *)&datStack[j]);    //Create thread
        }
        for (int j = 0; j < availableThread; j++){  //to combine all of the counts from different threads
            pthread_join(threadStack[j], NULL); //suspend execution of thread until the target threads terminate
            for(int k = 0; k < 26; k++){
                mainCountList[k] += datStack[j].charCount[k];
            }
        }
        free(buffer);   //free the memory 
        free(threadStack);  //free the memory 
        printf("%s\n", sorted[i]);  //print file name
        printResult(mainCountList); //print result line #49
    }


    return 0;
}