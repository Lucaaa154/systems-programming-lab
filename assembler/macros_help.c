/* file includes the hekoing functions if the macros handling and expansion */

#include "assembler.h"


/*
*   Adds a new macro to the macro table.
*   
*   This function 'add_macro' inserts a new macro with the specified name into the macro list.
*   It updates the macro list by linking the new macro to the existing macros.
* 
*   @param fp: Pointer to the file where the macro data is stored.
*   @param file_name: The name of the file
*   @param table_head: Pointer to the pointer first macro in the list.
*   @param table_tail: Pointer to the pointer last macro in the list.
*   @param word: The name of the new macro to be added.
*   @param lineNum: The line number in the file where the macro is defined.
* 
*   @return 0 if the macro was successfully added; otherwise, returns ERROR(-1).
*/

int add_macro(FILE *fp, char *file_name, struct macro_att **table_head, struct macro_att **table_tail, char *line, int *lineNum)
{
	/* variable declarations */
    char temp_line[MAX_LINE_LEN]; /* a string to store the lines */
    char bkup_line[MAX_LINE_LEN]; /* a string to backup the string line */
    char *temp_word; /* helping string to store the words */
    char **temp_pointer; /* temporary pointer for reallocating */
    struct macro_att *new_macro; /* pointer for macro object */
    
    /* variable initialization */
    new_macro = (struct macro_att*) malloc(sizeof(struct macro_att)); /* allocating macro object */


	if (new_macro == NULL) /* memory allocation failed */
	{
    	printf("For line %d, Memory allocation failed\n",*lineNum);
    	/* assuming that "endmacr" is exist */
    	while(fgets(temp_line, sizeof(temp_line), fp) && temp_line != NULL && strcmp(temp_line,"endmacr") != 0)
    	{
    	    (*lineNum)++; /* updating the lineNum */
    	}
    	return ERROR;
	}
    
    temp_word = strtok(line," \r\n\t"); /* word = "macr" */
    temp_word = strtok(NULL," \r\n\t"); /* word = macro name */
    
	if ((*table_head) == NULL)
	{
		(*table_head) = new_macro; /* first macro in the table */
	}

	if ((*table_tail) != NULL)
	{
    	(*table_tail)->next = new_macro; /* the last macro points to the new macro */
	}

	(*table_tail) = new_macro; /* updating tail */

	/* initializing macro attributes */
	memmove(new_macro->macro_name, temp_word, strlen(temp_word)); /* macro name */
	new_macro->macro_name[strlen(temp_word)] = '\0';
	new_macro->next = NULL; /* next field to null */
	new_macro->lines = NULL;
	new_macro->lines_number = 0; /* initializing the macro lines number to 0 */
    
    (*lineNum)++;
	while(1) /* when recognizing "endmacr" word the while loop will be broken, assuming that endmacr exists */
	{	/* reading the lines and allocating memory for them */
		memset(temp_line, '\0', sizeof(temp_line)); /* clear the string line */
		memset(bkup_line, '\0', sizeof(bkup_line)); /* clear the string bkup_line */
		fgets(temp_line, sizeof(temp_line), fp);
        memmove(bkup_line, temp_line, strlen(temp_line));
        bkup_line[strlen(temp_line)] = '\0';
        temp_word = strtok(temp_line," \r\n\t");

        /* assuming that every line in macro is not more than 80 characters */
        
    	if((temp_word != NULL) && (strncmp(temp_word,"endmacr",7) == 0)) /* we check if this is the macro definition */
    	{
    	    if(strcmp(temp_word,"endmacr") != 0) /* extra connected text */
    	    {
    	        printf("Error: in line %d, wrong definition of the macro. extra text after \"endmacr\"\n", *lineNum);
    			return ERROR;
    	    }
    	    temp_word = strtok(NULL," \r\n\t");
 			if(temp_word != NULL) /* checking extra text after "endmacr" */
        	{
    			printf("Error: in line %d, wrong definition of the macro. extra text after \"endmacr\"\n", *lineNum);
    			return ERROR;
    	   	}
		return 0;
    	}

		if (new_macro->lines_number != 0) /* more than one macro line, so we will use realloc and not malloc */
    	{
    		temp_pointer = (char**)realloc(new_macro->lines, sizeof(char*) * (new_macro->lines_number+1)); /* reallocating memory for the line */

    		if (temp_pointer == NULL)
    		{
        		printf("For line %d, Memory allocation failed\n", *lineNum);
        		/* assuming that "endmacr" is exist */
            	while(fgets(temp_line, sizeof(temp_line), fp) && temp_line != NULL && strcmp(temp_line,"endmacr") != 0)
            	{
            	    (*lineNum)++; /* updating the lineNum */
            	}
        		return ERROR;
    		}

    		new_macro->lines = temp_pointer;
    		new_macro->lines[new_macro->lines_number] = (char*)malloc(sizeof(char)* MAX_LINE_LEN); /* allocating array to the new array cell */

    		if (new_macro->lines[new_macro->lines_number] == NULL)
    		{
        		printf("For line %d, Memory allocation failed\n", *lineNum);
        		/* assuming that "endmacr" is exist */
            	while(fgets(temp_line, sizeof(temp_line), fp) && temp_line != NULL && strcmp(temp_line,"endmacr") != 0)
            	{
            	    (*lineNum)++; /* updating the lineNum */
            	}
        		return ERROR;
    		}
    	}

    	else /* allocate memory for the first line in this macro */
    	{
    		new_macro->lines = (char**)malloc(sizeof(char*));

    		if (new_macro->lines == NULL)
    		{
        		printf("For line %d, Memory allocation failed\n", *lineNum);
        		/* assuming that "endmacr" is exist */
        	while(fgets(temp_line, sizeof(temp_line), fp) && temp_line != NULL && strcmp(temp_line,"endmacr") != 0)
        	{
        	    (*lineNum)++; /* updating the lineNum */
        	}
        		return ERROR;
    		}

    		new_macro->lines [0] = (char*)malloc(sizeof(char) * MAX_LINE_LEN); /* pointer of the array points to array with size of 83 */

    		if (new_macro->lines [0] == NULL)
    		{
        		printf("For line %d, Memory allocation failed\n", *lineNum);
        		/* assuming that "endmacr" is exist */
            	while(fgets(temp_line, sizeof(temp_line), fp) && temp_line != NULL && strcmp(temp_line,"endmacr") != 0)
            	{
            	    (*lineNum)++; /* updating the lineNum */
            	}
        		return ERROR;
    		}
    	}

		memmove(new_macro->lines[new_macro->lines_number],bkup_line, strlen(bkup_line)); /* copying the line to the allocated memory */
        (new_macro->lines[new_macro->lines_number])[strlen(bkup_line)] = '\0';
        new_macro->lines_number++;
        (*lineNum)++;
    } /* end of while loop */
    
} /* end of add_macro function */


/*
*   This function checks if the received word is a macro name that exists in the macros table.
*   It searches through the table of defined macros to find a match.
* 
*   @param word: The word to check if it's a macro name.
*   @param table_pointer: The pointer to the table of defined macros.
* 
*   @return The macro if found; otherwise, NULL if the macro is not in the table or if the table is empty.
*/
struct macro_att* defined_macro(char *word, struct macro_att *table_pointer)
{
    while(table_pointer != NULL)
    {
        if(strcmp(table_pointer->macro_name, word) == 0) /* the same name */
        {
            return table_pointer; /* returning the macro */
        }
        table_pointer = table_pointer->next;
    }

    return NULL;
} /* end of defined_macro function */

/*
*   This function expands the given macro by writing its content to the received file.
* 
*   @param fp: The file pointer to which the expanded macro content will be written.
*   @param macro_pointer: Pointer to the macro structure that contains the macro's content to be expanded.
*/
void expand_macro(FILE *fp, struct macro_att *macro_pointer)
{
    /* variable declarations */
    int i;
    int size;
    
    /* variable initialization */
    size = macro_pointer->lines_number; /* lines number to expand */

    for(i=0; i<size; i++)
    {
        fputs(macro_pointer->lines [i],fp); /* copying the macro lines to the file */
    }
} /* end of expand_macro function */

/*
*   Frees the memory allocated for the macros table, including each macro's content.
* 
*   This function iterates through the macros table and releases the memory allocated for each macro's lines.
*   It then frees the memory allocated for the macros table itself.
* 
*   @param macros_table: Pointer to the head of the macros table.
*/
void free_macros(struct macro_att *macros_table)
{
	int i = 0;
	int j = 0;
	struct macro_att *temp_macro = NULL;
	
	while(macros_table != NULL)
	{
    	temp_macro = macros_table;
    	macros_table = macros_table->next;
    	j = (temp_macro)->lines_number; /* for loop limit  */

    	for(i=0; i<j; i++) /* till lines_number */
    	{
    		free ((temp_macro)->lines[i]); /* free every line */
    	}
    	
    	if((temp_macro)->lines != NULL)
    	{
    	    free ((temp_macro)->lines); /* free the lines "array" */
    	}
    	free (temp_macro); /* free the macro object */
	
	} /* end of while loop */
}

