#include "bank.h"
#include <time.h>
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
    for(int i=0; i<num_of_accs; i++){
        sem_destroy(bank->pacc_arr[i]->account_sem_read);
        sem_destroy(bank->pacc_arr[i]->account_sem_write);
        free(bank->pacc_arr[i]->account_sem_read);
        free(bank->pacc_arr[i]->account_sem_write);
        free(bank->pacc_arr[i]);
    }
    for(int i=0; i<ATM_NUM; i++){
        free(ATM_ARR[i]);
    }
    //free(bank->pacc_arr);
	free(bank);
}


void commision(pbank bank) {
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
    printf("\033[2J");
	printf("\033[1;1H");



	//READ_LOCK(bank_sem_read, bank_sem_write, &bank_readers); //lock

	if (num_of_accs == 0) {
		//READ_UNLOCK(bank_sem_read, bank_sem_write, &bank_readers); //unlock
		return;
	}
   // sleep(1);
    sem_wait(sem_write_to_log);
	fprintf(log_file, "Current Bank Status\n");
	sem_post(sem_write_to_log);

	for (int i = 0; account_full[i]==true; i++) {
        READ_LOCK(account_ARR[i]->account_sem_read,account_ARR[i]->account_sem_write,&(account_ARR[i]->account_readers));
        sem_wait(sem_write_to_log);
		fprintf(log_file, "Account %d: Balance = %d , \n", bank->pacc_arr[i]->number, bank->pacc_arr[i]->balance);
		fprintf(log_file, "Account Password = %s, \n", bank->pacc_arr[i]->password);
		sem_post(sem_write_to_log);
		READ_UNLOCK(account_ARR[i]->account_sem_read,account_ARR[i]->account_sem_write,&(account_ARR[i]->account_readers));
	}
    sem_wait(sem_write_to_log);
	fprintf(log_file, "The Bank has %d $\n", bank->bank_balance);
	sem_post(sem_write_to_log);
	//READ_UNLOCK(bank_sem_read, bank_sem_write, &bank_readers); //unlock
}

/******************************************************************************/



