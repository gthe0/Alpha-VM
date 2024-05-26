#include <avm-mem.h>

#include <stdlib.h>
#include <assert.h>

avm_memcell stack[AVM_STACKSIZE];
avm_memcell ax, bx, cv, retval;

/**
* @brief Used to initialize the stack module
*/
void avm_initstack(void)
{
	for (unsigned i = 0; i < AVM_STACKSIZE ; i++)
	{	
		AVM_WIPEOUT(stack[i]);
		stack[i].type = undef_m;
	}

	return;
}

void avm_table_inc_refcounter(avm_table *table) {
    ++table->refCounter;
}

void avm_tabledec_refcounter(avm_table *table) {
	assert(table->refCounter > 0);
    
	/* Activate garbage collection... */
	if (!--table->refCounter) 
		avm_tabledestroy(table); 
}

void avm_table_buckets_init(avm_table_bucket** p)
{
	/* Same as assigningn NULL to each bucket */
	for (unsigned i = 0; i < AVM_TABLE_HASH_SIZE; i++)
		p[i] = (avm_table_bucket*)0; 
}


avm_table* avm_table_new(void)
{
    avm_table *table = malloc(sizeof(avm_table));
    AVM_WIPEOUT(*table);

    table->refCounter = table->total = 0;

    avm_tablebuckets_init(table->numIndexed);
    avm_tablebuckets_init(table->strIndexed);
    avm_tablebuckets_init(table->boolIndexed);
    avm_tablebuckets_init(table->userIndexed);
    avm_tablebuckets_init(table->libIndexed);

    return table;
}

void avm_mem_cell_clear(avm_memcell* m)
{
	assert(m);

	/* Free the correct data based on its type */
	switch (m->type)
	{
		case string_m:
			free(m->data.strVal);
			break;
		
		case libfunc_m:
			free(m->data.libfuncVal);
			break;

		case table_m:
			assert(m->data.tableVal);
    		avm_table_dec_ref_counter(m->data.tableVal);
			break;

		default:
			break;
	}
	return;
}

void avm_table_buckets_destroy(avm_table_bucket** p)
{
	for (unsigned i = 0; i < AVM_TABLE_HASH_SIZE ; i++, p++)
	{
		for (avm_table_bucket* b = p; i < b;)
		{
			avm_table_bucket* del = b;
			b = b->next;

			/* Free the cells before freeing the bucket */
			avm_mem_cell_clear(&del->key);
			avm_mem_cell_clear(&del->value);

			free(del);
		}

		p[i] = NULL ;
	}
}

void avm_table_destroy (avm_table* t)
{
	avm_table_buckets_destroy(t->numIndexed);
	avm_table_buckets_destroy(t->strIndexed);
	avm_table_buckets_destroy(t->boolIndexed);
	avm_table_buckets_destroy(t->userIndexed);
	avm_table_buckets_destroy(t->libIndexed);

	free(t);
}

