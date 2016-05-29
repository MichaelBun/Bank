#include "atm.h"
#include "bank.h"
#define SIZE_OF_COMMAND_FILE 20
#define PLACE_OF_N_CHAR 5
#define MAX_WORD_SIZE 20

    void* atm_start(void* atm_data)
    {

        ATM* My_ATM = (ATM*)atm_data; //Getting my current atm serial number


       /* char cwd[100]; //CURRENT DIRECTORY TEST
        getcwd(cwd,100);
        printf("%s\n",cwd); */

         /*printf("WE ARE HERE\n");
        printf("I'm number %d\n",My_ATM->serial);*/

        char my_file_name[SIZE_OF_COMMAND_FILE];
        strcpy(my_file_name,"ATM_n_input_file.txt"); //Finding out which file should we open
        my_file_name[PLACE_OF_N_CHAR-1] = My_ATM->serial + '0' + 1; //From int to ascii
        printf("Our file name is: %s\n",my_file_name);

        FILE* command_file = fopen(my_file_name,"r");
        if(command_file == NULL)
        {
            printf("NO FILE FOUND\n");
            exit(1);
        }
        char* line = NULL; //A string that should hold our current line
        ssize_t read; //For the line read
        size_t len =0; //For the lmine read
        char* delim = (char*)malloc(sizeof(char)*3); //Delimeters
        strcpy(delim," \t\n\0");
        char* args[5];


		//pbank bank = bank_init(account_ARR);
        flag = false;
        while((read = getline(&line, &len, command_file)) != -1) //We got a new line
        {
         //   bool illegal_command = false;
            //printf("%s",line);
            //NEW ARGS
            args[0] = NULL;
            args[1] = NULL;
            args[2] = NULL;
            args[3] = NULL;
            args[4] = NULL;
            //NEW ARGS

            args[0] = strtok(line,delim); //Command


            /*printf("%s",line);
            args[0] = (char*)malloc(sizeof(*line));
            strcpy(args[0],line);*/
            if(args[0] == NULL) //Checking we actually got the argument
            {
               // illegal_command = true;
                printf("CANT READ THE COMMAND\n");
            }
            //printf("%s\n",args[0]);
            int args_num = 1;


            if(!(strcmp(args[0],"O") && strcmp(args[0],"D") && strcmp(args[0],"W") && strcmp(args[0],"B") && strcmp(args[0],"T" )))
            {
                for(int j=1; (args[j] = strtok(NULL,delim))!=NULL; j++)
                {
                    args_num++;
                    if(args_num > 5)
                    {
                 //       illegal_command = true;
                        break;
                    }
                }

                /*Illegal command check*/
                /*Illegal command check*/
                /*Illegal command check*/
                /*Illegal command check*/

                //Time to decide which command this is
                if(!strcmp(args[0],"O"))
                {
                    open_account(args[1],args[2],args[3],My_ATM->serial);
                }

                else if(!strcmp(args[0],"D"))
                {
                    deposit(args[1],args[2],args[3],My_ATM->serial);
                }

                else if(!strcmp(args[0],"B"))
                {
                    balance(args[1],args[2],My_ATM->serial);
                }

                else if(!strcmp(args[0],"W"))
                {
                    withdraw(args[1],args[2],args[3],My_ATM->serial);
                }
                else if(!strcmp(args[0],"T"))
                {
                    transfer(args[1],args[2],args[3],args[4],My_ATM->serial);
                }
            }

        }

        /*free(args[0]);
        free(args[1]);
        free(args[2]);
        free(args[3]);
        free(args[4]);*/
        free(line);
        fclose(command_file);
        flag=true;
        pthread_exit(NULL);
}




void open_account(char* account_number, char* password, char* initial_ammount, int atm_num)
{
   sem_wait(bank_sem_write);
    //Initializing
    bool account_num_taken = false;
    account* new_account = (account*)malloc(sizeof(account));
    new_account->number = atoi(account_number);
    new_account->balance = atoi(initial_ammount);
    new_account->password = (char*)malloc(sizeof(*password));
    strcpy(new_account->password,password);
    new_account->account_sem_read = (sem_t*)malloc(sizeof(sem_t));
    new_account->account_sem_write = (sem_t*)malloc(sizeof(sem_t));
    sem_init(new_account->account_sem_read,0,1);
    sem_init(new_account->account_sem_write,0,1);
    new_account->account_readers = 0;
    for(int i=0; i<MAX_ACCOUNT_NUM;i++)
    {
        if(account_full[i]==true) //CHECKING IF THIS ACCOUNT ALREADY EXISTS
        {
            if(account_ARR[i]->number==atoi(account_number))
            {
                //sleep(1);
                fprintf(log_file,"Error %d: Your transaction failed – account with the same id exists\n",atm_num);
                account_num_taken = true;
                free(new_account->account_sem_read);
                free(new_account->account_sem_write);
                free(new_account->password);
                free(new_account);
            }
        }
    }

    //Pushing the new account into the array
    for(int i=0; (i<MAX_ACCOUNT_NUM) && (account_num_taken == false)  ;i++) //While we find an empty space and
    {
        if(account_full[i]==false)
        {
            //sleep(1);
            account_ARR[i]=new_account;
            num_of_accs++;
            account_full[i]=true;
            fprintf(log_file,"%d: New account id is %d with password %s and initial balance %d\n",atm_num,new_account->number,new_account->password,new_account->balance);
            break;
        }
    }
    sem_post(bank_sem_write);

}

void deposit (char* account_number, char* password, char* ammount,int atm_num)
{
    READ_LOCK(bank_sem_read,bank_sem_write,&bank_readers); //Bank READ
    bool account_found = false;
    bool password_correct = false;
    for(int i=0; i<MAX_ACCOUNT_NUM;i++) //Checking if this acccount exists
    {
        if(account_full[i]==true)
        {
            if((account_ARR[i]->number==atoi(account_number))) //THIS IS THE ACCOUNT
            {
                account_found = true;
                if(!strcmp(account_ARR[i]->password,password)) //password matches
                {
                    sem_wait(account_ARR[i]->account_sem_write); //START WRITE
                    sleep(1);
                    password_correct = true;
                    account_ARR[i]->balance = account_ARR[i]->balance + atoi(ammount);
                    sem_wait(sem_write_to_log);
                    fprintf(log_file,"%d: Account %d new balance is %d after %d $ was deposited\n",atm_num+1,account_ARR[i]->number,account_ARR[i]->balance,atoi(ammount));
                    sem_post(sem_write_to_log);
                    sem_post(account_ARR[i]->account_sem_write); //END WRITE
                }
            }
        }
    }

    if(account_found == false)
    {
        sem_wait(sem_write_to_log);
        fprintf(log_file,"Error %d: Your transaction failed – account id %d does not exist\n",atm_num+1,atoi(account_number));
        sem_post(sem_write_to_log);
    }
    else if(password_correct == false)
    {
        sem_wait(sem_write_to_log);
        fprintf(log_file,"Error %d: Your transaction failed – password for account id %d is incorrect\n",atm_num+1,atoi(account_number));
        sem_post(sem_write_to_log);
    }

    READ_UNLOCK(bank_sem_read,bank_sem_write,&bank_readers);
}

void withdraw(char* account_number, char* password, char* ammount,int atm_num)
{
    READ_LOCK(bank_sem_read,bank_sem_write,&bank_readers); //Bank READ
    int balance_memory; //To remember the balance in the chosen account
    bool account_found = false;
    bool password_correct = false;
    bool enough_money = false;
    for(int i=0; i<MAX_ACCOUNT_NUM;i++) //Checking if this acccount exists
    {
        if(account_full[i]==true)
        {
            if((account_ARR[i]->number==atoi(account_number))) //THIS IS THE ACCOUNT
            {
                account_found = true;
               if(!strcmp(account_ARR[i]->password,password)) //password matches
                {
                    password_correct = true;
                    if(account_ARR[i]->balance >= (balance_memory = atoi(ammount)))
                    {
                        enough_money = true;
                        sem_wait(account_ARR[i]->account_sem_write); //START WRITE
                        sleep(1);
                        account_ARR[i]->balance = account_ARR[i]->balance + atoi(ammount);
                        sem_wait(sem_write_to_log);
                         fprintf(log_file,"%d: Account %d new balance is %d after %d $ was withdrew\n",atm_num+1,account_ARR[i]->number,account_ARR[i]->balance,atoi(ammount));
                         sem_post(sem_write_to_log);
                        sem_post(account_ARR[i]->account_sem_write); //END WRITE
                    }
                }
            }
        }
    }

    if(account_found == false)
    {
        sem_wait(sem_write_to_log);
        fprintf(log_file,"Error %d: Your transaction failed – account id %d does not exist\n",atm_num+1,atoi(account_number));
        sem_post(sem_write_to_log);
    }
    else if(password_correct == false)
    {
        sem_wait(sem_write_to_log);
        fprintf(log_file,"Error %d: Your transaction failed – password for account id %d is incorrect\n",atm_num+1,atoi(account_number));
        sem_post(sem_write_to_log);
    }
    else if(enough_money == false)
    {
        sem_wait(sem_write_to_log);
        fprintf(log_file,"Error %d: Your transaction failed – account id %d balance is lower than %d\n",atm_num+1,atoi(account_number),balance_memory);
        sem_post(sem_write_to_log);
    }

    READ_UNLOCK(bank_sem_read,bank_sem_write,&bank_readers);
}


void balance (char* account_number, char* password,int atm_num)
{
    READ_LOCK(bank_sem_read,bank_sem_write,&bank_readers); //Bank READ
    bool account_found = false;
    bool password_correct = false;
    for(int i=0; i<MAX_ACCOUNT_NUM;i++) //Checking if this acccount exists
    {
        if(account_full[i]==true)
        {
            if((account_ARR[i]->number==atoi(account_number))) //THIS IS THE ACCOUNT
            {
                account_found = true;
                if(!strcmp(account_ARR[i]->password,password)) //password matches
                {
                READ_LOCK(account_ARR[i]->account_sem_read,account_ARR[i]->account_sem_write,&(account_ARR[i]->account_readers));
                password_correct = true;
                sleep(1);
                sem_wait(sem_write_to_log);
                fprintf(log_file,"%d: Account %d balance is %d\n",atm_num+1,account_ARR[i]->number,account_ARR[i]->balance);
                sem_post(sem_write_to_log);
                READ_UNLOCK(account_ARR[i]->account_sem_read,account_ARR[i]->account_sem_write,&(account_ARR[i]->account_readers));
                }
            }
        }
    }

    if(account_found == false)
    {
        sem_wait(sem_write_to_log);
        fprintf(log_file,"Error %d: Your transaction failed – account id %d does not exist\n",atm_num+1,atoi(account_number));
        sem_post(sem_write_to_log);
    }
    else if(password_correct == false)
    {
        sem_wait(sem_write_to_log);
        fprintf(log_file,"Error %d: Your transaction failed – password for account id %d is incorrect\n",atm_num+1,atoi(account_number));
        sem_post(sem_write_to_log);
    }

    READ_UNLOCK(bank_sem_read,bank_sem_write,&bank_readers);

}




void transfer(char* account_number, char* password, char* target_account, char* ammount,int atm_num)
{
    READ_LOCK(bank_sem_read,bank_sem_write,&bank_readers); //Bank READ
    int balance_memory;
    account* from_account=NULL;
    account* to_account=NULL;
    bool from_account_found = false;
    bool password_correct = false;
    bool enough_money = false;
    bool to_account_found = false;

    //SAVING THE POINTERS
    for(int i=0; i<MAX_ACCOUNT_NUM;i++) //Checking if this acccount exists
    {
        if(account_full[i]==true)
        {
            if((account_ARR[i]->number==atoi(account_number))) //THIS IS THE ACCOUNT
            {
                from_account_found = true;
                if(!strcmp(account_ARR[i]->password,password)) //password matches
                {
                    password_correct = true;
                    if(account_ARR[i]->balance >= (balance_memory = atoi(ammount))) //Checking if we have more than ammount
                    {
                        READ_LOCK(account_ARR[i]->account_sem_read,account_ARR[i]->account_sem_write,&(account_ARR[i]->account_readers));
                        enough_money = true;
                        sleep(1);
                        from_account = account_ARR[i]; //Saving the pointer to this account
                        READ_UNLOCK(account_ARR[i]->account_sem_read,account_ARR[i]->account_sem_write,&(account_ARR[i]->account_readers));
                    }
                }
            }
            else if(account_ARR[i]->number == atoi(target_account))
            {
                READ_LOCK(account_ARR[i]->account_sem_read,account_ARR[i]->account_sem_write,&(account_ARR[i]->account_readers));
                to_account_found = true;
                to_account = account_ARR[i]; //Saving the pointer to this account
                READ_UNLOCK(account_ARR[i]->account_sem_read,account_ARR[i]->account_sem_write,&(account_ARR[i]->account_readers));
            }
        }
    }

    //WRITING TO THE POINTERS
    if((from_account_found == true) && (to_account_found == true ) && (password_correct == true) && (enough_money == true))
    {
        sem_wait(from_account->account_sem_write);
        sem_wait(to_account->account_sem_write);
        from_account->balance = from_account->balance - atoi(ammount);
        to_account->balance = to_account->balance + atoi(ammount);
        sem_wait(sem_write_to_log);
        fprintf(log_file,"%d: Transfer %d from account %d to account %d new account balance is %d new target account balance is %d\n",atm_num+1,atoi(ammount),from_account->number,to_account->number,from_account->balance,to_account->balance);
        sem_post(sem_write_to_log);
        sem_post(from_account->account_sem_write);
        sem_post(to_account->account_sem_write);
    }

    if(from_account_found == false)
    {
        sem_wait(sem_write_to_log);
        fprintf(log_file,"Error %d: Your transaction failed – account id %d does not exist\n",atm_num+1,atoi(account_number));
        sem_post(sem_write_to_log);
    }
    else if(to_account_found == false)
    {
        sem_wait(sem_write_to_log);
        fprintf(log_file,"Error %d: Your transaction failed – account id %d does not exist\n",atm_num+1,atoi(target_account));
        sem_post(sem_write_to_log);
    }
    else if(password_correct == false)
    {
        sem_wait(sem_write_to_log);
        fprintf(log_file,"Error %d: Your transaction failed – password for account id %d is incorrect\n",atm_num+1,atoi(account_number));
        sem_post(sem_write_to_log);
    }
    else if(enough_money == false)
    {
        sem_wait(sem_write_to_log);
        fprintf(log_file,"Error %d: Your transaction failed – account id %d balance is lower than %d\n",atm_num+1,atoi(account_number),balance_memory);
        sem_post(sem_write_to_log);
    }


    READ_UNLOCK(bank_sem_read,bank_sem_write,&bank_readers);
}




//HELPING FUNCTIONS
//READERS WRITERS 2016S
void READ_LOCK(sem_t* read_sem, sem_t* write_sem, int* readers)
{
    sem_wait(read_sem);
    (*readers)++;
    if(*readers == 1)
    {
        sem_wait(write_sem);
    }
    sem_post(read_sem);
}
//READERS WRITERS 2016S
void READ_UNLOCK(sem_t* read_sem, sem_t* write_sem, int* readers)
{
    sem_wait(read_sem);
    (*readers)--;
    if(*readers==0)
    {
        sem_post(write_sem);
    }
    sem_post(read_sem);
}

int account_commision(account* pacc, double percent) {

	sem_wait(pacc->account_sem_write); //START WRITE
	//sleep(1);
	int comission = round(pacc->balance*percent / 100);
	pacc->balance -= comission;
	sem_post(pacc->account_sem_write); //END WRITE//unlock
	return comission;

}
