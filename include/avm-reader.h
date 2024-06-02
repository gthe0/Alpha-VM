/*
 	Authors: csd4881, csd4988, csd5038														
	
	avm-reader.h										

	Declarations of functions and variables/arrays
	used to stroe the binary file information
*/

#include <avm-types.h>

/* The arrays defined in avm-reader.c
 to stroe the binary information 
*/
UserFunc_T userFuncs = (UserFunc_T)0;
unsigned total_userFuncs;

extern double *numConsts;
extern unsigned total_numConsts;

extern char **stringConsts;
extern unsigned total_stringConsts;

extern char **namedLibfuncs;
extern unsigned total_namedLibfuncs;

/**
* @brief Used to read an Alpha binary file and confirm its validity  
* 
* @param ist_name Input Stream name aka. the alpha binary file name
*/
void alpha_bin_reader(char* ist_name);