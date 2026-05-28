#include "header.h"

int add(void)
{
	struct people person;

	printf("\nWhat is the Name? : ");
	scanf("%s", person.name);

	printf("\nWhat's the age? : ");
	scanf("%i", &person.age);
	
	FILE *data = fopen("database.txt", "a");
	
	fprintf(data, "%s ", person.name);
	fprintf(data, "%i\n", person.age);

	fclose(data);
	return 0;
}
