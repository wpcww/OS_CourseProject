/*
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your group information here.
 *
 * Group No.: 10 (Join a project group in Canvas)
 * First member's full name: Marcus Owen LEE
 * First member's email address: marcuslee6-c@my.cityu.edu.hk
 * Second member's full name: Sze Man FUNG
 * Second member's email address: szfung9-c@my.cityu.edu.hk
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

typedef struct singleFileMemory {
    char *filename; //*sorted[x]
    int fileCharCount[26];
    int aThread;    //Number of threads that will be used
} singleFileMemory;

void *thread(void *arg){
    sharedMemory *datL = (sharedMemory *)arg;
    int indL = datL->startIndex;
    int endL = datL->counter;
    for(int i = 0; i < endL; i++){
        if(datL->content[indL + i] - 97 >= 0 && datL->content[indL + i] - 97 <= 25){
            datL->charCount[datL->content[indL + i] - 97] += 1; //Convert the character to the index of the counting list, increment the count value
        }
    }

    pthread_exit(NULL);
}

void printResult(int arr[26]){
    for(int i = 0; i < 26; i++){
        if(arr[i] > 0){
            printf("%c %d\n", 97 + i, arr[i]);
        }
    }
}

sharedMemory datInit(int j, int byteCount, int divisor, char *buffer){
    sharedMemory dat;
    dat.content = buffer;
    dat.startIndex = 0 + j * (byteCount - byteCount%divisor)/divisor;
    if(j == divisor){
        dat.counter = byteCount % divisor;
    }else{
        dat.counter = (byteCount - byteCount % divisor)/divisor;
    }
    memset(dat.charCount, 0, sizeof(dat.charCount)); //Initialize with all 0, otherwise random number will appear
    return dat;
}

void *singleFileThread(void *arg){
    singleFileMemory *datL = (singleFileMemory *)arg;
    char *filename = datL->filename;
    int availableThread = datL->aThread;
    int divisor = availableThread - 1;
    char *buffer;
    long byteCount;
    FILE *f;
    f = fopen(filename, "r");
    fseek(f, 0L, SEEK_END);
    byteCount = ftell(f);
    fseek(f, 0L, SEEK_SET);
    buffer = (char*)calloc(byteCount, sizeof(char));
    byteCount = fread(buffer, sizeof(char), byteCount, f); //fread return value needs to be stored and used
    fclose(f);

    pthread_t *threadStack;
    threadStack = (pthread_t *)malloc(availableThread * sizeof(pthread_t));
    sharedMemory datStack[availableThread];
    for(int j = 0; j < availableThread; j++){
        datStack[j] = datInit(j, byteCount, divisor, buffer);
        pthread_create(&threadStack[j], NULL, thread, (void *)&datStack[j]);
    }
    for (int j = 0; j < availableThread; j++){
        pthread_join(threadStack[j], NULL);
        for(int k = 0; k < 26; k++){
            datL->fileCharCount[k] += datStack[j].charCount[k];
        }
    }
    free(buffer);
    free(threadStack); 


    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    char argvCopy[20][40];  //Create a copy of argv
    struct stat dirOrFile;  //stat object to for file or directory check
    struct dirent *d_temp;  //temp char for storing prefix of the file
    int fileCount = 0;
    for(int i = 1; i < argc; i++){
        stat(argv[i], &dirOrFile);  //Store the stat information of input to the structure
        if(S_ISREG(dirOrFile.st_mode) == 1){ //1 is a file, 0 is a directory
            strcpy(argvCopy[i - 1], argv[i]); //Save the filename to the copy list
            fileCount += 1;
        }else{
            DIR *dir = opendir(argv[i]);    //If it is a dir, open it
            int j = i - 1;
            readdir(dir); //Skip current dir
            readdir(dir); //Skip previous dir
            while((d_temp = readdir(dir)) != NULL){
                strcpy(argvCopy[j], argv[i]);   //Add file prefix
                strcat(argvCopy[j], "/");   //Add directory symbol
                strcat(argvCopy[j], d_temp->d_name);    //Add filename
                fileCount += 1;
                j++;
            }
            closedir(dir);
        }
    }

    char *sorted[fileCount];
    if(fileCount == 0){
        printf("pzip: file1 [file2 ...]\n");
        return 1;
    }else if(fileCount == 1){
        sorted[0] = argvCopy[0];
    }else if(fileCount > 1){
        for(int i = 0; i < fileCount; i++){
            sorted[i] = argvCopy[i];
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

    pthread_t *fileThreadStack;
    fileThreadStack=(pthread_t *)malloc(fileCount * sizeof(pthread_t ));    //Initialize thread stack for file multithreading
    singleFileMemory fileDataStack[fileCount];  //Initialize information structure required by the thread
            for (int i = 0; i < fileCount; i++){
                fileDataStack[i].filename = sorted[i];  //Assign filename
                fileDataStack[i].aThread = 7;   //Assign number of threads that would be used by a thread
                memset(fileDataStack[i].fileCharCount, 0, sizeof(fileDataStack[i].fileCharCount));  //Initialze list with all 0
                pthread_create(&fileThreadStack[i], NULL, singleFileThread, (void *)&fileDataStack[i]);
            }
            for (int i = 0; i < fileCount; i++){
                pthread_join(fileThreadStack[i], NULL);
                printf("%s\n", fileDataStack[i].filename);  //Print file name
                printResult(fileDataStack[i].fileCharCount);    //Print result
            }

    free(fileThreadStack); 
    return 0;
}