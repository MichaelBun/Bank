#include "atm.h"

int main()
{

    void* (*atm_func)(void*); //ATM function pointer
    atm_func = &atm_start;

    for(int i=0; i<MAX_ACCOUNT_NUM; i++)
    {
        account_full[i] = false;
    }


    //BANK SEMAPHONE INIT
    //BANK SEMAPHONE INIT
    bank_sem_read = (sem_t*)malloc(sizeof(sem_t));
    bank_sem_write = (sem_t*)malloc(sizeof(sem_t));
    sem_init(bank_sem_read,0,1);
    sem_init(bank_sem_write,0,1);
    bank_readers=0;
    //BANK SEMAPHONE INIT
    //BANK SEMAPHONE INIT

    //Initializing ATMS
    for(int i=0; i<ATM_NUM; i++)
    {
        ATM_ARR[i] = (ATM*)malloc(sizeof(ATM)); //Common memory
        ATM_ARR[i]->serial = i;
    }

    //Initializing account pointers
    for(int i=0; i<MAX_ACCOUNT_NUM; i++)
    {
        account_ARR[i] = NULL;
    }

    //Creating a new file
    log_file = fopen("log.txt","w");



    //Threads
    pthread_t ATM_THR[ATM_NUM];
    for(int i=0; i<ATM_NUM; i++)
    {
        int rc;
        if(rc = pthread_create(&ATM_THR[i],NULL,atm_func,(void*)ATM_ARR[i]))
        {
            printf("ERROR\n");
            exit (-1);
        }
    }

    //Checking if they have finished
    for(int i=0; i<ATM_NUM; i++)
    {
        pthread_join(ATM_THR[i],NULL);
    }

    //DESTROYING BANK SEMAPHORE
    //DESTROYING BANK SEMAPHORE
    //DESTROYING BANK SEMAPHORE
    sem_destroy(bank_sem_read);
    sem_destroy(bank_sem_write);
    free(bank_sem_read);
    free(bank_sem_write);
    //DESTROYING BANK SEMAPHORE
    //DESTROYING BANK SEMAPHORE
    //DESTROYING BANK SEMAPHORE

    exit(0);
}
