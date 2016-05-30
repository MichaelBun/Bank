#include "bank.h"
#include "atm.h"

#include <stdio.h>
#include <stdlib.h>

//HELPING FUNCTIONS
int cmpfunc(const void* elem1, const void* elem2);





//*****************************API functions***********************************/
pbank bank_init(account** pacc_arr) {

    //READ_LOCK(bank_sem_read, bank_sem_write, &bank_readers);
	pbank bank = (pbank)malloc(sizeof(bank));
	bank->pacc_arr = pacc_arr;
	bank->bank_balance = 0;
	return bank;
	//bank->num_of_accs = 0;
	//READ_UNLOCK(bank_sem_read, bank_sem_write, &bank_readers);
}

void free_bank(pbank bank) {
    for(int i=0; i < num_of_accs; i++){
        sem_destroy(bank->pacc_arr[i]->account_sem_read);
        sem_destroy(bank->pacc_arr[i]->account_sem_write);
        free(bank->pacc_arr[i]->account_sem_read);
        free(bank->pacc_arr[i]->account_sem_write);
        free(bank->pacc_arr[i]->password);
        free(bank->pacc_arr[i]);
    }
    for(int i=0; i<ATM_NUM; i++){
        free(ATM_ARR[i]);
    }
	free(bank);
}


void commision(pbank bank) {
    if(num_of_accs==0) return;
	srand(time(NULL));
	double percentage = (rand() % 2 + 2);

	//READ_LOCK(bank_sem_read, bank_sem_write, &bank_readers); //semaphore_read bank
	for (int i = 0; account_full[i]==true ; i++) {
		int sum = account_commision(bank->pacc_arr[i], percentage);
		sem_wait(account_ARR[i]->account_sem_write);
		bank->bank_balance += sum;
		//Need lock for log file
		sem_wait(sem_write_to_log);
		fprintf(log_file, "Bank: commissions of %d %% were charged, the bank gained %d $ from account %d\n", (int)percentage, sum, bank->pacc_arr[i]->number);
		sem_post(sem_write_to_log);
        sem_post(account_ARR[i]->account_sem_write);
	}


	//READ_UNLOCK(bank_sem_read, bank_sem_write, &bank_readers); //unlock
}

void print_acc(pbank bank) {
    if(num_of_accs==0) return;
    //printf("\033[2J");
	//printf("\033[1;1H");

    bool acc_printed[num_of_accs];
    for(int i=0; i<num_of_accs; i++)
    {
        acc_printed[i] = false;
    }

	//READ_LOCK(bank_sem_read, bank_sem_write, &bank_readers); //lock

   // sleep(1);
    sem_wait(sem_write_to_log);
	fprintf(log_file, "Current Bank Status\n");
	sem_post(sem_write_to_log);

	for(int j=0; j<num_of_accs; j++)
	{
	//printf("FIRST FOR\n");
        account* current_min = NULL;
        int counter_mem;
        for(int i=0; i<num_of_accs; i++)
        {
        //printf("SECOND FOR\n");
            if(current_min == NULL && acc_printed[i] != true)
            {
                //printf("FIRST IF\n");
                current_min = account_ARR[i];
                counter_mem = i;
            }
            else
            {
                //printf("ELSE\n");
                if(current_min!= NULL)
                {
                    if((current_min->number > account_ARR[i]->number) && acc_printed[i] != true)
                    {
                        //printf("ELSE - IF\n");
                        current_min = account_ARR[i];
                        counter_mem = i;
                    }
                }
                else return;
            }

        }

        acc_printed[counter_mem] = true;

        //for (int i = 0; account_full[i]==true; i++) {
            READ_LOCK(current_min->account_sem_read,current_min->account_sem_write,&(current_min->account_readers));
            sem_wait(sem_write_to_log);
            fprintf(log_file, "Account %d: Balance = %d , \n", current_min->number, current_min->balance);
            fprintf(log_file, "Account Password = %s, \n", current_min->password);
            sem_post(sem_write_to_log);
            READ_UNLOCK(current_min->account_sem_read,current_min->account_sem_write,&(current_min->account_readers));
        //}
    }
    sem_wait(sem_write_to_log);
	fprintf(log_file, "The Bank has %d $\n", bank->bank_balance);
	sem_post(sem_write_to_log);
	//READ_UNLOCK(bank_sem_read, bank_sem_write, &bank_readers); //unlock
}

/******************************************************************************/



