#ifndef _ATM_H
#define _ATM_H
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <math.h>

#define MAX_LINE_SIZE 30
#define MAX_ATM_NUM 200
#define MAX_ACCOUNT_NUM 1000

int ATM_NUM;
sem_t* bank_sem_read;
sem_t* bank_sem_write;
int bank_readers;
sem_t* sem_write_to_log;

typedef enum _bool {false,true}bool;


typedef struct _ATM
{
    int serial;
    char* file_name;
}ATM;

typedef struct _account
{
    int number;
    char* password;
    int balance;
    sem_t* account_sem_read;
    sem_t* account_sem_write;
    int account_readers;
}account;

bool account_full[MAX_ACCOUNT_NUM]; //Shows if the account in place i is empty or not
ATM *ATM_ARR[MAX_ATM_NUM]; //Initialize the ATM list
account *account_ARR[MAX_ACCOUNT_NUM];//Initializing the account array
FILE* log_file; //Pointer to our log file
int num_of_accs;
bool flag;

//IMPLEMENTATION FUNCTIONS
void* atm_start(void* atm_data);
void open_account(char* account_name, char* password, char* initial_ammount,int atm_num); //opens a new account
void deposit (char* account_number, char* password, char* ammount,int atm_num);
void balance (char* account_number, char* password,int atm_num);
void transfer(char* account_number, char* password, char* target_account, char* ammount,int atm_num);
void withdraw(char* account_number, char* password, char* ammount,int atm_num);

int account_commision(account* acc, double percent);//TODO

//HELPING FUNCTIONS
void READ_LOCK(sem_t* read_sem, sem_t* write_sem, int* readers);
void READ_UNLOCK(sem_t* read_sem, sem_t* write_sem, int* readers);

#endif
