/*
 	Authors: csd4881, csd4988, csd5038														
	
	main.c									

	Here lies the start of the program...
*/

#include <avm-mem.h>
#include <avm-log.h>
#include <avm-reader.h>
#include <dispatcher.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    FILE* ost_tcg = NULL;

	if(argc < 2 || argc > 4)
	{
		avm_log(USAGE,"%s [-i|flag to write the instructions in a file] <Alpha-binary file>\n",argv[0]);
		return EXIT_FAILURE;
	}

	if(argc == 2 && argv[1][0] == '-')
	{
		avm_log(USAGE,"%s [-i|flag to write the instructions in a file] <Alpha-binary file>\n",argv[0]);
		avm_log(NOTE,"You did not pass <Alpha-binary file>\n",argv[0]);
		return EXIT_FAILURE;
	}

	if(!strcmp(argv[1],"-i"))
		ost_tcg = fopen("tcg_instructions.txt","w");

	alpha_bin_reader(argv[argc-1]);
	print_tcg_arrays(ost_tcg);

	avm_initialize();

	while (!executionFinished){
		execute_cycle();
	}

	return EXIT_SUCCESS;
}