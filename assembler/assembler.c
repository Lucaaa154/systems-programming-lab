/*
  Assembler project for an imaginary assembley language.
  The program checks the validity of the received file that contains the assembley program, if it passed the validity
  tests, it will be converted to a binary code machine which will be printed in the output file using the octal base.
  Authors: Lucas Rohana , Joseph soussan
  Date: 18.08.2024
*/

#include "assembler.h"

/*
*   Main function of the assembler program.
* 
*   This function handles the processing of command line arguments, where each argument represents
*   a file containing assembly code. It calls the 'files_handle' function for each file to perform
*   validation, conversion, and output generation. The program checks if any files are provided
*   and processes them accordingly. 
* 
*   @param argc: The number of command line arguments.
*   @param argv: An array of strings representing the command line arguments. Each argument should be a file name.
* 
*   @return: 0 on successful completion of the file processing, indicating the program finished successfully.
*/

int main(int argc, char *argv[])
{
	int i; /* argc index */
    
	if (argc<2) /* no files have been found */
	{
		printf("No received files have been found\n");
	}
    
	for(i=1; i<argc; i++) /* checking all the received files from the command line */
	{
	    files_handle(argv[i]);
	} /* end of loop */
    
	return 0;
} /* end of main */

