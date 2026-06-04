/*
 * Project: Strux Database.
 * File: regndlog.c (Register and Login )
 * Author: Aromal J
 *
 * Purpose: Handles user Registers and logins.
 *
 */

//================================================= HEADERS ========================================================

#include <stdio.h>
#include <string.h>
#include <errno.h>

//================================================= GLOBAL  ========================================================

#define MIN_USERNAME_LEN 3
#define MAX_USERNAME_LEN 32
#define MIN_PASSWORD_LEN 8
#define MAX_PASSWORD_LEN 128

#define USERNAME_BUFFER_SIZE 256
#define PASSWORD_BUFFER_SIZE 256


struct credentials
{
	char username[USERNAME_BUFFER_SIZE];
	char password[PASSWORD_BUFFER_SIZE];
};

struct credentials reg;

//================================================ PROTOTYPES ======================================================

int registr(void);
int usercheck(void);
int login(void);
int analyse_password(void);
int analyse_username(void);
int dbms(char *name);

//================================================ MAIN ============================================================

int main(void)
{
	int option;
	char input[32];
	
	printf("\nWelcome to Strux Database\n");

// MENU , This is the menu section that makes it interactive..

	while(1)
	{
		printf("\n-----\nmenu\n-----\n1.Register\n2.Login\n3.Exit\n\nOption: ");
		if (fgets(input, sizeof(input), stdin) == NULL)
		{
			break;
		}

		if (sscanf(input, "%d", &option) != 1)
		{
			printf("\nInvalid input! Enter valid options [ 1/2/3 ]\n");
			continue;
		}
		if ( option == 1)
		{
			int result = registr();		
			if (result == 2)
			{
				printf("Fatal error : Something went wrong!");  
				return 2;
			}
		}
		else if ( option == 2 )
		{
			int result = login();
			
			if (result == 2)
			{
				printf("Fatal error : Something went wrong!");	
				return 2;
			}
		}
		else if ( option == 3 )
		{
			return 0;
		}
		else 
		{
			printf("\nInvalid option!\n");
		}
	}
	return 0;
}

//=============================================== REGISTER =========================================================

int registr (void)
{
	char re_password[PASSWORD_BUFFER_SIZE];
	
	while(1)	// Loop for username input
	{
		while(1)
		{
			printf("\nCreate new username : ");
			fgets(reg.username, sizeof(reg.username), stdin );
			reg.username[strcspn(reg.username, "\n")] = 0; 
	
			for ( int i = 0; reg.username[i] != '\0'; i++ )
			{
		                 if ( reg.username[i] >= 'A' && reg.username[i] <= 'Z' )
				 {
					 reg.username[i] += 32;
				 }
			}
	
			int result = analyse_username();
			if ( result == 1 )
			{
				continue;
			}
			else 
			{
				break;
			}
		}


		int result = usercheck();
		
		if ( result == 0 )
		{
			while(1)	// Loop for password input
			{
				while(1)
				{
					printf("\nSet password for %s\n", reg.username);
					printf("Password : ");
					fgets( reg.password, sizeof(reg.password), stdin);
					reg.password[strcspn(reg.password, "\n")] = 0; 

					int result = analyse_password();
					
					if( result == 0 )
					{
						break;
					}
					else
					{
						continue;
					}
				}

				printf("Renter the Password : ");
				fgets( re_password, sizeof(re_password), stdin);
				re_password[strcspn(re_password, "\n")] = 0;
			
				if (strcmp( reg.password, re_password ) == 0)
				{
					break;
				}
				else
				{
					printf("\nPasswords didn't match\n");
					continue;
				}
			}
				
			FILE *file = fopen( "data/credentials.strux", "a" );

			if( file == NULL)
			{
				return 2;
			}

			fprintf(file, "%s %s\n", reg.username, reg.password ); 
			fclose(file);
			printf("\nYou have registered successfully!\nLogin to continue.\n\n");
			return 0;
		}
		else if ( result == 1 )
		{
			continue;
		}
		else 
		{
			return 2;
		}	
	}
}

//=============================================== USERCHECK ========================================================

int usercheck(void)
{
	char scanbuffer[USERNAME_BUFFER_SIZE];

	FILE *file = fopen( "data/credentials.strux", "r" );

	if ( file == NULL )
	{
		if ( errno == ENOENT )
		{
			// No credentials file exists yet, so the username is available.
			return 0;
		}
		return 2;
	}

	while ( fscanf( file, "%s%*[^\n]", scanbuffer ) != EOF )
	{
		if ( strcmp( scanbuffer, reg.username ) == 0 )
		{
			printf("\nUsername is not available\n");
			return 1;
		}
	}

	fclose(file);
	return 0;
}

//=================================================== LOGIN ========================================================

int login(void)
{
	char username[USERNAME_BUFFER_SIZE], password[PASSWORD_BUFFER_SIZE];
		
	while(1)
	{
		printf("\nLogin\n");
		printf("\nUsername : ");
		fgets(username, sizeof(username), stdin);
		username[strcspn(username, "\n")] = 0;
		
		printf("Password : ");
		fgets(password, sizeof(password), stdin);
		password[strcspn(password, "\n")] = 0;

	         for ( int i = 0; username[i] != '\0'; i++ )
	         {
	                 if ( username[i] >= 'A' && username[i] <= 'Z' )
	                 {
	                         username[i] += 32;
	                 }
	         }
	
		FILE *file = fopen( "data/credentials.strux", "r" );

	if ( file == NULL ) 
	{
		if ( errno == ENOENT )
		{
			printf("\nNo registered users found. Please register first.\n");
			continue;
		}
		return 2;
	}

	
		while ( fscanf( file, "%s %s", reg.username, reg.password) != EOF)
		{
			if ( strcmp( username, reg.username ) == 0 && strcmp( password, reg.password ) == 0 )
			{
				printf( "\nLogin Successful.\n" );
				fclose(file);

				dbms(reg.username);
				return 0;
			}
		}

		printf("\nInvalid username or password!\n");
		continue;
	}
}

//=============================================== ANALYSE PASSWORD =================================================

int analyse_password(void)
{
	int flag = 0, count = 0, length = 0, symbol = 0, number = 0, uppercase = 0, lowercase = 0, whitespace = 0, invalid = 0;
	for ( count = 0; reg.password[count] != '\0'; count++)
	{
		length++;
		
		if ( ( reg.password[count] >= '!' && reg.password[count] <= '/' ) || ( reg.password[count] >= ':' && reg.password[count] <= '@' ) || ( reg.password[count] >= '[' && reg.password[count] <= '`' ) || ( reg.password[count] >= '{' && reg.password[count] <= '~' ) )
		{
			symbol++;
		}

		else if ( reg.password[count] >= '0' && reg.password[count] <= '9' )
		{
			number++;
		}
		
		else if ( reg.password[count] >= 'A' && reg.password[count] <= 'Z' )
		{
			uppercase++;
		}

		else if ( reg.password[count] >= 'a' && reg.password[count] <= 'z' )
		{
			lowercase++;
		}
		else if ( reg.password[count] == ' ' )
		{
			whitespace++;
		}
		else
		{
			invalid++;
		}
	}
	
	if ( count < MIN_PASSWORD_LEN )
	{
		printf("Password should atleast contain 8 characters\n");
		flag = 1;
	}
	
	if ( count > MAX_PASSWORD_LEN )
	{
		printf("Password should does not exceed 128 characters\n");
		flag = 1;
	}

	if ( symbol == 0 )
	{
		printf("Password should atleast contain 1 symbol\n");
		flag = 1;
	}

	if ( number == 0 )
	{
		printf("Password should atleast contain 1 number\n"); 
		flag = 1;
	}
	
	if ( uppercase == 0 )
	{
		printf("Password should atleast contain 1 uppercase\n");
		flag = 1;
	}
	
	if ( lowercase == 0 )
	{
		printf("Password should atleast contain 1 lowercase\n");
		flag = 1;
	}
	if ( strcmp( reg.username, reg.password ) == 0 ) 
	{
		printf("Password and username cannot be same\n");
		flag = 1; 
	}
	if ( whitespace > 0 )
	{
		printf("Password should never contains whitespace.\n");
		flag = 1;
	}
	if ( invalid > 0 )
	{
		printf("Password contains something that is invalid.\n");
		flag = 1;
	}

	if ( flag == 1 )
	{
		return 1;
	}
	else
	{
		return 0;
	}


}

//================================================ ANALYSE USERNAME ================================================

int analyse_username(void)
{
	int number = 0, flag = 0, length = 0, count = 0;
	for( count = 0; reg.username[count] != '\0' ; count++ )
	{
		length++;
		if ( reg.username[count] >= '0' && reg.username[count] <= '9' )
		{
			number++;
		}

		if ( ( reg.username[count] >= '0' && reg.username[count] <= '9' ) || ( reg.username[count] >= 'A' && reg.username[count] <= 'Z' ) || ( reg.username[count] >= 'a' && reg.username[count] <= 'z' ) || reg.username[count] == '_' || reg.username[count] == '.' )
		{
			
			continue;
		}
		else
		{
			flag++;
		}
	}

	if ( flag != 0 )
	{
		printf("Username can only contains letters, numbers , underscores(_) and periods(.)");
	}

	if ( count > MAX_USERNAME_LEN )
	{
		printf("Username should never exceed 32 characters.\n");
		flag++;
	}
	if ( count < MIN_USERNAME_LEN )
	{
		printf("Username should atleast contain 3 characters.\n");
		flag++;
	}

	if ((( reg.username[0] >= '0' && reg.username[0] <= '9' ) || reg.username[0] == '_' ))
	{
		printf("Username cannot begin with  numbers or underscore\n");
		flag++; 

	}
	if ( length == number )
	{
		printf("Username cannot be numbers only\n");
		flag++;
	}

	if( flag != 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//================================================== THE END =======================================================
