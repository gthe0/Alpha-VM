/*
 	Authors: csd4881, csd4988, csd5038														
	
	avm-lib.h										

	Interface containing the library functions.
*/

#ifndef AVM_LIB_H
#define AVM_LIB_H

/* Typedefinition of libfunc */
typedef void (*library_func_t)(void);
typedef struct library_map library_map;

/*
 Struct used to map library 
 function ids to the functions 
*/
struct library_map{
	char* id;
	library_func_t func;
};

/**
* @brief Executes the corresponding function from the func table
* @param id The id of the function, used to find the right mapping
*/
void execute_lib_func(char* id);

#endif /* avm-library */