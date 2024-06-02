/*
 	Authors: csd4881, csd4988, csd5038														
	
	avm-mem.h										

	Here we define the avm-table ADT
*/

#ifndef AVM_MEM_H
#define AVM_MEM_H

#include <avm-types.h>

#define AVM_STACKSIZE 0x1000
#define AVM_STACKENV_SIZE 4
#define AVM_TABLE_HASH_SIZE 211

#define AVM_WIPEOUT(m) memset(&(m),0,sizeof(m))

/* Used to gain access to the variables */
extern avm_memcell stack[AVM_STACKSIZE];
extern avm_memcell ax, bx, cx, retval;
extern unsigned int top, topsp; 

/* Module typedefintion */
typedef struct avm_table avm_table, *AVM_T;

/* Typedefs of the various structs used */
typedef struct avm_memcell avm_memcell;
typedef struct avm_table_bucket avm_table_bucket;

typedef enum avm_memcell_t avm_memcell_t;

/* The type of the memory cell of the avm */
enum avm_memcell_t
{
    number_m =	0,
    string_m =	1,
    bool_m =	2,
    table_m =	3,
    userfunc_m = 4,
    libfunc_m = 5,
    nil_m =		6,
    undef_m =	7
};

/*
 Used to store the extracted
 info from the alpha bianry file
*/
struct avm_memcell
{
	avm_memcell_t type;
    union {
        double			numVal;
        char* 			strVal;
        char			boolVal;
        AVM_T 			tableVal;
        unsigned int 	funcVal; 
        unsigned int	libfuncVal;
    } data;
};

/* The bucket/list of the avm table */
struct avm_table_bucket
{
	avm_memcell key;
    avm_memcell value;
    avm_table_bucket *next;
};

/* Definition of the avm table type */
struct avm_table
{
    unsigned refCounter;

    avm_table_bucket *strIndexed[AVM_TABLE_HASH_SIZE];
    avm_table_bucket *numIndexed[AVM_TABLE_HASH_SIZE];
    avm_table_bucket *boolIndexed[AVM_TABLE_HASH_SIZE];
    avm_table_bucket *userIndexed[AVM_TABLE_HASH_SIZE];
    avm_table_bucket *libIndexed[AVM_TABLE_HASH_SIZE];

    unsigned total;
};


/**
* @brief Used to initialize the stack module
*/
void avm_initstack(void);

/**
* @brief Increments the refcounter of a table
* @param table The table to which we will increment the refcoutner 
*/
void avm_table_inc_refcounter(avm_table *table);

/**
* @brief Decrements the refcounter of a table
* @param table The table to which we will decrement the refcoutner 
*/
void avm_table_dec_refcounter(avm_table *table);

/**
* @brief Initializes the buckets of a table
* @param p Pointer to bucket to be initialized
*/
void avm_table_buckets_init(avm_table_bucket** p);

/**
* @brief Initializes a bucket of a table
* @param p Pointer to the bucket to be initialized
*/
void avm_mem_cell_clear(avm_memcell* m);


/**
* @brief Destroy a bucket of a table
* @param p Pointer to the bucket to be initialized
*/
void avm_table_buckets_destroy(avm_table_bucket** p);

/**
* @brief Table destructor
* @param t The table to be destroyed
*/
void avm_table_destroy (avm_table* t);

/**
* @brief used for lvalue assignments  
* @param lv The lvalue of the assignment
* @param rv The rvalue of the assignment
*/
void avm_assign(avm_memcell* lv, avm_memcell* rv);

/**
 * @brief used to translate the operands (vmarg_T) into avm_memcells
 * @param arg The argument to be translated
 * @param reg The memcell to 
 * @return The result of the translation
*/
avm_memcell* avm_translate_operand(vmarg_T arg, avm_memcell* reg);

/**
* @brief decreases the top counter of the stack  
*/
void avm_dec_top(void);

/**
* @brief Used to store the enviroment information in the stack
* @param val the value to be pushed in the stack
*/
void avm_push_envvalue(unsigned val);

#endif /* avm module */