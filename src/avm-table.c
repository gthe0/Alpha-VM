/*
 	Authors: csd4881, csd4988, csd5038														
	
	avm-table.c										

	Here we implement the avm-table related functions 
*/

#include <avm-mem.h>
#include <avm-log.h>
#include <avm-utils.h>
#include <avm-reader.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define HASH_MULTIPLIER 65599
#define NO_NEW_MEM 0
#define ADDED_MEM 1

/* typedefinitions of setters and getters */
typedef avm_memcell* (*table_getter_t)(avm_table*,avm_memcell*);
typedef int (*table_setter_t)(avm_table*,avm_memcell*,avm_memcell*);


/* Number hash, used for number and userfunc indexes*/
static unsigned number_hash(unsigned num)
{
	return (num % AVM_TABLE_HASH_SIZE);
}

/* String hash, used for libfuncs and string*/
static unsigned string_hash(char* name)
{
    unsigned int ui;
    unsigned int uiHash = 0U;

    for (ui = 0U; name[ui] != '\0'; ui++)
        uiHash = uiHash * HASH_MULTIPLIER + name[ui];

    return (uiHash % AVM_TABLE_HASH_SIZE);
}

/* Used to get an element from the boolean bucket */
static avm_memcell* bool_bucket_get(
	avm_table* table,
	avm_memcell* index
)
{
	assert(index->type == bool_m);	
	avm_table_bucket* boolIndexed = table->boolIndexed[index->data.boolVal];

	/*
	 Boolean elements are allocated 
	 only in the two first buckets,
	 so we search for them there
	*/
	if(boolIndexed)	return &boolIndexed->value;
	else
		avm_log(ERROR,"The element at index %s,"
					" does not exist!\n",avm_to_string(index));

	return NULL;
}


/* Used to get an element from the number bucket */
static avm_memcell* number_bucket_get(
	avm_table* table,
	avm_memcell* index
)
{
	assert(index->type == number_m);	

	int hash = number_hash(index->data.numVal);
	avm_table_bucket* numIndexed = table->numIndexed[hash];

	/* Numbers may be in a list.
	*/
	while (
		numIndexed &&
		numIndexed->key.data.numVal != index->data.numVal)
		numIndexed = numIndexed->next;

	if (!numIndexed)
		avm_log(ERROR,"The element at index %s,"
				" does not exist!\n",avm_to_string(index));

	return &numIndexed->value;
}


/* Used to get an element from the string buckets */
static avm_memcell* str_bucket_get(
	avm_table* table,
	avm_memcell* index
)
{
	assert(index->type == string_m);	

	int hash = string_hash(index->data.strVal);
	avm_table_bucket* strIndexed = table->strIndexed[hash];

	/* Strings may be in a list.
	*/
	while (
		strIndexed &&
		strcmp(strIndexed->key.data.strVal,index->data.strVal))
		strIndexed = strIndexed->next;

	if (!strIndexed)
		avm_log(ERROR,"The element at index %s,"
				" does not exist!\n",avm_to_string(index));

	return &strIndexed->value;
}


/* Used to get an element from the libfuncs buckets */
static avm_memcell* lib_bucket_get(
	avm_table* table,
	avm_memcell* index
)
{
	assert(index->type == libfunc_m);	

	int hash = string_hash(get_libFuncs(index->data.libfuncVal));
	avm_table_bucket* libIndexed = table->libIndexed[hash];

	/* library funcs may be in a list.
	*/
	while (
		libIndexed &&
		libIndexed->key.data.libfuncVal == index->data.libfuncVal)
		libIndexed = libIndexed->next;

	if (!libIndexed)
		avm_log(ERROR,"The element at index %s,"
				" does not exist!\n",avm_to_string(index));

	return &libIndexed->value;
}

/* Used to get an element from the userfuncs buckets */
static avm_memcell* userfunc_bucket_get(
	avm_table* table,
	avm_memcell* index
)
{
	assert(index->type == userfunc_m);	

	int hash = number_hash(index->data.funcVal);
	avm_table_bucket* userIndexed = table->userIndexed[hash];

	/* library funcs may be in a list.
	*/
	while (
		userIndexed &&
		userIndexed->key.data.funcVal == index->data.funcVal)
		userIndexed = userIndexed->next;

	if (!userIndexed)
		avm_log(ERROR,"The element at index %s,"
				" does not exist!\n",avm_to_string(index));

	return &userIndexed->value;
}

/* Array of getter functions */
static table_getter_t table_get[] = {
	number_bucket_get,
	str_bucket_get,
	bool_bucket_get,
	NULL, /* table */
	userfunc_bucket_get,
	lib_bucket_get,
	NULL, /* nil */
	NULL, /* undef*/
};


/* Used to set an element in the boolean bucket */
static int bool_bucket_set(
	avm_table* table,
	avm_memcell* index,
	avm_memcell* content
)
{
	assert(index->type == bool_m);	
	avm_table_bucket* boolIndexed = table->boolIndexed[index->data.boolVal];

	/*
	 Boolean elements are allocated 
	 only in the two first buckets,
	 so we search for them there
	*/
	if(boolIndexed)
	{
		boolIndexed->value = *content;

		if(content->type == string_m)
			boolIndexed->value.data.strVal = strdup(content->data.strVal);

		return NO_NEW_MEM;
	}
	else
	{
		table->boolIndexed[index->data.boolVal] = malloc(sizeof(avm_table_bucket));
		table->boolIndexed[index->data.boolVal]->key = *index;
		table->boolIndexed[index->data.boolVal]->value = *content;
		table->boolIndexed[index->data.boolVal]->next = NULL;

		if (content->type == string_m)
			table->boolIndexed[index->data.boolVal]->value.data.strVal = strdup(content->data.strVal);	

		return ADDED_MEM;
	}	

	return NO_NEW_MEM;
}


/* Used to set an element in the number bucket */
static int number_bucket_set(
	avm_table* table,
	avm_memcell* index,
	avm_memcell* content
)
{
	assert(index->type == number_m);

	int hash = number_hash(index->data.numVal);
	avm_table_bucket	*numIndexed = table->numIndexed[hash];
	avm_table_bucket	*node = NULL;

	if(!numIndexed)
	{
		table->numIndexed[hash] = malloc(sizeof(avm_table_bucket));
		table->numIndexed[hash]->key = *index;
		table->numIndexed[hash]->value = *content;
		table->numIndexed[hash]->next = NULL;

		if (content->type == string_m)
			table->numIndexed[hash]->value.data.strVal = strdup(content->data.strVal);	

		return ADDED_MEM;
	}

	/* Numbers may be in a list.
	*/
	while (
		numIndexed &&
		numIndexed->key.data.numVal != index->data.numVal &&
		numIndexed->next)
		numIndexed = numIndexed->next;

	/* If we reach the end, do not execute this block*/
	if (numIndexed->key.data.numVal != index->data.numVal)
	{
		numIndexed->value = *content;

		if(content->type == string_m)
			numIndexed->value.data.strVal = strdup(content->data.strVal);
		
		return NO_NEW_MEM;
	}

	node = malloc(sizeof(avm_table_bucket));	
	node->next = NULL;
	node->key = *index;
	node->value = *content;

	if (content->type == string_m)
		node->value.data.strVal = strdup(content->data.strVal);	

	numIndexed->next = node;

	return ADDED_MEM;
}


/* Used to get an element from the string buckets */
static int str_bucket_set(
	avm_table* table,
	avm_memcell* index,
	avm_memcell* content
)
{
	assert(index->type == string_m);	

	int hash = string_hash(index->data.strVal);
	avm_table_bucket* strIndexed = table->strIndexed[hash];
	avm_table_bucket	*node = NULL;

	if(!strIndexed)
	{
		table->strIndexed[hash] = malloc(sizeof(avm_table_bucket));
		table->strIndexed[hash]->key = *index;
		table->strIndexed[hash]->key.data.strVal = strdup(index->data.strVal);
		table->strIndexed[hash]->value = *content;
		table->strIndexed[hash]->next = NULL;

		if (content->type == string_m)
			table->strIndexed[hash]->value.data.strVal = strdup(content->data.strVal);	

		return ADDED_MEM;
	}
	/* Numbers may be in a list.
	*/
	while (
		strIndexed &&
		strcmp(strIndexed->key.data.strVal,index->data.strVal)&&
		strIndexed->next)
		strIndexed = strIndexed->next;


	/* If we reach the end, do not execute this block*/
	if (!strcmp(strIndexed->key.data.strVal,index->data.strVal))
	{
		strIndexed->value = *content;

		if(content->type == string_m)
			strIndexed->value.data.strVal = strdup(content->data.strVal);
		
		return NO_NEW_MEM;
	}

	node = malloc(sizeof(avm_table_bucket));	
	node->next = NULL;
	node->key = *index;
	node->key.data.strVal = strdup(index->data.strVal);
	node->value = *content;

	if (content->type == string_m)
		node->value.data.strVal = strdup(content->data.strVal);	

	strIndexed->next = node;

	return ADDED_MEM;
	
}


/* Used to get an element from the libfuncs buckets */
static int lib_bucket_set(
	avm_table* table,
	avm_memcell* index,
	avm_memcell* content
)
{
	assert(index->type == libfunc_m);	

	int hash = string_hash(get_libFuncs(index->data.libfuncVal));
	avm_table_bucket* libIndexed = table->libIndexed[hash];
	avm_table_bucket	*node = NULL;

	if(!libIndexed)
	{
		table->libIndexed[hash] = malloc(sizeof(avm_table_bucket));
		table->libIndexed[hash]->key = *index;
		table->libIndexed[hash]->value = *content;
		table->libIndexed[hash]->next = NULL;

		if (content->type == string_m)
			table->libIndexed[hash]->value.data.strVal = strdup(content->data.strVal);	

		 ADDED_MEM;
	}

	/* Numbers may be in a list.
	*/
	while (
		libIndexed &&
		libIndexed->key.data.libfuncVal != index->data.libfuncVal &&
		libIndexed->next)
		libIndexed = libIndexed->next;

	/* If we reach the end, do not execute this block*/
	if (libIndexed->key.data.libfuncVal != index->data.libfuncVal)
	{
		libIndexed->value = *content;

		if(content->type == string_m)
			libIndexed->value.data.strVal = strdup(content->data.strVal);
		
		return NO_NEW_MEM;
	}

	node = malloc(sizeof(avm_table_bucket));	
	node->next = NULL;
	node->key = *index;
	node->value = *content;

	if (content->type == string_m)
		node->value.data.strVal = strdup(content->data.strVal);	

	libIndexed->next = node;

	return ADDED_MEM;	

}

/* Used to set an element from the userfuncs buckets */
static int userfunc_bucket_set(
	avm_table* table,
	avm_memcell* index,
	avm_memcell* content
)
{
	assert(index->type == userfunc_m);	

	int hash = number_hash(index->data.funcVal);
	avm_table_bucket* userIndexed = table->userIndexed[hash];
	avm_table_bucket	*node = NULL;

	if(!userIndexed)
	{
		table->userIndexed[hash] = malloc(sizeof(avm_table_bucket));
		table->userIndexed[hash]->key = *index;
		table->userIndexed[hash]->value = *content;
		table->userIndexed[hash]->next= NULL;

		if (content->type == string_m)
			table->userIndexed[hash]->value.data.strVal = strdup(content->data.strVal);	

		return ADDED_MEM;
	}

	/* Numbers may be in a list.
	*/
	while (
		userIndexed &&
		userIndexed->key.data.funcVal != index->data.funcVal &&
		userIndexed->next)
		userIndexed = userIndexed->next;

	/* If we reach the end, do not execute this block*/
	if (userIndexed->key.data.funcVal != index->data.funcVal)
	{
		userIndexed->value = *content;

		if(content->type == string_m)
			userIndexed->value.data.strVal = strdup(content->data.strVal);
		
		return NO_NEW_MEM;
	}

	node = malloc(sizeof(avm_table_bucket));	
	node->next = NULL;
	node->key = *index;
	node->value = *content;

	if (content->type == string_m)
		node->value.data.strVal = strdup(content->data.strVal);	

	userIndexed->next = node;

	return ADDED_MEM;	
}


/* Array of getter functions */
static table_setter_t table_set[] = {
	number_bucket_set,
	str_bucket_set,
	bool_bucket_set,
	NULL, /* table */
	userfunc_bucket_set,
	lib_bucket_set,
	NULL, /* nil */
	NULL, /* undef*/
};


/* avm table bucket getter */
avm_memcell* avm_tablegetelem (
	avm_table* table,
	avm_memcell* index)
{
	assert(table && index);
	table_getter_t f = table_get[index->type];

	if(f) return (*f)(table,index);
	else 
		avm_log(ERROR,"%s cannot be used for indexing",typeString[index->type]);

	return NULL;
}

/* avm table bucket setter*/
void avm_tablesetelem (
	avm_table* table,
	avm_memcell* index,
	avm_memcell* content)
{
	assert(table && index && content);

	int t = 0;
	table_setter_t f = table_set[index->type];

	if(content->type == table_m)
	{
		/* I define that it is illegal to recurcively 
		set the table itself as its elements, is illegal */
		if (content->data.tableVal == table)
		{
			avm_log(ERROR,"Setting the table itself as an element"
						" of the table is illegal\n");

			return ;
		}
		avm_table_inc_refcounter(content->data.tableVal);
	}

	if(f) t = (*f)(table,index,content);
	else 
		avm_log(ERROR,"%s cannot be used for indexing",typeString[index->type]);

	/*
	 If t is zero, that means that 
	 we added an element to the table
	*/
	if(t)
		table->total++ ;

	return ;
}