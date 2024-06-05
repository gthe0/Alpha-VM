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
#include <stdlib.h>

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		avm_log(USAGE,"%s <Alpha-binary file>\n",argv[0]);
		return EXIT_FAILURE;
	}

	alpha_bin_reader(argv[1]);
	print_tcg_arrays(stdout);

	avm_initialize();

	while (!executionFinished){
		execute_cycle();
	}

	return EXIT_SUCCESS;
}