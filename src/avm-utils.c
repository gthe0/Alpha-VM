#include <dispatcher.h>
#include <avm-reader.h>
#include <avm-mem.h>
#include <avm-log.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Definition of the array in avm-utils.h... */
char* typeString[] = {
	"number",
	"string",
	"bool",
	"table",
	"userfunc",
	"libfunc",
	"nil",
	"undef"
};

/*
 Typedefinition of functions that 
 turn a memcell to a string
*/
typedef char* (*to_string_Funcs)(avm_memcell*);

#define MAX_DOUBLE_LENGTH 25

/* I do not care about the 80 pixel ruler and I do not care
 if you have to scroll horizontally to read it. This way, it is more beautiful.
 If you also dislike the tab size and that I use tabs instead of spaces, confort to my style 
 and use tabs with 4 spaces of length. I do not care about you reading it or about your opinion to my coding style that is constantly changing. Deal with it... */
static char* string_to_string(avm_memcell* m) 		{ assert(m && m->type == string_m); return strdup(m->data.strVal);}
static char* bool_to_string(avm_memcell* m)	 		{ assert(m && m->type == bool_m); return strdup( m->data.boolVal == 1 ? "true" : "false");}
static char* libfunc_to_string(avm_memcell* m) 		{ assert(m && m->type == libfunc_m); return strdup(get_libFuncs(m->data.libfuncVal)); }
static char* nil_to_string(avm_memcell* m)	 		{ assert(m && m->type == nil_m); return strdup("nil");} 
static char* undef_to_string(avm_memcell* m) 		{ return strdup("undef");} 

/* We will print the userfunc name and address here */
static char* userfunc_to_string(avm_memcell* m) 
{
	assert(m && m->type == userfunc_m);

	return strdup(get_UserFunc(m->data.funcVal).id);
} 

/* We ll print the whole table */
static char* table_to_string(avm_memcell* m)
{
	assert(m && m->type == table_m);

	int total = 0,
		curr = 0,
		final_str_len = 3; /*Because it contains surely []\0*/

	avm_table* table = m->data.tableVal;

	if (!(total = table->total))
		return strdup("[]");	
	
	/* If the table is not empty, 
	make a string for each of its elements 
	and store them in an array of strings...
	We are not compiling for c99, so we can have
	dynamically sized arrays, but for the shake of
	backwards compatibility, we will allocate the memory...
	*/
	char** table_str = malloc(sizeof(char*)*total);
	char* string = NULL;

	/*
	 We initialize the array with NULLs
	 Get rid of the garbage...
	*/
	for (int i = 0; i < total ; i++)
	{
		table_str[i] = NULL;
	}

	/*
	 for each type of bucket, make a string and return it.
	 If the string returned in not NULL, then add its length
	 to the final string length
	*/
	for (int i = 0; i < 2 ; i++)
	{
		string = avm_bucket_tostring(table,table->boolIndexed[i]);

		if(string)
		{
			table_str[curr++] = string ;		
			final_str_len += strlen(string);
		}
	}

	for (int i = 0; i < AVM_TABLE_HASH_SIZE ; i++)
	{
		string = avm_bucket_tostring(table,table->numIndexed[i]);

		if(string)
		{
			table_str[curr++] = string ;		
			final_str_len += strlen(string);
		}
	}

	for (int i = 0; i < AVM_TABLE_HASH_SIZE ; i++)
	{
		string = avm_bucket_tostring(table,table->strIndexed[i]);

		if(string)
		{
			table_str[curr++] = string ;		
			final_str_len += strlen(string);
		}
	}

	for (int i = 0; i < AVM_TABLE_HASH_SIZE ; i++)
	{
		string = avm_bucket_tostring(table,table->userIndexed[i]);

		if(string)
		{
			table_str[curr++] = string ;		
			final_str_len += strlen(string);
		}		
	}

	for (int i = 0; i < AVM_TABLE_HASH_SIZE ; i++)
	{
		string = avm_bucket_tostring(table,table->libIndexed[i]);

		if(string)
		{
			table_str[curr++] = string ;		
			final_str_len += strlen(string);
		}
	}

	/* Now we just need to concantinate the strings... */
	string = malloc(final_str_len*sizeof(char)+1);
	assert(string);


	for (int i = 0; i < final_str_len+1; i++)
	{
		string[i] = '\0'; 
	}

	/*Start of the array */
	*string = '[';

	/*We ll use a strcat */
	for (int i = 0; i < total; i++)
	{
		strcat(string,table_str[i]);

		/* After concantinating them, free them*/
		free(table_str[i]);
	}
	
	/* Delete last comma */
	string[strlen(string) - 1] = '\0'; 
	strcat(string,"]");

	free(table_str);

	return string;
}

/*
 It turns a double into a string basically. 
 The code is ugly yet functional so I won't refactor it
 probably... Nobody is ever going to read it anyways, so who cares ?
*/
static char* number_to_string(avm_memcell* m) 
{ 
	assert(m && m->type == number_m);

	double num = m->data.numVal;
	char string_num[MAX_DOUBLE_LENGTH];

	int is_decimal = 0;
	char* head = string_num;


	sprintf(string_num, "%lf", num);
	
	/* If it is a float remove trailing decimals */
	while(*head != '\0')
		if(*head++ == '.')
			is_decimal = 1;
	
	/* Go inside the string */
	head-- ;

	/* Do not overextend */
	while (is_decimal && *head == '0' && head != string_num)
		head-- ;

	if(is_decimal)
		if(*head == '.')
			*head = '\0';
		else if(*head != '0' && head != string_num)
			*++head = '\0';

	/* Allocate the memory needed and return the string */	
	char *generated_string = malloc((strlen(string_num)) * sizeof(char) + 1);
	assert(generated_string);

	strcpy(generated_string, string_num);

	return generated_string;
}

static to_string_Funcs Stringify_cell[] = {
	number_to_string,
	string_to_string,
	bool_to_string,
	table_to_string,
	userfunc_to_string,
	libfunc_to_string,
	nil_to_string,
	undef_to_string
};

char* avm_to_string(avm_memcell* m)
{
	assert(m && m->type >= 0 && m->type <= undef_m );
	
	return	(*Stringify_cell[m->type])(m);
}

/* Function used to check 
if an input is a number to call atof
*/
int is_num(char* input)
{
	assert(input);
	
	int str_size = strlen(input);

	/*If the string is "" then return*/
	if(!str_size)	return 0;

	int has_dot = 0;
	int is_neg = 0;

	is_neg = (*input == '-');

	if(is_neg)
	{
		/*If the string starts with '-'
		and its size is 1 then return... */	
		if(str_size == 1)	return 0;

		/*...else increment pointer by 1, 
		to check the rest of the nums*/
		input++;
	}

	char c  = 0;

	while ((c = *input) != '\0')
	{
		if(c < '0' || c > '9' || (c =='.' && !has_dot++))
			return 0;

		input++;		
	}

	/* If we did not return thus far, 
	then the input is a number, return 1*/
	return 1;
}