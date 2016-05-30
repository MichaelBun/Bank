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
        free(ATM_ARR[i]->file_name);
        free(ATM_ARR[i]);
    }
	free(bank);
}


void commision(pbank bank) {
    if(num_of_accs==0) return;
	srand(time(NULL));
	double percentage;
    double range = (4 - 2);
    double div = RAND_MAX / range;
    percentage= 2 + (rand() / div);

	READ_LOCK(bank_sem_read, bank_sem_write, &bank_readers); //semaphore_read bank
	for (int i = 0; account_full[i]==true ; i++) {

  
    READ_LOCK(account_ARR[i]->account_sem_read,account_ARR[i]->account_sem_write,&(account_ARR[i]->account_readers));
		 // int sum = account_commision(bank->pacc_arr[i], percentage);
	    int comission = round(bank->pacc_arr[i]->balance*percentage / 100);
    	bank->pacc_arr[i]->balance -= comission;
      READ_UNLOCK(account_ARR[i]->account_sem_read,account_ARR[i]->account_sem_write,&(account_ARR[i]->account_readers));
   
		//READ_LOCK(bank_sem_read, bank_sem_write, &bank_readers); //semaphore_read bank
		bank->bank_balance += comission;
		//READ_UNLOCK(bank_sem_read, bank_sem_write, &bank_readers); //unlock

		//Need lock for log file
		sem_wait(sem_write_to_log);
		fprintf(log_file, "Bank: commissions of %.2f %% were charged, the bank gained %d $ from account %d\n", percentage, comission, bank->pacc_arr[i]->number);
		sem_post(sem_write_to_log);

 

	}


	READ_UNLOCK(bank_sem_read, bank_sem_write, &bank_readers); //unlock
}

void print_acc(pbank bank) {
    if(num_of_accs==0) return;
    printf("\033[2J");
  	printf("\033[1;1H");

 

    READ_LOCK(bank_sem_read, bank_sem_write, &bank_readers); //semaphore_read bank
     bool acc_printed[num_of_accs];
     
    for(int i=0; i<num_of_accs; i++)
    {
        acc_printed[i] = false;
    }

	printf("Current Bank Status\n");

	for(int j=0; j<num_of_accs; j++)
	{
	//printf("FIRST FOR\n");

        account* current_min = NULL;
        int counter_mem;
        for(int i=0; i<num_of_accs; i++)
        {            
         bool flagg =false; 
        //printf("SECOND FOR\n");
       //  READ_LOCK(account_ARR[i]->account_sem_read,account_ARR[i]->account_sem_write,&(account_ARR[i]->account_readers));
       //  READ_LOCK(current_min->account_sem_read,current_min->account_sem_write,&(current_min->account_readers));
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
                      flagg=true;
                        //printf("ELSE - IF\n");
               //  READ_UNLOCK(current_min->account_sem_read,current_min->account_sem_write,&(current_min->account_readers));                
                        current_min = account_ARR[i];
                        counter_mem = i;
                    }
                }
                else {
                 READ_UNLOCK(bank_sem_read, bank_sem_write, &bank_readers); //unlock
                 // READ_UNLOCK(account_ARR[i]->account_sem_read,account_ARR[i]->account_sem_write,&(account_ARR[i]->account_readers));
                 return;
                }
            }
      //  if(flagg!=true)
      //    READ_UNLOCK(account_ARR[i]->account_sem_read,account_ARR[i]->account_sem_write,&(account_ARR[i]->account_readers));
        }

        acc_printed[counter_mem] = true;

            printf("Account %d: Balance = %d , \n", current_min->number, current_min->balance);
            printf("Account Password = %s, \n", current_min->password);

    }

	  printf("The Bank has %d $\n", bank->bank_balance);
    READ_UNLOCK(bank_sem_read, bank_sem_write, &bank_readers); //unlock

}

/******************************************************************************/



