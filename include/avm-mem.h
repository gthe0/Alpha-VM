/*
 	Authors: csd4881, csd4988, csd5038														
	
	avm-mem.h										

	Here we define the avm-table module
*/

#ifndef AVM_MEM_H
#define AVM_MEM_H

#include <avm-types.h>

/* Module typedefintion */
typedef struct avm_table avm_table, *AVM_T;

/* Typedefs of the various structs used */
typedef struct avm_memcell avm_memcell, *AVM_Cell;
typedef struct avm_memcell avm_memcell, *AVM_Cell;

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
        bool			boolVal;
        AVM_T 			tableVal;
        unsigned int 	funcVal; 
        char			libfuncVal;
    } data;
};



#endif /* avm module */