#include "bank.h"
#include <time.h>

pbank bank_init(account* pacc_arr, int num_of_accs) {

	pbank bank = (pbank)malloc(sizeof(bank));
	bank->pacc_arr = pacc_arr;
	bank->bank_balance = 0;
	bank->num_of_accs = num_of_accs;
}

void free_bank(pbank bank) {
	free(bank);
}


void commision(pbank bank) {
	srand(time(NULL));
	int percentage = (rand() % 2 + 2);
	//lock??
	for (int i = 0; i < bank->num_of_accs; i++) {
		int sum = account_commision(bank->pacc_arr[i], percentage);
		bank->bank_balance += sum;
		//Need lock for log file
		//READ_LOCK(bank->pacc_arr[i].account_sem_read, bank->pacc_arr[i].account_sem_write, &bank->pacc_arr[i].account_readers));
		fprintf(log_file, "Bank: commissions of %d % were charged, the bank gained %d $ from account %d\n", percentage, sum, bank->pacc_arr[i].number);
		//READ_UNLOCK(bank->pacc_arr[i].account_sem_read, bank->pacc_arr[i].account_sem_write, &bank->pacc_arr[i].account_readers));
	}


	//unlock
}

void print_acc(pbank bank) {
	printf("\033[2J");
	printf("\033[1;1H");
	//lock
	
	if (bank->num_of_accs == 0) {
		//unlock
		return;
	}

	printf("Current Bank Status\n");
	for (int i = 0; i < bank->num_of_accs; i++) {
		printf(log_file, "Account %d: Balance = %d , ", bank->pacc_arr[i].number, bank->pacc_arr[i].balance);
		printf(log_file, "Account Password = %d, \n", bank->pacc_arr[i].password);
	}
	printf(log_file, "The Bank has %d $", bank->bank_balance);
	//unlock
}
