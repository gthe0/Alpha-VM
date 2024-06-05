/*
 	Authors: csd4881, csd4988, csd5038														
	
	avm-reader.h										

	Declarations of functions and variables/arrays
	used to stroe the binary file information
*/

#ifndef AVM_READER_H
#define AVM_READER_H

#include <avm-types.h>
#include <stdio.h>

/* The extracted instructions... */
extern Instruction_T code;
extern unsigned codeSize;
extern unsigned programVarOffset;

#define AVM_ENDING_PC codeSize

/* Getters of array elements */
userfunc_t get_UserFunc(unsigned int i);
double get_numConst(unsigned int i);
char* get_strConsts(unsigned int i);
char* get_libFuncs(unsigned int i);

/**
* @brief Used to read an Alpha binary file and confirm its validity  
* 
* @param ist_name Input Stream name aka. the alpha binary file name
*/
void alpha_bin_reader(char* ist_name);

/* Print Instructions
*/
void print_tcg_arrays(FILE *ost);

#endif /* Library used to read Alpha binary files*/