/*
 	Authors: csd4881, csd4988, csd5038														
	
	avm-mem.c										

	Here we implement the avm-table ADT
*/

#include <avm-mem.h>
#include <avm-log.h>
#include <avm-utils.h>
#include <avm-reader.h>
#include <dispatcher.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* Stack module */
avm_memcell stack[AVM_STACKSIZE];
avm_memcell ax, bx, cx, retval;

unsigned top, topsp, totalActuals = 0;

typedef void (*memclear_func_t)(avm_memcell*);

/**
* @brief Called by avm_mem_cell_clear to free the string data type
* @param m The memcell to be cleared
*/
static void memclear_string(avm_memcell* m)
{
	assert(m->data.strVal);
	free(m->data.strVal);
}

/**
* @brief Called by avm_mem_cell_clear to free the table data type
* @param m The memcell to be cleared
*/
static void memclear_table(avm_memcell* m)
{
	assert(m->data.tableVal);
	avm_table_dec_refcounter(m->data.tableVal);
}

/*
 Function table used to call the correct memclear
 function based on the memcell type
*/
memclear_func_t memclearFuncs[] = {
	0,		/* number*/
	memclear_string,
	0,		/* bool */
	memclear_table,
	0,		/* userfunc */
	0,		/* libfunc */
	0,		/* nil */
	0,		/* undef */
};

/* Initializes the stack */
void avm_initstack(void)
{
	for (unsigned i = 0; i < AVM_STACKSIZE ; i++)
	{	
		AVM_WIPEOUT(stack[i]);
		stack[i].type = undef_m;
	}

	return;
}

/* Initializes all the structs of the vm */
void avm_initialize (void)
{
	avm_initstack();
	top = AVM_STACKSIZE - programVarOffset -1;
	topsp = 0;
	ax.type = bx.type = cx.type = retval.type = undef_m;

	return ;
}

void avm_table_inc_refcounter(avm_table *table) {
    ++table->refCounter;
}

void avm_table_dec_refcounter(avm_table *table) {
	assert(table->refCounter > 0);
    
	/* Activate garbage collection... */
	if (!--table->refCounter) 
		avm_table_destroy(table); 
}

void avm_table_buckets_init(avm_table_bucket** p)
{
	/* Same as assigningn NULL to each bucket */
	for (unsigned i = 0; i < AVM_TABLE_HASH_SIZE; i++)
		p[i] = (avm_table_bucket*)0; 
}

/* Creates a new avm_table */
avm_table* avm_table_new(void)
{
    avm_table *table = malloc(sizeof(avm_table));
    AVM_WIPEOUT(*table);

    table->refCounter = table->total = 0;

    avm_table_buckets_init(table->numIndexed);
    avm_table_buckets_init(table->strIndexed);
    avm_table_buckets_init(table->boolIndexed);
    avm_table_buckets_init(table->userIndexed);
    avm_table_buckets_init(table->libIndexed);

    return table;
}

/* Clears the memory cells */
void avm_mem_cell_clear(avm_memcell* m)
{
	assert(m);

	if(m->type == undef_m) return;

	memclear_func_t f = memclearFuncs[m->type];

	/* Free the correct data based on its type */
	if(f)
		(*f)(m);
	
	m->type = undef_m;
}

/* Destructor of the avm table buckets */
void avm_table_buckets_destroy(avm_table_bucket** p)
{
	for (unsigned i = 0; i < AVM_TABLE_HASH_SIZE ; i++, p++)
	{
		for (avm_table_bucket* b = *p; b;)
		{
			avm_table_bucket* del = b;
			b = b->next;

			/* Free the cells before freeing the bucket */
			avm_mem_cell_clear(&del->key);
			avm_mem_cell_clear(&del->value);

			free(del);
		}

		*p = NULL ;
	}
}

/* Destructor of the avm table buckets */
void avm_table_destroy (avm_table* t)
{
	avm_table_buckets_destroy(t->numIndexed);
	avm_table_buckets_destroy(t->strIndexed);
	avm_table_buckets_destroy(t->boolIndexed);
	avm_table_buckets_destroy(t->userIndexed);
	avm_table_buckets_destroy(t->libIndexed);

	free(t);
}

void avm_assign(avm_memcell* lv, avm_memcell* rv)
{
	/* No need for assignment here, they are the same*/
	if	(lv == rv)	
		return ;

	/* If they are references to the same table,
	 there is no need for assignment
	*/
	if	(lv->type == table_m &&
		 rv->type == table_m &&
		 lv->data.tableVal == rv->data.tableVal)	
		return;
	
	/* Clear the old cell to replace it with the rv*/
	avm_mem_cell_clear(lv);		
	memcpy(lv,rv,sizeof(avm_memcell));

	/* Now we deep copy the data to make sure it does not get deleted */
	if	(lv->type == string_m)
		lv->data.strVal = strdup(rv->data.strVal);
	else 
	if	(lv->type == table_m)
		avm_table_inc_refcounter(lv->data.tableVal);
}

/*
 This here is the most important function,
 turning instructions into memory cells to be
 proccessed by the AVM
*/
avm_memcell* avm_translate_operand(vmarg_T arg, avm_memcell* reg)
{
	assert(arg);
	switch (arg->type)
	{
		case global_a:	return &stack[AVM_STACKSIZE-arg->val-1];
		case local_a:	return &stack[topsp-arg->val];
		case formal_a:	return &stack[topsp+AVM_STACKENV_SIZE+arg->val+1];

		case retval_a:	return &retval;

		case number_a:
		{
			reg->type = number_m;
			reg->data.numVal = get_numConst(arg->val);
			return reg;
		}
		
		case string_a:
		{
			reg->type = string_m;
			reg->data.strVal = strdup(get_strConsts(arg->val));
			return reg;
		}
		
		case bool_a:
		{
			reg->type = bool_m;
			reg->data.boolVal = arg->val;
			return reg;
		}

		case undef_a: reg->type = undef_m;	return reg;
		case nil_a:	reg->type = nil_m;	return reg;
		
		case userfunc_a:
		{
			reg->type = userfunc_m;
			reg->data.funcVal = arg->val;
			return reg;
		}

		case libfunc_a:
		{
			reg->type = libfunc_m;
			reg->data.libfuncVal = arg->val;
			return reg;
		} 

		default:
			break;
	}

	return NULL;
}

void avm_dec_top(void)
{
	if (!top)
	{
		avm_log(ERROR,"stack overflow\n");
	}
	else
		--top;
}

void avm_push_envvalue(unsigned val)
{
	stack[top].type = number_m;
	stack[top].data.numVal = val;

	avm_dec_top();
}

void avm_call_saveenvironment(void)
{
	avm_push_envvalue(totalActuals);
	avm_push_envvalue(pc + 1);
	avm_push_envvalue(top + totalActuals + 2);
	avm_push_envvalue(topsp);

	return ;
}

unsigned avm_get_envvalue(unsigned i)
{
	assert(stack[i].type == number_m);
	unsigned val = (unsigned) stack[i].data.numVal;
	assert(stack[i].data.numVal == ((double) val));
	return val;
}

unsigned avm_total_actuals(void)
{
	return avm_get_envvalue(topsp + AVM_NUMACTUALS_OFFSET);
}

avm_memcell* avm_getactual(unsigned i)
{
	assert( i < avm_total_actuals());
	return &stack[topsp + AVM_STACKENV_SIZE + 1 + i ];
}

char* avm_bucket_tostring(
	avm_table* table,
	avm_table_bucket* bucket)
{
	assert(table);

	/*
	 If the bucket list is empty, 
	 return NULL (flag for do nothing)
	*/
	if(bucket == NULL)
		return NULL;

	/* We do not know how many strings are stored in a bucket,
	 so for safety reasons, we ll allocate a number of 'total' 
	 strings in the array and we will follow the same strategy 
	 that we applied to table_to_string */
	char* index = NULL;
	char* value = NULL;

	int total = table->total, curr = 0, final_size = 1;

	char** bucket_str = malloc(sizeof(char*)*total);
	assert(bucket_str);

	char* string = NULL;

	/*
	 We initialize the array with NULLs
	 Get rid of the garbage...
	*/
	for (int i = 0; i < total ; i++)
	{
		bucket_str[i] = NULL;
	}
	
	while (bucket)
	{
		index = avm_to_string(&bucket->key);
		value = avm_to_string(&bucket->value);

		/* The +6 is because we allocate memory also for { , }'\0'*/
		string = malloc(strlen(index) + strlen(value) + 9);
		sprintf(string,"{%s : %s}",index,value);

		bucket_str[curr++] = string;
		
		final_size += strlen(string); 

		bucket = bucket->next;
		string = index = value = NULL;
	}

	/* Now we just need to concantinate the strings... */
	string = malloc((final_size + curr)*sizeof(char));
	assert(string);

	for (int i = 0; i < final_size + curr ; i++)
	{
		string[i] = '\0'; 
	}

	/*We ll use a strcat */
	for (int i = 0; i < curr; i++)
	{
		strcat(string,bucket_str[i]);

		strcat(string,",");

		/* After concantinating them, free them*/
		free(bucket_str[i]);
	}

	free(bucket_str);
	return string;
}