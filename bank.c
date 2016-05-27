#include "bank.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

//HELPING FUNCTIONS
int cmpfunc(const void* elem1, const void* elem2);





//*****************************API functions***********************************/
pbank bank_init(account** pacc_arr) {

	pbank bank = (pbank)malloc(sizeof(bank));
	bank->pacc_arr = pacc_arr;
	bank->bank_balance = 0;
	//bank->num_of_accs = 0;
}

void free_bank(pbank bank) {
	free(bank);
}


void commision(pbank bank) {
	srand(time(NULL));
	double percentage = (rand() % 2 + 2);

	READ_LOCK(bank_sem_read, bank_sem_write, &bank_readers); //semaphore_read bank 
	for (int i = 0; i<num_of_accs; i++) {
		int sum = account_commision(bank->pacc_arr[i], percentage); 
		bank->bank_balance += sum;
		//Need lock for log file
		//READ_LOCK(bank->pacc_arr[i].account_sem_read, bank->pacc_arr[i].account_sem_write, &bank->pacc_arr[i].account_readers));
		fprintf(log_file, "Bank: commissions of %.3f % were charged, the bank gained %d $ from account %d\n", percentage, sum, bank->pacc_arr[i]->number);
		//READ_UNLOCK(bank->pacc_arr[i].account_sem_read, bank->pacc_arr[i].account_sem_write, &bank->pacc_arr[i].account_readers));
	}


	READ_UNLOCK(bank_sem_read, bank_sem_write, &bank_readers); //unlock
}

void print_acc(pbank bank) {
	printf("\033[2J");
	printf("\033[1;1H");

	READ_LOCK(bank_sem_read, bank_sem_write, &bank_readers); //lock
	

	sem_wait(bank_sem_write);//lock
	qsort(bank->pacc_arr, num_of_accs, sizeof(account*), cmpfunc); //sort account array
	sem_post(bank_sem_write);//unlock


	if (num_of_accs == 0) {
		READ_UNLOCK(bank_sem_read, bank_sem_write, &bank_readers); //unlock
		return;
	}

	printf("Current Bank Status\n");
	for (int i = 0; i<num_of_accs; i++) {
		printf(log_file, "Account %d: Balance = %d , ", bank->pacc_arr[i]->number, bank->pacc_arr[i]->balance);
		printf(log_file, "Account Password = %d, \n", bank->pacc_arr[i]->password);
	}

	printf(log_file, "The Bank has %d $", bank->bank_balance);
	READ_UNLOCK(bank_sem_read, bank_sem_write, &bank_readers); //unlock
}

/******************************************************************************/




//func compares account ids'
int cmpfunc(const void* elem1, const void* elem2) {

	account* f = (account*)elem1;
	account* s = (account*)elem2;
	if (f->number > s->number) return 1;
	if (f->number <	 s->number) return -1;
	return 0;
}

