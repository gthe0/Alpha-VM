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
	FILE *ost = NULL;

	if (!(ost = fopen(ist_name, "rb")))
	{
		avm_log(ERROR, "Could not open file stream %s\n", ist_name);
		exit(EXIT_FAILURE);
	}

	/* Read and compare the magic Number */
	fread(&magicNum, sizeof(unsigned),1,ost);

	if(magicNum != AVM_BIN_MAGIC_NUM)
	{
		avm_log(ERROR,"Wrong binary file format\n");
		exit(EXIT_FAILURE);
	}

	/* Read the constant Numbers */
	fread(&total_numConsts, sizeof(unsigned),1,ost);

	numConsts = malloc(sizeof(double)*total_numConsts);
	fread(numConsts,sizeof(double),total_numConsts,ost);


	/* Read the constant Strings */
	fread(&total_stringConsts, sizeof(unsigned),1,ost);
	stringConsts =  malloc(sizeof(char*)*total_stringConsts);

	for (int i = 0; i < total_stringConsts; i++)
	{
		unsigned length;

		fread(&length, sizeof(length),1,ost);

		stringConsts[i] = malloc(sizeof(char)*(length+1));
		fread(stringConsts[i],length*sizeof(char),1,ost);

		stringConsts[i][length] = '\0';
	}

	/* Read the total numbers of Library Functions */
	fread(&total_namedLibfuncs, sizeof(unsigned),1,ost);
	char** libfunc_string =  malloc(sizeof(char*)*total_namedLibfuncs);
	
	for (int i = 0; i < total_namedLibfuncs; i++)
	{
		unsigned length;

		fread(&length, sizeof(length),1,ost);

		libfunc_string[i] = malloc(sizeof(char)*(length+1));
		fread(libfunc_string[i],length*sizeof(char),1,ost);

		libfunc_string[i][length] = '\0';
	}

	/* Read the user defined functions */
	UserFunc_T user_func_array = NULL;
	
	fread(&total_userFuncs, sizeof(unsigned),1,ost);
	user_func_array = malloc(sizeof(userfunc_t)*total_userFuncs);

	for (int i = 0; i < total_userFuncs; i++)
	{
		/* Read each field separately... */
		fread(&(user_func_array[i].address),sizeof(unsigned),1,ost);
		fread(&(user_func_array[i].localSize),sizeof(unsigned),1,ost);

		/*
		 Read the id, but because it is constant,
		 first read it in a regular string to add
		 the null terminator at the very end '\0'
		*/
		unsigned length = 0;
		fread(&length,sizeof(unsigned),1,ost);

		char* str =  malloc(sizeof(char)*(length+1));
		fread(str,length*sizeof(char),1,ost);

		str[length] = '\0';
		user_func_array[i].id = str;
	}

	/* Read Instructions */
	fread(codeSize,sizeof(unsigned),1,ost);
	fread(code,sizeof(instruction),codeSize,ost);

	fclose(ost);
	return;
}