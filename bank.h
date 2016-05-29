#ifndef BANK_H_
#define BANK_H_

#include "atm.h"
#include <time.h>

typedef struct  _bank
{
	account** pacc_arr;
	int bank_balance;
	//int num_of_accs;
	//sem_t* bank_sem_read;
	//sem_t* bank_sem_write;

}bank, *pbank;


pbank bank_init(account**);
void commision(pbank);
void print_acc(pbank);
void free_bank(pbank);


#endif
