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


struct dataStore {
    int index;      //current index on the input string
    char *content;  //pointer to the input string
    int resultArr[26];
} dataStore;

void *test1(void *arg){
    struct dataStore *datL = (struct dataStore *)arg;
    char c = 97; //ascii a = 97

    while(datL->index < strlen(datL->content)){
        //sleep(1);
        char *cp1 = &datL->content[datL->index];
        if(strncmp(cp1, &c, 1) == 0){
            datL->resultArr[0] += 1;
        }
        datL->index += 1;
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
    char input[] = "aaxasaaoirgha[oi"; //file input here

    dat.index = 0;
    dat.content = input;
    memset(dat.resultArr, 0, sizeof(dat.resultArr));

    pthread_create(&t1, NULL, test1, (void *)&dat);
    pthread_create(&t2, NULL, test1, (void *)&dat);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    //printf("a count: %d\n", dat.resultArr[0]);
    printResult(dat.resultArr);


    return 0;
}
