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

typedef struct dataStore {
    int a_count;    //count of char "a"
    int index;      //current index on the input string
    char *content;  //pointer to the input string
} dataStore;

void *test1(void * arg){
    dataStore *datL = (dataStore *)arg;
    while(datL->index < strlen(datL->content)){
        //sleep(1);
        char *cp1 = &datL->content[datL->index];
        if(strncmp(cp1,"a",1) == 0){
            datL->a_count += 1;
        }
        datL->index += 1;
    }
    pthread_exit(NULL);
}

int main(){
    pthread_t t1;
    pthread_t t2;
    dataStore dat;
    char input[] = "aaxasaaoirgha[oi"; //file input here

    dat.a_count = 0;
    dat.index = 0;
    dat.content = input;

    pthread_create(&t1, NULL, test1, (void *)&dat);
    pthread_create(&t2, NULL, test1, (void *)&dat);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("a count: %d\n", dat.a_count);


    return 0;
}
