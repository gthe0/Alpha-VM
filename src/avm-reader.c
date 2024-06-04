#include <avm-reader.h>
#include <avm-log.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define AVM_BIN_MAGIC_NUM 3401334

/* Array to store the Instructions */
Instruction_T code = (Instruction_T)0;
unsigned codeSize = 0;

/* Array to store user functions */
UserFunc_T userFuncs = (UserFunc_T)0;
unsigned total_userFuncs = 0;

/* Arrays to store Const variable information */
double *numConsts = (double*)0;
unsigned total_numConsts = 0;

char **stringConsts = (char**)0;
unsigned total_stringConsts = 0;

char **namedLibfuncs = (char**)0;
unsigned total_namedLibfuncs = 0;

/* Array getters */
userfunc_t get_UserFunc(unsigned int i)
{
	assert(userFuncs && i <= total_userFuncs);
	return userFuncs[i];
}

double get_numConst(unsigned int i)
{
	assert(numConsts && i <= total_numConsts);
	return numConsts[i];
}

char* get_strConsts(unsigned int i)
{
	assert(stringConsts && i <= total_stringConsts);
	return stringConsts[i];
}

char* get_libFuncs(unsigned int i)
{
	assert(namedLibfuncs && i <= total_namedLibfuncs);
	return namedLibfuncs[i];
}

/* Reads the binary file and puts inputs into the arrays */
void alpha_bin_reader(char* ist_name)
{
	unsigned magicNum;
	FILE *ist = NULL;

	if (!(ist = fopen(ist_name, "rb")))
	{
		avm_log(ERROR, "Could not open file stream %s\n", ist_name);
		exit(EXIT_FAILURE);
	}

	/* Read and compare the magic Number */
	fread(&magicNum, sizeof(unsigned),1,ist);

	if(magicNum != AVM_BIN_MAGIC_NUM)
	{
		avm_log(ERROR,"Wrong binary file format\n");
		exit(EXIT_FAILURE);
	}

	/* Read the constant Numbers */
	fread(&total_numConsts, sizeof(unsigned),1,ist);

	numConsts = malloc(sizeof(double)*total_numConsts);
	fread(numConsts,sizeof(double),total_numConsts,ist);


	/* Read the constant Strings */
	fread(&total_stringConsts, sizeof(unsigned),1,ist);
	stringConsts =  malloc(sizeof(char*)*total_stringConsts);

	for (int i = 0; i < total_stringConsts; i++)
	{
		unsigned length;

		fread(&length, sizeof(length),1,ist);

		stringConsts[i] = malloc(sizeof(char)*(length+1));
		fread(stringConsts[i],length*sizeof(char),1,ist);

		stringConsts[i][length] = '\0';
	}

	/* Read the total numbers of Library Functions */
	fread(&total_namedLibfuncs, sizeof(unsigned),1,ist);
	namedLibfuncs =  malloc(sizeof(char*)*total_namedLibfuncs);
	
	for (int i = 0; i < total_namedLibfuncs; i++)
	{
		unsigned length;

		fread(&length, sizeof(length),1,ist);

		namedLibfuncs[i] = malloc(sizeof(char)*(length+1));
		fread(namedLibfuncs[i],length*sizeof(char),1,ist);

		namedLibfuncs[i][length] = '\0';
	}

	/* Read the user defined functions */
	fread(&total_userFuncs, sizeof(unsigned),1,ist);
	userFuncs = malloc(sizeof(userfunc_t)*total_userFuncs);

	for (int i = 0; i < total_userFuncs; i++)
	{
		/* Read each field separately... */
		fread(&(userFuncs[i].address),sizeof(unsigned),1,ist);
		fread(&(userFuncs[i].localSize),sizeof(unsigned),1,ist);

		/*
		 Read the id, but because it is constant,
		 first read it in a regular string to add
		 the null terminator at the very end '\0'
		*/
		unsigned length = 0;
		fread(&length,sizeof(unsigned),1,ist);

		char* str =  malloc(sizeof(char)*(length+1));
		fread(str,length*sizeof(char),1,ist);

		str[length] = '\0';
		userFuncs[i].id = str;
	}

	/* Read Instructions */
	fread(&codeSize,sizeof(unsigned),1,ist);

	code = malloc(sizeof(instruction)*codeSize);
	fread(code,sizeof(instruction),codeSize,ist);

	fclose(ist);
	return;
}