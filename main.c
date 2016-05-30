#include "atm.h"
#include "bank.h"

void *bank_print_thread_func(void *arg);
void *bank_commission_thread_func(void *arg);

//global
pbank p_bank;

int main(int argc, char** argv)
{
     	//init number of accounts
	  num_of_accs = 0;
    //printf("%d\n",argc);
    ATM_NUM = argc-2;
    //printf("%d\n",ATM_NUM);
    char* file_names[ATM_NUM];

    for(int i=0; i<ATM_NUM; i++)
    {
        //printf("%s\n",argv[i+2]);
        file_names[i] = (char*)malloc(sizeof(argv[i+2]));
        strcpy(file_names[i],argv[i+2]);
    }

    void* (*atm_func)(void*); //ATM function pointer
    atm_func= atm_start;


    //BANK SEMAPHONE INIT
    //BANK SEMAPHONE INIT
    bank_sem_read = (sem_t*)malloc(sizeof(sem_t));
    bank_sem_write = (sem_t*)malloc(sizeof(sem_t));
    sem_write_to_log = (sem_t*)malloc(sizeof(sem_t));
    sem_init(bank_sem_read,0,1);
    sem_init(bank_sem_write,0,1);
    sem_init(sem_write_to_log,0,1);
    bank_readers=0;
    //BANK SEMAPHONE INIT
    //BANK SEMAPHONE INIT
    	//init bank
	p_bank = bank_init(account_ARR);
    //Initializing ATMS
    for(int i=0; i<ATM_NUM; i++)
    {
        ATM_ARR[i] = (ATM*)malloc(sizeof(ATM)); //Common memory
        ATM_ARR[i]->serial = i;
        ATM_ARR[i]->file_name = file_names[i];
    }

    //Initializing account pointers
    for(int i=0; i<MAX_ACCOUNT_NUM; i++)
    {
        account_ARR[i] = NULL;
    }




    if(fopen("log.txt","r")!=NULL)
    {
        remove("log.txt");
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
            exit(1);
        }
    }

	//creating bank threads
	pthread_t commission_thread, print_thread;
	if (pthread_create(&commission_thread, NULL, bank_commission_thread_func, (void*)p_bank))
	{
		printf("ERROR\n");
		exit(1);
	}

	if (pthread_create(&print_thread, NULL, bank_print_thread_func, (void*)p_bank))
	{
		printf("ERROR\n");
		exit(1);
	}
	//print_thread=pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
   // commission_thread=pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    //Checking if they have finished
    for(int i=0; i<ATM_NUM; i++)
    {
        pthread_join(ATM_THR[i],NULL);
    }

    if(pthread_cancel (commission_thread)) return 1;
    if(pthread_cancel (print_thread)) return 1;

    pthread_join (commission_thread, NULL);
	pthread_join (print_thread, NULL);

    //DESTROYING BANK SEMAPHORE
    //DESTROYING BANK SEMAPHORE
    //DESTROYING BANK SEMAPHORE
    sem_destroy(bank_sem_read);
    sem_destroy(bank_sem_write);
    sem_destroy(sem_write_to_log);
    free(bank_sem_read);
    free(bank_sem_write);
    free(sem_write_to_log);
    //DESTROYING BANK SEMAPHORE
    //DESTROYING BANK SEMAPHORE
    //DESTROYING BANK SEMAPHORE

	free_bank(p_bank);

    exit(0);
}

void *bank_commission_thread_func(void *arg)
{
	while (1) {
        //if(num_of_accs==0) continue;
        //printf("commission\n");
		commision(p_bank);
        sleep(3);
		pthread_testcancel();
	}

	pthread_exit(NULL);
}

void *bank_print_thread_func(void *arg)
{
	while (1) {
       // if(num_of_accs==0) continue;
        //printf("print\n");
		print_acc(p_bank);
		usleep(500000); //0.5 sec
		pthread_testcancel();
	}
	pthread_exit(NULL);
}
