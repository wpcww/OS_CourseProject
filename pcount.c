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

typedef struct node{
    char data;
    struct node* next;
} node;

typedef struct sharedMemory {
    node *start;
    int counter;        //Record sorted number of chars
    int charCount[26];
} sharedMemory;

node* append(char data){
    node* new = (node*)malloc(sizeof(node));
    new->data = data;
    new->next = NULL;
    return new;
}

void freeList(node* hnode) {
    node* temp;
    while(hnode != NULL){
        temp = hnode;
        hnode = hnode->next;
        free(temp);
    }
}

void *thread(void *arg){
    sharedMemory *datL = (sharedMemory *)arg;
    node* curr = datL->start;
    while(datL->counter > 0 && curr->next != NULL){
        //printf("char: %c\n", curr->data);
        datL->charCount[curr->data - 97] += 1;
        curr = curr->next;
        datL->counter -= 1;
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

sharedMemory datInit(node *hnode, int j, int fileCharCount){
    sharedMemory dat;
    node *temp = hnode->next;
    //dat.start = temp + j * (2 * (fileCharCount - fileCharCount%3)/3);
    for(int i = 0; i < j * (fileCharCount - fileCharCount%3)/3; i++){
        temp = temp->next;
    }
    dat.start = temp;
    //printf("%d\n", j);
    //printf("%d\n", (j * (2 * (fileCharCount - fileCharCount%3)/3)));
    //printf("%c\n", dat.start->data);
    if(j == 3){
        dat.counter = fileCharCount%3;
    }else{
        dat.counter = (fileCharCount - fileCharCount%3)/3;
    }
    memset(dat.charCount, 0, sizeof(dat.charCount)); //Initialize with all 0, otherwise random number will appear
    return dat;
}

int main(int argc, char *argv[]){
    for (int i = 1; i < argc; i++){
        FILE *f;
        f = fopen(argv[i], "r");
        node *hnode;
        hnode = (node*)malloc(sizeof(node));    //assign mem for first node
        node *cur = hnode;
        int fileCharCount = -1; // Remove the count of null at EOF

        while(!feof(f)){ //Insert string array into linkedlist
            cur->next = append(fgetc(f));
            fileCharCount++;
            cur = cur->next;
        }
        fclose(f);

        cur = hnode->next;
        while(cur->next != NULL){
            //printf("%d\n", &cur->data);
            cur = cur->next;
        }
        //printf("\n");

        //printf("%d\n", fileCharCount);
        pthread_t *threadStack;
        threadStack = (pthread_t *)malloc(4 * sizeof(pthread_t));
        sharedMemory datStack[4];
        for(int j = 0; j < 4; j++){
            datStack[j] = datInit(hnode, j, fileCharCount);
            //printf("%c\n", datStack[j].start->data);
            pthread_create(&threadStack[j], NULL, thread, (void *)&datStack[j]);
        }
        for (int j = 0; j < 4; j++){
            pthread_join(threadStack[j], NULL);
            for(int k = 0; k < 26; k++){
                mainCountList[k] += datStack[j].charCount[k];
            }
        }

        printf("%s\n", argv[i]);
        printResult(mainCountList);
        freeList(hnode);        
    }


    return 0;
}