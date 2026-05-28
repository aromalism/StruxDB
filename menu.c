#include "header.h"

void menu(void)
{
	int option = 0;

	while(1)
	{
		printf("\n-----\n|Menu|\n-----\n1.Add new data.\n2.view existing data.\n3.Edit existing data.\n4.Exit.\n\nOption: ");

		scanf(" %i", &option);
	
		if ( option == 1 )
		{
			add();
		}
		else if ( option == 2 )
		{
			view();
		}
		else if ( option == 3 )
		{
			edit();
		}
		else if ( option == 4 )
		{
			break;
		}
		else 
		{
			printf("Invaild input!\n");
			while (getchar() != '\n');
		}

	}


}
