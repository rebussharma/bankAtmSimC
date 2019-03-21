// ***** SERVER *******

#include	"unp.h"
#include <string.h>
#include <stdio.h>

bool logged_in = false;
int acc_no = -1;
int balance = 0;
char* file_arr[512][512];
int balance_file[512];
int balance_file_size = 0;

//char* file_name = "";


char *itoa (int value, char *result, int base)
{
    // check that the base if valid
    if (base < 2 || base > 36) 
	{ 
		*result = '\0'; 
		return result;
	  }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

int number_of_accounts()
{
	FILE *fileptr;
    int count_lines = 0;
    char filechar[400], chr;
 
    fileptr = fopen("accounts", "r");
   //extract character from file and store in chr
    chr = getc(fileptr);
    while (chr != EOF)
    {
        //Count whenever new line is encountered
        if (chr == '\n')
        {
            count_lines = count_lines + 1;
        }
        //take next character from file.
        chr = getc(fileptr);
    }
    fclose(fileptr); //close file.
    return count_lines;
}

void balance_file_to_array()
{
	FILE* f = fopen("balance", "r");
    int n = 0;
    int i = 0;

    while( fscanf(f, "%d\n", &n) > 0 ) 
    {
    	//printf("%d\n", n);
        balance_file[i++] = n;
        balance_file_size++;
    }

    fclose(f);
}

void balance_array_to_file()
{
	FILE *f = fopen("balance", "w");
	
	for (int i = 0; i < balance_file_size; i++)
	{
		fprintf(f, "%d\n", balance_file[i]);
	}

	fclose(f);
}

char* get_file_line(int line, char* file_name)
{
	// given a line number, return that line

	FILE *fp; // create file pointer
	fp = fopen(file_name, "r"); // open the file

	int current_line = 0;
	char singleLine[150];
	while (!feof(fp))
	{
		fgets(singleLine, 150, fp);

		if (current_line == line)
		{
			//printf("%s", singleLine);
			char *str_to_ret = (char *)malloc (sizeof (char) * 150);
			str_to_ret = singleLine; 
			return str_to_ret;
		}

		current_line++;
	}

	fclose(fp); // close file
}

char* get_balance()
{
	if (logged_in)
	{
        
		
		return get_file_line(acc_no, "balance");
	}
}

void deposit(int amount) 
{
	// deposit money into an account
	balance_file_to_array(); // put balance file into array
	balance_file[acc_no] = (int)(atoi(get_balance()) + amount); // add amount to current balance in array
	balance_array_to_file(); // write array back to file
}

void withdraw(int sockfd,int amount)
{
	// withdraw money from an account
	 int money = atoi(get_balance());
	if(amount > money)
	{
		  write(sockfd, "\nInsufficient Funds to Withdraw\n\nEnter Command to Interact: ", sizeof("\nInsufficient Funds to Withdraw\n\nEnter Command to Interact: "));
	}else{
	balance_file_to_array(); // put balance file into array
	balance_file[acc_no] = (int)(atoi(get_balance()) - amount); // add amount to current balance in array
	balance_array_to_file(); // write array back to file
 write(sockfd, "Withdrawl Successful.\n\nEnter a Command to Proceed: ", sizeof("Withdrawl Successful.\n\nEnter a Command to Procees: "));
	}
}

void login(int sockfd, char buf[MAXLINE], ssize_t n)
{
	write(sockfd, "\nPlease enter your account number: \n", sizeof("\nPlease enter your account number: \n")); 
	n = read(sockfd, buf, MAXLINE);
	buf[n] = 0; // null terminate the buffer
	acc_no = atoi(buf);

	write(sockfd, "\nPlease enter your password: \n", sizeof("\nPlease enter your password: \n")); 
	n = read(sockfd, buf, MAXLINE);
    buf[n] = 0;

	if (strstr(buf, get_file_line(acc_no, "accounts")))
	{
        write(sockfd, "\nYou are Logged in.\n\nUse the following commands to interact\n3:   Withdraw\n4:   Deposit\n5:   Balance\n6:   LogOut\n\nEnter your command: ", sizeof("\nYou are Logged in.\n\nUse the following commands to interact\n3:   Withdraw\n4:   Deposit\n5:    Balance\n6:  LogOut\n\nEnter your command: "));
		
		logged_in = true;
	} else {
		write(sockfd, "Invalid password!\nEnter 1 to Login: ", sizeof("Invalid password!\nEnter 1 to login: "));
	}
}

void create_account(int sockfd, char buf[MAXLINE], ssize_t n)
{
	write(sockfd, "\nPlease enter a password: \n", sizeof("\nPlease enter a password: \n"));
	n = read(sockfd, buf, MAXLINE);
	buf[n] = 0; // null terminate the buffer

	FILE *fp; // create file pointer
	fp = fopen("balance", "a"); // open the file

	fprintf(fp, "%d\n", 0); // write 0 to balance
	fclose(fp); // close file

	FILE *fp2; // create file pointer
	fp2 = fopen("accounts", "a"); // open the file

	fprintf(fp2, "%s", buf); // write password to file

	fclose(fp2); // close file

	// int to string
	char snum[128];
	itoa(number_of_accounts()-1, snum, 10);
	//end int to string

	char message[] = "\nYour account number is: ";
	strcat(message, snum);

	write(sockfd, message, strlen(message));
	write(sockfd, "\nEnter 1 to log In: ", sizeof("\nEnter 1 to log In: "));
}

void str_echo(int sockfd)
{
	ssize_t n; // size socket msg
	char buf[MAXLINE]; // buffer
	char sendline[MAXLINE]; // send buffer
	int deposit_amount, withdraw_amount; 

	again:
		while ( ( n = read(sockfd, buf, MAXLINE) ) > 0) // read from socket while still getting messages
		{
			buf[n] = 0; // null terminate the buffer

			fflush(stdout);
			if (strstr(buf, "1")) 
			{
				login(sockfd, buf, n); // allow user to login
			}

			if (strstr(buf, "2")) 
			{
				create_account(sockfd, buf, n); // allow user to login
			}
		 
			if (strstr(buf, "3") && logged_in) 
			{
				write(sockfd, "\nEnter Amount to Withdraw: ", sizeof("\nEnter Amount to Withdraw: ")); 

			
				n = read(sockfd, buf, MAXLINE);
				buf[n] = 0; // null terminate the buffer
				withdraw_amount = atoi(buf); // convert buf to int
				withdraw(sockfd,withdraw_amount); // withdraw
				
                               
			}
			
			 else if (strstr(buf, "3") && !logged_in) {
				write(sockfd, "Error: Login or Create Account to Proceed\n\nEnter your Command: ", sizeof("Error: Login or Create Account to Proceed\n\nEnter your Command: ")); 
			}

			if (strstr(buf, "4") && logged_in)
			{
				write(sockfd, "\nEnter Amount to Deposit: ", sizeof("\nEnter Amount to Deposit: ")); 
				n = read(sockfd, buf, MAXLINE);

				buf[n] = 0; // null terminate the buffer
				deposit_amount = atoi(buf); // convert buf to int
				deposit(deposit_amount); // deposit 
				write(sockfd, "Deposit Successful.\n\nEnter a Command to Proceed: ", sizeof("Deposit Successful.\n\nEnter a Command to proceed: ")); 
			} else if (strstr(buf, "4") && !logged_in) {
				write(sockfd, "Error: Login or Create Account to Proceed\n\nEnter your Command: ", sizeof("Error: Login or Create Account to Proceed\n\nEnter your Command: "));
                
			}

			if (strstr(buf, "5") && logged_in)
			{
				fflush(stdout);
				char *snum = get_balance();
				buf[n] = 0;
    			        char message[] = "Your Balance is: ";
                
                
				
				strcat(message, snum);
				write(sockfd, message, strlen(message));
				write(sockfd, "\nEnter a Command to Proceed: ", sizeof("\nEnter a Command to proceed: ")); 

				
			} else if (strstr(buf, "5") && !logged_in) {
				write(sockfd, "Error: Login or Create Account to Proceed\n\nEnter your Command: ", sizeof("Error: Login or Create Account to Proceed\n\nEnter your Command: ")); 
			}
            else if (strstr(buf, "6")) {
                write(sockfd, "\nLogged Out\n", sizeof("\nLogged Out\n"));
                write(sockfd, "\nLogged noOut\n", sizeof("\nLogged noOut\n"));
                login(sockfd, buf, n);
           
                
            
                     buf[n] = 0;
            
            }
            
	
			

		}



		// ** ERROR HANDLING **
		if (n < 1 && errno == EINTR) // interrupted
		{
			goto again; // go back to "again"
		}
		else if (n < 0) // if we're not getting more messages
		{	
			printf("str_echo: read error"); // print an error msg
			return;
		}	
		// ** END ERROR HANDLING **
}

int main(int argc, char **argv)
{
	int			listenfd, connfd; // socket file descriptors
	pid_t				childpid; // process id
	socklen_t			clilen; // addr size
	struct sockaddr_in	cliaddr, servaddr; // socket addr

	listenfd = socket(AF_INET, SOCK_STREAM, 0); // create socket

	bzero(&servaddr, sizeof(servaddr)); // zero out servaddr
	servaddr.sin_family      = AF_INET; // set family
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // set addr
	servaddr.sin_port        = htons(SERV_PORT); // set port

	bind(listenfd, (SA *) &servaddr, sizeof(servaddr)); // bind to addr

	listen(listenfd, LISTENQ); // listen to socket

	for ( ; ; ) {
		clilen = sizeof(cliaddr); // get size
		connfd = accept(listenfd, (SA *) &cliaddr, &clilen); // accept

		if ( (childpid = fork()) == 0) {	/* child process */
			close(listenfd);	/* close listening socket */

			str_echo(connfd);	/* process the request */
			exit(0);
		}
		close(connfd);			/* parent closes connected socket */
	}
}


