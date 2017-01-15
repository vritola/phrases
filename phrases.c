#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

//Struct to store a phrase, it's associations and the amount of associations. 
//If there was any other use to associations than printing, they should be stored as an array of pointers instead.
struct Item
{
	char phrase[50];
	char associations[10000];
	int amount;
};

struct Item* hashTable[9999999];

//Hash function based on djb2.
unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
    {
        hash = ((hash << 5) + hash) + c;
	}

	hash = hash % 9999889;
    return hash;
}

//Compare function that used in sorting an array of pointers to structs, based on the amount of associations within the structs.
int compare (const void *a, const void *b)
{
	struct Item *itemA = *(struct Item**)a;
	struct Item *itemB = *(struct Item**)b;
	
    return (itemB->amount - itemA->amount);
}

int main (void)
{   
    FILE *fptr;
    
	char fphrase[50]; //Buffer for reading from the file.
	char firstphrase[50];
	char association[50];
	unsigned long index;
	
	char filename[20];
	int inputchoice;
	
	//Read preferences from the user.
	//In a real-world application, user input should be validated for errors.
	printf("Enter the filename >");
	scanf("%s", &filename);
	printf("Enter 1 if you want to print the associated phrases of the most common phrase as well. Othwerwise enter 0 >");
	scanf("%d", &inputchoice);
	
	printf("\nReading file...\n");
    	
    clock_t t1 = clock(); //Start the clock to measure full execution time.
		
    if ((fptr = fopen(filename, "r")) == NULL)
    {
       printf("File opening failed!\n");
       return(0);
    }

	int i = 0;		
	clock_t t2 = clock(); //Start the clock to measure file reading and hashing time.
	
	//Read file line by line
	while (fgets(fphrase, "%s", fptr) != NULL)
	{
        //convert to uppercase
		for(i = 0; fphrase[i]; i++)
		{
  			fphrase[i] = toupper(fphrase[i]);
		}
		
		sscanf(fphrase, "%[^:]%*[:]%[^:\n]", firstphrase, association); //Split the line into two variables, divided by ":"
		strcat(association, ", "); //Add a comma to separate later associations.
		
		index = hash (firstphrase); //Create an index number by hashing the phrase
		
		//Check if the element is already initialized
		if (hashTable[index] != NULL)
		{
			struct Item *cmpitem = hashTable[index];
			char cmpphrase[50];
			strcpy(cmpphrase, cmpitem->phrase);
			
			//If the phrase is the same, just add an association to the existing item.
			if(strcmp(cmpphrase, firstphrase) == 0)
			{
				cmpitem->amount++;
				strcat(cmpitem->associations, association);
			}
			else //Otherwise there must be a collision, try to find an empty element by probing and create a new item there
			{
				while(hashTable[index] != NULL)
				{
					index++;
					index %= 9999999;
				}
				struct Item *newitem = (struct Item*) malloc (sizeof(struct Item));
		        strcpy(newitem->phrase, firstphrase);
				newitem->amount = 1;
				hashTable[index] = newitem;
			}
		}
		else //Create a new item
		{
			struct Item *item = (struct Item*) malloc (sizeof(struct Item));
			strcpy(item->phrase, firstphrase);
			item->amount = 1;
			hashTable[index] = item;
		}
	}
	
	//Hashing complete, print the time
	t2 = clock() - t2;
    double hashtime = ((double)t2)/CLOCKS_PER_SEC;
    printf("All phrases read and hashed in %f seconds\n", hashtime);
	
	//Create a new, smaller table that is exactly the size of unique phrases
	int j = 0;
	int k = 0;
	for(j=0;j<9999999;j++)
	{
		if(hashTable[j] != NULL)
		{
			k++;
		}
	}
	struct Item* newtable[k];
    int l = 0;	
	for(j=0;j<9999999;j++)
	{
		if(hashTable[j] != NULL)
		{
			newtable[l] = hashTable[j];
			l++;
		}
	}
	
	printf("\nSorting...\n");
	clock_t t3 = clock(); //Measure sorting time
	qsort (newtable, k, sizeof(struct Item*), compare); //Sort the array using quicksort
    t3 = clock() - t3;
    double sorttime = ((double)t3)/CLOCKS_PER_SEC;
	printf("Array sorted in %f seconds\n\n", sorttime);
	
	//Print the top 100 phrases, and if the user wanted, the top phrases' associations.
	int m = 0;
	for(m=0;m<100;m++)
	{
        printf("%d: %s %d\n", m + 1, newtable[m]->phrase, newtable[m]->amount);
    }
    if(inputchoice != 0)
    {
        printf("\nThe associated phrases of %s were %s\n", newtable[0]->phrase, newtable[0]->associations);
    }
    
    //Calculate and print the total execution time
    t1 = clock() - t1;
    double fulltime = ((double)t1)/CLOCKS_PER_SEC;
    printf("\nThe whole program took %f seconds to execute\n", fulltime);
    
	system("pause");
    return 0;
}
