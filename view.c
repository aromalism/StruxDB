#include "header.h"
#include <string.h>

int view(void)
{
	struct people person;
	char name[20];
	printf("whose name do you wanted to find? : ");
	scanf("%s", name);

	FILE *data = fopen( "database.txt", "r" );

	while ( fscanf( data, "%s %i", person.name, &person.age ) != EOF )
	{
		if ( strcmp(name,person.name) == 0 )
		{
			printf("\nThe data is : %i", person.age);
			return 0;
		}
	}

	printf("\nData not Found!\n");
	return 0;
}
