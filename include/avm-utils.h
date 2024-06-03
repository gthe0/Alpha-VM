/*
 	Authors: csd4881, csd4988, csd5038														
	
	avm-utils.h										

	Function used for logging avm errors
*/

#ifndef AVM_UTILS_H
#define AVM_UTILS_H

#include <avm-mem.h>

/* Arrays used to log the vairous types of args */
extern char* typeString[];

/**
* @brief Produces a string based on the contents of the memcell 
* @param m The avm memory cell
* 
* @return The string produced 
*/
char* avm_to_string(avm_memcell* m);

#endif /* avm utilities lib */
