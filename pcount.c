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

struct node{
    char data;
    struct node* next;
} ;
typedef struct node Node;

struct dataStore {
    Node *content;  //pointer to the input string
    Node *curr; //current node, replace index later
    int resultArr[26];
} dataStore;

Node* append(char data){
    Node* new = (Node*)malloc(sizeof(Node));
    new->data = data;
    new->next = NULL;
    return new;
}

void freeList(Node* hnode) {
    Node* temp;
    while(hnode != NULL){
        temp = hnode;
        hnode = hnode->next;
        free(temp);
    }
}

void *test1(void *arg){
    struct dataStore *datL = (struct dataStore *)arg;
    Node* curr = datL->curr;
    while(curr != NULL){
        //sleep(1);
        pthread_mutex_lock(&mutex); //lock when interacting with shared memory
        curr = datL->curr;
        char *cp1 = &curr->data;
        char currentChar = curr->data;
        datL->resultArr[currentChar - 97] += 1;
        curr = curr->next;
        datL->curr = curr;
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

void printResult(int arr[26]){
    for(int i = 0; i < 25; i++){
        if(arr[i] > 0){
            printf("%c %d\n", 97 + i, arr[i]);
        }
    }
}

int main(int argc, char *argv[]){
    pthread_t t1;
    pthread_t t2;
    struct dataStore dat;
    char input[] = "abxasaaoirghaoi"; //file input here
    Node *hnode;
    hnode = (Node*)malloc(sizeof(Node));    //assign mem for first node
    Node *cur = hnode;
    for(int j = 0; j < strlen(input); j++){ //Insert string array into linkedlist
        cur->next = append(input[j]);
        cur = cur->next;
    }

    dat.content = hnode; //store linked list
    dat.curr = hnode->next;
    memset(dat.resultArr, 0, sizeof(dat.resultArr)); //Initialize with all 0, otherwise random number will appear

    printf("Input: ");
    cur = hnode;
    cur = cur->next;
    while(cur != NULL){
        printf("%c", cur->data);
        cur = cur->next;

    }
    printf("\n");

    pthread_create(&t1, NULL, test1, (void *)&dat);
    pthread_create(&t2, NULL, test1, (void *)&dat);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printResult(dat.resultArr);

    freeList(hnode);

    return 0;
}
