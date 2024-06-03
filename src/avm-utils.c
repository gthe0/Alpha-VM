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
static char* undef_to_string(avm_memcell* m) 		{ assert(0); return NULL;} 

/* We will print the userfunc name and address here */
static char* userfunc_to_string(avm_memcell* m) 
{
	assert(m && m->type == userfunc_m);

	return strdup(get_UserFunc(m->data.funcVal).id);
} 

/*
 We won't print the whole damn array for an error message.
 Just the name of the table shall suffice 
*/
static char* table_to_string(avm_memcell* m)
{
	assert(m && m->type == table_m);

	return strdup("_table_");
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