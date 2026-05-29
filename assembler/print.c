/*
 * This program is responsible for generating the final output of the project.
 * It includes two functions that handle file creation and data output.
 * Specifically, the program writes data in octal format and manages external 
 * and entry labels, if they are present.
 */


#include "assembler.h"

/*
*  The `octal_print` function creates a file and writes the values from the instruction 
*  and data tables in octal format.
* 
*  The function first checks if the combined size of instruction and data is zero, 
*  If the file cannot be opened, it prints an error message. Otherwise, it writes the instruction 
*  and data values to the file, including their addresses.
* 
*  @param filename: The name of the file to be created and written to
*  @param buffer_data: guidance memory image
*  @param buffer_inst: instructions memory image
*  @param IC: The instruction counter (number of instructions)
*  @param DC: The data counter (number of data entries)
*/

void octal_print(char *filename, unsigned int buffer_data[MEM_IMG_SIZE], unsigned int buffer_inst[MEM_IMG_SIZE], int IC, int DC)
{
	/* variable declarations */
    int i;
    int j;
    int address;
    int mask;
    FILE *fp;
    
    /* variable initialization */
    i = MEM_FIRST_INDEX; /* counter */
    j = 0; /* counter 2 */
    address = MEM_FIRST_INDEX;
    mask = 32767; /* first 15 bits from right = 1 and the others = 0 */


	if (IC+DC == 0) /* memory image is empty */
	{
	    convert_ending(filename,1); /* add the ".s" ending */
		printf("Warning: the memory image is empty for file %s\n", filename);
		return;
	}

	fp = fopen (filename, "w");

	if (fp == NULL)
	{
		printf ("Can't open a file for output\n");
		return;
	}

	
	/* prints the title */
	fprintf (fp, "   %d ", IC);
	fprintf (fp, "%d\n", DC);
	j = IC; /* j will count the number of the instructions that have to be printed */

	while (j > 0) /* printing in the object file the instructions before */ /* loop1 */
	{
    	fprintf (fp, "%04d ", address); /* printing the address in file */
    	fprintf (fp, "%05o\n", mask & buffer_inst[i]); /* printing the left digit in file */
    	address++;
    	j--; 
		i++;
	} /* end of loop2 */

	i = MEM_FIRST_INDEX;
	j = DC; /* j will count the number of the guidances that have to be printed */
    
	while (j > 0)  /* loop3 */
	{ 
		fprintf (fp, "%04d ", address); /* printing the address in file */
		fprintf (fp, "%05o\n", mask & buffer_data[i]); /* printing the left digit in file */
		address++;
		j--;
		i++;
	} /* end of loop2 */

	fclose (fp);
} /* end of octal_print function */


/*
*  The `label_print` function generates files for labels of type `.entry` and `.extern`.
*  It iterates through the list of labels and creates two separate files: one for 
*  `.extern` labels and another for `.entry` labels.
* 
*  In the `.extern` file, it prints each extern label along with its address.
*  In the `.entry` file, it prints each entry label along with its address.
* 
*  @param head: A pointer to the list of labels
*  @param filename: The name of the file to be created (files will be created with ".ext" and ".ent" endings)
*/
void label_print(struct label_node **head, char *filename)
{
	/* Variable declarations */
    int i;
    struct label_node *current;
    FILE *ext_fp; /* extern file pointer*/
    FILE *ent_fp;
    
    /* Variable initialization */
    i = 0;
    current = *head; /* label list current node */
    ext_fp = NULL; /* pointer to the extern labels */
    ent_fp = NULL; /* pointer to the entry labels */


	convert_ending(filename,5); /* add the ".ext" ending */
	while (current != NULL && i != ERROR) /* i will be 0 if the file cant be open so the we will exit the loop */ /* loop1 */
	{
		if ((current->label_type) == 'x' && (current->dec_num) != -1) /* extern label */
		{
			if (i == 0) /* first extern label will open the file and will be added to it */
			{
				ext_fp = fopen (filename, "w");
				i++;

				if (ext_fp == NULL)
				{
					printf ("Can't open a file for extern labels\n");
					i = ERROR;
				}

				else
				{
					fprintf (ext_fp, "%s   ", current->label); /* label name */
					fprintf (ext_fp, "%04d\n", current->dec_num); /* label address */
				}
			}

			else /* not the first extern label */
			{
				fprintf (ext_fp, "%s   ", current->label); /* label name */
				fprintf (ext_fp, "%04d\n", current->dec_num); /* label address */
			}
		}

		current = current->next;
	} /* end of loop1 */

	if (ext_fp != NULL)
	{
		fclose (ext_fp);
	}

	convert_ending(filename,4); /* add the ".ent" ending */
	i = 0;
	current = *head; /* back to the head of the label list */

	while (current != NULL && i != ERROR) /* loop2 */
	{
		if ((current->label_type) == 'n') /* declared as entry label */
		{
			if (i == 0) /* first entry label will open the entries file and will be added to it */
			{
				ent_fp = fopen (filename, "w"); /* open for writing */
				i++;

				if (ent_fp == NULL)
				{
					printf ("Can't open a file for extern labels\n");
					i = ERROR;
				}

				else
				{
					fprintf (ent_fp, "%s   ", current->label); /* label name */
					fprintf (ent_fp, "%04d\n", current->dec_num); /* label address */
				}
			}

	  		else /* not the first entry label */
    		{
      			fprintf (ent_fp, "%s   ", current->label); /* label name */
      			fprintf (ent_fp, "%04d\n", current->dec_num); /* label address */
    		}
		}

		current = current->next;
	} /* end of loop2 */

	if (ent_fp != NULL)
	{
        fclose (ent_fp);
	}

} /* end of label_print function */

